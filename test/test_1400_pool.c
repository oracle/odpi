//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2022, Oracle and/or its affiliates.
//
// This software is dual-licensed to you under the Universal Permissive License
// (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
// 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
// either license.
//
// If you elect to accept the software under the Apache License, Version 2.0,
// the following applies:
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// test_1400_pool.c
//   Test suite for testing dpiPool functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

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
    int pingInterval;
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

    dpiPool_setMaxSessionsPerShard(pool, 5);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getMaxSessionsPerShard(pool, &count);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_setPingInterval(pool, 30);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_getPingInterval(pool, &pingInterval);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    dpiPool_reconfigure(pool, 1, 5, 1);
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
// dpiTest_1400_withoutParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and both the dpiCommonParams and dpiPoolCreateParams structures are NULL.
//-----------------------------------------------------------------------------
int dpiTest_1400_withoutParams(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1401_withCommonParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and dpiCommonParams structure set to NULL.
//-----------------------------------------------------------------------------
int dpiTest_1401_withCommonParams(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1402_withCreateParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and dpiPoolCreateParams structure set to NULL.
//-----------------------------------------------------------------------------
int dpiTest_1402_withCreateParams(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1403_withBothParams()
//   Verify that dpiPool_create() succeeds when valid credentials are passed
// and both the dpiCommonParams and dpiPoolCreateParams structures are set.
//-----------------------------------------------------------------------------
int dpiTest_1403_withBothParams(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1404_invalidCred()
//   Verify that dpiPool_create() fails when invalid credentials are passed.
//-----------------------------------------------------------------------------
int dpiTest_1404_invalidCred(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, "X", 1, "X", 1, params->connectString,
            params->connectStringLength, NULL, NULL, &pool);
    return dpiTestCase_expectError(testCase, "ORA-01017:");
}


//-----------------------------------------------------------------------------
// dpiTest_1405_setMinSessions()
//   Verify that dpiPool_create() specifying a non-default value for
// minSessions and confirm that this number of sessions is created (no error).
//-----------------------------------------------------------------------------
int dpiTest_1405_setMinSessions(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t count;
    dpiPool *pool;

    // create a pool
    if (dpiTestCase_getPool(testCase, &pool) < 0)
        return DPI_FAILURE;

    // verify open count matches the minimum number of sessions
    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count,
            DPI_TEST_POOL_MIN_SESSIONS) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1406_setMaxSessions()
//   Verify that dpiPool_create() specifying a non-default value for
// maxSessions and confirm that this number of sessions is the maximum that is
// created (no error).
//-----------------------------------------------------------------------------
int dpiTest_1406_setMaxSessions(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedErrors[] = { "ORA-24418:", "ORA-24496:", NULL };
    dpiConn *conn[DPI_TEST_POOL_MAX_SESSIONS], *tempConn;
    uint32_t count, i;
    dpiPool *pool;

    // create a pool
    if (dpiTestCase_getPool(testCase, &pool) < 0)
        return DPI_FAILURE;

    // acquire connections up to the maximum
    for (i = 0; i < DPI_TEST_POOL_MAX_SESSIONS; i++) {
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                &conn[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // verify that the open count matches
    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count,
            DPI_TEST_POOL_MAX_SESSIONS) < 0)
        return DPI_FAILURE;

    // attempt to acquire one more connection
    dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &tempConn);
    if (dpiTestCase_expectAnyError(testCase, expectedErrors) < 0)
        return DPI_FAILURE;

    // cleanup
    for (i = 0; i < DPI_TEST_POOL_MAX_SESSIONS; i++) {
        if (dpiConn_release(conn[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1407_setSessionIncr()
//   Verify that dpiPool_create() specifying a non-default value for
// sessionIncrement and confirm that this number of sessions is created each
// time more session are needed (no error).
//-----------------------------------------------------------------------------
int dpiTest_1407_setSessionIncr(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn[DPI_TEST_POOL_MIN_SESSIONS + 1];
    uint32_t count, i;
    dpiPool *pool;

    // create a pool
    if (dpiTestCase_getPool(testCase, &pool) < 0)
        return DPI_FAILURE;

    // acquire connections up to the minimum number of sessions
    for (i = 0; i < DPI_TEST_POOL_MIN_SESSIONS; i++) {
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                &conn[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // verify that the number of open connections matches
    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count,
            DPI_TEST_POOL_MIN_SESSIONS) < 0)
        return DPI_FAILURE;

    // acquire one more connection; this should create the number of
    // connections defined by the session increment
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn[DPI_TEST_POOL_MIN_SESSIONS]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getOpenCount(pool, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count,
            DPI_TEST_POOL_MIN_SESSIONS + DPI_TEST_POOL_SESSION_INCREMENT) < 0)
        return DPI_FAILURE;

    // cleanup
    for (i = 0; i < DPI_TEST_POOL_MIN_SESSIONS + 1; i++) {
        if (dpiConn_release(conn[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1408_getModeNoWait()
//   Verify that dpiPool_create() specifying no wait for the get mode works as
// expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1408_getModeNoWait(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedErrors[] = { "ORA-24418:", "ORA-24496:", NULL };
    dpiPoolCreateParams createParams;
    dpiConn *conn1, *conn2;
    dpiContext *context;
    dpiPool *pool;

    // create a pool that can contain only one connection
    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = 0;
    createParams.maxSessions = 1;
    createParams.sessionIncrement = 1;
    createParams.getMode = DPI_MODE_POOL_GET_NOWAIT;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire that connection from the pool
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn1) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // attempt to acquire a second connection from the pool; this should fail
    dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn2);
    if (dpiTestCase_expectAnyError(testCase, expectedErrors) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiConn_release(conn1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1409_getModeForceGet()
//   Verify that dpiPool_create() specifying force get for the get mode works
// as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1409_getModeForceGet(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiConn *conn1, *conn2;
    dpiContext *context;
    dpiPool *pool;

    // create a pool that can contain only one connection
    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = 0;
    createParams.maxSessions = 1;
    createParams.sessionIncrement = 1;
    createParams.getMode = DPI_MODE_POOL_GET_FORCEGET;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams,  &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire that connection from the pool
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn1) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // attempt to acquire a second connection from the pool; this should
    // succeed, even though the maximum number of sessions has been reached
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiConn_release(conn1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1410_createWithNullContext()
//   Verify that dpiPool_create() with NULL context (error DPI-1002)
//-----------------------------------------------------------------------------
int dpiTest_1410_createWithNullContext(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiPool *pool;

    dpiPool_create(NULL, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            params->connectString, params->connectStringLength, NULL, NULL,
            &pool);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_1411_releaseTwice()
//   Verify that dpiPool_create() followed by dpiPool_release() twice
// (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1411_releaseTwice(dpiTestCase *testCase, dpiTestParams *params)
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
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_1412_callWithClosedPool()
//   Call dpiPool_create() with valid credentials and call dpiPool_close();
// then call all other public dpiPool functions except for dpiPool_addRef() and
// dpiPool_release() (error DPI-1010).
//-----------------------------------------------------------------------------
int dpiTest_1412_callWithClosedPool(dpiTestCase *testCase,
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
// dpiTest_1413_callWithNullPool()
//   Call each of the public functions for dpiPool with the pool parameter set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1413_callWithNullPool(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__callFunctionsWithError(testCase, params, NULL,
            "DPI-1002: invalid dpiPool handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1414_proxyAuthHeteroPool()
//   Verify that proxy authentication can be used with a pool (no error).
//-----------------------------------------------------------------------------
int dpiTest_1414_proxyAuthHeteroPool(dpiTestCase *testCase,
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
// dpiTest_1415_proxyAuthHomoPool()
//   Verify that proxy authentication cannot be used with a homogeneous pool
// (error DPI-1012).
//-----------------------------------------------------------------------------
int dpiTest_1415_proxyAuthHomoPool(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1012:") < 0)
        return DPI_FAILURE;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1416_createWithNull()
//   Call dpiPool_create with valid credentials and NULL pool (error DPI-1046).
//-----------------------------------------------------------------------------
int dpiTest_1416_createWithNull(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            params->connectString, params->connectStringLength, NULL, NULL,
            NULL);
    return dpiTestCase_expectError(testCase, "DPI-1046:");
}


//-----------------------------------------------------------------------------
// dpiTest_1417_createNoCred()
//   Call dpiPool_create with no credentials (error ORA-24415).
//-----------------------------------------------------------------------------
int dpiTest_1417_createNoCred(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, &pool);
    return dpiTestCase_expectError(testCase, "ORA-24415:");
}


//-----------------------------------------------------------------------------
// dpiTest_1418_invalidConnStr()
//   Verify that dpiPool_create() fails when an invalid connect string
// is passed (error ORA-12154).
//-----------------------------------------------------------------------------
int dpiTest_1418_invalidConnStr(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *connectString = "an_invalid_connect_string";
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    dpiPool_create(context, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            connectString, strlen(connectString), NULL, NULL, &pool);
    return dpiTestCase_expectError(testCase, "ORA-12154:");
}


//-----------------------------------------------------------------------------
// dpiTest_1419_heteroPoolWithCredentials()
//   Create a heterogeneous pool with credentials and verify that acquiring
// connections from it works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1419_heteroPoolWithCredentials(dpiTestCase *testCase,
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
// dpiTest_1420_heteroPoolNoCredentials()
//   Create a heterogeneous pool with no credentials and verify that acquiring
// connections from it works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1420_heteroPoolNoCredentials(dpiTestCase *testCase,
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
// dpiTest_1421_heteroPoolAcquireWithInvalidCredentials()
//   Verify that attempting to acquire a connection from a heterogeneous pool
// with invalid or missing credentials fails as expected (ORA-24415, ORA-01017,
// and ORA-24419).
//-----------------------------------------------------------------------------
int dpiTest_1421_heteroPoolAcquireWithInvalidCredentials(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "ORA-24415:") < 0)
        return DPI_FAILURE;

    // acquire connection with invalid password
    dpiPool_acquireConnection(pool, params->mainUserName,
            params->mainUserNameLength, invalidPassword,
            strlen(invalidPassword), NULL, &conn);
    if (dpiTestCase_expectError(testCase, "ORA-01017:") < 0)
        return DPI_FAILURE;

    // acquire connection with user name but no password
    dpiPool_acquireConnection(pool, params->proxyUserName,
            params->proxyUserNameLength, NULL, 0, NULL, &conn);
    if (dpiTestCase_expectError(testCase, "ORA-24419:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1422_verifyPingInterval()
//   Verify that dpiPool_setPingInterval() and dpiPool_getPingInterval() work
// as expected.
//-----------------------------------------------------------------------------
int dpiTest_1422_verifyPingInterval(dpiTestCase *testCase, dpiTestParams *params)
{
    int getVal, setVal;
    dpiPool *pool;

    if (dpiTestCase_getPool(testCase, &pool) < 0)
        return DPI_FAILURE;
    if (dpiPool_getPingInterval(pool, &getVal) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getVal,
            DPI_DEFAULT_PING_INTERVAL) < 0)
        return DPI_FAILURE;

    setVal = -1;
    if (dpiPool_setPingInterval(pool, setVal) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getPingInterval(pool, &getVal) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getVal, setVal) < 0)
        return DPI_FAILURE;

    setVal = 30;
    if (dpiPool_setPingInterval(pool, setVal) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getPingInterval(pool, &getVal) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getVal, setVal) < 0)
        return DPI_FAILURE;

    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1423_verifyReconfigureMaxSessions()
//   Verify dpiPool_reconfigure() adjusts maximum sessions correctly.
//-----------------------------------------------------------------------------
int dpiTest_1423_verifyReconfigureMaxSessions(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedErrors[] = { "ORA-24418:", "ORA-24496:", NULL };
    uint32_t minSessions = 1, maxSessions = 3, sessionIncrement = 1;
    dpiConn *connections[3], *tempConn;
    dpiPool *pool;
    int i;

    // create a pool
    if (dpiTestCase_getPool(testCase, &pool) < 0)
        return DPI_FAILURE;

    // acquire 3 connections from the pool
    for (i = 0; i < 3; i++) {
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                &connections[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // the fourth connection should be able to be acquired as well
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &tempConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(tempConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // reconfigure the pool to only allow a maximum of 3 sessions
    if (dpiPool_reconfigure(pool, minSessions, maxSessions,
            sessionIncrement) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // now attempt to acquire the fourth connection, which should fail
    dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &tempConn);
    if (dpiTestCase_expectAnyError(testCase, expectedErrors) < 0)
        return DPI_FAILURE;

    // release the third connection; an attempt to acquire the third connection
    // should now succeed
    if (dpiConn_release(connections[2]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &tempConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiConn_release(tempConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < 2; i++) {
        if (dpiConn_release(connections[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1400);
    dpiTestSuite_addCase(dpiTest_1400_withoutParams,
            "dpiPool_create() with valid credentials, no parameters");
    dpiTestSuite_addCase(dpiTest_1401_withCommonParams,
            "dpiPool_create() with valid credentials and common params");
    dpiTestSuite_addCase(dpiTest_1402_withCreateParams,
            "dpiPool_create() with valid credentials and create params");
    dpiTestSuite_addCase(dpiTest_1403_withBothParams,
            "dpiPool_create() with valid credentials and both params");
    dpiTestSuite_addCase(dpiTest_1404_invalidCred,
            "dpiPool_create() with invalid credentials fails");
    dpiTestSuite_addCase(dpiTest_1405_setMinSessions,
            "dpiPool_create() with Min Sessions");
    dpiTestSuite_addCase(dpiTest_1406_setMaxSessions,
            "dpiPool_create() with Max Sessions");
    dpiTestSuite_addCase(dpiTest_1407_setSessionIncr,
            "dpiPool_create() with Session Increment");
    dpiTestSuite_addCase(dpiTest_1408_getModeNoWait,
            "dpiPool_create() with get mode set to no wait");
    dpiTestSuite_addCase(dpiTest_1409_getModeForceGet,
            "dpiPool_create() with get mode set to force get");
    dpiTestSuite_addCase(dpiTest_1410_createWithNullContext,
            "dpiPool_create() with NULL context");
    dpiTestSuite_addCase(dpiTest_1411_releaseTwice,
            "dpiPool_create() and release twice");
    dpiTestSuite_addCase(dpiTest_1412_callWithClosedPool,
            "call all pool functions with closed pool");
    dpiTestSuite_addCase(dpiTest_1413_callWithNullPool,
            "call all pool functions with NULL pool");
    dpiTestSuite_addCase(dpiTest_1414_proxyAuthHeteroPool,
            "proxy authentication with pool");
    dpiTestSuite_addCase(dpiTest_1415_proxyAuthHomoPool,
            "proxy authentication cannot be used with homogeneous pool");
    dpiTestSuite_addCase(dpiTest_1416_createWithNull,
            "dpiPool_create() with NULL pool");
    dpiTestSuite_addCase(dpiTest_1417_createNoCred,
            "dpiPool_create() with no credentials");
    dpiTestSuite_addCase(dpiTest_1418_invalidConnStr,
            "dpiPool_create() with invalid connect string");
    dpiTestSuite_addCase(dpiTest_1419_heteroPoolWithCredentials,
            "dpiPool_acquireConnection() from hetero pool with credentials");
    dpiTestSuite_addCase(dpiTest_1420_heteroPoolNoCredentials,
            "dpiPool_acquireConnection() from hetero pool without credentials");
    dpiTestSuite_addCase(dpiTest_1421_heteroPoolAcquireWithInvalidCredentials,
            "dpiPool_acquireConnection() from hetero pool invalid credentials");
    dpiTestSuite_addCase(dpiTest_1422_verifyPingInterval,
            "dpiPool_getPingInterval() and dpiPool_setPingInterval()");
    dpiTestSuite_addCase(dpiTest_1423_verifyReconfigureMaxSessions,
            "dpiPool_reconfigure() adjusting max sessions");
    return dpiTestSuite_run();
}
