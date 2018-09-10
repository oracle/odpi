//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2017, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestAppContext.c
//   Tests the use of application context.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

#define APP_CTX_NAMESPACE   "CLIENTCONTEXT"
#define APP_CTX_NUM_KEYS    3
#define SQL_TEXT_GET_CTX    "select sys_context(:1, :2) from dual"

static const char *gc_ContextKeys[APP_CTX_NUM_KEYS] =
        { "ATTR1", "ATTR2", "ATTR3" };
static const char *gc_ContextValues[APP_CTX_NUM_KEYS] =
        { "VALUE1", "VALUE2", "VALUE3" };

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData *namespaceData, *keyData, *valueData;
    uint32_t numQueryColumns, i, bufferRowIndex;
    dpiAppContext appContext[APP_CTX_NUM_KEYS];
    dpiVar *namespaceVar, *keyVar, *valueVar;
    dpiConnCreateParams createParams;
    dpiSampleParams *params;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // get parameters
    params = dpiSamples_getParams();

    // populate app context
    for (i = 0; i < APP_CTX_NUM_KEYS; i++) {
        appContext[i].namespaceName = APP_CTX_NAMESPACE;
        appContext[i].namespaceNameLength = strlen(APP_CTX_NAMESPACE);
        appContext[i].name = gc_ContextKeys[i];
        appContext[i].nameLength = strlen(gc_ContextKeys[i]);
        appContext[i].value = gc_ContextValues[i];
        appContext[i].valueLength = strlen(gc_ContextValues[i]);
    }

    // connect to the database
    if (dpiContext_initConnCreateParams(params->context, &createParams) < 0)
        return dpiSamples_showError();
    createParams.appContext = appContext;
    createParams.numAppContext = APP_CTX_NUM_KEYS;
    if (dpiConn_create(params->context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &conn) < 0)
        return dpiSamples_showError();

    // prepare statement for multiple execution
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_GET_CTX,
            strlen(SQL_TEXT_GET_CTX), NULL, 0, &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            1, 30, 1, 0, NULL, &namespaceVar, &namespaceData) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            1, 30, 1, 0, NULL, &keyVar, &keyData) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            1, 30, 1, 0, NULL, &valueVar, &valueData) < 0)
        return dpiSamples_showError();

    // get the values for each key
    for (i = 0; i < APP_CTX_NUM_KEYS; i++) {
        if (dpiVar_setFromBytes(namespaceVar, 0, APP_CTX_NAMESPACE,
                strlen(APP_CTX_NAMESPACE)) < 0)
            return dpiSamples_showError();
        if (dpiVar_setFromBytes(keyVar, 0, gc_ContextKeys[i],
                strlen(gc_ContextKeys[i])) < 0)
            return dpiSamples_showError();
        if (dpiStmt_bindByPos(stmt, 1, namespaceVar) < 0)
            return dpiSamples_showError();
        if (dpiStmt_bindByPos(stmt, 2, keyVar) < 0)
            return dpiSamples_showError();
        if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
            return dpiSamples_showError();
        if (dpiStmt_define(stmt, 1, valueVar) < 0)
            return dpiSamples_showError();
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        printf("Value of context key %s is %.*s\n", gc_ContextKeys[i],
                valueData->value.asBytes.length,
                valueData->value.asBytes.ptr);
    }

    // clean up
    dpiVar_release(namespaceVar);
    dpiVar_release(keyVar);
    dpiVar_release(valueVar);
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

