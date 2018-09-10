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
// TestPoolProperties.c
//   Test suite for testing pool properties.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define MINSESSIONS 2
#define MAXSESSIONS 9
#define SESSINCREMENT 2

//-----------------------------------------------------------------------------
// dpiTest_600_busyCount()
//   Call dpiPool_getBusyCount() in various scenarios to verify that the busy
// count is being returned correctly (no error).
//-----------------------------------------------------------------------------
int dpiTest_600_busyCount(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    uint32_t count, iter;
    dpiContext *context;
    dpiConn *conn[3];
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
    dpiPool_getBusyCount(pool, &count);

    if (dpiTestCase_expectUintEqual(testCase, count, 0) < 0)
        return DPI_FAILURE;

    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_getBusyCount(pool, &count);

    if (dpiTestCase_expectUintEqual(testCase, count, 1) < 0)
        return DPI_FAILURE;

    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_getBusyCount(pool, &count);

    if (dpiTestCase_expectUintEqual(testCase, count, 2) < 0)
        return DPI_FAILURE;

    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn[2]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiPool_getBusyCount(pool, &count);

    if (dpiTestCase_expectUintEqual(testCase, count, 3) < 0)
        return DPI_FAILURE;

    for (iter = 0; iter < 3; ++iter)
       dpiConn_release(conn[iter]);
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_601_openCount()
//   Call dpiPool_getOpenCount() in various scenarios to verify that the open
// count is being returned correctly (no error).
//-----------------------------------------------------------------------------
int dpiTest_601_openCount(dpiTestCase *testCase, dpiTestParams *params)
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

    dpiPool_getOpenCount(pool, &count);
    if (dpiTestCase_expectUintEqual(testCase, count, MINSESSIONS) < 0)
        return DPI_FAILURE;
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_602_encodingInfo()
//   Call dpiPool_create() specifying a value for the encoding and nencoding
// attributes of the dpiCommonCreateParams structure and then call
// dpiPool_getEncodingInfo() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_602_encodingInfo(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *charSet = "ISO-8859-13";
    dpiCommonCreateParams commonParams;
    dpiEncodingInfo info;
    dpiContext *context;
    dpiPool *pool;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    commonParams.encoding = charSet;
    commonParams.nencoding = charSet;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getEncodingInfo(pool, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.encoding,
            strlen(info.encoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, info.nencoding,
            strlen(info.nencoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_603_checkGetMode()
//   Call dpiPool_setGetMode(); call dpiPool_getGetMode() and verify that the
// value returned matches (no error).
//-----------------------------------------------------------------------------
int dpiTest_603_checkGetMode(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiPoolCreateParams createParams;
    dpiPoolGetMode value;
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
    if (dpiPool_setGetMode(pool, DPI_MODE_POOL_GET_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_getGetMode(pool, &value) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTestCase_expectUintEqual(testCase, value,
            DPI_MODE_POOL_GET_WAIT) < 0)
        return DPI_FAILURE;

    if (dpiPool_setGetMode(pool, DPI_MODE_POOL_GET_NOWAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_getGetMode(pool, &value) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTestCase_expectUintEqual(testCase, value,
            DPI_MODE_POOL_GET_NOWAIT) < 0)
        return DPI_FAILURE;

    if (dpiPool_setGetMode(pool, DPI_MODE_POOL_GET_FORCEGET) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiPool_getGetMode(pool, &value) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTestCase_expectUintEqual(testCase, value,
            DPI_MODE_POOL_GET_FORCEGET) < 0)
        return DPI_FAILURE;

    dpiPool_release(pool);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_604_checkMaxLifetimeSession()
//   Call dpiPool_setMaxLifetimeSession(); call dpiPool_getMaxLifetimeSession()
// and verify that the value returned matches (no error).
//-----------------------------------------------------------------------------
int dpiTest_604_checkMaxLifetimeSession(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t value, sessMaxTime = 10;
    dpiPoolCreateParams createParams;
    dpiContext *context;
    dpiPool *pool;

    // only supported in 12.1 and higher
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;

    // create a pool
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

    // test the getting and setting of the attribute
    if (dpiPool_setMaxLifetimeSession(pool, sessMaxTime) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getMaxLifetimeSession(pool, &value) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, value, sessMaxTime) < 0)
        return DPI_FAILURE;
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_605_checkTimeout()
//   Call dpiPool_setTimeout(); call dpiPool_getTimeout() and verify that the
// value returned matches (no error).
//-----------------------------------------------------------------------------
int dpiTest_605_checkTimeout(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t value, sessTimeout = 10;
    dpiPoolCreateParams createParams;
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
    if (dpiPool_setTimeout(pool, sessTimeout) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getTimeout(pool, &value) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, value, sessTimeout) < 0)
        return DPI_FAILURE;
    dpiPool_release(pool);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_606_encodingInfo()
//   Call dpiPool_create() specifying a value for the encoding and null for
// nencoding of the dpiCommonCreateParams structure and then call
// dpiPool_getEncodingInfo() to verify that the values are as expected.
//-----------------------------------------------------------------------------
int dpiTest_606_encodingInfo(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *charSet = "ISO-8859-13";
    dpiCommonCreateParams commonParams;
    dpiEncodingInfo info, defaultInfo;
    dpiConn *defaultConn;
    dpiContext *context;
    dpiPool *pool;

    // get default encodings
    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL,
            &defaultConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(defaultConn, &defaultInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create pool with just the encoding specified
    commonParams.encoding = charSet;
    commonParams.nencoding = NULL;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getEncodingInfo(pool, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.encoding,
            strlen(info.encoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, info.nencoding,
            strlen(info.nencoding), defaultInfo.nencoding,
            strlen(defaultInfo.nencoding)) < 0)
        return DPI_FAILURE;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create pool with just the nencoding specified
    commonParams.encoding = NULL;
    commonParams.nencoding = charSet;
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_getEncodingInfo(pool, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.encoding,
            strlen(info.encoding), defaultInfo.encoding,
            strlen(defaultInfo.encoding)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, info.nencoding,
            strlen(info.nencoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiConn_release(defaultConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(600);
    dpiTestSuite_addCase(dpiTest_600_busyCount,
            "dpiPool_getBusyCount() with various scenarios");
    dpiTestSuite_addCase(dpiTest_601_openCount,
            "dpiPool_getOpenCount() with various scenarios");
    dpiTestSuite_addCase(dpiTest_602_encodingInfo,
            "dpiPool_getEncodingInfo() to verify that the values match");
    dpiTestSuite_addCase(dpiTest_603_checkGetMode,
            "check get / set mode for getting connections from pool");
    dpiTestSuite_addCase(dpiTest_604_checkMaxLifetimeSession,
            "check get / set maximum lifetime session of pool");
    dpiTestSuite_addCase(dpiTest_605_checkTimeout,
            "check get / set pool timeout");
    dpiTestSuite_addCase(dpiTest_606_encodingInfo,
            "specifying a value for nencoding and null for encoding");
    return dpiTestSuite_run();
}

