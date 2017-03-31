//-----------------------------------------------------------------------------
// Copyright (c) 2017 Oracle and/or its affiliates.  All rights reserved.
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
// Test_ValidCredWithoutParams() [PRIVATE]
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and both the dpiCommonParams and dpiPoolCreateParams structures are NULL.
//-----------------------------------------------------------------------------
int Test_ValidCredWithoutParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_ValidCredWithCommonParams() [PRIVATE]
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and dpiCommonParams structure set to NULL.
//-----------------------------------------------------------------------------
int Test_ValidCredWithCommonParams(dpiTestCase *testCase, 
        dpiTestParams *params)
{
    dpiCommonCreateParams commonParams;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_ValidCredWithCreateParams() [PRIVATE]
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and dpiPoolCreateParams structure set to NULL.
//-----------------------------------------------------------------------------
int Test_ValidCredWithCreateParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_ValidCredWithParams() [PRIVATE]
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and both the dpiCommonParams and dpiPoolCreateParams structures are set.
//-----------------------------------------------------------------------------
int Test_ValidCredWithParams(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, &commonParams, 
            &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_InvalidCred() [PRIVATE]
//   Verify that dpiPool_create() fails when invalid credentials are passed.
//-----------------------------------------------------------------------------
int Test_InvalidCred(dpiTestCase *testCase, dpiTestParams *params)
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
// Test_SetMinSessions() [PRIVATE]
//   Verify that dpiPool_create() specifying a non-default value for
// minSessions and confirm that this number of sessions is created (no error).
//-----------------------------------------------------------------------------
int Test_SetMinSessions(dpiTestCase *testCase, dpiTestParams *params)
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

    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return dpiTestCase_expectUintEqual(testCase, count, MINSESSIONS);
}


//-----------------------------------------------------------------------------
// Test_SetMaxSessions() [PRIVATE]
//   Verify that dpiPool_create() specifying a non-default value for 
// maxSessions and confirm that this number of sessions is the maximum that is
// created (no error).
//-----------------------------------------------------------------------------
int Test_SetMaxSessions(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
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
// Test_SessionIncrement() [PRIVATE]
//   Verify that dpiPool_create() specifying a non-default value for 
// sessionIncrement and confirm that this number of sessions is created each
// time more session are needed (no error).
//-----------------------------------------------------------------------------
int Test_SessionIncrement(dpiTestCase *testCase, dpiTestParams *params)
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

    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
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

    return dpiTestCase_expectUintEqual(testCase, count2-count, SESSINCREMENT);
}


//-----------------------------------------------------------------------------
// Test_EnumdpiPoolModeGetNoWait() [PRIVATE]
//   Verify that dpiPool_create() specifying each of the values from the
// enumeration dpiPoolGetMode and verify that the setting behaves as expected
// (no error).
//-----------------------------------------------------------------------------
int Test_EnumdpiPoolModeGetNoWait(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiConn *conn[3];
    dpiPool *pool;
    uint32_t iter;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = MINSESSIONS-1;
    createParams.maxSessions = 2;
    createParams.sessionIncrement = SESSINCREMENT-1;
    createParams.getMode = DPI_MODE_POOL_GET_NOWAIT;

    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
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
        return DPI_SUCCESS;
    }

    for (iter = 0; iter < 3; ++iter) {
        if (dpiConn_release(conn[iter]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    dpiPool_release(pool);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_EnumdpiPoolModeGetForceGet() [PRIVATE]
//   Verify that dpiPool_create() specifying each of the values from the
// enumeration dpiPoolGetMode and verify that the setting behaves as expected
// (no error).
//-----------------------------------------------------------------------------
int Test_EnumdpiPoolModeGetForceGet(dpiTestCase *testCase,
        dpiTestParams *params)
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

    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
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
// Test_EnumdpiPoolGetMode() [PRIVATE]
//   Verify that dpiPool_create() specifying each of the values from the
// enumeration dpiPoolGetMode and verify that the setting behaves as expected
// (no error).
//-----------------------------------------------------------------------------
int Test_EnumdpiPoolGetMode(dpiTestCase *testCase, dpiTestParams *params)
{
    if (Test_EnumdpiPoolModeGetNoWait(testCase, params) < 0)
        return DPI_FAILURE;
    if (Test_EnumdpiPoolModeGetForceGet(testCase, params) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}
                

//-----------------------------------------------------------------------------
// Test_CreateWithNullContext() [PRIVATE]
//   Verify that dpiPool_create() with NULL context (error DPI-1002)
//-----------------------------------------------------------------------------
int Test_CreateWithNullContext(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPool *pool;

    dpiPool_create(NULL, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, NULL,  &pool);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiContext handle");
}


//-----------------------------------------------------------------------------
// Test_PoolReleaseTwice() [PRIVATE]
//   Verify that dpiPool_create() followed by dpiPool_release() twice
// (error DPI-1002).
//-----------------------------------------------------------------------------
int Test_PoolReleaseTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, NULL,  &pool) < 0)
        return DPI_SUCCESS;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_release(pool);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiPool handle");
}


//-----------------------------------------------------------------------------
// Test_PoolPublicFunctionsWithError() [PRIVATE]
//   Call all public functions with the specified pool and expect an error for
// each of them.
//-----------------------------------------------------------------------------
int Test_PoolPublicFunctionsWithError(dpiTestCase *testCase,
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
// Test_DpiPoolFncsCloseDefault() [PRIVATE]
//   Call dpiPool_create() with valid credentials and call dpiPool_close();
// then call all other public dpiPool functions except for dpiPool_addRef() and
// dpiPool_release(DPI_MODE_POOL_CLOSE_DEFAULT) (error DPI-1010).
//-----------------------------------------------------------------------------
int Test_DpiPoolFncsCloseDefault(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_close(pool, DPI_MODE_POOL_CLOSE_DEFAULT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (Test_PoolPublicFunctionsWithError(testCase, params, pool,
            "DPI-1010: not connected") < 0)
        return DPI_FAILURE;
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_DpiPoolFncsCloseForce() [PRIVATE]
//   Call dpiPool_create() with valid credentials and call dpiPool_close();
// then call all other public dpiPool functions except for dpiPool_addRef() and
// dpiPool_release(DPI_MODE_POOL_CLOSE_FORCE) (error DPI-1010).
//-----------------------------------------------------------------------------
int Test_DpiPoolFncsCloseForce(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_close(pool, DPI_MODE_POOL_CLOSE_FORCE) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return Test_PoolPublicFunctionsWithError(testCase, params, pool,
            "DPI-1010: not connected");
}


//-----------------------------------------------------------------------------
// Test_DpiPoolFncs() [PRIVATE]
//   Verify that dpiPool_create() with valid credentials and call 
// dpiPool_close(); then call all other public dpiPool functions except for
// dpiPool_addRef() and dpiPool_release(DPI_MODE_POOL_CLOSE_DEFAULT)
// (error DPI-1010).
//-----------------------------------------------------------------------------
int Test_DpiPoolFncs(dpiTestCase *testCase, dpiTestParams *params)
{
    if (Test_DpiPoolFncsCloseDefault(testCase, params) < 0)
        return DPI_FAILURE;
    if (Test_DpiPoolFncsCloseForce(testCase, params) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_DpiPoolPublicFncs() [PRIVATE]
//   Call each of the public functions for dpiPool with the pool parameter set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int Test_DpiPoolPublicFncs(dpiTestCase *testCase, dpiTestParams *params)
{
    return Test_PoolPublicFunctionsWithError(testCase, params, NULL,
            "DPI-1002: invalid dpiPool handle");
}


//-----------------------------------------------------------------------------
// Test_ProxyAuthWithPool() [PRIVATE]
//   Verify that proxy authentication can be used with a pool (no error).
//-----------------------------------------------------------------------------
int Test_ProxyAuthWithPool(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    char proxyName[150];
    dpiPool *pool;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 0;
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    sprintf(proxyName, "%.*s_PROXY", params->userNameLength, params->userName);
    if (dpiPool_acquireConnection(pool, proxyName, strlen(proxyName),
            params->password, params->passwordLength, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_ProxyAuthWithHomogeneousPool() [PRIVATE]
//   Verify that proxy authentication cannot be used with a homogeneous pool
// (error DPI-1012).
//-----------------------------------------------------------------------------
int Test_ProxyAuthWithHomogeneousPool(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 1;
    if (dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    dpiPool_acquireConnection(pool, "X", 1, params->password,
            params->passwordLength, NULL, &conn);
    if (dpiTestCase_expectError(testCase,
            "DPI-1012: proxy authentication is not possible with homogeneous "
            "pools") < 0)
        return DPI_FAILURE;
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_ValidCredWithNullPool() [PRIVATE]
//   Call dpiPool_create with valid credentials and NULL pool.
//-----------------------------------------------------------------------------
int Test_ValidCredWithNullPool(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, NULL, NULL, NULL);
    return dpiTestCase_expectError(testCase,
            "DPI-1046: parameter pool cannot be a NULL pointer");
}


//-----------------------------------------------------------------------------
// Test_ValidCredWithNullArgs() [PRIVATE]
//   Call dpiPool_create with valid credentials and NULL args.
//-----------------------------------------------------------------------------
int Test_ValidCredWithNullArgs(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, &pool);
    return dpiTestCase_expectError(testCase,
            "ORA-24415: Missing or null username.");
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(500);
    dpiTestSuite_addCase(Test_ValidCredWithoutParams,
            "dpiPool_create() with valid credentials, no parameters");
    dpiTestSuite_addCase(Test_ValidCredWithCommonParams,
            "dpiPool_create() with valid credentials and common params");
    dpiTestSuite_addCase(Test_ValidCredWithCreateParams,
            "dpiPool_create() with valid credentials and create params");
    dpiTestSuite_addCase(Test_ValidCredWithParams, 
            "dpiPool_create() with valid credentials and both params");
    dpiTestSuite_addCase(Test_InvalidCred,
            "dpiPool_create() with invalid credentials fails");
    dpiTestSuite_addCase(Test_SetMinSessions,
            "dpiPool_create() with Min Sessions");
    dpiTestSuite_addCase(Test_SetMaxSessions,
            "dpiPool_create() with Max Sessions");
    dpiTestSuite_addCase(Test_SessionIncrement,
            "dpiPool_create() with Session Increment");
    dpiTestSuite_addCase(Test_EnumdpiPoolGetMode,
            "dpiPool_create() verify the settings behaves as expected");
    dpiTestSuite_addCase(Test_CreateWithNullContext,
            "dpiPool_create() with NULL context");
    dpiTestSuite_addCase(Test_PoolReleaseTwice,
            "dpiPool_create() and release twice");
    dpiTestSuite_addCase(Test_DpiPoolFncs,
            "call all pool functions with closed pool");
    dpiTestSuite_addCase(Test_DpiPoolPublicFncs,
            "call all pool functions with NULL pool");
    dpiTestSuite_addCase(Test_ProxyAuthWithPool,
            "proxy authentication with pool");
    dpiTestSuite_addCase(Test_ProxyAuthWithHomogeneousPool,
            "proxy authentication cannot be used with homogeneous pool");
    dpiTestSuite_addCase(Test_ValidCredWithNullPool,
            "dpiPool_create() with NULL pool");
    dpiTestSuite_addCase(Test_ValidCredWithNullArgs,
            "dpiPool_create() with NULL args");
    return dpiTestSuite_run();
}

