//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestConn.c
//   Test suite for testing dpiConn functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__callFunctionsWithError() [INTERNAL]
//   Test all public functions with the specified connection and expect an
// error for each one of them.
//-----------------------------------------------------------------------------
int dpiTest__callFunctionsWithError(dpiTestCase *testCase,
        dpiTestParams *params, dpiConn *conn, const char *expectedError)
{
    uint32_t msgIdLength, valueLength, releaseStringLength, cacheSize;
    const char *sql = "SELECT count(*) FROM TestNumbers";
    const char *msgId, *value, *releaseString;
    dpiSubscrCreateParams subscrParams;
    dpiVersionInfo versionInfo;
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiEncodingInfo info;
    dpiMsgProps *props;
    dpiSubscr *subscr;
    int commitNeeded;
    dpiData *data;
    dpiStmt *stmt;
    void *handle;
    dpiLob *lob;
    dpiVar *var;

    dpiConn_beginDistribTrans(conn, -1, NULL, 0, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_breakExecution(conn);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_changePassword(conn, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, "X", 1);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_commit(conn);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_deqObject(conn, "X", 1, NULL, NULL, NULL, &msgId, &msgIdLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_enqObject(conn, "X", 1, NULL, NULL, NULL, &msgId, &msgIdLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getCurrentSchema(conn, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getEdition(conn, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getEncodingInfo(conn, &info);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getExternalName(conn, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getHandle(conn, &handle);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getInternalName(conn, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getLTXID(conn, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getObjectType(conn, NULL, 0, &objType);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getServerVersion(conn, &releaseString, &releaseStringLength,
            &versionInfo);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_getStmtCacheSize(conn, &cacheSize);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_newDeqOptions(conn, &deqOptions);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_newEnqOptions(conn, &enqOptions);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_newMsgProps(conn, &props);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_subscribe(conn, &subscrParams, &subscr);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_NUMBER, &lob);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64, 5,
            50000, 0, 0, NULL, &var, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_ping(conn);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_prepareDistribTrans(conn, &commitNeeded);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_rollback(conn);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setAction(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setClientIdentifier(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setClientInfo(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setCurrentSchema(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setDbOp(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setExternalName(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setInternalName(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setModule(conn, value, valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_setStmtCacheSize(conn, 5);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_shutdownDatabase(conn, DPI_MODE_SHUTDOWN_DEFAULT);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiConn_startupDatabase(conn, DPI_MODE_STARTUP_DEFAULT);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_300_createNoParams()
//   Verify that dpiConn_create() succeeds when valid credentials are passed
// and both the dpiCommonParams and dpiConnCreateParams structures are NULL.
//-----------------------------------------------------------------------------
int dpiTest_300_createNoParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_release(conn);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_301_invalidCredentials()
//   Verify that dpiConn_create() fails when invalid credentials are passed.
//-----------------------------------------------------------------------------
int dpiTest_301_invalidCredentials(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiContext *context;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    dpiConn_create(context, "X", 1, "X", 1, params->connectString,
            params->connectStringLength, NULL, NULL, &conn);
    return dpiTestCase_expectError(testCase,
            "ORA-01017: invalid username/password; logon denied");
}


//-----------------------------------------------------------------------------
// dpiTest_302_createWithParams()
//   Call dpiConn_create() with valid credentials and both the parameters
// commonParams and createParams initialize to all default values (no error).
//-----------------------------------------------------------------------------
int dpiTest_302_createWithParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiCommonCreateParams commonParams;
    dpiConnCreateParams createParams;
    dpiContext *context;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initConnCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, &createParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_release(conn);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_303_createExternal()
//   Call dpiConn_create() with valid credentials; call dpiConn_getHandle()
// to get the handle of the connection; call dpiConn_create() with
// createParams->externalHandle set to that value; perform queries on both
// connections to verify that they both work (no error).
//-----------------------------------------------------------------------------
int dpiTest_303_createExternal(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "select count(*) from TestNumbers";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiConnCreateParams createParams;
    dpiNativeTypeNum nativeTypeNum;
    dpiConn *conn, *conn2;
    dpiContext *context;
    uint64_t count;
    dpiData *data;
    dpiStmt *stmt;
    void *handle;
    int found;

    // create first connection and get handle
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getHandle(conn, &handle) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create second connection using handle of first connection
    dpiTestSuite_getContext(&context);
    if (dpiContext_initConnCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.externalHandle = handle;
    if (dpiConn_create(context, NULL, 0, NULL, 0, NULL, 0, NULL, &createParams,
            &conn2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform query on first connection
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    count = data->value.asUint64;
    dpiStmt_release(stmt);

    // perform query on second connection
    if (dpiConn_prepareStmt(conn2, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, data->value.asUint64, count) < 0)
        return DPI_FAILURE;

    dpiStmt_release(stmt);
    dpiConn_release(conn2);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_304_externalClose()
//   Call dpiConn_create() with valid credentials; call dpiConn_getHandle()
// to get the handle of the connection; call dpiConn_create() with
// createParams->externalHandle set to that value; call dpiConn_close() on the
// second connection (error DPI-1034).
//-----------------------------------------------------------------------------
int dpiTest_304_externalClose(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConnCreateParams createParams;
    dpiConn *conn, *conn2;
    dpiContext *context;
    void *handle;

    // create first connection and get handle
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getHandle(conn, &handle) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create second connection using handle of first connection
    dpiTestSuite_getContext(&context);
    if (dpiContext_initConnCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.externalHandle = handle;
    if (dpiConn_create(context, NULL, 0, NULL, 0, NULL, 0, NULL, &createParams,
            &conn2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // attempt to close second connection
    dpiConn_close(conn2, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0);
    if (dpiTestCase_expectError(testCase,
            "DPI-1034: connection created from external handle cannot "
            "be closed") < 0)
        return DPI_FAILURE;

    dpiConn_release(conn2);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_305_createValidPool()
//   Call dpiConn_create() specifying a valid pool handle in
// createParams->pool (no error).
//-----------------------------------------------------------------------------
int dpiTest_305_createValidPool(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConnCreateParams createParams;
    dpiContext *context;
    dpiConn *conn;
    dpiPool *pool;

    // create pool
    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create connection from pool
    if (dpiContext_initConnCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.pool = pool;
    if (dpiConn_create(context, NULL, 0, NULL, 0, NULL, 0, NULL, &createParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_release(conn);
    dpiPool_release(pool);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_306_createInvalidPool()
//   Call dpiConn_create() specifying an invalid pool handle in
// createParams->pool (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_306_createInvalidPool(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConnCreateParams createParams;
    dpiContext *context;
    dpiConn *conn;
    dpiPool *pool;

    // create and immediately destroy pool
    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_release(pool);

    // attempt to create connection using destroyed pool
    if (dpiContext_initConnCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.pool = pool;
    dpiConn_create(context, NULL, 0, NULL, 0, NULL, 0, NULL, &createParams,
            &conn);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiPool handle");
}


//-----------------------------------------------------------------------------
// dpiTest_307_callWithClosedConn()
//   Call dpiConn_create() with valid credentials; call dpiConn_close()
// and then call each of the public functions for dpiConn except for
// dpiConn_addRef() and dpiConn_release() (error DPI-1010).
//-----------------------------------------------------------------------------
int dpiTest_307_callWithClosedConn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__callFunctionsWithError(testCase, params, conn,
            "DPI-1010: not connected");
}


//-----------------------------------------------------------------------------
// dpiTest_308_createNullContext()
//   Call dpiConn_create() with NULL context (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_308_createNullContext(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;

    dpiConn_create(NULL, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            params->connectString, params->connectStringLength, NULL, NULL,
            &conn);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiContext handle");
}


//-----------------------------------------------------------------------------
// dpiTest_309_createReleaseTwice()
//   Call dpiConn_create() with valid credentials; call dpiConn_release()
// twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_309_createReleaseTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    testCase->conn = NULL;
    dpiConn_release(conn);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiConn handle");
}


//-----------------------------------------------------------------------------
// dpiTest_310_callWithNullConn()
//   Call each of the public functions for dpiConn with the connection
// parameter set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_310_callWithNullConn(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__callFunctionsWithError(testCase, params, NULL,
            "DPI-1002: invalid dpiConn handle");
}


//-----------------------------------------------------------------------------
// dpiTest_311_acquireConnNullPool()
//   Call dpiPool_acquireConnection() with pool set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_311_acquireConnNullPool(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;

    dpiPool_acquireConnection(NULL, NULL, 0, NULL, 0, NULL, &conn);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiPool handle");
}


//-----------------------------------------------------------------------------
// dpiTest_312_acquireConn()
//   Call dpiPool_acquireConnection() with pool created using the common
// session pool creation method (no error).
//-----------------------------------------------------------------------------
int dpiTest_312_acquireConn(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiConn *conn;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                    &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_313_createWithNewPassword()
//   Call dpiConn_create() and specify a new password in the
// dpiConnCreateParams structure; create a new connection using the new
// password to verify that the password was indeed changed (no error).
//-----------------------------------------------------------------------------
int dpiTest_313_createWithNewPassword(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConnCreateParams createParams;
    const char *newPassword = "abc";
    dpiContext *context;
    dpiConn *conn;

    // create connection and change password at the same time
    dpiTestSuite_getContext(&context);
    if (dpiContext_initConnCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.newPassword = newPassword;
    createParams.newPasswordLength = strlen(newPassword);
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_release(conn);

    // create connection and change password back to original password
    createParams.newPassword = params->mainPassword;
    createParams.newPasswordLength = params->mainPasswordLength;
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, newPassword, strlen(newPassword),
            params->connectString, params->connectStringLength, NULL,
            &createParams, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_release(conn);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_314_createWithAppContext()
//   Call dpiConn_create() and specify application context in the
// dpiConnCreateParams structure; verify that the application context was
// indeed set correctly (no error).
//-----------------------------------------------------------------------------
int dpiTest_314_createWithAppContext(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select sys_context(:1, :2) from dual";
    dpiData *namespaceData, *keyData, *valueData;
    dpiVar *namespaceVar, *keyVar, *valueVar;
    uint32_t numQueryColumns, bufferRowIndex;
    dpiConnCreateParams createParams;
    dpiAppContext appContext;
    dpiContext *context;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // initialize application context
    appContext.namespaceName = "E2E_CONTEXT";
    appContext.namespaceNameLength = strlen(appContext.namespaceName);
    appContext.name = "MY_NAME";
    appContext.nameLength = strlen(appContext.name);
    appContext.value = "MY_VAL";
    appContext.valueLength = strlen(appContext.value);

    // create connection using application context
    dpiTestSuite_getContext(&context);
    if (dpiContext_initConnCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.appContext = &appContext;
    createParams.numAppContext = 1;
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform query to determine if application context was set correctly
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            30, 1, 0, NULL, &namespaceVar, &namespaceData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            30, 1, 0, NULL, &keyVar, &keyData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            30, 1, 0, NULL, &valueVar, &valueData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(namespaceVar, 0, appContext.namespaceName,
            appContext.namespaceNameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(keyVar, 0, appContext.name,
            appContext.nameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, namespaceVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, keyVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_define(stmt, 1, valueVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, valueData->value.asBytes.ptr,
            valueData->value.asBytes.length, appContext.value,
            appContext.valueLength) < 0)
        return DPI_FAILURE;

    dpiVar_release(namespaceVar);
    dpiVar_release(keyVar);
    dpiVar_release(valueVar);
    dpiStmt_release(stmt);
    dpiConn_release(conn);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_315_createAndCloseTwice()
//   Call dpiConn_create() and close connection twice expected error.
//-----------------------------------------------------------------------------
int dpiTest_315_createAndCloseTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0);
    return dpiTestCase_expectError(testCase, "DPI-1010: not connected");
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(300);
    dpiTestSuite_addCase(dpiTest_300_createNoParams,
            "dpiConn_create() with valid credentials, no parameters");
    dpiTestSuite_addCase(dpiTest_301_invalidCredentials,
            "dpiConn_create() with invalid credentials fails");
    dpiTestSuite_addCase(dpiTest_302_createWithParams,
            "dpiConn_create() with valid credentials and params");
    dpiTestSuite_addCase(dpiTest_303_createExternal,
            "dpiConn_create() with external handle");
    dpiTestSuite_addCase(dpiTest_304_externalClose,
            "dpiConn_create() with external handle and close");
    dpiTestSuite_addCase(dpiTest_305_createValidPool,
            "dpiConn_create() with valid pool handle");
    dpiTestSuite_addCase(dpiTest_306_createInvalidPool,
            "dpiConn_create() with an invalid pool handle");
    dpiTestSuite_addCase(dpiTest_307_callWithClosedConn,
            "call connection functions with closed connection");
    dpiTestSuite_addCase(dpiTest_308_createNullContext,
            "dpiConn_create() with NULL context");
    dpiTestSuite_addCase(dpiTest_309_createReleaseTwice,
            "dpiConn_create() with dpiConn_release() twice");
    dpiTestSuite_addCase(dpiTest_310_callWithNullConn,
            "call connection functions with NULL connection");
    dpiTestSuite_addCase(dpiTest_311_acquireConnNullPool,
            "dpiPool_acquireConnection() with NULL pool");
    dpiTestSuite_addCase(dpiTest_312_acquireConn,
            "dpiPool_acquireConnection() with valid pool");
    dpiTestSuite_addCase(dpiTest_313_createWithNewPassword,
            "dpiConn_create() with new password");
    dpiTestSuite_addCase(dpiTest_314_createWithAppContext,
            "dpiConn_create() with application context");
    dpiTestSuite_addCase(dpiTest_315_createAndCloseTwice,
            "dpiConn_create() and call dpiConn_close() twice");
    return dpiTestSuite_run();
}

