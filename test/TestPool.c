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
// TestPool.c
//   Test suite for testing dpiPool functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define MINSESSIONS 2
#define MAXSESSIONS 9
#define SESSINCREMENT 2

//-----------------------------------------------------------------------------
// dpiTest__callFunctionsWithError() [INTERNAL]
//   Call all public functions with the specified pool and expect an error for
// each of them.
//-----------------------------------------------------------------------------
int dpiTest__callFunctionsWithError(dpiTestCase *testCase,
        dpiTestParams *params, dpiPool *pool, const char *expectedError)
{
    dpiEncodingInfo info;
    dpiPoolGetMode value;
    uint32_t count;
    dpiConn *conn;

    dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_close(pool, DPI_MODE_POOL_CLOSE_DEFAULT);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getBusyCount(pool, &count);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getEncodingInfo(pool, &info);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getGetMode(pool, &value);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getMaxLifetimeSession(pool, &count);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getOpenCount(pool, &count);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getTimeout(pool, &count);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_setGetMode(pool, value);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_setMaxLifetimeSession(pool, 5);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_setTimeout(pool, 5);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__acquireAndVerifySession() [INTERNAL]
//   Acquire a session from the pool using the given credentials and then
// verify that the session user and proxy user are the expected values.
//-----------------------------------------------------------------------------
static int dpiTest__acquireAndVerifySession(dpiTestCase *testCase,
        dpiPool *pool, const char *userName, uint32_t userNameLength,
        const char *password, uint32_t passwordLength,
        const char *expectedSessionUser, uint32_t expectedSessionUserLength,
        const char *proxyUser, uint32_t proxyUserLength)
{
    const char *sql =
            "select sys_context('userenv', 'session_user'), "
            "sys_context('userenv', 'proxy_user') from dual";
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiData *getValue;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // acquire connection from the pool
    if (dpiPool_acquireConnection(pool, userName, userNameLength, password,
            passwordLength, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch session user and proxy user
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue->value.asBytes.ptr,
            getValue->value.asBytes.length, expectedSessionUser,
            expectedSessionUserLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue->value.asBytes.ptr,
            getValue->value.asBytes.length, proxyUser, proxyUserLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_500_withoutParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and both the dpiCommonParams and dpiPoolCreateParams structures are NULL.
//-----------------------------------------------------------------------------
int dpiTest_500_withoutParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_501_withCommonParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and dpiCommonParams structure set to NULL.
//-----------------------------------------------------------------------------
int dpiTest_501_withCommonParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiCommonCreateParams commonParams;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_502_withCreateParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and dpiPoolCreateParams structure set to NULL.
//-----------------------------------------------------------------------------
int dpiTest_502_withCreateParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_503_withBothParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and both the dpiCommonParams and dpiPoolCreateParams structures are set.
//-----------------------------------------------------------------------------
int dpiTest_503_withBothParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiCommonCreateParams commonParams;
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams,
            &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_504_invalidCred()
//   Verify that dpiPool_create() fails when invalid credentials are passed.
//-----------------------------------------------------------------------------
int dpiTest_504_invalidCred(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, "X", 1, "X", 1, params->connectString,
            params->connectStringLength, NULL, NULL, &pool);
    return dpiTestCase_expectError(testCase,
            "ORA-01017: invalid username/password; logon denied");
}


//-----------------------------------------------------------------------------
// dpiTest_505_setMinSessions()
//   Verify that dpiPool_create() specifying a non-default value for
// minSessions and confirm that this number of sessions is created (no error).
//-----------------------------------------------------------------------------
int dpiTest_505_setMinSessions(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    uint32_t count;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = MINSESSIONS;
    createParams.maxSessions = MAXSESSIONS;
    createParams.sessionIncrement = SESSINCREMENT;

    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return dpiTestCase_expectUintEqual(testCase, count, MINSESSIONS);
}


//-----------------------------------------------------------------------------
// dpiTest_506_setMaxSessions()
//   Verify that dpiPool_create() specifying a non-default value for
// maxSessions and confirm that this number of sessions is the maximum that is
// created (no error).
//-----------------------------------------------------------------------------
int dpiTest_506_setMaxSessions(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn[MAXSESSIONS], *tempConn;
    dpiPoolCreateParams createParams;
    dpiContext *context;
    uint32_t count, i;
    dpiPool *pool;

    // create pool with a maximum session value set
    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = MINSESSIONS;
    createParams.maxSessions = MAXSESSIONS;
    createParams.sessionIncrement = SESSINCREMENT;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create connections up to the maximum
    for (i = 0; i < MAXSESSIONS; i++) {
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                &conn[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count, MAXSESSIONS) < 0)
        return DPI_FAILURE;

    // attempt to acquire one more connection
    dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &tempConn);
    if (dpiTestCase_expectError(testCase,
            "ORA-24418: Cannot open further sessions.") < 0)
        return DPI_FAILURE;

    // close connections that were created
    for (i = 0; i < MAXSESSIONS; i++)
        dpiConn_release(conn[i]);
    dpiPool_release(pool);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_507_setSessionIncr()
//   Verify that dpiPool_create() specifying a non-default value for
// sessionIncrement and confirm that this number of sessions is created each
// time more session are needed (no error).
//-----------------------------------------------------------------------------
int dpiTest_507_setSessionIncr(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiConn *conn[MINSESSIONS + 1];
    uint32_t count, iter, count2;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = MINSESSIONS;
    createParams.maxSessions = MAXSESSIONS;
    createParams.sessionIncrement = SESSINCREMENT;

    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    for (iter = 0; iter < MINSESSIONS; ++iter) {
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                &conn[iter]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_acquireConnection(pool, NULL, 0,
                    NULL, 0, NULL, &conn[iter]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_getOpenCount(pool, &count2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    for(iter = 0; iter < MINSESSIONS+1; ++iter) {
        if (dpiConn_release(conn[iter]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return dpiTestCase_expectUintEqual(testCase, count2 - count,
            SESSINCREMENT);
}


//-----------------------------------------------------------------------------
// dpiTest_508_getModeNoWait()
//   Verify that dpiPool_create() specifying no wait for the get mode works as
// expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_508_getModeNoWait(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiConn *conn[3];
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = MINSESSIONS-1;
    createParams.maxSessions = 2;
    createParams.sessionIncrement = SESSINCREMENT-1;
    createParams.getMode = DPI_MODE_POOL_GET_NOWAIT;

    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn[2]);
    if (dpiTestCase_expectError(testCase,
            "ORA-24418: Cannot open further sessions.") < 0)
        return DPI_FAILURE;
    if (dpiConn_release(conn[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_509_getModeForceGet()
//   Verify that dpiPool_create() specifying force get for the get mode works
// as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_509_getModeForceGet(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiConn *conn[3];
    dpiPool *pool;
    uint32_t iter;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = MINSESSIONS - 1;
    createParams.maxSessions = 2;
    createParams.sessionIncrement = SESSINCREMENT - 1;
    createParams.getMode = DPI_MODE_POOL_GET_FORCEGET;

    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn[0]) < 0) {
        if (dpiPool_release(pool) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        return dpiTestCase_setFailedFromError(testCase);
    }

    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn[1]) < 0) {
        if (dpiPool_release(pool) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        return dpiTestCase_setFailedFromError(testCase);
    }

    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn[2]) < 0) {
        if (dpiPool_release(pool) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        return dpiTestCase_setFailedFromError(testCase);
    }

    for (iter = 0; iter < 3; ++iter) {
        if (dpiConn_release(conn[iter]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_510_createWithNullContext()
//   Verify that dpiPool_create() with NULL context (error DPI-1002)
//-----------------------------------------------------------------------------
int dpiTest_510_createWithNullContext(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiPool *pool;

    dpiPool_create(NULL, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            params->connectString, params->connectStringLength, NULL, NULL,
            &pool);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiContext handle");
}


//-----------------------------------------------------------------------------
// dpiTest_511_releaseTwice()
//   Verify that dpiPool_create() followed by dpiPool_release() twice
// (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_511_releaseTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL,  &pool) < 0)
        return DPI_SUCCESS;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_release(pool);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiPool handle");
}


//-----------------------------------------------------------------------------
// dpiTest_512_callWithClosedPool()
//   Call dpiPool_create() with valid credentials and call dpiPool_close();
// then call all other public dpiPool functions except for dpiPool_addRef() and
// dpiPool_release() (error DPI-1010).
//-----------------------------------------------------------------------------
int dpiTest_512_callWithClosedPool(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_close(pool, DPI_MODE_POOL_CLOSE_DEFAULT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__callFunctionsWithError(testCase, params, pool,
            "DPI-1010: not connected") < 0)
        return DPI_FAILURE;
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_513_callWithNullPool()
//   Call each of the public functions for dpiPool with the pool parameter set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_513_callWithNullPool(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__callFunctionsWithError(testCase, params, NULL,
            "DPI-1002: invalid dpiPool handle");
}


//-----------------------------------------------------------------------------
// dpiTest_514_proxyAuthHeteroPool()
//   Verify that proxy authentication can be used with a pool (no error).
//-----------------------------------------------------------------------------
int dpiTest_514_proxyAuthHeteroPool(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 0;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__acquireAndVerifySession(testCase, pool, params->proxyUserName,
            params->proxyUserNameLength, NULL, 0, params->proxyUserName,
            params->proxyUserNameLength, params->mainUserName,
            params->mainUserNameLength) < 0)
        return DPI_FAILURE;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_515_proxyAuthHomoPool()
//   Verify that proxy authentication cannot be used with a homogeneous pool
// (error DPI-1012).
//-----------------------------------------------------------------------------
int dpiTest_515_proxyAuthHomoPool(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 1;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_acquireConnection(pool, params->proxyUserName,
            params->proxyUserNameLength, NULL, 0, NULL, &conn);
    if (dpiTestCase_expectError(testCase,
            "DPI-1012: proxy authentication is not possible with homogeneous "
            "pools") < 0)
        return DPI_FAILURE;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_516_createWithNull()
//   Call dpiPool_create with valid credentials and NULL pool (error DPI-1046).
//-----------------------------------------------------------------------------
int dpiTest_516_createWithNull(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            params->connectString, params->connectStringLength, NULL, NULL,
            NULL);
    return dpiTestCase_expectError(testCase,
            "DPI-1046: parameter pool cannot be a NULL pointer");
}


//-----------------------------------------------------------------------------
// dpiTest_517_createNoCred()
//   Call dpiPool_create with no credentials (error ORA-24415).
//-----------------------------------------------------------------------------
int dpiTest_517_createNoCred(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, &pool);
    return dpiTestCase_expectError(testCase,
            "ORA-24415: Missing or null username.");
}


//-----------------------------------------------------------------------------
// dpiTest_518_invalidConnStr()
//   Verify that dpiPool_create() fails when an invalid connect string
// is passed (error ORA-12154).
//-----------------------------------------------------------------------------
int dpiTest_518_invalidConnStr(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *connectString = "an_invalid_connect_string";
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            connectString, strlen(connectString), NULL, NULL, &pool);
    return dpiTestCase_expectError(testCase,
            "ORA-12154: TNS:could not resolve the connect identifier "
            "specified");
}


//-----------------------------------------------------------------------------
// dpiTest_519_heteroPoolWithCredentials()
//   Create a heterogeneous pool with credentials and verify that acquiring
// connections from it works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_519_heteroPoolWithCredentials(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    uint32_t userNameLength;
    dpiContext *context;
    char *userName;
    dpiPool *pool;

    // create heterogeneous pool with credentials specified
    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 0;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquiring connection without password
    if (dpiTest__acquireAndVerifySession(testCase, pool, NULL, 0, NULL, 0,
            params->mainUserName, params->mainUserNameLength, NULL, 0) < 0)
        return DPI_FAILURE;

    // acquiring connection with the same user name and pasword specified
    if (dpiTest__acquireAndVerifySession(testCase, pool, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->mainUserName,
            params->mainUserNameLength, NULL, 0) < 0)
        return DPI_FAILURE;

    // acquiring connection with a different user name and pasword specified
    if (dpiTest__acquireAndVerifySession(testCase, pool, params->proxyUserName,
            params->proxyUserNameLength, params->proxyPassword,
            params->proxyPasswordLength, params->proxyUserName,
            params->proxyUserNameLength, NULL, 0) < 0)
        return DPI_FAILURE;

    // acquiring connection with proxy syntax
    userNameLength = params->mainUserNameLength + params->proxyUserNameLength +
            2;
    userName = malloc(userNameLength + 1);
    if (!userName)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    sprintf(userName, "%.*s[%.*s]", params->mainUserNameLength,
            params->mainUserName, params->proxyUserNameLength,
            params->proxyUserName);
    if (dpiTest__acquireAndVerifySession(testCase, pool, userName,
            userNameLength, params->mainPassword, params->mainPasswordLength,
            params->proxyUserName, params->proxyUserNameLength,
            params->mainUserName, params->mainUserNameLength) < 0)
        return DPI_FAILURE;
    free(userName);

    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_520_heteroPoolNoCredentials()
//   Create a heterogeneous pool with no credentials and verify that acquiring
// connections from it works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_520_heteroPoolNoCredentials(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    uint32_t userNameLength;
    dpiContext *context;
    char *userName;
    dpiPool *pool;

    // create heterogeneous pool with no credentials specified
    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 0;
    if (dpiPool_create(context, NULL, 0, NULL, 0, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquiring connection with the main user name and pasword specified
    if (dpiTest__acquireAndVerifySession(testCase, pool, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->mainUserName,
            params->mainUserNameLength, NULL, 0) < 0)
        return DPI_FAILURE;

    // acquiring connection with the proxy user name and pasword specified
    if (dpiTest__acquireAndVerifySession(testCase, pool, params->proxyUserName,
            params->proxyUserNameLength, params->proxyPassword,
            params->proxyPasswordLength, params->proxyUserName,
            params->proxyUserNameLength, NULL, 0) < 0)
        return DPI_FAILURE;

    // acquiring connection with proxy syntax
    userNameLength = params->mainUserNameLength + params->proxyUserNameLength +
            2;
    userName = malloc(userNameLength + 1);
    if (!userName)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    sprintf(userName, "%.*s[%.*s]", params->mainUserNameLength,
            params->mainUserName, params->proxyUserNameLength,
            params->proxyUserName);
    if (dpiTest__acquireAndVerifySession(testCase, pool, userName,
            userNameLength, params->mainPassword, params->mainPasswordLength,
            params->proxyUserName, params->proxyUserNameLength,
            params->mainUserName, params->mainUserNameLength) < 0)
        return DPI_FAILURE;
    free(userName);

    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_521_heteroPoolAcquireWithInvalidCredentials()
//   Verify that attempting to acquire a connection from a heterogeneous pool
// with invalid or missing credentials fails as expected (ORA-24415, ORA-01017,
// and ORA-24419).
//-----------------------------------------------------------------------------
int dpiTest_521_heteroPoolAcquireWithInvalidCredentials(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *invalidPassword = "an_invalid_password";
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;
    dpiConn *conn;

    // create heterogeneous pool without credentials
    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 0;
    if (dpiPool_create(context, NULL, 0, NULL, 0, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire connection without specifying credentials
    dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn);
    if (dpiTestCase_expectError(testCase,
            "ORA-24415: Missing or null username.") < 0)
        return DPI_FAILURE;

    // acquire connection with invalid password
    dpiPool_acquireConnection(pool, params->mainUserName,
            params->mainUserNameLength, invalidPassword,
            strlen(invalidPassword), NULL, &conn);
    if (dpiTestCase_expectError(testCase,
            "ORA-01017: invalid username/password; logon denied") < 0)
        return DPI_FAILURE;

    // acquire connection with user name but no password
    dpiPool_acquireConnection(pool, params->proxyUserName,
            params->proxyUserNameLength, NULL, 0, NULL, &conn);
    if (dpiTestCase_expectError(testCase,
            "ORA-24419: Proxy sessions are not supported in this mode.") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(500);
    dpiTestSuite_addCase(dpiTest_500_withoutParams,
            "dpiPool_create() with valid credentials, no parameters");
    dpiTestSuite_addCase(dpiTest_501_withCommonParams,
            "dpiPool_create() with valid credentials and common params");
    dpiTestSuite_addCase(dpiTest_502_withCreateParams,
            "dpiPool_create() with valid credentials and create params");
    dpiTestSuite_addCase(dpiTest_503_withBothParams,
            "dpiPool_create() with valid credentials and both params");
    dpiTestSuite_addCase(dpiTest_504_invalidCred,
            "dpiPool_create() with invalid credentials fails");
    dpiTestSuite_addCase(dpiTest_505_setMinSessions,
            "dpiPool_create() with Min Sessions");
    dpiTestSuite_addCase(dpiTest_506_setMaxSessions,
            "dpiPool_create() with Max Sessions");
    dpiTestSuite_addCase(dpiTest_507_setSessionIncr,
            "dpiPool_create() with Session Increment");
    dpiTestSuite_addCase(dpiTest_508_getModeNoWait,
            "dpiPool_create() with get mode set to no wait");
    dpiTestSuite_addCase(dpiTest_509_getModeForceGet,
            "dpiPool_create() with get mode set to force get");
    dpiTestSuite_addCase(dpiTest_510_createWithNullContext,
            "dpiPool_create() with NULL context");
    dpiTestSuite_addCase(dpiTest_511_releaseTwice,
            "dpiPool_create() and release twice");
    dpiTestSuite_addCase(dpiTest_512_callWithClosedPool,
            "call all pool functions with closed pool");
    dpiTestSuite_addCase(dpiTest_513_callWithNullPool,
            "call all pool functions with NULL pool");
    dpiTestSuite_addCase(dpiTest_514_proxyAuthHeteroPool,
            "proxy authentication with pool");
    dpiTestSuite_addCase(dpiTest_515_proxyAuthHomoPool,
            "proxy authentication cannot be used with homogeneous pool");
    dpiTestSuite_addCase(dpiTest_516_createWithNull,
            "dpiPool_create() with NULL pool");
    dpiTestSuite_addCase(dpiTest_517_createNoCred,
            "dpiPool_create() with no credentials");
    dpiTestSuite_addCase(dpiTest_518_invalidConnStr,
            "dpiPool_create() with invalid connect string");
    dpiTestSuite_addCase(dpiTest_519_heteroPoolWithCredentials,
            "dpiPool_acquireConnection() from hetero pool with credentials");
    dpiTestSuite_addCase(dpiTest_520_heteroPoolNoCredentials,
            "dpiPool_acquireConnection() from hetero pool without credentials");
    dpiTestSuite_addCase(dpiTest_521_heteroPoolAcquireWithInvalidCredentials,
            "dpiPool_acquireConnection() from hetero pool invalid credentials");
    return dpiTestSuite_run();
}

