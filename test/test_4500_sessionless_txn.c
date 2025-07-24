//-----------------------------------------------------------------------------
// Copyright (c) 2025, Oracle and/or its affiliates.
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
// test_4500_sessionless_txn.c
//   Test suite for all the Sessionless transaction  related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_4500()
//   Call each of the Sessionless Transaction public functions with the
// parameter set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_4500(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiSessionlessTransactionId transactionId;
    const char *expectedError = "DPI-1002:";
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 6) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    dpiConn_beginSessionlessTransaction(NULL, &transactionId, 5, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_resumeSessionlessTransaction(NULL, &transactionId, 5, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_suspendSessionlessTransaction(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4501()
//   Call dpiConn_resumeSessionlessTransaction with an invalid transaction id
// (error ORA-26218).
//-----------------------------------------------------------------------------
int dpiTest_4501(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiSessionlessTransactionId transactionId;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 6) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    strcpy(transactionId.value, "not_a_valid_transaction_id");
    transactionId.length = strlen(transactionId.value);
    dpiConn_resumeSessionlessTransaction(conn, &transactionId, 5, 0);
    if (dpiTestCase_expectError(testCase, "ORA-26218:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4502()
//   Call dpiConn_beginSessionlessTransaction with zero timeout value
// (error ORA-26206).
//-----------------------------------------------------------------------------
int dpiTest_4502(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiSessionlessTransactionId transactionId;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 6) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    strcpy(transactionId.value, "txn_id_4502");
    transactionId.length = strlen(transactionId.value);
    dpiConn_beginSessionlessTransaction(conn, &transactionId, 0, 0);
    if (dpiTestCase_expectError(testCase, "ORA-26206:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4503()
//   Begin a sessionless transaction and suspend it, then use a different
// connection to resume.
//-----------------------------------------------------------------------------
int dpiTest_4503(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiSessionlessTransactionId transactionId;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 6) < 0)
        return DPI_FAILURE;

    // begin and suspend on one connection
    strcpy(transactionId.value, "txn_id_4503");
    transactionId.length = strlen(transactionId.value);
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_beginSessionlessTransaction(conn, &transactionId, 5, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_suspendSessionlessTransaction(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // resume on a new connection and commit
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_resumeSessionlessTransaction(conn, &transactionId, 5, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4504()
//   Begin a sessionless transaction and suspend it, then wait for the timeout
// to expire and attempt resume the transaction (error ORA-26218).
//-----------------------------------------------------------------------------
int dpiTest_4504(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiSessionlessTransactionId transactionId;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 6) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    strcpy(transactionId.value, "txn_id_4504");
    transactionId.length = strlen(transactionId.value);
    if (dpiConn_beginSessionlessTransaction(conn, &transactionId, 1, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_suspendSessionlessTransaction(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    sleep(3);
    dpiConn_resumeSessionlessTransaction(conn, &transactionId, 2, 0);
    if (dpiTestCase_expectError(testCase, "ORA-26218:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4505()
//   After performing suspend/resume of a sessionless transaction, verify
// that a rollback has taken place.
//-----------------------------------------------------------------------------
int dpiTest_4505(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *insertSql = "insert into TestTempTable values (4506, 'test1')";
    const char *truncateSql = "truncate table TestTempTable";
    const char *selectSql = "select * from TestTempTable";
    dpiSessionlessTransactionId transactionId;
    uint32_t bufferRowIndex;
    uint64_t rowCount;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 6) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // begin transaction and insert data
    if (dpiConn_beginSessionlessTransaction(conn, &transactionId, 5, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // suspend and then immediately resume transaction
    if (dpiConn_suspendSessionlessTransaction(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_resumeSessionlessTransaction(conn, &transactionId, 5, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // rollback transaction
    if (dpiConn_rollback(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify that the rollback has taken place
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, rowCount, 0) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


int main(int argc, char **argv)
{
    dpiTestSuite_initialize(4500);
    dpiTestSuite_addCase(dpiTest_4500,
            "verify API with NULL parameters");
    dpiTestSuite_addCase(dpiTest_4501,
            "verify with invalid transaction ID");
    dpiTestSuite_addCase(dpiTest_4502,
            "verify with zero timeout value");
    dpiTestSuite_addCase(dpiTest_4503,
            "resume transaction on a different connection");
    dpiTestSuite_addCase(dpiTest_4504,
            "verify timeout expiry");
    dpiTestSuite_addCase(dpiTest_4505,
            "verify rollback");
    return dpiTestSuite_run();
}
