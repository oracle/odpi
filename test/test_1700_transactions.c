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
// test_1700_transactions.c
//   Test suite for testing Transactions.
//-----------------------------------------------------------------------------
#include "TestLib.h"

#define FORMAT_ID                  100
#define TRANSACTION_ID             "txn-odpic"
#define BRANCH_QUALIFIER           "bqual-odpic"

//-----------------------------------------------------------------------------
// dpiTest__insertRowsInTable() [INTERNAL]
//   Inserts rows into the test table.
//-----------------------------------------------------------------------------
int dpiTest__insertRowsInTable(dpiTestCase *testCase, dpiConn *conn)
{
    const char *sql = "insert into TestTempTable values (:1, :2)";
    dpiData intColValue, stringColValue;
    char *strValue = "String 1";
    dpiStmt *stmt;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&intColValue, 1);
    dpiData_setBytes(&stringColValue, strValue, strlen(strValue));
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64,
            &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES,
            &stringColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__populateXid() [INTERNAL]
//   Populate the XID structure.
//-----------------------------------------------------------------------------
void dpiTest__populateXid(dpiXid *xid)
{
    xid->formatId = FORMAT_ID;
    xid->globalTransactionId = TRANSACTION_ID;
    xid->globalTransactionIdLength = strlen(TRANSACTION_ID);
    xid->branchQualifier = BRANCH_QUALIFIER;
    xid->branchQualifierLength = strlen(BRANCH_QUALIFIER);
}


//-----------------------------------------------------------------------------
// dpiTest__truncateTable() [INTERNAL]
//   Truncate the rows in the test table.
//-----------------------------------------------------------------------------
int dpiTest__truncateTable(dpiTestCase *testCase, dpiConn *conn)
{
    const char *sql = "truncate table TestTempTable";
    dpiStmt *stmt;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyData() [INTERNAL]
//   Verify that the table contains the expected number of rows. A new
// connection is established to ensure the transaction was truly committed or
// rolled back.
//-----------------------------------------------------------------------------
int dpiTest__verifyData(dpiTestCase *testCase, int64_t expectedNumRows)
{
    const char *sql = "select count(*) from TestTempTable";
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_defineValue(stmt, 1, DPI_ORACLE_TYPE_NUMBER,
            DPI_NATIVE_TYPE_INT64, 0, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(data),
            expectedNumRows) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1700_tpcBeginValidParams()
//   Call dpiConn_tpcBegin() with parameters globalTransactionIdLength and
// branchQualifierLength <= 64 (no error).
//-----------------------------------------------------------------------------
int dpiTest_1700_tpcBeginValidParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiXid xid;

    dpiTest__populateXid(&xid);
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_tpcBegin(conn, &xid, DPI_TPC_BEGIN_NEW, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1701_tpcBeginInvalidTranLength()
//   Call dpiConn_tpcBegin() with parameter globalTransactionIdLength > 64
// (error DPI-1035).
//-----------------------------------------------------------------------------
int dpiTest_1701_tpcBeginInvalidTranLength(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiXid xid;

    dpiTest__populateXid(&xid);
    xid.globalTransactionIdLength = 65;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_tpcBegin(conn, &xid, DPI_TPC_BEGIN_NEW, 0);
    return dpiTestCase_expectError(testCase, "DPI-1035:");
}


//-----------------------------------------------------------------------------
// dpiTest_1702_tpcBeginInvalidBranchLength()
//   Call dpiConn_tpcBegin() with parameter branchQualifierLength > 64
// (error DPI-1036).
//-----------------------------------------------------------------------------
int dpiTest_1702_tpcBeginInvalidBranchLength(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiXid xid;

    dpiTest__populateXid(&xid);
    xid.branchQualifierLength = 65;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_tpcBegin(conn, &xid, DPI_TPC_BEGIN_NEW, 0);
    return dpiTestCase_expectError(testCase, "DPI-1036:");
}


//-----------------------------------------------------------------------------
// dpiTest_1703_tpcPrepareNoTran()
//   Call dpiConn_tpcBegin(), then call dpiConn_tpcPrepare() and verify that
// commitNeeded has the value 0 (no error).
//-----------------------------------------------------------------------------
int dpiTest_1703_tpcPrepareNoTran(dpiTestCase *testCase, dpiTestParams *params)
{
    int commitNeeded;
    dpiConn *conn;
    dpiXid xid;

    dpiTest__populateXid(&xid);
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_tpcBegin(conn, &xid, DPI_TPC_BEGIN_NEW, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_tpcPrepare(conn, NULL, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, commitNeeded, 0) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1704_tpcNoDml()
//   Call dpiConn_tpcBegin(), then call dpiConn_tpcPrepare(), then call
// dpiConn_tpcCommit() (error ORA-24756).
//-----------------------------------------------------------------------------
int dpiTest_1704_tpcNoDml(dpiTestCase *testCase, dpiTestParams *params)
{
    int commitNeeded;
    dpiConn *conn;
    dpiXid xid;

    dpiTest__populateXid(&xid);
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_tpcBegin(conn, &xid, DPI_TPC_BEGIN_NEW, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_tpcPrepare(conn, NULL, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_tpcCommit(conn, NULL, 1);
    return dpiTestCase_expectError(testCase, "ORA-24756:");
}


//-----------------------------------------------------------------------------
// dpiTest_1705_tpcCommit()
//   Call dpiConn_tpcBegin(), then execute some DML, then call
// dpiConn_tpcPrepare() and verify that commitNeeded has the value 1;
// call dpiConn_tpcCommit() and create a new connection using the common
// connection creation method and verify that the changes have been committed
// to the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_1705_tpcCommit(dpiTestCase *testCase, dpiTestParams *params)
{
    int commitNeeded;
    dpiConn *conn;
    dpiXid xid;

    // setup table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateTable(testCase, conn) < 0)
        return DPI_FAILURE;

    // perform transaction
    dpiTest__populateXid(&xid);
    if (dpiConn_tpcBegin(conn, &xid, DPI_TPC_BEGIN_NEW, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__insertRowsInTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_tpcPrepare(conn, NULL, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, commitNeeded, 1) < 0)
        return DPI_FAILURE;
    if (dpiConn_tpcCommit(conn, NULL, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify commit succeeded
    if (dpiTest__verifyData(testCase, 1) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1706_tpcRollback()
//   Call dpiConn_tpcBegin(), then execute some DML, then call
// dpiConn_tpcPrepare(); call dpiConn_tpcRollback() and create a new
// connection using the common connection creation method and verify that the
// changes have been rolled back (no error).
//-----------------------------------------------------------------------------
int dpiTest_1706_tpcRollback(dpiTestCase *testCase, dpiTestParams *params)
{
    int commitNeeded;
    dpiConn *conn;
    dpiXid xid;

    // setup table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateTable(testCase, conn) < 0)
        return DPI_FAILURE;

    // perform transaction
    dpiTest__populateXid(&xid);
    if (dpiConn_tpcBegin(conn, &xid, DPI_TPC_BEGIN_NEW, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__insertRowsInTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_tpcPrepare(conn, NULL, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, commitNeeded, 1) < 0)
        return DPI_FAILURE;
    if (dpiConn_tpcRollback(conn, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify rollback succeeded
    if (dpiTest__verifyData(testCase, 0) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1707_verifyTPCFuncsWithNullConn()
//   Call TPC functions with NULL connection (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1707_verifyTPCFuncsWithNullConn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    dpiConn_tpcBegin(NULL, NULL, 0, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_tpcCommit(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_tpcEnd(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_tpcForget(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_tpcPrepare(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_tpcRollback(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1708_verifyTPCFuncsWithNullXid()
//   Call TPC functions that expect a valid XID with a null value (error
// DPI-1046).
//-----------------------------------------------------------------------------
int dpiTest_1708_verifyTPCFuncsWithNullXid(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1046:";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    dpiConn_tpcBegin(conn, NULL, 0, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_tpcForget(conn, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_tpcPrepare(conn, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1700);
    dpiTestSuite_addCase(dpiTest_1700_tpcBeginValidParams,
            "dpiConn_tpcBegin() with valid parameters");
    dpiTestSuite_addCase(dpiTest_1701_tpcBeginInvalidTranLength,
            "dpiConn_tpcBegin() with transactionIdLength > 64");
    dpiTestSuite_addCase(dpiTest_1702_tpcBeginInvalidBranchLength,
            "dpiConn_tpcBegin() with branchQualifierLength > 64");
    dpiTestSuite_addCase(dpiTest_1703_tpcPrepareNoTran,
            "dpiConn_tpcPrepare() with no transaction");
    dpiTestSuite_addCase(dpiTest_1704_tpcNoDml,
            "dpiConn_tpcCommit() of transaction with no DML");
    dpiTestSuite_addCase(dpiTest_1705_tpcCommit,
            "dpiConn_tpcCommit() of transaction with DML");
    dpiTestSuite_addCase(dpiTest_1706_tpcRollback,
            "dpiConn_tpcRollback() of transaction with DML");
    dpiTestSuite_addCase(dpiTest_1707_verifyTPCFuncsWithNullConn,
            "verify tpc functions with NULL connection");
    dpiTestSuite_addCase(dpiTest_1708_verifyTPCFuncsWithNullXid,
            "verify tpc functions with NULL XID");
    return dpiTestSuite_run();
}
