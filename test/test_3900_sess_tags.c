//-----------------------------------------------------------------------------
// Copyright (c) 2019, 2022, Oracle and/or its affiliates.
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
// test_3900_sess_tags.c
//   Test suite for testing session tagging.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_3900_verifyTags()
//   Verify that session tagging works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_3900_verifyTags(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConnCreateParams connParams;
    dpiContext *context;
    dpiPool *pool;
    dpiConn *conn;
    char tag[20];
    uint32_t i;

    // create a pool
    if (dpiTestCase_getPool(testCase, &pool) < 0)
        return DPI_FAILURE;

    // acquire all of the possible connections from the pool and then
    // immediately return them with a different tag for each iteration
    dpiTestSuite_getContext(&context);
    if (dpiContext_initConnCreateParams(context, &connParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < DPI_TEST_POOL_MAX_SESSIONS; i++) {
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
                &conn) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession,
                1) < 0)
            return DPI_FAILURE;
        sprintf(tag, "SCHEMA=HR%d", i);
        if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_RETAG, tag,
                strlen(tag)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiConn_release(conn) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // now attempt to find each of those sessions again; each session should
    // have the requested tag
    for (i = 0; i < DPI_TEST_POOL_MAX_SESSIONS; i++) {
        sprintf(tag, "SCHEMA=HR%d", i);
        connParams.tag = tag;
        connParams.tagLength = strlen(tag);
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
                &conn) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession,
                0) < 0)
            return DPI_FAILURE;
        if (dpiTestCase_expectStringEqual(testCase, connParams.outTag,
                connParams.outTagLength, connParams.tag,
                connParams.tagLength) < 0)
            return DPI_FAILURE;
        if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiConn_release(conn) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // attempt to get a session with a non-existent tag without matchAnyTag
    // set; it should be a new session and not have any tag
    connParams.tag = "DOES_NOT_EXIST=1";
    connParams.tagLength = strlen(connParams.tag);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, connParams.outTag,
            connParams.outTagLength, NULL, 0) < 0)
        return DPI_FAILURE;
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // attempt to get a session with a non-existent tag with matchAnyTag set;
    // it should be an existing session
    connParams.matchAnyTag = 1;
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession, 0) < 0)
        return DPI_FAILURE;
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3901_verifySessionWithInvalidTag()
//   Specify tag name with invalid format and verify it throws an error
// (ORA-24488).
//-----------------------------------------------------------------------------
int dpiTest_3901_verifySessionWithInvalidTag(dpiTestCase *testCase,
        dpiTestParams *params)
{
    char *tag = "INVALID_TAG";
    dpiConn *conn;
    dpiPool *pool;

    // versions less than 12.2 don't raise an error so skip them
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 1, 12, 2) < 0)
        return DPI_FAILURE;

    // create a pool
    if (dpiTestCase_getPool(testCase, &pool) < 0)
        return DPI_FAILURE;

    // acquire a connection from the pool
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // attempt to tag the connection with an invalid tag
    dpiConn_close(conn, DPI_MODE_CONN_CLOSE_RETAG, tag, strlen(tag));
    if (dpiTestCase_expectError(testCase, "ORA-24488:") < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3902_verifySessionCallbackPLSQL()
//   Demonstrate how to use a session callback written in PL/SQL. The callback
// is invoked whenever the tag requested by the application does not match the
// tag associated with the session in the pool. It should be used to set
// session state, so that the application can count on known session state,
// which allows the application to reduce the number of round trips to the
// database.
//-----------------------------------------------------------------------------
int dpiTest_3902_verifySessionCallbackPLSQL(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *querySql = "select count(*) from PLSQLSessionCallbacks";
    const char *truncateSql = "truncate table PLSQLSessionCallbacks";
    dpiPoolCreateParams poolParams;
    dpiConnCreateParams connParams;
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiContext *context;
    dpiConn *conn;
    dpiPool *pool;
    dpiStmt *stmt;
    dpiData *data;
    int found;

    // only supported in 12.2 and higher
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 1, 12, 2) < 0)
        return DPI_FAILURE;

    // create a pool with a PL/SQL session callback specified
    dpiTestSuite_getContext(&context);
    if (dpiContext_initPoolCreateParams(context, &poolParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    poolParams.minSessions = DPI_TEST_POOL_MIN_SESSIONS;
    poolParams.maxSessions = DPI_TEST_POOL_MAX_SESSIONS;
    poolParams.sessionIncrement = DPI_TEST_POOL_SESSION_INCREMENT;
    poolParams.plsqlFixupCallback = "pkg_SessionCallback.TheCallback";
    poolParams.plsqlFixupCallbackLength =
            strlen(poolParams.plsqlFixupCallback);
    if (dpiPool_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &poolParams, &pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire a connection without specifying a tag; the callback will not be
    // invoked and no session state is changed
    if (dpiContext_initConnCreateParams(context, &connParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession,
            1) < 0)
        return DPI_FAILURE;

    // truncate table logging calls to PL/SQL callback
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire a connection, specifying a tag; since the session returned has
    // no tag, the callback will be invoked; session state will be changed and
    // the tag is saved when the connection is closed
    connParams.tag = "NLS_DATE_FORMAT=SIMPLE";
    connParams.tagLength = strlen(connParams.tag);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_RETAG, connParams.tag,
            connParams.tagLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire session, specifying the same tag; since a session exists in
    // the pool with this tag, it will be returned and the callback will
    // not be invoked but the connection will still have the session state
    // defined previously
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, connParams.outTag,
            connParams.outTagLength, connParams.tag,
            connParams.tagLength) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession, 0) < 0)
        return DPI_FAILURE;
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire session, specifying a different tag; since no session exists
    // in the pool with this tag, a new session will be returned and the
    // callback will be invoked; session state will be changed and the tag is
    // saved when the connection is closed
    connParams.tag = "NLS_DATE_FORMAT=FULL;TIME_ZONE=UTC";
    connParams.tagLength = strlen(connParams.tag);
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession,
            1) < 0)
        return DPI_FAILURE;
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_RETAG, connParams.tag,
            connParams.tagLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire session, specifying a different tag but also specifying that a
    // session with any tag can be acquired from the pool; a session with one
    // of the previously set tags will be returned and the callback will be
    // invoked; session state will be changed and the tag is saved when the
    // connection is closed
    connParams.tag = "NLS_DATE_FORMAT=FULL;TIME_ZONE=MST";
    connParams.tagLength = strlen(connParams.tag);
    connParams.matchAnyTag = 1;
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, &connParams,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, connParams.outNewSession,
            1) < 0)
        return DPI_FAILURE;
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_RETAG, connParams.tag,
            connParams.tagLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire session and verify results from PL/SQL session logs
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, querySql, strlen(querySql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, (uint32_t) data->value.asDouble,
            3) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiConn_close(conn, DPI_MODE_CONN_CLOSE_DEFAULT, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiPool_release(pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3900);
    dpiTestSuite_addCase(dpiTest_3900_verifyTags,
            "verify session tagging works as expected");
    dpiTestSuite_addCase(dpiTest_3901_verifySessionWithInvalidTag,
            "specify invalid tag and verify");
    dpiTestSuite_addCase(dpiTest_3902_verifySessionCallbackPLSQL,
            "verify SessionCallbackPLSQL works as expected");
    return dpiTestSuite_run();
}
