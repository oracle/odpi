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
// dpiConn.c
//   Implementation of connection.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"
#include <time.h>

// forward declarations of internal functions only used in this file
static int dpiConn__getSession(dpiConn *conn, uint32_t mode,
        const char *connectString, uint32_t connectStringLength,
        dpiConnCreateParams *params, OCIAuthInfo *authInfo, dpiError *error);
static int dpiConn__setAttributeHelper(void *handle, uint32_t handleType,
        uint32_t attribute, const char *action, const void *value,
        uint32_t valueLength, dpiError *error);
static int dpiConn__setAttributesFromCreateParams(void *handle,
        uint32_t handleType, const char *userName, uint32_t userNameLength,
        const char *password, uint32_t passwordLength,
        const dpiConnCreateParams *params, dpiError *error);


//-----------------------------------------------------------------------------
// dpiConn__checkConnected() [INTERNAL]
//   Validate the connection handle and determine the error structure to use.
// Check to see that the connection is connected to the database.
//-----------------------------------------------------------------------------
static int dpiConn__checkConnected(dpiConn *conn, const char *fnName,
        dpiError *error)
{
    if (dpiGen__startPublicFn(conn, DPI_HTYPE_CONN, fnName, error) < 0)
        return DPI_FAILURE;
    if (!conn->handle)
        return dpiError__set(error, "check connected", DPI_ERR_NOT_CONNECTED);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn__close() [INTERNAL]
//   Internal method used for closing the connection. Any transaction is rolled
// back and any handles allocated are freed. For connections acquired from a
// pool and that aren't marked as needed to be dropped, the last time used is
// updated. This is called from dpiConn_close() where errors are expected to be
// propagated and from dpiConn__free() where errors are ignored.
//-----------------------------------------------------------------------------
static int dpiConn__close(dpiConn *conn, dpiConnCloseMode mode,
        const char *tag, uint32_t tagLength, int propagateErrors,
        dpiError *error)
{
    uint32_t serverStatus;
    time_t *lastTimeUsed;
    sword status;

    // rollback any outstanding transaction
    status = OCITransRollback(conn->handle, error->handle, OCI_DEFAULT);
    if (propagateErrors && dpiError__check(error, status, conn,
            "rollback") < 0)
        return DPI_FAILURE;

    // handle standalone connections
    if (conn->standalone) {

        // end session and free session handle
        status = OCISessionEnd(conn->handle, error->handle,
                conn->sessionHandle, OCI_DEFAULT);
        if (propagateErrors && dpiError__check(error, status, conn,
                "end session") < 0)
            return DPI_FAILURE;
        OCIHandleFree(conn->sessionHandle, OCI_HTYPE_SESSION);
        conn->sessionHandle = NULL;

        // detach from server and free server handle
        status = OCIServerDetach(conn->serverHandle, error->handle,
                OCI_DEFAULT);
        if (propagateErrors && dpiError__check(error, status, conn,
                "detach from server") < 0)
            return DPI_FAILURE;
        OCIHandleFree(conn->serverHandle, OCI_HTYPE_SERVER);

        // free service context handle
        OCIHandleFree(conn->handle, OCI_HTYPE_SVCCTX);

    // handle pooled connections
    } else {

        // if the session isn't marked as needing to be dropped, update the
        // last time used (this is checked when the session is acquired)
        if (!conn->dropSession && conn->sessionHandle) {

            // get the pointer from the context
            lastTimeUsed = NULL;
            status = OCIContextGetValue(conn->sessionHandle, error->handle,
                    (ub1*) DPI_CONTEXT_LAST_TIME_USED,
                    (ub1) strlen(DPI_CONTEXT_LAST_TIME_USED),
                    (void**) &lastTimeUsed);
            if (propagateErrors && dpiError__check(error, status, conn,
                    "get last time used") < 0)
                return DPI_FAILURE;

            // if no pointer available, allocate and set it
            if (!lastTimeUsed) {
                status = OCIMemoryAlloc(conn->sessionHandle, error->handle,
                        (void**) &lastTimeUsed, OCI_DURATION_SESSION,
                        sizeof(time_t), OCI_MEMORY_CLEARED);
                if (propagateErrors && dpiError__check(error, status, conn,
                        "allocate memory for last time used") < 0)
                    return DPI_FAILURE;
                status = OCIContextSetValue(conn->sessionHandle, error->handle,
                        OCI_DURATION_SESSION,
                        (ub1*) DPI_CONTEXT_LAST_TIME_USED,
                        (ub1) strlen(DPI_CONTEXT_LAST_TIME_USED),
                        lastTimeUsed);
                if (propagateErrors && dpiError__check(error, status, conn,
                        "set last time used") < 0) {
                    OCIMemoryFree(conn->sessionHandle, error->handle,
                            lastTimeUsed);
                    return DPI_FAILURE;
                }
            }

            // set last time used
            if (lastTimeUsed)
                *lastTimeUsed = time(NULL);

        }

        // check server status; if not connected, ensure session is dropped
        status = OCIAttrGet(conn->serverHandle, OCI_HTYPE_SERVER,
                (void*) &serverStatus, 0, OCI_ATTR_SERVER_STATUS,
                error->handle);
        if (status != OCI_SUCCESS || serverStatus != OCI_SERVER_NORMAL)
            conn->dropSession = 1;

        // release session
        if (conn->dropSession)
            mode |= OCI_SESSRLS_DROPSESS;
        status = OCISessionRelease(conn->handle, error->handle, (text*) tag,
                tagLength, mode);
        if (propagateErrors && dpiError__check(error, status, conn,
                "release session") < 0)
            return DPI_FAILURE;
        conn->sessionHandle = NULL;

    }

    conn->handle = NULL;
    conn->serverHandle = NULL;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn__create() [PRIVATE]
//   Create a standalone connection to the database using the parameters
// specified.
//-----------------------------------------------------------------------------
static int dpiConn__create(dpiConn *conn, const char *userName,
        uint32_t userNameLength, const char *password, uint32_t passwordLength,
        const char *connectString, uint32_t connectStringLength,
        const dpiCommonCreateParams *commonParams,
        const dpiConnCreateParams *createParams, dpiError *error)
{
    uint32_t credentialType;
    sword status;

    // mark the connection as a standalone connection
    conn->standalone = 1;

    // allocate the server handle
    status = OCIHandleAlloc(conn->env->handle, (dvoid**) &conn->serverHandle,
            OCI_HTYPE_SERVER, 0, 0);
    if (dpiError__check(error, status, conn, "allocate server handle") < 0)
        return DPI_FAILURE;

    // attach to the server
    status = OCIServerAttach(conn->serverHandle, error->handle,
            (text*) connectString, connectStringLength, OCI_DEFAULT);
    if (dpiError__check(error, status, conn, "server attach") < 0)
        return DPI_FAILURE;

    // allocate the service context handle
    status = OCIHandleAlloc(conn->env->handle, (dvoid**) &conn->handle,
            OCI_HTYPE_SVCCTX, 0, 0);
    if (dpiError__check(error, status, conn,
            "allocate service context handle") < 0)
        return DPI_FAILURE;

    // set attribute for server handle
    status = OCIAttrSet(conn->handle, OCI_HTYPE_SVCCTX, conn->serverHandle, 0,
            OCI_ATTR_SERVER, error->handle);
    if (dpiError__check(error, status, conn, "set server handle") < 0)
        return DPI_FAILURE;

    // allocate the session handle
    status = OCIHandleAlloc(conn->env->handle, (dvoid**) &conn->sessionHandle,
            OCI_HTYPE_SESSION, 0, 0);
    if (dpiError__check(error, status, conn, "allocate session handle") < 0)
        return DPI_FAILURE;

    // driver name and edition are only relevant for standalone connections
    if (dpiConn__setAttributesFromCommonCreateParams(conn->env->context,
            conn->sessionHandle, OCI_HTYPE_SESSION, commonParams, error) < 0)
        return DPI_FAILURE;

    // populate attributes on the session handle
    if (dpiConn__setAttributesFromCreateParams(conn->sessionHandle,
            OCI_HTYPE_SESSION, userName, userNameLength, password,
            passwordLength, createParams, error) < 0)
        return DPI_FAILURE;

    // set the session handle on the service context handle
    status = OCIAttrSet(conn->handle, OCI_HTYPE_SVCCTX,
            conn->sessionHandle, 0, OCI_ATTR_SESSION, error->handle);
    if (dpiError__check(error, status, conn, "set session handle") < 0)
        return DPI_FAILURE;

    // if a new password is specified, change it (this also creates the session
    // so a call to OCISessionBegin() is not needed)
    if (createParams->newPassword && createParams->newPasswordLength > 0) {
        status = OCIPasswordChange(conn->handle, error->handle,
                (OraText*) userName, userNameLength,
                (OraText*) password, passwordLength,
                (OraText*) createParams->newPassword,
                createParams->newPasswordLength, OCI_AUTH);
        return dpiError__check(error, status, conn, "change password");
    }

    // begin the session
    credentialType = (createParams->externalAuth) ? OCI_CRED_EXT :
            OCI_CRED_RDBMS;
    status = OCISessionBegin(conn->handle, error->handle, conn->sessionHandle,
            credentialType, createParams->authMode | OCI_STMT_CACHE);
    return dpiError__check(error, status, conn, "begin session");
}


//-----------------------------------------------------------------------------
// dpiConn__free() [INTERNAL]
//   Free the memory and any resources associated with the connection.
//-----------------------------------------------------------------------------
void dpiConn__free(dpiConn *conn, dpiError *error)
{
    if (conn->handle)
        dpiConn__close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0, 0,
                error);
    if (conn->pool) {
        dpiGen__setRefCount(conn->pool, error, -1);
        conn->pool = NULL;
        conn->env = NULL;
    }
    if (conn->env) {
        dpiEnv__free(conn->env, error);
        conn->env = NULL;
    }
    if (conn->releaseString) {
        free((void*) conn->releaseString);
        conn->releaseString = NULL;
    }
    free(conn);
}


//-----------------------------------------------------------------------------
// dpiConn__get() [INTERNAL]
//   Create a connection to the database using the parameters specified. This
// method uses the simplified OCI session creation protocol which is required
// when using pools and session tagging.
//-----------------------------------------------------------------------------
int dpiConn__get(dpiConn *conn, const char *userName, uint32_t userNameLength,
        const char *password, uint32_t passwordLength,
        const char *connectString, uint32_t connectStringLength,
        dpiConnCreateParams *createParams, dpiPool *pool, dpiError *error)
{
    OCIAuthInfo *authInfo;
    int externalAuth;
    uint32_t mode;
    sword status;

    // set things up for the call to acquire a session
    if (pool) {
        if (dpiGen__setRefCount(pool, error, 1) < 0)
            return DPI_FAILURE;
        conn->pool = pool;
        mode = OCI_SESSGET_SPOOL;
        externalAuth = pool->externalAuth;
        if (userName && pool->homogeneous)
            return dpiError__set(error, "check proxy", DPI_ERR_INVALID_PROXY);
        if (userName)
            mode |= OCI_SESSGET_CREDPROXY;
        if (createParams->matchAnyTag)
            mode |= OCI_SESSGET_SPOOL_MATCHANY;
    } else {
        mode = OCI_SESSGET_STMTCACHE;
        externalAuth = createParams->externalAuth;
    }
    if (createParams->authMode & DPI_MODE_AUTH_SYSDBA)
        mode |= OCI_SESSGET_SYSDBA;
    if (externalAuth)
        mode |= OCI_SESSGET_CREDEXT;

    // create authorization handle
    status = OCIHandleAlloc(conn->env->handle, (dvoid*) &authInfo,
            OCI_HTYPE_AUTHINFO, 0, NULL);
    if (dpiError__check(error, status, conn, "allocate authinfo handle") < 0)
        return DPI_FAILURE;

    // set attributes for create parameters
    if (dpiConn__setAttributesFromCreateParams(authInfo, OCI_HTYPE_AUTHINFO,
            userName, userNameLength, password, passwordLength, createParams,
            error) < 0) {
        OCIHandleFree(authInfo, OCI_HTYPE_AUTHINFO);
        return DPI_FAILURE;
    }

    // get a session from the pool
    status = dpiConn__getSession(conn, mode, connectString,
            connectStringLength, createParams, authInfo, error);
    OCIHandleFree(authInfo, OCI_HTYPE_AUTHINFO);
    return status;
}


//-----------------------------------------------------------------------------
// dpiConn__getAttributeText() [INTERNAL]
//   Get the value of the OCI attribute from a text string.
//-----------------------------------------------------------------------------
int dpiConn__getAttributeText(dpiConn *conn, uint32_t attribute,
        const char **value, uint32_t *valueLength, const char *fnName)
{
    dpiError error;
    sword status;

    // make sure connection is connected
    if (dpiConn__checkConnected(conn, fnName, &error) < 0)
        return DPI_FAILURE;

    // validate pointers are not NULL
    if (!value)
        return dpiError__set(&error, "check value pointer",
                DPI_ERR_NULL_POINTER_PARAMETER, "value");
    if (!valueLength)
        return dpiError__set(&error, "check value length pointer",
                DPI_ERR_NULL_POINTER_PARAMETER, "valueLength");

    // determine pointer to pass (OCI uses different sizes)
    switch (attribute) {
        case OCI_ATTR_CURRENT_SCHEMA:
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
        case OCI_ATTR_LTXID:
#endif
        case OCI_ATTR_EDITION:
            status = OCIAttrGet(conn->sessionHandle, OCI_HTYPE_SESSION,
                    (text**) value, valueLength, attribute, error.handle);
            return dpiError__check(&error, status, conn, "get session value");
        case OCI_ATTR_INTERNAL_NAME:
        case OCI_ATTR_EXTERNAL_NAME:
            status = OCIAttrGet(conn->serverHandle, OCI_HTYPE_SERVER,
                    (text**) value, valueLength, attribute, error.handle);
            return dpiError__check(&error, status, conn, "get server value");
        default:
            break;
    }

    return dpiError__set(&error, "get attribute text", DPI_ERR_NOT_SUPPORTED);
}


//-----------------------------------------------------------------------------
// dpiConn__getSession() [INTERNAL]
//   Ping and loop until we get a good session. When a database instance goes
// down, it can leave several bad connections that need to be flushed out
// before a good connection can be acquired. If the connection is brand new
// (ping time context value has not been set) there is no need to do a ping.
// This also ensures that the loop cannot run forever!
//   Note as well that this is only needed for clients less than 12.2. In the
// 12.2 release a much faster internal check is performed that makes these
// checks unnecessary.
//-----------------------------------------------------------------------------
static int dpiConn__getSession(dpiConn *conn, uint32_t mode,
        const char *connectString, uint32_t connectStringLength,
        dpiConnCreateParams *params, OCIAuthInfo *authInfo, dpiError *error)
{
#if DPI_ORACLE_CLIENT_VERSION_HEX < DPI_ORACLE_CLIENT_VERSION(12, 2)
#ifdef OCI_ATTR_BREAK_ON_NET_TIMEOUT
    uint8_t savedBreakOnTimeout, breakOnTimeout;
#endif
    uint32_t savedTimeout;
    time_t *lastTimeUsed;
#endif
    boolean found;
    sword status;

    while (1) {

        // acquire the new session
        status = OCISessionGet(conn->env->handle, error->handle, &conn->handle,
                authInfo, (text*) connectString, connectStringLength,
                (OraText*) params->tag, params->tagLength,
                (OraText**) &params->outTag, &params->outTagLength, &found,
                mode);
        params->outTagFound = found;
        if (dpiError__check(error, status, conn, "get session") < 0)
            return DPI_FAILURE;

        // acquire the session handle
        status = OCIAttrGet(conn->handle, OCI_HTYPE_SVCCTX,
                (dvoid**) &conn->sessionHandle, 0, OCI_ATTR_SESSION,
                error->handle);
        if (dpiError__check(error, status, conn, "get session handle") < 0)
            return DPI_FAILURE;

        // acquire the server handle
        status = OCIAttrGet(conn->handle, OCI_HTYPE_SVCCTX,
                (dvoid**) &conn->serverHandle, 0, OCI_ATTR_SERVER,
                error->handle);
        if (dpiError__check(error, status, conn, "get server handle") < 0)
            return DPI_FAILURE;

#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 2)
        break;
#else

        // get last time used from session context
        lastTimeUsed = NULL;
        status = OCIContextGetValue(conn->sessionHandle, error->handle,
                (uint8_t*) DPI_CONTEXT_LAST_TIME_USED,
                (uint8_t) strlen(DPI_CONTEXT_LAST_TIME_USED),
                (void**) &lastTimeUsed);
        if (dpiError__check(error, status, conn, "get last time used") < 0)
            return DPI_FAILURE;

        // if value is not found, a new connection has been created and there
        // is no need to perform a ping; nor if we are creating a standalone
        // connection
        if (!lastTimeUsed || !conn->pool)
            break;

        // if ping interval is negative or the ping interval (in seconds)
        // has not been exceeded yet, there is also no need to perform a ping
        if (conn->pool->pingInterval < 0 ||
                *lastTimeUsed + conn->pool->pingInterval > time(NULL))
            break;

        // ping needs to be done at this point; set parameters to ensure that
        // the ping does not take too long to complete; keep original values
        OCIAttrGet(conn->serverHandle, OCI_HTYPE_SERVER, &savedTimeout, 0,
                OCI_ATTR_RECEIVE_TIMEOUT, error->handle);
        OCIAttrSet(conn->serverHandle, OCI_HTYPE_SERVER,
                &conn->pool->pingTimeout, 0, OCI_ATTR_RECEIVE_TIMEOUT,
                error->handle);
#ifdef OCI_ATTR_BREAK_ON_NET_TIMEOUT
        OCIAttrGet(conn->serverHandle, OCI_HTYPE_SERVER, &savedBreakOnTimeout,
                0, OCI_ATTR_BREAK_ON_NET_TIMEOUT, error->handle);
        breakOnTimeout = 0;
        OCIAttrSet(conn->serverHandle, OCI_HTYPE_SERVER, &breakOnTimeout, 0,
                OCI_ATTR_BREAK_ON_NET_TIMEOUT, error->handle);
#endif

        // if ping is successful, the connection is valid and can be returned
        // restore original network parameters
        status = OCIPing(conn->handle, error->handle, OCI_DEFAULT);
        if (status == OCI_SUCCESS) {
            OCIAttrSet(conn->serverHandle, OCI_HTYPE_SERVER, &savedTimeout, 0,
                    OCI_ATTR_RECEIVE_TIMEOUT, error->handle);
#ifdef OCI_ATTR_BREAK_ON_NET_TIMEOUT
            OCIAttrSet(conn->serverHandle, OCI_HTYPE_SERVER,
                    &savedBreakOnTimeout, 0, OCI_ATTR_BREAK_ON_NET_TIMEOUT,
                    error->handle);
#endif
            break;
        }

        // session is bad, need to release and drop it
        OCISessionRelease(conn->handle, error->handle, NULL, 0,
                OCI_SESSRLS_DROPSESS);
        conn->handle = NULL;
        conn->serverHandle = NULL;
        conn->sessionHandle = NULL;

#endif
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn__setAppContext() [INTERNAL]
//   Populate the session handle with the application context.
//-----------------------------------------------------------------------------
static int dpiConn__setAppContext(void *handle, uint32_t handleType,
        const dpiConnCreateParams *params, dpiError *error)
{
    void *listHandle, *entryHandle;
    dpiAppContext *entry;
    sword status;
    uint32_t i;

    // set the number of application context entries
    if (dpiConn__setAttributeHelper(handle, handleType, OCI_ATTR_APPCTX_SIZE,
            "set app context size", &params->numAppContext,
            sizeof(params->numAppContext), error) < 0)
        return DPI_FAILURE;

    // get the application context list handle
    status = OCIAttrGet(handle, handleType, &listHandle, 0,
            OCI_ATTR_APPCTX_LIST, error->handle);
    if (dpiError__check(error, status, NULL, "get context list handle") < 0)
        return DPI_FAILURE;

    // set each application context entry
    for (i = 0; i < params->numAppContext; i++) {
        entry = &params->appContext[i];

        // retrieve the context element descriptor
        status = OCIParamGet(listHandle, OCI_DTYPE_PARAM, error->handle,
                &entryHandle, i + 1);
        if (dpiError__check(error, status, NULL,
                "get context entry handle") < 0)
            return DPI_FAILURE;

        // set the namespace name
        status = OCIAttrSet(entryHandle, OCI_DTYPE_PARAM,
                (void*) entry->namespaceName, entry->namespaceNameLength,
                OCI_ATTR_APPCTX_NAME, error->handle);
        if (dpiError__check(error, status, NULL, "set namespace name") < 0)
            return DPI_FAILURE;

        // set the name
        status = OCIAttrSet(entryHandle, OCI_DTYPE_PARAM, (void*) entry->name,
                entry->nameLength, OCI_ATTR_APPCTX_ATTR, error->handle);
        if (dpiError__check(error, status, NULL, "set name") < 0)
            return DPI_FAILURE;

        // set the value
        status = OCIAttrSet(entryHandle, OCI_DTYPE_PARAM, (void*) entry->value,
                entry->valueLength, OCI_ATTR_APPCTX_VALUE, error->handle);
        if (dpiError__check(error, status, NULL, "set value") < 0)
            return DPI_FAILURE;

    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn__setAttributeHelper() [INTERNAL]
//   Helper for setting a string attribute.
//-----------------------------------------------------------------------------
static int dpiConn__setAttributeHelper(void *handle, uint32_t handleType,
        uint32_t attribute, const char *action, const void *value,
        uint32_t valueLength, dpiError *error)
{
    sword status;

    if (value && valueLength > 0) {
        status = OCIAttrSet(handle, handleType, (void*) value, valueLength,
                attribute, error->handle);
        return dpiError__check(error, status, NULL, action);
    }
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn__setAttributesFromCommonCreateParams() [INTERNAL]
//   Populate the authorization info structure or session handle using the
// context parameters specified.
//-----------------------------------------------------------------------------
int dpiConn__setAttributesFromCommonCreateParams(const dpiContext *context,
        void *handle, uint32_t handleType, const dpiCommonCreateParams *params,
        dpiError *error)
{
    uint32_t driverNameLength;
    const char *driverName;

    if (params->driverName && params->driverNameLength > 0) {
        driverName = params->driverName;
        driverNameLength = params->driverNameLength;
    } else {
        driverName = DPI_DEFAULT_DRIVER_NAME;
        driverNameLength = strlen(driverName);
    }
    if (dpiConn__setAttributeHelper(handle, handleType, OCI_ATTR_DRIVER_NAME,
            "set driver name", driverName, driverNameLength, error) < 0)
        return DPI_FAILURE;
    if (dpiConn__setAttributeHelper(handle, handleType, OCI_ATTR_EDITION,
            "set edition", params->edition, params->editionLength, error) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn__setAttributesFromCreateParams() [INTERNAL]
//   Populate the authorization info structure or session handle using the
// create parameters specified.
//-----------------------------------------------------------------------------
static int dpiConn__setAttributesFromCreateParams(void *handle,
        uint32_t handleType, const char *userName, uint32_t userNameLength,
        const char *password, uint32_t passwordLength,
        const dpiConnCreateParams *params, dpiError *error)
{
    uint32_t purity;

    // set credentials
    if (dpiConn__setAttributeHelper(handle, handleType, OCI_ATTR_USERNAME,
            "set user name", userName, userNameLength, error) < 0)
        return DPI_FAILURE;
    if (dpiConn__setAttributeHelper(handle, handleType, OCI_ATTR_PASSWORD,
            "set password", password, passwordLength, error) < 0)
        return DPI_FAILURE;

    // set connection class and purity parameters
    if (dpiConn__setAttributeHelper(handle, handleType,
            OCI_ATTR_CONNECTION_CLASS, "set connection class",
            params->connectionClass, params->connectionClassLength, error) < 0)
        return DPI_FAILURE;
    if (params->purity != OCI_ATTR_PURITY_DEFAULT) {
        purity = params->purity;
        if (dpiConn__setAttributeHelper(handle, handleType, OCI_ATTR_PURITY,
                "set purity", &purity, sizeof(purity), error) < 0)
            return DPI_FAILURE;
    }

    // set application context, if applicable
    if (handleType == OCI_HTYPE_SESSION && params->numAppContext > 0)
        return dpiConn__setAppContext(handle, handleType, params, error);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn__setAttributeText() [INTERNAL]
//   Set the value of the OCI attribute from a text string.
//-----------------------------------------------------------------------------
int dpiConn__setAttributeText(dpiConn *conn, uint32_t attribute,
        const char *value, uint32_t valueLength, const char *fnName)
{
    dpiError error;
    sword status;

    // make sure connection is connected
    if (dpiConn__checkConnected(conn, fnName, &error) < 0)
        return DPI_FAILURE;

    // determine pointer to pass (OCI uses different sizes)
    switch (attribute) {
        case OCI_ATTR_ACTION:
        case OCI_ATTR_CLIENT_IDENTIFIER:
        case OCI_ATTR_CLIENT_INFO:
        case OCI_ATTR_CURRENT_SCHEMA:
        case OCI_ATTR_EDITION:
        case OCI_ATTR_MODULE:
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
        case OCI_ATTR_DBOP:
#endif
            status = OCIAttrSet(conn->sessionHandle, OCI_HTYPE_SESSION,
                    (text*) value, valueLength, attribute, error.handle);
            return dpiError__check(&error, status, conn, "set session value");
        case OCI_ATTR_INTERNAL_NAME:
        case OCI_ATTR_EXTERNAL_NAME:
            status = OCIAttrSet(conn->serverHandle, OCI_HTYPE_SERVER,
                    (text*) value, valueLength, attribute, error.handle);
            return dpiError__check(&error, status, conn, "set server value");
        default:
            break;
    }

    return dpiError__set(&error, "set attribute text", DPI_ERR_NOT_SUPPORTED);
}


//-----------------------------------------------------------------------------
// dpiConn_addRef() [PUBLIC]
//   Add a reference to the connection.
//-----------------------------------------------------------------------------
int dpiConn_addRef(dpiConn *conn)
{
    return dpiGen__addRef(conn, DPI_HTYPE_CONN, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_beginDistribTrans() [PUBLIC]
//   Begin a distributed transaction.
//-----------------------------------------------------------------------------
int dpiConn_beginDistribTrans(dpiConn *conn, long formatId,
        const char *transactionId, uint32_t transactionIdLength,
        const char *branchId, uint32_t branchIdLength)
{
    OCITrans *transactionHandle;
    dpiError error;
    sword status;
    XID xid;

    // validate arguments
    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (transactionIdLength > MAXGTRIDSIZE)
        return dpiError__set(&error, "check size of transaction id",
                DPI_ERR_TRANS_ID_TOO_LARGE, transactionIdLength, MAXGTRIDSIZE);
    if (branchIdLength > MAXBQUALSIZE)
        return dpiError__set(&error, "check size of branch id",
                DPI_ERR_BRANCH_ID_TOO_LARGE, branchIdLength, MAXBQUALSIZE);

    // determine if a transaction handle was previously allocated
    status = OCIAttrGet(conn->handle, OCI_HTYPE_SVCCTX,
            (dvoid**) &transactionHandle, 0, OCI_ATTR_TRANS, error.handle);
    if (dpiError__check(&error, status, conn, "get transaction handle") < 0)
        return DPI_FAILURE;

    // if one was not found, create one and associate it with the connection
    if (!transactionHandle) {

        // create new handle
        status = OCIHandleAlloc(conn->env->handle,
                (dvoid**) &transactionHandle, OCI_HTYPE_TRANS, 0, 0);
        if (dpiError__check(&error, status, conn,
                "create transaction handle") < 0)
            return DPI_FAILURE;

        // associate the transaction with the connection
        OCIAttrSet(conn->handle, OCI_HTYPE_SVCCTX, transactionHandle, 0,
                OCI_ATTR_TRANS, error.handle);
        if (dpiError__check(&error, status, conn,
                "associate transaction") < 0) {
            OCIHandleFree(transactionHandle, OCI_HTYPE_TRANS);
            return DPI_FAILURE;
        }

    }

    // set the XID for the transaction, if applicable
    if (formatId != -1) {
        xid.formatID = formatId;
        xid.gtrid_length = transactionIdLength;
        xid.bqual_length = branchIdLength;
        if (transactionIdLength > 0)
            strncpy(xid.data, transactionId, transactionIdLength);
        if (branchIdLength > 0)
            strncpy(&xid.data[transactionIdLength], branchId, branchIdLength);
        OCIAttrSet(transactionHandle, OCI_HTYPE_TRANS, &xid, sizeof(XID),
                OCI_ATTR_XID, error.handle);
        if (dpiError__check(&error, status, conn, "set XID") < 0)
            return DPI_FAILURE;
    }

    // start the transaction
    status = OCITransStart(conn->handle, error.handle, 0, OCI_TRANS_NEW);
    return dpiError__check(&error, status, conn, "start transaction");
}


//-----------------------------------------------------------------------------
// dpiConn_breakExecution() [PUBLIC]
//   Break (interrupt) the currently executing operation.
//-----------------------------------------------------------------------------
int dpiConn_breakExecution(dpiConn *conn)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIBreak(conn->handle, error.handle);
    if (dpiError__check(&error, status, conn, "break execution") < 0)
        return DPI_FAILURE;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_changePassword() [PUBLIC]
//   Change the password for the specified user.
//-----------------------------------------------------------------------------
int dpiConn_changePassword(dpiConn *conn, const char *userName,
        uint32_t userNameLength, const char *oldPassword,
        uint32_t oldPasswordLength, const char *newPassword,
        uint32_t newPasswordLength)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIPasswordChange(conn->handle, error.handle,
            (OraText*) userName, userNameLength, (OraText*) oldPassword,
            oldPasswordLength, (OraText*) newPassword, newPasswordLength,
            OCI_DEFAULT);
    return dpiError__check(&error, status, conn, "change password");
}


//-----------------------------------------------------------------------------
// dpiConn_close() [PUBLIC]
//   Close the connection and ensure it can no longer be used.
//-----------------------------------------------------------------------------
int dpiConn_close(dpiConn *conn, dpiConnCloseMode mode, const char *tag,
        uint32_t tagLength)
{
    int propagateErrors = !(mode & DPI_MODE_CONN_CLOSE_DROP);
    dpiError error;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (mode && !conn->pool)
        return dpiError__set(&error, "check in pool",
                DPI_ERR_CONN_NOT_IN_POOL);
    if (conn->externalHandle)
        return dpiError__set(&error, "check external",
                DPI_ERR_CONN_IS_EXTERNAL);
    return dpiConn__close(conn, mode, tag, tagLength, propagateErrors, &error);
}


//-----------------------------------------------------------------------------
// dpiConn_commit() [PUBLIC]
//   Commit the transaction associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_commit(dpiConn *conn)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCITransCommit(conn->handle, error.handle,
            conn->commitMode);
    if (dpiError__check(&error, status, conn, "commit") < 0)
        return DPI_FAILURE;
    conn->commitMode = OCI_DEFAULT;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_create() [PUBLIC]
//   Create a standalone connection to the database using the parameters
// specified.
//-----------------------------------------------------------------------------
int dpiConn_create(const dpiContext *context, const char *userName,
        uint32_t userNameLength, const char *password, uint32_t passwordLength,
        const char *connectString, uint32_t connectStringLength,
        const dpiCommonCreateParams *commonParams,
        dpiConnCreateParams *createParams, dpiConn **conn)
{
    dpiCommonCreateParams localCommonParams;
    dpiConnCreateParams localCreateParams;
    dpiConn *tempConn;
    dpiError error;
    int status;

    // validate context
    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;

    // validate connection handle
    if (!conn)
        return dpiError__set(&error, "check connection handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "conn");

    // use default parameters if none provided
    if (!commonParams) {
        if (dpiContext__initCommonCreateParams(context, &localCommonParams,
                &error) < 0)
            return DPI_FAILURE;
        commonParams = &localCommonParams;
    }
    if (!createParams) {
        if (dpiContext__initConnCreateParams(context, &localCreateParams,
                &error) < 0)
            return DPI_FAILURE;
        createParams = &localCreateParams;
    }

    // ensure that username and password are not specified if external
    // authentication is desired
    if (createParams->externalAuth &&
            ((userName && userNameLength > 0) ||
             (password && passwordLength > 0)))
        return dpiError__set(&error, "check mixed credentials",
                DPI_ERR_EXT_AUTH_WITH_CREDENTIALS);

    // allocate connection
    if (dpiGen__allocate(DPI_HTYPE_CONN, NULL, (void**) &tempConn, &error) < 0)
        return DPI_FAILURE;

    // initialize environment
    if (dpiEnv__init(tempConn->env, context, commonParams, &error) < 0) {
        dpiConn__free(tempConn, &error);
        return DPI_FAILURE;
    }

    // if a handle is specified, use it
    if (createParams->externalHandle) {
        tempConn->handle = createParams->externalHandle;
        tempConn->externalHandle = 1;
        return DPI_SUCCESS;
    }

    // connection class requires the use of the OCISessionGet() method
    // all other cases use the OCISessionBegin() method which is more
    // capable
    if (createParams->connectionClass &&
            createParams->connectionClassLength > 0)
        status = dpiConn__get(tempConn, userName, userNameLength, password,
                passwordLength, connectString, connectStringLength,
                createParams, NULL, &error);
    status = dpiConn__create(tempConn, userName, userNameLength, password,
            passwordLength, connectString, connectStringLength, commonParams,
            createParams, &error);
    if (status < 0) {
        dpiConn__free(tempConn, &error);
        return DPI_FAILURE;
    }

    *conn = tempConn;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_deqObject() [PUBLIC]
//   Dequeue a message from the specified queue.
//-----------------------------------------------------------------------------
int dpiConn_deqObject(dpiConn *conn, const char *queueName,
        uint32_t queueNameLength, dpiDeqOptions *options, dpiMsgProps *props,
        dpiObject *payload, const char **msgId, uint32_t *msgIdLength)
{
    OCIRaw *ociMsgId = NULL;
    dpiError error;
    sword status;

    *msgId = NULL;
    *msgIdLength = 0;
    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (dpiGen__checkHandle(options, DPI_HTYPE_DEQ_OPTIONS, "verify options",
            &error) < 0)
        return DPI_FAILURE;
    if (dpiGen__checkHandle(props, DPI_HTYPE_MSG_PROPS,
            "verify message properties", &error) < 0)
        return DPI_FAILURE;
    if (dpiGen__checkHandle(payload, DPI_HTYPE_OBJECT, "verify payload",
            &error) < 0)
        return DPI_FAILURE;
    if (!msgId)
        return dpiError__set(&error, "check message id pointer",
                DPI_ERR_NULL_POINTER_PARAMETER, "msgId");
    if (!msgIdLength)
        return dpiError__set(&error, "check message id length pointer",
                DPI_ERR_NULL_POINTER_PARAMETER, "msgIdLength");
    status = OCIAQDeq(conn->handle, error.handle, (oratext*) queueName,
            options->handle, props->handle, payload->type->tdo,
            &payload->instance, &payload->indicator, &ociMsgId, OCI_DEFAULT);
    if (dpiError__check(&error, status, conn, "dequeue message") < 0)
        return (error.buffer->code == 25228) ? DPI_SUCCESS : DPI_FAILURE;
    *msgId = (const char*) OCIRawPtr(conn->env->handle, ociMsgId);
    *msgIdLength = OCIRawSize(conn->env->handle, ociMsgId);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_enqObject() [PUBLIC]
//   Enqueue a message to the specified queue.
//-----------------------------------------------------------------------------
int dpiConn_enqObject(dpiConn *conn, const char *queueName,
        uint32_t queueNameLength, dpiEnqOptions *options, dpiMsgProps *props,
        dpiObject *payload, const char **msgId, uint32_t *msgIdLength)
{
    OCIRaw *ociMsgId = NULL;
    dpiError error;
    sword status;

    *msgId = NULL;
    *msgIdLength = 0;
    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (dpiGen__checkHandle(options, DPI_HTYPE_ENQ_OPTIONS, "verify options",
            &error) < 0)
        return DPI_FAILURE;
    if (dpiGen__checkHandle(props, DPI_HTYPE_MSG_PROPS,
            "verify message properties", &error) < 0)
        return DPI_FAILURE;
    if (dpiGen__checkHandle(payload, DPI_HTYPE_OBJECT, "verify payload",
            &error) < 0)
        return DPI_FAILURE;
    if (!msgId)
        return dpiError__set(&error, "check message id pointer",
                DPI_ERR_NULL_POINTER_PARAMETER, "msgId");
    if (!msgIdLength)
        return dpiError__set(&error, "check message id length pointer",
                DPI_ERR_NULL_POINTER_PARAMETER, "msgIdLength");
    status = OCIAQEnq(conn->handle, error.handle, (oratext*) queueName,
            options->handle, props->handle, payload->type->tdo,
            &payload->instance, &payload->indicator, &ociMsgId, OCI_DEFAULT);
    if (dpiError__check(&error, status, conn, "enqueue message") < 0)
        return DPI_FAILURE;
    *msgId = (const char*) OCIRawPtr(conn->env->handle, ociMsgId);
    *msgIdLength = OCIRawSize(conn->env->handle, ociMsgId);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_getCurrentSchema() [PUBLIC]
//   Return the current schema associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_getCurrentSchema(dpiConn *conn, const char **value,
        uint32_t *valueLength)
{
    return dpiConn__getAttributeText(conn, OCI_ATTR_CURRENT_SCHEMA, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_getEdition() [PUBLIC]
//   Return the edition associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_getEdition(dpiConn *conn, const char **value,
        uint32_t *valueLength)
{
    return dpiConn__getAttributeText(conn, OCI_ATTR_EDITION, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_getEncodingInfo() [PUBLIC]
//   Get the encodings from the connection.
//-----------------------------------------------------------------------------
int dpiConn_getEncodingInfo(dpiConn *conn, dpiEncodingInfo *info)
{
    dpiError error;

    if (dpiGen__startPublicFn(conn, DPI_HTYPE_CONN, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiEnv__getEncodingInfo(conn->env, info);
}


//-----------------------------------------------------------------------------
// dpiConn_getExternalName() [PUBLIC]
//   Return the external name associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_getExternalName(dpiConn *conn, const char **value,
        uint32_t *valueLength)
{
    return dpiConn__getAttributeText(conn, OCI_ATTR_EXTERNAL_NAME, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_getHandle() [PUBLIC]
//   Get the OCI service context handle associated with the connection. This is
// available in order to allow for extensions to the library using OCI
// directly.
//-----------------------------------------------------------------------------
int dpiConn_getHandle(dpiConn *conn, void **handle)
{
    dpiError error;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    *handle = conn->handle;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_getInternalName() [PUBLIC]
//   Return the internal name associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_getInternalName(dpiConn *conn, const char **value,
        uint32_t *valueLength)
{
    return dpiConn__getAttributeText(conn, OCI_ATTR_INTERNAL_NAME, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_getLTXID() [PUBLIC]
//   Return the logical transaction id associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_getLTXID(dpiConn *conn, const char **value, uint32_t *valueLength)
{
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
    return dpiConn__getAttributeText(conn, OCI_ATTR_LTXID, value, valueLength,
            __func__);
#else
    return dpiConn__getAttributeText(conn, 0, value, valueLength, __func__);
#endif
}


//-----------------------------------------------------------------------------
// dpiConn_getObjectType() [PUBLIC]
//   Look up an object type given its name and return it.
//-----------------------------------------------------------------------------
int dpiConn_getObjectType(dpiConn *conn, const char *name, uint32_t nameLength,
        dpiObjectType **objType)
{
    OCIDescribe *describeHandle;
    OCIParam *param;
    dpiError error;
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
    OCIType *tdo;
#endif
    sword status;

    // ensure connection is actually open first
    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;

    // validate object type handle
    if (!objType)
        return dpiError__set(&error, "check object type handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "objType");

    // allocate describe handle
    status = OCIHandleAlloc(conn->env->handle, (dvoid**) &describeHandle,
            OCI_HTYPE_DESCRIBE, 0, 0);
    if (dpiError__check(&error, status, conn, "allocate describe handle") < 0)
        return DPI_FAILURE;

    // describe the object
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
    status = OCITypeByFullName(conn->env->handle, error.handle, conn->handle,
            (text*) name, nameLength, NULL, 0, OCI_DURATION_SESSION,
            OCI_TYPEGET_ALL, &tdo);
    if (dpiError__check(&error, status, conn, "get type by full name") < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }
    status = OCIDescribeAny(conn->handle, error.handle, (dvoid*) tdo, 0,
            OCI_OTYPE_PTR, 0, OCI_PTYPE_TYPE, describeHandle);
    if (dpiError__check(&error, status, conn, "describe type") < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }
#else
    status = OCIDescribeAny(conn->handle, error.handle, (text*) name,
            nameLength, OCI_OTYPE_NAME, 0, OCI_PTYPE_TYPE, describeHandle);
    if (dpiError__check(&error, status, conn, "describe type") < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }
#endif

    // get the parameter handle
    status = OCIAttrGet(describeHandle, OCI_HTYPE_DESCRIBE, &param, 0,
            OCI_ATTR_PARAM, error.handle);
    if (dpiError__check(&error, status, conn, "get param") < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }

    // create object type
    status = dpiObjectType__allocate(conn, param, OCI_ATTR_NAME, objType,
            &error);
    OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
    return status;
}


//-----------------------------------------------------------------------------
// dpiConn_getServerVersion() [PUBLIC]
//   Get the server version string from the database.
//-----------------------------------------------------------------------------
int dpiConn_getServerVersion(dpiConn *conn, const char **releaseString,
        uint32_t *releaseStringLength, int *versionNum, int *releaseNum,
        int *updateNum, int *portReleaseNum, int *portUpdateNum)
{
    uint32_t serverRelease;
    char buffer[512];
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!conn->releaseString) {
        status = OCIServerRelease(conn->handle, error.handle,
                (oratext*) buffer, sizeof(buffer), OCI_HTYPE_SVCCTX,
                &serverRelease);
        if (dpiError__check(&error, status, conn, "get server version") < 0)
            return DPI_FAILURE;
        conn->releaseStringLength = strlen(buffer);
        conn->releaseString = malloc(conn->releaseStringLength);
        if (!conn->releaseString)
            return dpiError__set(&error, "allocate release string",
                    DPI_ERR_NO_MEMORY);
        strncpy( (char*) conn->releaseString, buffer,
                conn->releaseStringLength);
        conn->versionNum = (int)((serverRelease >> 24) & 0xFF);
        conn->releaseNum = (int)((serverRelease >> 20) & 0x0F);
        conn->updateNum = (int)((serverRelease >> 12) & 0xFF);
        conn->portReleaseNum = (int)((serverRelease >> 8) & 0x0F);
        conn->portUpdateNum = (int)((serverRelease) & 0xFF);
    }
    *releaseString = conn->releaseString;
    *releaseStringLength = conn->releaseStringLength;
    *versionNum = conn->versionNum;
    *releaseNum = conn->releaseNum;
    *updateNum = conn->updateNum;
    *portReleaseNum = conn->portReleaseNum;
    *portUpdateNum = conn->portUpdateNum;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_getStmtCacheSize() [PUBLIC]
//   Return the current size of the statement cache.
//-----------------------------------------------------------------------------
int dpiConn_getStmtCacheSize(dpiConn *conn, uint32_t *cacheSize)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIAttrGet(conn->handle, OCI_HTYPE_SVCCTX, cacheSize, NULL,
            OCI_ATTR_STMTCACHESIZE, error.handle);
    return dpiError__check(&error, status, conn, "get stmt cache size");
}


//-----------------------------------------------------------------------------
// dpiConn_newDeqOptions() [PUBLIC]
//   Create a new dequeue options object and return it.
//-----------------------------------------------------------------------------
int dpiConn_newDeqOptions(dpiConn *conn, dpiDeqOptions **options)
{
    dpiDeqOptions *tempOptions;
    dpiError error;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!options)
        return dpiError__set(&error, "check options handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "options");
    if (dpiGen__allocate(DPI_HTYPE_DEQ_OPTIONS, conn->env,
            (void**) &tempOptions, &error) < 0)
        return DPI_FAILURE;
    if (dpiDeqOptions__create(tempOptions, conn, &error) < 0) {
        dpiDeqOptions__free(tempOptions, &error);
        return DPI_FAILURE;
    }

    *options = tempOptions;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_newEnqOptions() [PUBLIC]
//   Create a new enqueue options object and return it.
//-----------------------------------------------------------------------------
int dpiConn_newEnqOptions(dpiConn *conn, dpiEnqOptions **options)
{
    dpiEnqOptions *tempOptions;
    dpiError error;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!options)
        return dpiError__set(&error, "check options handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "options");
    if (dpiGen__allocate(DPI_HTYPE_ENQ_OPTIONS, conn->env,
            (void**) &tempOptions, &error) < 0)
        return DPI_FAILURE;
    if (dpiEnqOptions__create(tempOptions, conn, &error) < 0) {
        dpiEnqOptions__free(tempOptions, &error);
        return DPI_FAILURE;
    }

    *options = tempOptions;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_newTempLob() [PUBLIC]
//   Create a new temporary LOB and return it.
//-----------------------------------------------------------------------------
int dpiConn_newTempLob(dpiConn *conn, dpiOracleTypeNum lobType, dpiLob **lob)
{
    const dpiOracleType *type;
    dpiLob *tempLob;
    dpiError error;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!lob)
        return dpiError__set(&error, "check LOB handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "lob");
    switch (lobType) {
        case DPI_ORACLE_TYPE_CLOB:
        case DPI_ORACLE_TYPE_BLOB:
        case DPI_ORACLE_TYPE_NCLOB:
            type = dpiOracleType__getFromNum(lobType, &error);
            break;
        default:
            return dpiError__set(&error, "check lob type",
                    DPI_ERR_INVALID_ORACLE_TYPE, lobType);
    }
    if (dpiLob__allocate(conn, type, &tempLob, &error) < 0)
        return DPI_FAILURE;
    if (dpiLob__createTemporary(tempLob, &error) < 0) {
        dpiLob__free(tempLob, &error);
        return DPI_FAILURE;
    }

    *lob = tempLob;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_newMsgProps() [PUBLIC]
//   Create a new message properties object and return it.
//-----------------------------------------------------------------------------
int dpiConn_newMsgProps(dpiConn *conn, dpiMsgProps **props)
{
    dpiMsgProps *tempProps;
    dpiError error;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!props)
        return dpiError__set(&error, "check message properties handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "props");
    if (dpiGen__allocate(DPI_HTYPE_MSG_PROPS, conn->env, (void**) &tempProps,
            &error) < 0)
        return DPI_FAILURE;
    if (dpiMsgProps__create(tempProps, conn, &error) < 0) {
        dpiMsgProps__free(tempProps, &error);
        return DPI_FAILURE;
    }

    *props = tempProps;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_newSubscription() [PUBLIC]
//   Create a new subscription and return it.
//-----------------------------------------------------------------------------
int dpiConn_newSubscription(dpiConn *conn, dpiSubscrCreateParams *params,
        dpiSubscr **subscr, uint32_t *subscrId)
{
    dpiSubscr *tempSubscr;
    dpiError error;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!subscr)
        return dpiError__set(&error, "check subscription handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "subscr");
    if (dpiGen__allocate(DPI_HTYPE_SUBSCR, conn->env, (void**) &tempSubscr,
            &error) < 0)
        return DPI_FAILURE;
    if (dpiSubscr__create(tempSubscr, conn, params, subscrId, &error) < 0) {
        dpiSubscr__free(tempSubscr, &error);
        return DPI_FAILURE;
    }

    *subscr = tempSubscr;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_newVar() [PUBLIC]
//   Create a new variable and return it.
//-----------------------------------------------------------------------------
int dpiConn_newVar(dpiConn *conn, dpiOracleTypeNum oracleTypeNum,
        dpiNativeTypeNum nativeTypeNum, uint32_t maxArraySize, uint32_t size,
        int sizeIsBytes, int isArray, dpiObjectType *objType, dpiVar **var,
        dpiData **data)
{
    dpiError error;

    *var = NULL;
    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!var)
        return dpiError__set(&error, "check variable handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "var");
    if (!data)
        return dpiError__set(&error, "check data pointer",
                DPI_ERR_NULL_POINTER_PARAMETER, "data");
    return dpiVar__allocate(conn, oracleTypeNum, nativeTypeNum, maxArraySize,
            size, sizeIsBytes, isArray, objType, var, data, &error);
}


//-----------------------------------------------------------------------------
// dpiConn_ping() [PUBLIC]
//   Makes a round trip call to the server to confirm that the connection and
// server are still active.
//-----------------------------------------------------------------------------
int dpiConn_ping(dpiConn *conn)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIPing(conn->handle, error.handle, OCI_DEFAULT);
    return dpiError__check(&error, status, conn, "ping");
}


//-----------------------------------------------------------------------------
// dpiConn_prepareDistribTrans() [PUBLIC]
//   Prepare a distributed transaction for commit. A boolean is returned
// indicating if a commit is actually needed as an attempt to perform a commit
// when nothing is actually prepared results in ORA-24756 (transaction does not
// exist). This is determined by the return value from OCITransPrepare() which
// is OCI_SUCCESS_WITH_INFO if there is no transaction requiring commit.
//-----------------------------------------------------------------------------
int dpiConn_prepareDistribTrans(dpiConn *conn, int *commitNeeded)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCITransPrepare(conn->handle, error.handle, OCI_DEFAULT);
    if (dpiError__check(&error, status, conn, "prepare transaction") < 0)
        return DPI_FAILURE;
    *commitNeeded = (status == OCI_SUCCESS);
    if (*commitNeeded)
        conn->commitMode = OCI_TRANS_TWOPHASE;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_prepareStmt() [PUBLIC]
//   Create a new statement and return it after preparing the specified SQL.
//-----------------------------------------------------------------------------
int dpiConn_prepareStmt(dpiConn *conn, int scrollable, const char *sql,
        uint32_t sqlLength, const char *tag, uint32_t tagLength,
        dpiStmt **stmt)
{
    dpiStmt *tempStmt;
    dpiError error;

    *stmt = NULL;
    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!stmt)
        return dpiError__set(&error, "check statement handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "stmt");
    if (dpiStmt__allocate(conn, scrollable, &tempStmt, &error) < 0)
        return DPI_FAILURE;
    if (dpiStmt__prepare(tempStmt, sql, sqlLength, tag, tagLength,
            &error) < 0) {
        dpiStmt__free(tempStmt, &error);
        return DPI_FAILURE;
    }
    *stmt = tempStmt;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_release() [PUBLIC]
//   Release a reference to the connection.
//-----------------------------------------------------------------------------
int dpiConn_release(dpiConn *conn)
{
    return dpiGen__release(conn, DPI_HTYPE_CONN, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_rollback() [PUBLIC]
//   Rollback the transaction associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_rollback(dpiConn *conn)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCITransRollback(conn->handle, error.handle, OCI_DEFAULT);
    if (dpiError__check(&error, status, conn, "rollback") < 0)
        return DPI_FAILURE;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiConn_setAction() [PUBLIC]
//   Set the action associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setAction(dpiConn *conn, const char *value, uint32_t valueLength)
{
    return dpiConn__setAttributeText(conn, OCI_ATTR_ACTION, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_setClientIdentifier() [PUBLIC]
//   Set the client identifier associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setClientIdentifier(dpiConn *conn, const char *value,
        uint32_t valueLength)
{
    return dpiConn__setAttributeText(conn, OCI_ATTR_CLIENT_IDENTIFIER, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_setClientInfo() [PUBLIC]
//   Set the client info associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setClientInfo(dpiConn *conn, const char *value,
        uint32_t valueLength)
{
    return dpiConn__setAttributeText(conn, OCI_ATTR_CLIENT_INFO, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_setCurrentSchema() [PUBLIC]
//   Set the current schema associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setCurrentSchema(dpiConn *conn, const char *value,
        uint32_t valueLength)
{
    return dpiConn__setAttributeText(conn, OCI_ATTR_CURRENT_SCHEMA, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_setDbOp() [PUBLIC]
//   Set the database operation associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setDbOp(dpiConn *conn, const char *value, uint32_t valueLength)
{
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
    return dpiConn__setAttributeText(conn, OCI_ATTR_DBOP, value, valueLength,
            __func__);
#else
    return dpiConn__setAttributeText(conn, 0, value, valueLength, __func__);
#endif
}


//-----------------------------------------------------------------------------
// dpiConn_setExternalName() [PUBLIC]
//   Set the external name associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setExternalName(dpiConn *conn, const char *value,
        uint32_t valueLength)
{
    return dpiConn__setAttributeText(conn, OCI_ATTR_EXTERNAL_NAME, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_setInternalName() [PUBLIC]
//   Set the internal name associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setInternalName(dpiConn *conn, const char *value,
        uint32_t valueLength)
{
    return dpiConn__setAttributeText(conn, OCI_ATTR_INTERNAL_NAME, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_setModule() [PUBLIC]
//   Set the module associated with the connection.
//-----------------------------------------------------------------------------
int dpiConn_setModule(dpiConn *conn, const char *value, uint32_t valueLength)
{
    return dpiConn__setAttributeText(conn, OCI_ATTR_MODULE, value,
            valueLength, __func__);
}


//-----------------------------------------------------------------------------
// dpiConn_setStmtCacheSize() [PUBLIC]
//   Set the size of the statement cache.
//-----------------------------------------------------------------------------
int dpiConn_setStmtCacheSize(dpiConn *conn, uint32_t cacheSize)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIAttrSet(conn->handle, OCI_HTYPE_SVCCTX, &cacheSize, 0,
            OCI_ATTR_STMTCACHESIZE, error.handle);
    return dpiError__check(&error, status, conn, "set stmt cache size");
}


//-----------------------------------------------------------------------------
// dpiConn_shutdownDatabase() [PUBLIC]
//   Shutdown the database. Note that this must be done in two phases except in
// the situation where the instance is being aborted.
//-----------------------------------------------------------------------------
int dpiConn_shutdownDatabase(dpiConn *conn, dpiShutdownMode mode)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIDBShutdown(conn->handle, error.handle, NULL, mode);
    return dpiError__check(&error, status, conn, "shutdown database");
}


//-----------------------------------------------------------------------------
// dpiConn_startupDatabase() [PUBLIC]
//   Startup the database. This is equivalent to "startup nomount" in SQL*Plus.
//-----------------------------------------------------------------------------
int dpiConn_startupDatabase(dpiConn *conn, dpiStartupMode mode)
{
    dpiError error;
    sword status;

    if (dpiConn__checkConnected(conn, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCIDBStartup(conn->handle, error.handle, NULL, OCI_DEFAULT,
            mode);
    return dpiError__check(&error, status, conn, "startup database");
}

