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

    return dpiTestCase_expectUintEqual(testCase, count2-count, SESSINCREMENT);
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
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.homogeneous = 0;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_acquireConnection(pool, params->proxyUserName,
            params->proxyUserNameLength, params->proxyPassword,
            params->proxyPasswordLength, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
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

    dpiPool_acquireConnection(pool, "X", 1, params->mainPassword,
            params->mainPasswordLength, NULL, &conn);
    if (dpiTestCase_expectError(testCase,
            "DPI-1012: proxy authentication is not possible with homogeneous "
            "pools") < 0)
        return DPI_FAILURE;
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_516_createWithNull()
//   Call dpiPool_create with valid credentials and NULL pool.
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
//   Call dpiPool_create with no credentials.
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
    return dpiTestSuite_run();
}

