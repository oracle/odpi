//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// dpiStmt.c
//   Implementation of statements (cursors).
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

// forward declarations of internal functions only used in this file
static int dpiStmt__getQueryInfo(dpiStmt *stmt, uint32_t pos,
        dpiQueryInfo *info, dpiError *error);
static int dpiStmt__getQueryInfoFromParam(dpiStmt *stmt, OCIParam *param,
        dpiQueryInfo *info, dpiError *error);
static int dpiStmt__postFetch(dpiStmt *stmt, dpiError *error);
static int dpiStmt__preFetch(dpiStmt *stmt, dpiError *error);
static int dpiStmt__reExecute(dpiStmt *stmt, uint32_t numIters,
        dpiExecMode mode, dpiError *error);


//-----------------------------------------------------------------------------
// dpiStmt__allocate() [INTERNAL]
//   Create a new statement object and return it. In case of error NULL is
// returned.
//-----------------------------------------------------------------------------
int dpiStmt__allocate(dpiConn *conn, int scrollable, dpiStmt **stmt,
        dpiError *error)
{
    dpiStmt *tempStmt;

    *stmt = NULL;
    if (dpiGen__allocate(DPI_HTYPE_STMT, conn->env, (void**) &tempStmt,
            error) < 0)
        return DPI_FAILURE;
    if (dpiGen__setRefCount(conn, error, 1) < 0) {
        dpiStmt__free(tempStmt, error);
        return DPI_FAILURE;
    }
    tempStmt->conn = conn;
    tempStmt->fetchArraySize = DPI_DEFAULT_FETCH_ARRAY_SIZE;
    tempStmt->scrollable = scrollable;
    *stmt = tempStmt;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__bind() [INTERNAL]
//   Bind the variable to the statement using either a position or a name. A
// reference to the variable will be retained.
//-----------------------------------------------------------------------------
static int dpiStmt__bind(dpiStmt *stmt, dpiVar *var, int addReference,
        uint32_t pos, const char *name, uint32_t nameLength, dpiError *error)
{
    dpiBindVar *bindVars, *entry;
    OCIBind *bindHandle = NULL;
    int found, dynamicBind;
    sword status;
    uint32_t i;

    // a zero length name is not supported
    if (pos == 0 && nameLength == 0)
        return dpiError__set(error, "bind zero length name",
                DPI_ERR_NOT_SUPPORTED);

    // check to see if the bind position or name has already been bound
    found = 0;
    for (i = 0; i < stmt->numBindVars; i++) {
        entry = &stmt->bindVars[i];
        if (entry->pos == pos && entry->nameLength == nameLength) {
            if (nameLength > 0 && strncmp(entry->name, name, nameLength) != 0)
                continue;
            found = 1;
            break;
        }
    }

    // if already found, use that entry
    if (found) {

        // if already bound, no need to bind a second time
        if (entry->var == var)
            return DPI_SUCCESS;

        // otherwise, release previously bound variable, if applicable
        else if (entry->var) {
            dpiGen__setRefCount(entry->var, error, -1);
            entry->var = NULL;
        }

    // if not found, add to the list of bind variables
    } else {

        // allocate memory for additional bind variables, if needed
        if (stmt->numBindVars == stmt->allocatedBindVars) {
            bindVars = calloc(stmt->allocatedBindVars + 8, sizeof(dpiBindVar));
            if (!bindVars)
                return dpiError__set(error, "allocate bind vars",
                        DPI_ERR_NO_MEMORY);
            if (stmt->bindVars) {
                for (i = 0; i < stmt->numBindVars; i++)
                    bindVars[i] = stmt->bindVars[i];
                free(stmt->bindVars);
            }
            stmt->bindVars = bindVars;
            stmt->allocatedBindVars += 8;
        }

        // add to the list of bind variables
        entry = &stmt->bindVars[stmt->numBindVars];
        entry->var = NULL;
        entry->pos = pos;
        if (name) {
            entry->name = malloc(nameLength);
            if (!entry->name)
                return dpiError__set(error, "allocate memory for name",
                        DPI_ERR_NO_MEMORY);
            entry->nameLength = nameLength;
            memcpy( (void*) entry->name, name, nameLength);
        }
        stmt->numBindVars++;

    }

    // for PL/SQL where the maxSize is greater than 32K, adjust the variable
    // so that LOBs are used internally
    if (var->isDynamic && (stmt->statementType == OCI_STMT_BEGIN ||
            stmt->statementType == OCI_STMT_DECLARE ||
            stmt->statementType == OCI_STMT_CALL)) {
        if (dpiVar__convertToLob(var, error) < 0)
            return DPI_FAILURE;
    }

    // perform actual bind
    if (addReference)
        dpiGen__setRefCount(var, error, 1);
    entry->var = var;
    dynamicBind = stmt->isReturning || var->isDynamic;
    if (pos > 0)
        status = OCIBINDBYPOS(stmt->handle, &bindHandle, error->handle,
                pos, (dynamicBind) ? NULL : var->data.asRaw,
                (dynamicBind) ? SB4MAXVAL : var->sizeInBytes,
                var->type->oracleType, (dynamicBind) ? NULL : var->indicator,
                (dynamicBind || var->type->sizeInBytes) ? NULL :
                        var->actualLength,
                (dynamicBind) ? NULL : var->returnCode,
                (var->isArray) ? var->maxArraySize : 0,
                (var->isArray) ? &var->actualArraySize : NULL,
                (dynamicBind) ? OCI_DATA_AT_EXEC : OCI_DEFAULT);
    else status = OCIBINDBYNAME(stmt->handle, &bindHandle, error->handle,
            (text*) name, nameLength, (dynamicBind) ? NULL : var->data.asRaw,
            (dynamicBind) ? SB4MAXVAL : var->sizeInBytes,
            var->type->oracleType, (dynamicBind) ? NULL : var->indicator,
            (dynamicBind || var->type->sizeInBytes) ? NULL : var->actualLength,
            (dynamicBind) ? NULL : var->returnCode,
            (var->isArray) ? var->maxArraySize : 0,
            (var->isArray) ? &var->actualArraySize : NULL,
            (dynamicBind) ? OCI_DATA_AT_EXEC : OCI_DEFAULT);
    if (dpiError__check(error, status, stmt->conn, "bind") < 0)
        return DPI_FAILURE;

    // set the charset form if applicable
    if (var->type->charsetForm != SQLCS_IMPLICIT) {
        status = OCIAttrSet(bindHandle, OCI_HTYPE_BIND,
                (dvoid*) &var->type->charsetForm, 0, OCI_ATTR_CHARSET_FORM,
                error->handle);
        if (dpiError__check(error, status, stmt->conn, "set charset form") < 0)
            return DPI_FAILURE;
    }

    // set the max data size, if applicable
    if (var->type->sizeInBytes == 0 && !var->isDynamic) {
        status = OCIAttrSet(bindHandle, OCI_HTYPE_BIND,
                (dvoid*) &var->sizeInBytes, 0, OCI_ATTR_MAXDATA_SIZE,
                error->handle);
        if (dpiError__check(error, status, stmt->conn,
                "set max data size") < 0)
            return DPI_FAILURE;
    }

    // bind object, if applicable
    if (var->objectIndicator) {
        status = OCIBindObject(bindHandle, error->handle, var->objectType->tdo,
                var->data.asRaw, 0, var->objectIndicator, 0);
        if (dpiError__check(error, status, stmt->conn, "bind object") < 0)
            return DPI_FAILURE;
    }

    // setup dynamic bind, if applicable
    if (dynamicBind) {
        status = OCIBindDynamic(bindHandle, error->handle, var,
                (OCICallbackInBind) dpiVar__inBindCallback, var,
                (OCICallbackOutBind) dpiVar__outBindCallback);
        if (dpiError__check(error, status, stmt->conn, "bind dynamic") < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__checkOpen() [INTERNAL]
//   Determine if the statement is open and available for use.
//-----------------------------------------------------------------------------
static int dpiStmt__checkOpen(dpiStmt *stmt, const char *fnName,
        dpiError *error)
{
    if (dpiGen__startPublicFn(stmt, DPI_HTYPE_STMT, fnName, error) < 0)
        return DPI_FAILURE;
    if (!stmt->handle)
        return dpiError__set(error, "check closed", DPI_ERR_STMT_CLOSED);
    if (!stmt->conn->handle)
        return dpiError__set(error, "check connection", DPI_ERR_NOT_CONNECTED);
    if (stmt->statementType == 0 && dpiStmt__init(stmt, error) < 0)
        return DPI_FAILURE;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__clearBatchErrors() [INTERNAL]
//   Clear the batch errors associated with the statement.
//-----------------------------------------------------------------------------
static void dpiStmt__clearBatchErrors(dpiStmt *stmt, dpiError *error)
{
    if (stmt->batchErrors) {
        free(stmt->batchErrors);
        stmt->batchErrors = NULL;
    }
    stmt->numBatchErrors = 0;
}


//-----------------------------------------------------------------------------
// dpiStmt__clearBindVars() [INTERNAL]
//   Clear the bind variables associated with the statement.
//-----------------------------------------------------------------------------
static void dpiStmt__clearBindVars(dpiStmt *stmt, dpiError *error)
{
    uint32_t i;

    if (stmt->bindVars) {
        for (i = 0; i < stmt->numBindVars; i++) {
            dpiGen__setRefCount(stmt->bindVars[i].var, error, -1);
            if (stmt->bindVars[i].name)
                free( (void*) stmt->bindVars[i].name);
        }
        free(stmt->bindVars);
        stmt->bindVars = NULL;
    }
    stmt->numBindVars = 0;
    stmt->allocatedBindVars = 0;
}


//-----------------------------------------------------------------------------
// dpiStmt__clearQueryVars() [INTERNAL]
//   Clear the query variables associated with the statement.
//-----------------------------------------------------------------------------
static void dpiStmt__clearQueryVars(dpiStmt *stmt, dpiError *error)
{
    uint32_t i;

    if (stmt->queryVars) {
        for (i = 0; i < stmt->numQueryVars; i++) {
            if (stmt->queryVars[i]) {
                dpiGen__setRefCount(stmt->queryVars[i], error, -1);
                stmt->queryVars[i] = NULL;
            }
            if (stmt->queryInfo[i].objectType) {
                dpiGen__setRefCount(stmt->queryInfo[i].objectType, error, -1);
                stmt->queryInfo[i].objectType = NULL;
            }
        }
        free(stmt->queryVars);
        stmt->queryVars = NULL;
    }
    if (stmt->queryInfo) {
        free(stmt->queryInfo);
        stmt->queryInfo = NULL;
    }
    stmt->numQueryVars = 0;
}


//-----------------------------------------------------------------------------
// dpiStmt__close() [INTERNAL]
//   Internal method used for closing the statement. If the statement is marked
// as needing to be dropped from the statement cache that is done as well. This
// is called from dpiStmt_close() where errors are expected to be propagated
// and from dpiStmt__free() where errors are ignored.
//-----------------------------------------------------------------------------
static int dpiStmt__close(dpiStmt *stmt, const char *tag,
        uint32_t tagLength, int propagateErrors, dpiError *error)
{
    uint32_t mode;
    sword status;

    dpiStmt__clearBatchErrors(stmt, error);
    dpiStmt__clearBindVars(stmt, error);
    dpiStmt__clearQueryVars(stmt, error);
    if (stmt->handle) {
        if (stmt->isOwned)
            status = OCIHandleFree(stmt->handle, OCI_HTYPE_STMT);
        else {
            mode = (stmt->deleteFromCache) ? OCI_STRLS_CACHE_DELETE :
                    OCI_DEFAULT;
            status = OCIStmtRelease(stmt->handle, error->handle, (text*) tag,
                tagLength, mode);
        }
        stmt->handle = NULL;
        if (propagateErrors && dpiError__check(error, status, stmt->conn,
                "close statement") < 0)
            return DPI_FAILURE;
    }
    if (stmt->conn) {
        dpiGen__setRefCount(stmt->conn, error, -1);
        stmt->conn = NULL;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__createBindVar() [INTERNAL]
//   Create a bind variable given a value to bind.
//-----------------------------------------------------------------------------
static int dpiStmt__createBindVar(dpiStmt *stmt,
        dpiNativeTypeNum nativeTypeNum, dpiData *data, dpiVar **var,
        uint32_t pos, const char *name, uint32_t nameLength, dpiError *error)
{
    dpiOracleTypeNum oracleTypeNum;
    dpiObjectType *objType;
    dpiData *varData;
    dpiVar *tempVar;
    uint32_t size;

    // determine the type (and size) of bind variable to create
    size = 0;
    objType = NULL;
    switch (nativeTypeNum) {
        case DPI_NATIVE_TYPE_INT64:
        case DPI_NATIVE_TYPE_UINT64:
        case DPI_NATIVE_TYPE_FLOAT:
        case DPI_NATIVE_TYPE_DOUBLE:
            oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
            break;
        case DPI_NATIVE_TYPE_BYTES:
            oracleTypeNum = DPI_ORACLE_TYPE_VARCHAR;
            size = data->value.asBytes.length;
            break;
        case DPI_NATIVE_TYPE_TIMESTAMP:
            oracleTypeNum = DPI_ORACLE_TYPE_TIMESTAMP;
            break;
        case DPI_NATIVE_TYPE_INTERVAL_DS:
            oracleTypeNum = DPI_ORACLE_TYPE_INTERVAL_DS;
            break;
        case DPI_NATIVE_TYPE_INTERVAL_YM:
            oracleTypeNum = DPI_ORACLE_TYPE_INTERVAL_YM;
            break;
        case DPI_NATIVE_TYPE_OBJECT:
            oracleTypeNum = DPI_ORACLE_TYPE_OBJECT;
            if (data->value.asObject)
                objType = data->value.asObject->type;
            break;
        case DPI_NATIVE_TYPE_BOOLEAN:
            oracleTypeNum = DPI_ORACLE_TYPE_BOOLEAN;
            break;
        default:
            return dpiError__set(error, "create bind var",
                    DPI_ERR_UNHANDLED_CONVERSION, 0, nativeTypeNum);
    }

    // create the variable and set its value
    if (dpiVar__allocate(stmt->conn, oracleTypeNum, nativeTypeNum, 1, size, 1,
            0, objType, &tempVar, &varData, error) < 0)
        return DPI_FAILURE;

    // copy value from source to target data
    if (dpiVar__copyData(tempVar, 0, data, error) < 0)
        return DPI_FAILURE;

    // bind variable to statement
    if (dpiStmt__bind(stmt, tempVar, 0, pos, name, nameLength, error) < 0) {
        dpiVar__free(tempVar, error);
        return DPI_FAILURE;
    }

    *var = tempVar;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__createQueryVars() [INTERNAL]
//   Create space for the number of query variables required to support the
// query.
//-----------------------------------------------------------------------------
static int dpiStmt__createQueryVars(dpiStmt *stmt, dpiError *error)
{
    uint32_t numQueryVars, i;
    sword status;

    // determine number of query variables
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, (dvoid*) &numQueryVars,
            0, OCI_ATTR_PARAM_COUNT, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get parameter count") < 0)
        return DPI_FAILURE;

    // clear the previous query vars if the number has changed
    if (stmt->numQueryVars > 0 && stmt->numQueryVars != numQueryVars)
        dpiStmt__clearQueryVars(stmt, error);

    // allocate space for the query vars, if needed
    if (numQueryVars != stmt->numQueryVars) {
        stmt->queryVars = calloc(numQueryVars, sizeof(dpiVar*));
        if (!stmt->queryVars)
            return dpiError__set(error, "allocate query vars",
                    DPI_ERR_NO_MEMORY);
        stmt->queryInfo = calloc(numQueryVars, sizeof(dpiQueryInfo));
        if (!stmt->queryInfo) {
            dpiStmt__clearQueryVars(stmt, error);
            return dpiError__set(error, "allocate query info",
                    DPI_ERR_NO_MEMORY);
        }
        stmt->numQueryVars = numQueryVars;
        for (i = 0; i < numQueryVars; i++) {
            if (dpiStmt__getQueryInfo(stmt, i + 1, &stmt->queryInfo[i],
                    error) < 0) {
                dpiStmt__clearQueryVars(stmt, error);
                return DPI_FAILURE;
            }
        }
    }

    // indicate start of fetch
    stmt->bufferRowIndex = stmt->fetchArraySize;
    stmt->hasRowsToFetch = 1;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__define() [INTERNAL]
//   Define the variable that will accept output from the statement in the
// specified column. At this point the statement, position and variable are all
// assumed to be valid.
//-----------------------------------------------------------------------------
int dpiStmt__define(dpiStmt *stmt, uint32_t pos, dpiVar *var, dpiError *error)
{
    OCIDefine *defineHandle = NULL;
    sword status;

    // no need to perform define if variable is unchanged
    if (stmt->queryVars[pos - 1] == var)
        return DPI_SUCCESS;

    // perform the define
    status = OCIDEFINEBYPOS(stmt->handle, &defineHandle,
            error->handle, pos, (var->isDynamic) ? NULL : var->data.asRaw,
            (var->isDynamic) ? SB4MAXVAL : var->sizeInBytes,
            var->type->oracleType, (var->isDynamic) ? NULL : var->indicator,
            (var->isDynamic) ? NULL : var->actualLength,
            (var->isDynamic) ? NULL : var->returnCode,
            (var->isDynamic) ? OCI_DYNAMIC_FETCH : OCI_DEFAULT);
    if (dpiError__check(error, status, stmt->conn, "define") < 0)
        return DPI_FAILURE;

    // set the charset form if applicable
    if (var->type->charsetForm != SQLCS_IMPLICIT) {
        status = OCIAttrSet(defineHandle, OCI_HTYPE_DEFINE,
                (dvoid*) &var->type->charsetForm, 0, OCI_ATTR_CHARSET_FORM,
                error->handle);
        if (dpiError__check(error, status, stmt->conn,
                "set charset form") < 0)
            return DPI_FAILURE;
    }

    // define objects, if applicable
    if (var->objectIndicator) {
        status = OCIDefineObject(defineHandle, error->handle,
                var->objectType->tdo, var->data.asRaw, 0, var->objectIndicator,
                0);
        if (dpiError__check(error, status, stmt->conn, "define object") < 0)
            return DPI_FAILURE;
    }

    // register callback for dynamic defines
    if (var->isDynamic) {
        status = OCIDefineDynamic(defineHandle, error->handle, var,
                (OCICallbackDefine) dpiVar__defineCallback);
        if (dpiError__check(error, status, stmt->conn, "define dynamic") < 0)
            return DPI_FAILURE;
    }

    // remove previous variable and retain new one
    if (stmt->queryVars[pos - 1]) {
        if (dpiGen__setRefCount(stmt->queryVars[pos - 1], error, -1) < 0)
            return DPI_FAILURE;
        stmt->queryVars[pos - 1] = NULL;
    }
    if (dpiGen__setRefCount(var, error, 1) < 0)
        return DPI_FAILURE;
    stmt->queryVars[pos - 1] = var;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__execute() [INTERNAL]
//   Internal execution of statement.
//-----------------------------------------------------------------------------
static int dpiStmt__execute(dpiStmt *stmt, uint32_t numIters,
        uint32_t mode, int reExecute, dpiError *error)
{
    uint32_t prefetchSize, i, j;
    sword status;
    dpiVar *var;

    // for all bound variables, transfer data from dpiData structure to Oracle
    // buffer structures
    for (i = 0; i < stmt->numBindVars; i++) {
        var = stmt->bindVars[i].var;
        for (j = 0; j < var->maxArraySize; j++) {
            if (dpiVar__setValue(var, j, &var->externalData[j], error) < 0)
                return DPI_FAILURE;
        }
        if (stmt->isReturning || var->isDynamic)
            var->error = error;
    }

    // for queries, set the prefetch rows to the fetch array size in order to
    // avoid the network round trip for the first fetch
    if (stmt->statementType == OCI_STMT_SELECT) {
        status = OCIAttrSet(stmt->handle, OCI_HTYPE_STMT,
                &stmt->fetchArraySize, sizeof(stmt->fetchArraySize),
                OCI_ATTR_PREFETCH_ROWS, error->handle);
        if (dpiError__check(error, status, stmt->conn,
                "set prefetch rows") < 0)
            return DPI_FAILURE;
    }

    // clear batch errors from any previous execution
    dpiStmt__clearBatchErrors(stmt, error);

    // adjust mode for scrollable cursors
    if (stmt->scrollable)
        mode |= OCI_STMT_SCROLLABLE_READONLY;

    // perform execution
    // re-execute statement for ORA-01007: variable not in select list
    // drop statement from cache for all but ORA-00001: unique key violated
    status = OCIStmtExecute(stmt->conn->handle, stmt->handle, error->handle,
            numIters, 0, 0, 0, mode);
    if (dpiError__check(error, status, stmt->conn, "execute") < 0) {
        OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, &error->buffer->offset, 0,
                OCI_ATTR_PARSE_ERROR_OFFSET, error->handle);
        if (reExecute && error->buffer->code == 1007)
            return dpiStmt__reExecute(stmt, numIters, mode, error);
        else if (error->buffer->code != 1)
            stmt->deleteFromCache = 1;
        return DPI_FAILURE;
    }

    // for all bound variables, transfer data from Oracle buffer structures to
    // dpiData structures; OCI doesn't provide a way of knowing if a variable
    // is an out variable so do this for all of them when this is a possibility
    if (stmt->isReturning || stmt->statementType == OCI_STMT_BEGIN ||
            stmt->statementType == OCI_STMT_DECLARE ||
            stmt->statementType == OCI_STMT_CALL) {
        for (i = 0; i < stmt->numBindVars; i++) {
            var = stmt->bindVars[i].var;
            for (j = 0; j < var->maxArraySize; j++) {
                if (dpiVar__getValue(var, j, &var->externalData[j],
                        error) < 0)
                    return DPI_FAILURE;
            }
            var->error = NULL;
        }
    }

    // determine number of query columns (for queries)
    // reset prefetch rows to 0 as subsequent fetches can fetch directly into
    // the defined fetch areas
    if (stmt->statementType == OCI_STMT_SELECT) {
        if (dpiStmt__createQueryVars(stmt, error) < 0)
            return DPI_FAILURE;
        prefetchSize = 0;
        status = OCIAttrSet(stmt->handle, OCI_HTYPE_STMT, &prefetchSize,
                sizeof(prefetchSize), OCI_ATTR_PREFETCH_ROWS,
                error->handle);
        if (dpiError__check(error, status, stmt->conn,
                "reset prefetch rows") < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__fetch() [INTERNAL]
//   Performs the actual fetch from Oracle.
//-----------------------------------------------------------------------------
static int dpiStmt__fetch(dpiStmt *stmt, dpiError *error)
{
    sword status;

    // perform any pre-fetch activities required
    if (dpiStmt__preFetch(stmt, error) < 0)
        return DPI_FAILURE;

    // perform fetch
    status = OCIStmtFetch2(stmt->handle, error->handle,
            stmt->fetchArraySize, OCI_FETCH_NEXT, 0, OCI_DEFAULT);
    if (status == OCI_NO_DATA)
        stmt->hasRowsToFetch = 0;
    else if (dpiError__check(error, status, stmt->conn, "fetch") < 0)
        return DPI_FAILURE;

    // determine the number of rows fetched into buffers
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, &stmt->bufferRowCount, 0,
            OCI_ATTR_ROWS_FETCHED, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get rows fetched") < 0)
        return DPI_FAILURE;

    // set buffer row info
    stmt->bufferMinRow = stmt->rowCount + 1;
    stmt->bufferRowIndex = 0;

    // perform post-fetch activities required
    if (dpiStmt__postFetch(stmt, error) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__free() [INTERNAL]
//   Free the memory associated with the statement.
//-----------------------------------------------------------------------------
void dpiStmt__free(dpiStmt *stmt, dpiError *error)
{
    dpiStmt__close(stmt, NULL, 0, 0, error);
    free(stmt);
}


//-----------------------------------------------------------------------------
// dpiStmt__getBatchErrors() [INTERNAL]
//   Get batch errors after statement executed with batch errors enabled.
//-----------------------------------------------------------------------------
static int dpiStmt__getBatchErrors(dpiStmt *stmt, dpiError *error)
{
    OCIError *batchErrorHandle, *localErrorHandle;
    dpiError localError;
    int overallStatus;
    int32_t rowOffset;
    sword status;
    uint32_t i;

    // determine the number of batch errors that were found
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, &stmt->numBatchErrors, 0,
            OCI_ATTR_NUM_DML_ERRORS, error->handle);
    if (dpiError__check(error, status, stmt->conn,
            "get batch error count") < 0)
        return DPI_FAILURE;

    // allocate memory for the batch errors
    stmt->batchErrors = calloc(stmt->numBatchErrors, sizeof(dpiErrorBuffer));
    if (!stmt->batchErrors) {
        stmt->numBatchErrors = 0;
        return dpiError__set(error, "allocate errors", DPI_ERR_NO_MEMORY);
    }

    // allocate error handle used for OCIParamGet()
    status = OCIHandleAlloc(stmt->env->handle, (dvoid**) &localErrorHandle,
            OCI_HTYPE_ERROR, 0, NULL);
    if (status != OCI_SUCCESS) {
        dpiStmt__clearBatchErrors(stmt, error);
        return dpiError__set(error, "allocate parameter error handle",
                DPI_ERR_NO_MEMORY);
    }

    // allocate error handle used for batch errors
    status = OCIHandleAlloc(stmt->env->handle, (dvoid**) &batchErrorHandle,
            OCI_HTYPE_ERROR, 0, NULL);
    if (status != OCI_SUCCESS) {
        dpiStmt__clearBatchErrors(stmt, error);
        OCIHandleFree(localErrorHandle, OCI_HTYPE_ERROR);
        return dpiError__set(error, "allocate batch error handle",
                DPI_ERR_NO_MEMORY);
    }

    // process each error
    overallStatus = DPI_SUCCESS;
    localError.encoding = error->encoding;
    localError.charsetId = error->charsetId;
    for (i = 0; i < stmt->numBatchErrors; i++) {

        // get error handle for iteration
        status = OCIParamGet(error->handle, OCI_HTYPE_ERROR,
                localErrorHandle, (void **) &batchErrorHandle, i);
        if (status != OCI_SUCCESS) {
            overallStatus = dpiError__set(error, "get batch error",
                    DPI_ERR_INVALID_INDEX, i);
            break;
        }

        // determine row offset
        status = OCIAttrGet(batchErrorHandle, OCI_HTYPE_ERROR, &rowOffset, 0,
                OCI_ATTR_DML_ROW_OFFSET, localErrorHandle);
        if (status != OCI_SUCCESS) {
            overallStatus = dpiError__set(error, "get row offset",
                    DPI_ERR_CANNOT_GET_ROW_OFFSET);
            break;
        }

        // get error message
        localError.buffer = &stmt->batchErrors[i];
        localError.handle = batchErrorHandle;
        dpiError__check(&localError, OCI_ERROR, stmt->conn, "get batch error");
        if (error->buffer->dpiErrorNum) {
            overallStatus = DPI_FAILURE;
            break;
        }
        localError.buffer->fnName = error->buffer->fnName;
        localError.buffer->offset = rowOffset;

    }

    // cleanup
    OCIHandleFree(localErrorHandle, OCI_HTYPE_ERROR);
    OCIHandleFree(batchErrorHandle, OCI_HTYPE_ERROR);
    if (overallStatus < 0)
        dpiStmt__clearBatchErrors(stmt, error);
    return overallStatus;
}


//-----------------------------------------------------------------------------
// dpiStmt__getQueryInfo() [INTERNAL]
//   Get query information for the position in question.
//-----------------------------------------------------------------------------
static int dpiStmt__getQueryInfo(dpiStmt *stmt, uint32_t pos,
        dpiQueryInfo *info, dpiError *error)
{
    OCIParam *param;
    sword status;

    // acquire parameter descriptor
    status = OCIParamGet(stmt->handle, OCI_HTYPE_STMT, error->handle,
            (void**) &param, pos);
    if (dpiError__check(error, status, stmt->conn, "get parameter") < 0)
        return DPI_FAILURE;

    // acquire information from the parameter descriptor
    status = dpiStmt__getQueryInfoFromParam(stmt, param, info, error);
    OCIDescriptorFree(param, OCI_DTYPE_PARAM);
    return status;
}


//-----------------------------------------------------------------------------
// dpiStmt__getQueryInfoFromParam() [INTERNAL]
//   Get query information from the parameter.
//-----------------------------------------------------------------------------
static int dpiStmt__getQueryInfoFromParam(dpiStmt *stmt, OCIParam *param,
        dpiQueryInfo *info, dpiError *error)
{
    const dpiOracleType *oracleType;
    uint8_t charsetForm, ociNullOk;
    uint16_t ociDataType, ociSize;
    sword status;

    // acquire data type of the parameter
    status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &ociDataType, 0,
            OCI_ATTR_DATA_TYPE, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get data type") < 0)
        return DPI_FAILURE;

    // acquire character set form of the parameter
    status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &charsetForm, 0,
            OCI_ATTR_CHARSET_FORM, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get charset form") < 0)
        return DPI_FAILURE;

    // acquire scale
    status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &info->scale, 0,
            OCI_ATTR_SCALE, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get scale") < 0)
        return DPI_FAILURE;

    // acquire precision
    status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &info->precision,
            0, OCI_ATTR_PRECISION, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get precision") < 0)
        return DPI_FAILURE;

    // determine default ODPI-C type of variable to use
    oracleType = dpiOracleType__getFromQueryInfo(ociDataType, charsetForm,
            error);
    if (!oracleType)
        return DPI_FAILURE;
    info->oracleTypeNum = oracleType->oracleTypeNum;
    info->defaultNativeTypeNum = oracleType->defaultNativeTypeNum;

    // aquire name of item
    status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &info->name,
            &info->nameLength, OCI_ATTR_NAME, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get name") < 0)
        return DPI_FAILURE;

    // acquire size (in bytes) of item
    info->sizeInChars = 0;
    if (oracleType->oracleTypeNum == DPI_ORACLE_TYPE_ROWID) {
        info->sizeInChars = oracleType->sizeInBytes;
        info->dbSizeInBytes = oracleType->sizeInBytes;
        info->clientSizeInBytes = oracleType->sizeInBytes;
    } else if (oracleType->sizeInBytes == 0) {
        status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &ociSize, 0,
                OCI_ATTR_DATA_SIZE, error->handle);
        if (dpiError__check(error, status, stmt->conn, "get size (bytes)") < 0)
            return DPI_FAILURE;
        info->dbSizeInBytes = ociSize;
        info->clientSizeInBytes = ociSize;
    } else {
        info->dbSizeInBytes = 0;
        info->clientSizeInBytes = 0;
    }

    // acquire size (in characters) of item (if applicable)
    if (oracleType->isCharacterData && oracleType->sizeInBytes == 0) {
        status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &ociSize, 0,
                OCI_ATTR_CHAR_SIZE, error->handle);
        if (dpiError__check(error, status, stmt->conn, "get size (chars)") < 0)
            return DPI_FAILURE;
        info->sizeInChars = ociSize;
        if (charsetForm == SQLCS_IMPLICIT)
            info->clientSizeInBytes = info->sizeInChars *
                    stmt->env->maxBytesPerCharacter;
        else info->clientSizeInBytes = info->sizeInChars *
                stmt->env->nmaxBytesPerCharacter;
    }

    // lookup whether null is permitted for the attribute
    status = OCIAttrGet(param, OCI_HTYPE_DESCRIBE, (dvoid*) &ociNullOk, 0,
            OCI_ATTR_IS_NULL, error->handle);
    if (dpiError__check(error, status, stmt->conn, "get null ok") < 0)
        return DPI_FAILURE;
    info->nullOk = ociNullOk;

    // determine object type, if applicable
    if (ociDataType == SQLT_NTY) {
        if (dpiObjectType__allocate(stmt->conn, param, OCI_ATTR_TYPE_NAME,
                &info->objectType, error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__init() [INTERNAL]
//   Initialize the statement for use. This is needed when preparing a
// statement for use and when returning a REF cursor.
//-----------------------------------------------------------------------------
int dpiStmt__init(dpiStmt *stmt, dpiError *error)
{
    sword status;

    // get statement type
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT,
            (dvoid*) &stmt->statementType, 0, OCI_ATTR_STMT_TYPE,
            error->handle);
    if (dpiError__check(error, status, stmt->conn, "get statement type") < 0)
        return DPI_FAILURE;

    // for queries, mark statement as having rows to fetch
    if (stmt->statementType == OCI_STMT_SELECT)
        stmt->hasRowsToFetch = 1;

    // otherwise, check if this is a RETURNING statement
    else {
        status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT,
                (dvoid*) &stmt->isReturning, 0, OCI_ATTR_STMT_IS_RETURNING,
                error->handle);
        if (dpiError__check(error, status, stmt->conn, "get is returning") < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__postFetch() [INTERNAL]
//   Performs the transformations required to convert Oracle data values into
// C data values.
//-----------------------------------------------------------------------------
static int dpiStmt__postFetch(dpiStmt *stmt, dpiError *error)
{
    uint32_t i, j;
    dpiVar *var;

    for (i = 0; i < stmt->numQueryVars; i++) {
        var = stmt->queryVars[i];
        for (j = 0; j < stmt->bufferRowCount; j++) {
            if (dpiVar__getValue(var, j, &var->externalData[j], error) < 0)
                return DPI_FAILURE;
            if (var->type->requiresPreFetch)
                var->requiresPreFetch = 1;
        }
        var->error = NULL;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__preFetch() [INTERNAL]
//   Performs work that needs to be done prior to fetch for each variable. In
// addition, variables are created if they do not already exist. A check is
// also made to ensure that the variable has enough space to support a fetch
// of the requested size.
//-----------------------------------------------------------------------------
static int dpiStmt__preFetch(dpiStmt *stmt, dpiError *error)
{
    dpiQueryInfo *queryInfo;
    dpiData *data;
    dpiVar *var;
    uint32_t i;

    if (!stmt->queryInfo && dpiStmt__createQueryVars(stmt, error) < 0)
        return DPI_FAILURE;
    for (i = 0; i < stmt->numQueryVars; i++) {
        var = stmt->queryVars[i];
        if (!var) {
            queryInfo = &stmt->queryInfo[i];
            if (dpiVar__allocate(stmt->conn, queryInfo->oracleTypeNum,
                    queryInfo->defaultNativeTypeNum, stmt->fetchArraySize,
                    queryInfo->clientSizeInBytes, 1, 0, queryInfo->objectType,
                    &var, &data, error) < 0)
                return DPI_FAILURE;
            if (dpiStmt__define(stmt, i + 1, var, error) < 0)
                return DPI_FAILURE;
            dpiGen__setRefCount(var, error, -1);
        }
        var->error = error;
        if (stmt->fetchArraySize > var->maxArraySize)
            return dpiError__set(error, "check array size",
                    DPI_ERR_ARRAY_SIZE_TOO_SMALL, var->maxArraySize);
        if (var->requiresPreFetch && dpiVar__extendedPreFetch(var, error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt__prepare() [INTERNAL]
//   Prepare a statement for execution.
//-----------------------------------------------------------------------------
int dpiStmt__prepare(dpiStmt *stmt, const char *sql, uint32_t sqlLength,
        const char *tag, uint32_t tagLength, dpiError *error)
{
    sword status;

    // prepare SQL
    status = OCIStmtPrepare2(stmt->conn->handle, &stmt->handle, error->handle,
            (text*) sql, sqlLength, (text*) tag, tagLength, OCI_NTV_SYNTAX,
            OCI_DEFAULT);
    if (dpiError__check(error, status, stmt->conn, "prepare SQL") < 0) {
        stmt->handle = NULL;
        return DPI_FAILURE;
    }

    // initialization
    return dpiStmt__init(stmt, error);
}


//-----------------------------------------------------------------------------
// dpiStmt__reExecute() [INTERNAL]
//   Re-execute the statement after receiving the error ORA-01007: variable not
// in select list. This takes place when one of the columns in a query is
// dropped, but the original metadata is still being used because the query
// statement was found in the statement cache.
//-----------------------------------------------------------------------------
static int dpiStmt__reExecute(dpiStmt *stmt, uint32_t numIters,
        dpiExecMode mode, dpiError *error)
{
    uint32_t sqlLength, i;
    dpiBindVar *bindVar;
    OCIStmt *handle;
    int dpiStatus;
    sword status;
    dpiVar *var;
    char *sql;

    // acquire the statement that was previously prepared; if this cannot be
    // determined, let the original error propagate
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, &sql, &sqlLength,
            OCI_ATTR_STATEMENT, error->handle);
    if (status != OCI_SUCCESS)
        return DPI_FAILURE;

    // prepare statement a second time before releasing the original statement;
    // release the original statement and delete it from the statement cache
    // so that it does not return with the invalid metadata; again, if this
    // cannot be done, let the original error propagate
    handle = stmt->handle;
    dpiStatus = dpiStmt__prepare(stmt, sql, sqlLength, NULL, 0, error);
    status = OCIStmtRelease(handle, error->handle, NULL, 0,
            OCI_STRLS_CACHE_DELETE);
    if (status != OCI_SUCCESS || dpiStatus < 0)
        return DPI_FAILURE;
    dpiStmt__clearBatchErrors(stmt, error);
    dpiStmt__clearQueryVars(stmt, error);

    // perform binds
    for (i = 0; i < stmt->numBindVars; i++) {
        bindVar = &stmt->bindVars[i];
        if (!bindVar->var)
            continue;
        var = bindVar->var;
        bindVar->var = NULL;
        if (dpiStmt__bind(stmt, var, 0, bindVar->pos, bindVar->name,
                bindVar->nameLength, error) < 0) {
            dpiGen__setRefCount(var, error, -1);
            return DPI_FAILURE;
        }
    }

    // now re-execute the statement
    return dpiStmt__execute(stmt, numIters, mode, 0, error);
}


//-----------------------------------------------------------------------------
// dpiStmt_addRef() [PUBLIC]
//   Add a reference to the statement.
//-----------------------------------------------------------------------------
int dpiStmt_addRef(dpiStmt *stmt)
{
    return dpiGen__addRef(stmt, DPI_HTYPE_STMT, __func__);
}


//-----------------------------------------------------------------------------
// dpiStmt_bindByName() [PUBLIC]
//   Bind the variable by name.
//-----------------------------------------------------------------------------
int dpiStmt_bindByName(dpiStmt *stmt, const char *name, uint32_t nameLength,
        dpiVar *var)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiStmt__bind(stmt, var, 1, 0, name, nameLength, &error);
}


//-----------------------------------------------------------------------------
// dpiStmt_bindByPos() [PUBLIC]
//   Bind the variable by position.
//-----------------------------------------------------------------------------
int dpiStmt_bindByPos(dpiStmt *stmt, uint32_t pos, dpiVar *var)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiStmt__bind(stmt, var, 1, pos, NULL, 0, &error);
}


//-----------------------------------------------------------------------------
// dpiStmt_bindValueByName() [PUBLIC]
//   Create a variable and bind it by name.
//-----------------------------------------------------------------------------
int dpiStmt_bindValueByName(dpiStmt *stmt, const char *name,
        uint32_t nameLength, dpiNativeTypeNum nativeTypeNum, dpiData *data)
{
    dpiVar *var = NULL;
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (dpiStmt__createBindVar(stmt, nativeTypeNum, data, &var, 0, name,
            nameLength, &error) < 0)
        return DPI_FAILURE;
    return dpiStmt_bindByName(stmt, name, nameLength, var);
}


//-----------------------------------------------------------------------------
// dpiStmt_bindValueByPos() [PUBLIC]
//   Create a variable and bind it by position.
//-----------------------------------------------------------------------------
int dpiStmt_bindValueByPos(dpiStmt *stmt, uint32_t pos,
        dpiNativeTypeNum nativeTypeNum, dpiData *data)
{
    dpiVar *var = NULL;
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (dpiStmt__createBindVar(stmt, nativeTypeNum, data, &var, pos, NULL, 0,
            &error) < 0)
        return DPI_FAILURE;
    return dpiStmt_bindByPos(stmt, pos, var);
}


//-----------------------------------------------------------------------------
// dpiStmt_close() [PUBLIC]
//   Close the statement so that it is no longer usable and all resources have
// been released.
//-----------------------------------------------------------------------------
int dpiStmt_close(dpiStmt *stmt, const char *tag, uint32_t tagLength)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiStmt__close(stmt, tag, tagLength, 1, &error);
}


//-----------------------------------------------------------------------------
// dpiStmt_define() [PUBLIC]
//   Define the variable that will accept output from the cursor in the
// specified column.
//-----------------------------------------------------------------------------
int dpiStmt_define(dpiStmt *stmt, uint32_t pos, dpiVar *var)
{
    dpiError error;

    // verify parameters
    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!stmt->queryInfo && dpiStmt__createQueryVars(stmt, &error) < 0)
        return DPI_FAILURE;
    if (pos == 0 || pos > stmt->numQueryVars)
        return dpiError__set(&error, "check query position",
                DPI_ERR_QUERY_POSITION_INVALID);
    if (dpiGen__checkHandle(var, DPI_HTYPE_VAR, "check variable", &error) < 0)
        return DPI_FAILURE;

    return dpiStmt__define(stmt, pos, var, &error);
}


//-----------------------------------------------------------------------------
// dpiStmt_execute() [PUBLIC]
//   Execute a statement. If the statement has been executed before, however,
// and this is a query, the describe information is already available so defer
// execution until the first fetch.
//-----------------------------------------------------------------------------
int dpiStmt_execute(dpiStmt *stmt, dpiExecMode mode, uint32_t *numQueryColumns)
{
    uint32_t numIters;
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    numIters = (stmt->statementType == OCI_STMT_SELECT) ? 0 : 1;
    if (dpiStmt__execute(stmt, numIters, mode, 1, &error) < 0)
        return DPI_FAILURE;
    *numQueryColumns = stmt->numQueryVars;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_executeMany() [PUBLIC]
//   Execute a statement multiple times. Queries are not supported. The bind
// variables are checked to ensure that their maxArraySize is sufficient to
// support this.
//-----------------------------------------------------------------------------
int dpiStmt_executeMany(dpiStmt *stmt, dpiExecMode mode, uint32_t numIters)
{
    dpiError error;
    uint32_t i;

    // verify statement is open
    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;

    // queries are not supported
    if (stmt->statementType == OCI_STMT_SELECT)
        return dpiError__set(&error, "check statement type",
                DPI_ERR_NOT_SUPPORTED);

    // ensure that all bind variables have a big enough maxArraySize to
    // support this operation
    for (i = 0; i < stmt->numBindVars; i++) {
        if (stmt->bindVars[i].var->maxArraySize < numIters)
            return dpiError__set(&error, "check array size",
                    DPI_ERR_ARRAY_SIZE_TOO_SMALL,
                    stmt->bindVars[i].var->maxArraySize);
    }

    // perform execution
    dpiStmt__clearBatchErrors(stmt, &error);
    if (dpiStmt__execute(stmt, numIters, mode, 0, &error) < 0)
        return DPI_FAILURE;

    // handle batch errors if mode was specified
    if (mode & DPI_MODE_EXEC_BATCH_ERRORS) {
        if (dpiStmt__getBatchErrors(stmt, &error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_fetch() [PUBLIC]
//   Fetch a row from the database.
//-----------------------------------------------------------------------------
int dpiStmt_fetch(dpiStmt *stmt, int *found, uint32_t *bufferRowIndex)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (stmt->bufferRowIndex >= stmt->bufferRowCount) {
        if (stmt->hasRowsToFetch && dpiStmt__fetch(stmt, &error) < 0)
            return DPI_FAILURE;
        if (stmt->bufferRowIndex >= stmt->bufferRowCount) {
            *found = 0;
            return DPI_SUCCESS;
        }
    }
    *found = 1;
    *bufferRowIndex = stmt->bufferRowIndex;
    stmt->bufferRowIndex++;
    stmt->rowCount++;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_fetchRows() [PUBLIC]
//   Fetch rows into buffers and return the number of rows that were so
// fetched. If there are still rows available in the buffer, no additional
// fetch will take place.
//-----------------------------------------------------------------------------
int dpiStmt_fetchRows(dpiStmt *stmt, uint32_t maxRows,
        uint32_t *bufferRowIndex, uint32_t *numRowsFetched, int *moreRows)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (stmt->bufferRowIndex >= stmt->bufferRowCount) {
        if (stmt->hasRowsToFetch && dpiStmt__fetch(stmt, &error) < 0)
            return DPI_FAILURE;
        if (stmt->bufferRowIndex >= stmt->bufferRowCount) {
            *moreRows = 0;
            *bufferRowIndex = 0;
            *numRowsFetched = 0;
            return DPI_SUCCESS;
        }
    }
    *bufferRowIndex = stmt->bufferRowIndex;
    *numRowsFetched = stmt->bufferRowCount - stmt->bufferRowIndex;
    *moreRows = stmt->hasRowsToFetch;
    if (*numRowsFetched > maxRows) {
        *numRowsFetched = maxRows;
        *moreRows = 1;
    }
    stmt->bufferRowIndex += *numRowsFetched;
    stmt->rowCount += *numRowsFetched;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getBatchErrorCount() [PUBLIC]
//   Return the number of batch errors that took place during the last
// execution of the statement.
//-----------------------------------------------------------------------------
int dpiStmt_getBatchErrorCount(dpiStmt *stmt, uint32_t *count)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    *count = stmt->numBatchErrors;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getBatchErrors() [PUBLIC]
//   Return the batch errors that took place during the last execution of the
// statement.
//-----------------------------------------------------------------------------
int dpiStmt_getBatchErrors(dpiStmt *stmt, uint32_t numErrors,
        dpiErrorInfo *errors)
{
    dpiError error, tempError;
    uint32_t i;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (numErrors < stmt->numBatchErrors)
        return dpiError__set(&error, "check num errors",
                DPI_ERR_ARRAY_SIZE_TOO_SMALL, numErrors);
    for (i = 0; i < stmt->numBatchErrors; i++) {
        tempError.buffer = &stmt->batchErrors[i];
        dpiError__getInfo(&tempError, &errors[i]);
    }
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getBindCount() [PUBLIC]
//   Return the number of bind variables referenced in the prepared SQL.
//-----------------------------------------------------------------------------
int dpiStmt_getBindCount(dpiStmt *stmt, uint32_t *count)
{
    dpiError error;
    sword status;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, (dvoid*) count, 0,
            OCI_ATTR_BIND_COUNT, error.handle);
    return dpiError__check(&error, status, stmt->conn, "get bind count");
}


//-----------------------------------------------------------------------------
// dpiStmt_getBindNames() [PUBLIC]
//   Return the names of the bind variables referenced in the prepared SQL.
//-----------------------------------------------------------------------------
int dpiStmt_getBindNames(dpiStmt *stmt, uint32_t numBindNames,
        const char **bindNames, uint32_t *bindNameLengths)
{
    ub1 bindNameLengthsBuffer[8], indNameLengthsBuffer[8], isDuplicate[8];
    char *bindNamesBuffer[8], *indNamesBuffer[8];
    uint32_t startLoc, i, pos, numThisPass;
    OCIBind *bindHandles[8];
    int32_t numFound;
    dpiError error;
    sword status;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    pos = 0;
    startLoc = 1;
    while (pos < numBindNames) {
        status = OCIStmtGetBindInfo(stmt->handle, error.handle, 8, startLoc,
                &numFound, (text**) bindNamesBuffer, bindNameLengthsBuffer,
                (text**) indNamesBuffer, indNameLengthsBuffer,
                isDuplicate, bindHandles);
        if (status == OCI_NO_DATA)
            break;
        if (dpiError__check(&error, status, stmt->conn, "get bind info") < 0)
            return DPI_FAILURE;
        numThisPass = abs(numFound) - startLoc + 1;
        if (numThisPass > 8)
            numThisPass = 8;
        for (i = 0; i < numThisPass && pos < numBindNames; i++) {
            startLoc++;
            if (isDuplicate[i])
                continue;
            bindNames[pos] = bindNamesBuffer[i];
            bindNameLengths[pos] = bindNameLengthsBuffer[i];
            pos++;
        }
        if (numFound > 0)
            break;
    }
    if (pos < numBindNames)
        return dpiError__set(&error, "check num bind names",
                DPI_ERR_ARRAY_SIZE_TOO_SMALL, numBindNames);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getFetchArraySize() [PUBLIC]
//   Get the array size used for fetches.
//-----------------------------------------------------------------------------
int dpiStmt_getFetchArraySize(dpiStmt *stmt, uint32_t *arraySize)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    *arraySize = stmt->fetchArraySize;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getImplicitResult() [PUBLIC]
//   Return the next implicit result from the previously executed statement. If
// no more implicit results exist, NULL is returned.
//-----------------------------------------------------------------------------
int dpiStmt_getImplicitResult(dpiStmt *stmt, dpiStmt **implicitResult)
{
    dpiError error;
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12,1)
    uint32_t returnType;
    dpiStmt *tempStmt;
    OCIStmt *handle;
    sword status;
#endif

    *implicitResult = NULL;
    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12,1)
    status = OCIStmtGetNextResult(stmt->handle, error.handle,
            (dvoid**) &handle, &returnType, OCI_DEFAULT);
    if (status == OCI_NO_DATA)
        return DPI_SUCCESS;
    if (dpiError__check(&error, status, stmt->conn, "get next result") < 0)
        return DPI_FAILURE;
    if (dpiStmt__allocate(stmt->conn, 0, &tempStmt, &error) < 0)
        return DPI_FAILURE;
    tempStmt->handle = handle;
    if (dpiStmt__createQueryVars(tempStmt, &error) < 0) {
        dpiStmt__free(tempStmt, &error);
        return DPI_FAILURE;
    }
    *implicitResult = tempStmt;
    return DPI_SUCCESS;
#else
    return dpiError__set(&error, "wrong Oracle client", DPI_ERR_NOT_SUPPORTED);
#endif
}


//-----------------------------------------------------------------------------
// dpiStmt_getInfo() [PUBLIC]
//   Return information about the statement in the provided structure.
//-----------------------------------------------------------------------------
int dpiStmt_getInfo(dpiStmt *stmt, dpiStmtInfo *info)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    info->isQuery = (stmt->statementType == OCI_STMT_SELECT);
    info->isPLSQL = (stmt->statementType == OCI_STMT_BEGIN ||
            stmt->statementType == OCI_STMT_DECLARE ||
            stmt->statementType == OCI_STMT_CALL);
    info->isDDL = (stmt->statementType == OCI_STMT_CREATE ||
            stmt->statementType == OCI_STMT_DROP ||
            stmt->statementType == OCI_STMT_ALTER);
    info->isDML = (stmt->statementType == OCI_STMT_INSERT ||
            stmt->statementType == OCI_STMT_UPDATE ||
            stmt->statementType == OCI_STMT_DELETE);
    info->statementType = stmt->statementType;
    info->isReturning = stmt->isReturning;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getNumQueryColumns() [PUBLIC]
//   Returns the number of query columns associated with a statement. If the
// statement does not refer to a query, 0 is returned.
//-----------------------------------------------------------------------------
int dpiStmt_getNumQueryColumns(dpiStmt *stmt, uint32_t *numQueryColumns)
{
    dpiError error;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (stmt->statementType == OCI_STMT_SELECT && stmt->numQueryVars == 0 &&
            dpiStmt__createQueryVars(stmt, &error) < 0)
        return DPI_FAILURE;
    *numQueryColumns = stmt->numQueryVars;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getQueryInfo() [PUBLIC]
//   Get query information for the position in question.
//-----------------------------------------------------------------------------
int dpiStmt_getQueryInfo(dpiStmt *stmt, uint32_t pos, dpiQueryInfo *info)
{
    dpiError error;

    // verify statement is open
    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;

    // ensure that query information has been retrieved
    if (!stmt->queryInfo && dpiStmt__createQueryVars(stmt, &error) < 0)
        return DPI_FAILURE;

    // validate query position
    if (pos == 0 || pos > stmt->numQueryVars)
        return dpiError__set(&error, "check query position",
                DPI_ERR_QUERY_POSITION_INVALID);

    // copy query information from internal cache
    memcpy(info, &stmt->queryInfo[pos - 1], sizeof(dpiQueryInfo));
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getQueryValue() [PUBLIC]
//   Get value from query at specified position.
//-----------------------------------------------------------------------------
int dpiStmt_getQueryValue(dpiStmt *stmt, uint32_t pos,
        dpiNativeTypeNum *nativeTypeNum, dpiData **data)
{
    dpiError error;
    dpiVar *var;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!stmt->queryVars)
        return dpiError__set(&error, "check query vars",
                DPI_ERR_QUERY_NOT_EXECUTED);
    if (pos == 0 || pos > stmt->numQueryVars)
        return dpiError__set(&error, "check query position",
                DPI_ERR_QUERY_POSITION_INVALID, pos);
    var = stmt->queryVars[pos - 1];
    if (!var || stmt->bufferRowIndex == 0 ||
            stmt->bufferRowIndex > stmt->bufferRowCount)
        return dpiError__set(&error, "check fetched row",
                DPI_ERR_NO_ROW_FETCHED);
    *nativeTypeNum = var->nativeTypeNum;
    *data = &var->externalData[stmt->bufferRowIndex - 1];
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getRowCount() [PUBLIC]
//   Return the number of rows affected by the last SQL executed (for insert,
// update or delete) or the number of rows fetched (for queries).
//-----------------------------------------------------------------------------
int dpiStmt_getRowCount(dpiStmt *stmt, uint64_t *count)
{
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12,1)
    uint64_t ociCount;
    uint32_t attribute = OCI_ATTR_UB8_ROW_COUNT;
#else
    uint32_t ociCount;
    uint32_t attribute = OCI_ATTR_ROW_COUNT;
#endif
    dpiError error;
    sword status;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (stmt->statementType == OCI_STMT_SELECT)
        *count = stmt->rowCount;
    else {
        status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, &ociCount, 0,
                attribute, error.handle);
        if (dpiError__check(&error, status, stmt->conn, "get row count") < 0)
            return DPI_FAILURE;
        *count = ociCount;
    }
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_getRowCounts() [PUBLIC]
//   Return the number of rows affected by each of the iterations executed
// using dpiStmt_executeMany().
//-----------------------------------------------------------------------------
int dpiStmt_getRowCounts(dpiStmt *stmt, uint32_t *numRowCounts,
        uint64_t **rowCounts)
{
    dpiError error;
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12,1)
    sword status;
#endif

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12,1)
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, rowCounts, numRowCounts,
            OCI_ATTR_DML_ROW_COUNT_ARRAY, error.handle);
    return dpiError__check(&error, status, stmt->conn, "get row counts");
#else
    return dpiError__set(&error, "invalid Oracle client",
            DPI_ERR_NOT_SUPPORTED);
#endif
}


//-----------------------------------------------------------------------------
// dpiStmt_getSubscrQueryId() [PUBLIC]
//   Return the query id for a query registered using this statement.
//-----------------------------------------------------------------------------
int dpiStmt_getSubscrQueryId(dpiStmt *stmt, uint64_t *queryId)
{
    dpiError error;
    sword status;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, queryId, 0,
            OCI_ATTR_CQ_QUERYID, error.handle);
    return dpiError__check(&error, status, stmt->conn, "get query id");

}


//-----------------------------------------------------------------------------
// dpiStmt_release() [PUBLIC]
//   Release a reference to the statement.
//-----------------------------------------------------------------------------
int dpiStmt_release(dpiStmt *stmt)
{
    return dpiGen__release(stmt, DPI_HTYPE_STMT, __func__);
}


//-----------------------------------------------------------------------------
// dpiStmt_scroll() [PUBLIC]
//   Scroll to the specified location in the cursor.
//-----------------------------------------------------------------------------
int dpiStmt_scroll(dpiStmt *stmt, dpiFetchMode mode, int32_t offset,
        int32_t rowCountOffset)
{
    uint32_t fetchMode, numRows, currentPosition;
    uint64_t desiredRow;
    dpiError error;
    sword status;

    // make sure the cursor is open
    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;

    // validate mode; determine desired row to fetch
    switch (mode) {
        case DPI_MODE_FETCH_NEXT:
            fetchMode = OCI_FETCH_NEXT;
            desiredRow = stmt->rowCount + rowCountOffset + 1;
            break;
        case DPI_MODE_FETCH_PRIOR:
            fetchMode = OCI_FETCH_PRIOR;
            desiredRow = stmt->rowCount + rowCountOffset - 1;
            break;
        case DPI_MODE_FETCH_FIRST:
            fetchMode = OCI_FETCH_FIRST;
            desiredRow = 1;
            break;
        case DPI_MODE_FETCH_LAST:
            fetchMode = OCI_FETCH_LAST;
            break;
        case DPI_MODE_FETCH_ABSOLUTE:
            fetchMode = OCI_FETCH_ABSOLUTE;
            desiredRow = offset;
            break;
        case DPI_MODE_FETCH_RELATIVE:
            fetchMode = OCI_FETCH_RELATIVE;
            desiredRow = stmt->rowCount + rowCountOffset + offset;
            offset = desiredRow -
                    (stmt->bufferMinRow + stmt->bufferRowCount - 1);
            break;
        default:
            return dpiError__set(&error, "scroll mode", DPI_ERR_NOT_SUPPORTED);
    }

    // determine if a fetch is actually required; "last" is always fetched
    if (fetchMode != OCI_FETCH_LAST && desiredRow >= stmt->bufferMinRow &&
            desiredRow < stmt->bufferMinRow + stmt->bufferRowCount) {
        stmt->bufferRowIndex = desiredRow - stmt->bufferMinRow;
        stmt->rowCount = desiredRow - 1;
        return DPI_SUCCESS;
    }

    // perform fetch; when fetching the last row, only fetch a single row
    numRows = (fetchMode == OCI_FETCH_LAST) ? 1 : stmt->fetchArraySize;
    status = OCIStmtFetch2(stmt->handle, error.handle, numRows, fetchMode,
            offset, OCI_DEFAULT);
    if (status == OCI_NO_DATA || fetchMode == OCI_FETCH_LAST)
        stmt->hasRowsToFetch = 0;
    else if (dpiError__check(&error, status, stmt->conn, "fetch") < 0)
        return DPI_FAILURE;
    else stmt->hasRowsToFetch = 1;

    // determine the number of rows actually fetched
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, &stmt->bufferRowCount, 0,
            OCI_ATTR_ROWS_FETCHED, error.handle);
    if (dpiError__check(&error, status, stmt->conn, "get rows fetched") < 0)
        return DPI_FAILURE;

    // check that we haven't gone outside of the result set
    if (stmt->bufferRowCount == 0) {
        if (fetchMode != OCI_FETCH_FIRST && fetchMode != OCI_FETCH_LAST)
            return dpiError__set(&error, "check result set bounds",
                    DPI_ERR_SCROLL_OUT_OF_RS);
        stmt->hasRowsToFetch = 0;
        stmt->rowCount = 0;
        stmt->bufferRowIndex = 0;
        stmt->bufferMinRow = 0;
        return DPI_SUCCESS;
    }

    // determine the current position of the cursor
    status = OCIAttrGet(stmt->handle, OCI_HTYPE_STMT, &currentPosition, 0,
            OCI_ATTR_CURRENT_POSITION, error.handle);
    if (dpiError__check(&error, status, stmt->conn, "get current pos") < 0)
        return DPI_FAILURE;

    // reset buffer row index and row count
    stmt->rowCount = currentPosition - stmt->bufferRowCount;
    stmt->bufferMinRow = stmt->rowCount + 1;
    stmt->bufferRowIndex = 0;

    // perform post-fetch activities required
    if (dpiStmt__postFetch(stmt, &error) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiStmt_setFetchArraySize() [PUBLIC]
//   Set the array size used for fetches. Using a value of zero will select the
// default value. A check is made to ensure that all defined variables have
// sufficient space to support the array size.
//-----------------------------------------------------------------------------
int dpiStmt_setFetchArraySize(dpiStmt *stmt, uint32_t arraySize)
{
    dpiError error;
    dpiVar *var;
    uint32_t i;

    if (dpiStmt__checkOpen(stmt, __func__, &error) < 0)
        return DPI_FAILURE;
    if (arraySize == 0)
        arraySize = DPI_DEFAULT_FETCH_ARRAY_SIZE;
    for (i = 0; i < stmt->numQueryVars; i++) {
        var = stmt->queryVars[i];
        if (var && var->maxArraySize < arraySize)
            return dpiError__set(&error, "check array size",
                    DPI_ERR_ARRAY_SIZE_TOO_BIG, arraySize);
    }
    stmt->fetchArraySize = arraySize;
    return DPI_SUCCESS;
}

