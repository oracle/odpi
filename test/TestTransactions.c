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
// TestTransactions.c
//   Test suite for testing Transactions.
//-----------------------------------------------------------------------------
#include "TestLib.h"

#define FORMAT_ID           100
#define TRANSACTION_ID      "123"
#define BRANCH_ID           "456"

//-----------------------------------------------------------------------------
// Test_TransValidParams() [PRIVATE]
//   Call dpiConn_beginDistribTrans() with parameters transactionIdLength and
// branchIdLength <= 64 (no error).
//-----------------------------------------------------------------------------
int Test_TransValidParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, strlen(BRANCH_ID)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransInvalidTransLen() [PRIVATE]
//   Call dpiConn_beginDistribTrans() with parameter transactionIdLength > 64
// (error DPI-1035).
//-----------------------------------------------------------------------------
int Test_TransInvalidTransLen(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID, 65, BRANCH_ID,
            strlen(BRANCH_ID));
    return dpiTestCase_expectError(testCase,
            "DPI-1035: size of the transaction ID is 65 and cannot exceed 64");
}


//-----------------------------------------------------------------------------
// Test_TransInvalidBranchLen() [PRIVATE]
//   Call dpiConn_beginDistribTrans() with parameter branchIdLength > 64
// (error DPI-1036).
//-----------------------------------------------------------------------------
int Test_TransInvalidBranchLen(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, 65);
    return dpiTestCase_expectError(testCase,
            "DPI-1036: size of the branch ID is 65 and cannot exceed 64");
}


//-----------------------------------------------------------------------------
// Test_TransVerifyCommit() [PRIVATE]
//   call dpiConn_beginDistribTrans(), then call dpiConn_prepareDistribTrans()
//   and verify that commitNeeded has the value 0 (no error)
//-----------------------------------------------------------------------------
int Test_TransVerifyCommit(dpiTestCase *testCase, dpiTestParams *params)
{
    int commitNeeded;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, strlen(BRANCH_ID)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareDistribTrans(conn, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, commitNeeded, 0) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransCommitErr() [PRIVATE]
//   Call dpiConn_beginDistribTrans(), then call dpiConn_prepareDistribTrans(),
// then call dpiConn_commit() (error ORA-24756).
//-----------------------------------------------------------------------------
int Test_TransCommitErr(dpiTestCase *testCase, dpiTestParams *params)
{
    int commitNeeded;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, strlen(BRANCH_ID)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareDistribTrans(conn, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_commit(conn);
    return dpiTestCase_expectError(testCase,
            "ORA-24756: transaction does not exist");
}


//-----------------------------------------------------------------------------
// Test_TransDelRowsFromTable() [PRIVATE]
//   Inserts rows into table.
//-----------------------------------------------------------------------------
int Test_TransInsIntoTable(dpiTestCase *testCase, dpiTestParams *params,
        dpiConn **conn, dpiStmt **stmt, const char *sqlQuery)
{
    dpiData intColValue, stringColValue;
    uint32_t numQueryColumns;
    uint64_t rowCount;

    if (dpiConn_prepareStmt(*conn, 0, sqlQuery, 
        strlen(sqlQuery), NULL, 0, stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    intColValue.isNull = 0;
    stringColValue.isNull = 0;
    intColValue.value.asInt64 = 1 + rand()%50;
    if (dpiStmt_bindValueByPos(*stmt, 1, DPI_NATIVE_TYPE_INT64,
            &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    stringColValue.value.asBytes.ptr = "TEST 1";
    stringColValue.value.asBytes.length = strlen("TEST 1");
    if (dpiStmt_bindValueByPos(*stmt, 2, DPI_NATIVE_TYPE_BYTES,
            &stringColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(*stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getRowCount(*stmt, &rowCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransDelRowsFromTable() [PRIVATE]
//   Deletes rows from table.
//-----------------------------------------------------------------------------
int Test_TransDelRowsFromTable(dpiTestCase *testCase, dpiTestParams *params,
        dpiConn **conn, dpiStmt **stmt, const char *sqlQuery)
{
    uint32_t numQueryColumns;
    uint64_t rowCount;

    if (dpiConn_prepareStmt(*conn, 0, sqlQuery, strlen(sqlQuery), NULL, 0,
            stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(*stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getRowCount(*stmt, &rowCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return rowCount;
}


//-----------------------------------------------------------------------------
// Test_TransVerifyDML() [PRIVATE]
//   Call dpiConn_beginDistribTrans(), then execute some DML, then call
// dpiConn_prepareDistribTrans() and verify that commitNeeded has the value 1;
// call dpiConn_commit() and create a new connection using the common
// connection creation method and verify that the changes have been committed
// to the database (no error).
//-----------------------------------------------------------------------------
int Test_TransVerifyDML(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sqlQueryIns = "insert into TestTempTable values (:1, :2)";
    const char *sqlQuery = "delete from TestTempTable";
    uint64_t rowCount;
    int commitNeeded;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, strlen(BRANCH_ID)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // delete rows from table
    if (Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    // perform insert
    if (Test_TransInsIntoTable(testCase, params, &conn, &stmt,
            sqlQueryIns) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareDistribTrans(conn, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!(dpiTestCase_expectUintEqual(testCase, commitNeeded, 1) 
            == DPI_SUCCESS && dpiConn_commit(conn) == DPI_SUCCESS))
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);

    // delete rows from table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, strlen(BRANCH_ID)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    rowCount = Test_TransDelRowsFromTable(testCase, 
                    params, &conn, &stmt, sqlQuery);
    if (dpiConn_prepareDistribTrans(conn, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!(dpiTestCase_expectUintEqual(testCase, commitNeeded, 1) 
            == DPI_SUCCESS && dpiConn_commit(conn) == DPI_SUCCESS))
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);
    if (dpiTestCase_expectUintEqual(testCase, rowCount, 1) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransVerifyRollback() [PRIVATE]
//   Call dpiConn_beginDistribTrans(), then execute some DML, then call
// dpiConn_prepareDistribTrans(); call dpiConn_rollback() and create a new
// connection using the common connection creation method and verify that the
// changes have been rolled back (no error).
//-----------------------------------------------------------------------------
int Test_TransVerifyRollback(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sqlQueryIns = "insert into TestTempTable values (:1, :2)";
    const char *sqlQuery = "delete from TestTempTable";
    int commitNeeded;
    uint64_t rowCount;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    if (dpiConn_beginDistribTrans(conn, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, strlen(BRANCH_ID)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // delete rows from table
    if (Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    // perform insert
    if (Test_TransInsIntoTable(testCase, params, &conn, &stmt,
            sqlQueryIns) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareDistribTrans(conn, &commitNeeded) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!(dpiTestCase_expectUintEqual(testCase, commitNeeded, 1) 
            == DPI_SUCCESS && dpiConn_rollback(conn) == DPI_SUCCESS))
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);

    // delete rows from table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    rowCount = Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery);
    dpiStmt_release(stmt);
    if (dpiTestCase_expectUintEqual(testCase, rowCount, 0) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransVerifyDMLChanges() [PRIVATE]
//   Execute any DML and call dpiConn_commit(); create new connection and
// verify that the changes were indeed committed (no error).
//-----------------------------------------------------------------------------
int Test_TransVerifyDMLChanges(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sqlQueryIns = "insert into TestTempTable values (:1, :2)";
    const char *sqlQuery = "delete from TestTempTable";
    uint64_t rowCount;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // delete rows from table
    if (Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    // perform insert
    if (Test_TransInsIntoTable(testCase, params, &conn, &stmt,
            sqlQueryIns) < 0)
        return DPI_FAILURE;
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);

    //delete rows from table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    rowCount = Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);
    if (dpiTestCase_expectUintEqual(testCase, rowCount, 1) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransVerifyDMLRollback() [PRIVATE]
//   Execute any DML and call dpiConn_rollback(); create new connection and
// verify that the changes were indeed rolled back (no error).
//-----------------------------------------------------------------------------
int Test_TransVerifyDMLRollback(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sqlQueryIns = "insert into TestTempTable values (:1, :2)";
    const char *sqlQuery = "delete from TestTempTable";
    uint64_t rowCount;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // delete rows from table
    if (Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    // perform insert
    if (Test_TransInsIntoTable(testCase, params, &conn, &stmt,
            sqlQueryIns) < 0)
        return DPI_FAILURE;
    if (dpiConn_rollback(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);

    // delete rows from table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    rowCount = Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery);
    dpiStmt_release(stmt);
    if (dpiTestCase_expectUintEqual(testCase, rowCount, 0) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransVerifyDMLConnCloseRollback() [PRIVATE]
//   Execute any DML and call dpiConn_close(); create new connection and verify
// that the changes were indeed rolled back (no error).
//-----------------------------------------------------------------------------
int Test_TransVerifyDMLConnCloseRollback(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sqlQueryIns = "insert into TestTempTable values (:1, :2)";
    const char *sqlQuery = "delete from TestTempTable";
    uint64_t rowCount;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // delete rows from table
    if (Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    // perform insert
    if (Test_TransInsIntoTable(testCase, params, &conn, &stmt,
            sqlQueryIns) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    // delete rows from table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    rowCount = Test_TransDelRowsFromTable(testCase, params, &conn, &stmt,
            sqlQuery);
    dpiStmt_release(stmt);
    if (dpiTestCase_expectUintEqual(testCase, rowCount, 0) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_TransBeginDistWithNullConn() [PRIVATE]
//   Call dpiConn_beginDistribTrans() with null connection and verify it throws
// error DPI-1002.
//-----------------------------------------------------------------------------
int Test_TransBeginDistWithNullConn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_beginDistribTrans(NULL, FORMAT_ID, TRANSACTION_ID,
            strlen(TRANSACTION_ID), BRANCH_ID, strlen(BRANCH_ID));
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiConn handle");
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(800);
    dpiTestSuite_addCase(Test_TransValidParams,
            "dpiConn_beginDistribTrans() with valid parameters");
    dpiTestSuite_addCase(Test_TransInvalidTransLen,
            "dpiConn_beginDistribTrans() with transactionIdLength > 64");
    dpiTestSuite_addCase(Test_TransInvalidBranchLen,
            "dpiConn_beginDistribTrans() with branchIdLength > 64");
    dpiTestSuite_addCase(Test_TransVerifyCommit,
            "dpiConn_prepareDistribTrans() with no transaction");
    dpiTestSuite_addCase(Test_TransCommitErr,
            "dpiConn_commit() of distrib transaction with no DML");
    dpiTestSuite_addCase(Test_TransVerifyDML,
            "dpiConn_commit() of distrib transaction with DML");
    dpiTestSuite_addCase(Test_TransVerifyRollback,
            "dpiConn_rollback() of distrib transaction with DML");
    dpiTestSuite_addCase(Test_TransVerifyDMLChanges,
            "dpiConn_commit() of distrib transaction in other connection");
    dpiTestSuite_addCase(Test_TransVerifyDMLRollback,
            "dpiConn_rollback() of distrib transaction in other connection");
    dpiTestSuite_addCase(Test_TransVerifyDMLConnCloseRollback,
            "dpiConn_close() rolls back distrib transaction");
    dpiTestSuite_addCase(Test_TransBeginDistWithNullConn,
            "dpiConn_beginDistribTrans() with NULL connection");
    return dpiTestSuite_run();
}

