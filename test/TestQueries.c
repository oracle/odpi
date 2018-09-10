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
// TestQueries.c
//   Test suite for testing queries.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__execStatement() [INTERNAL]
//   Prepare and execute statements and, if necessary, check number of rows.
//-----------------------------------------------------------------------------
int dpiTest__execStatement(dpiTestCase *testCase, dpiConn *conn,
        const char *sql, int cols)
{
    uint32_t numQueryColumns;
    dpiStmt *stmt;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);
    if (cols >= 0) {
        if (dpiTestCase_expectUintEqual(testCase, numQueryColumns, cols) < 0)
            return DPI_FAILURE;
    }
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__insertIntoTestLongs() [INTERNAL]
//   Inserts rows into table.
//-----------------------------------------------------------------------------
int dpiTest__insertIntoTestLongs(dpiTestCase *testCase, dpiConn *conn)
{
    uint32_t longValueLength, numQueryColumns, arraySize =3, numOfRows = 2;
    const char *insertSql = "insert into TestLongs values (:1, :2)";
    const char *dropSql = "delete from TestLongs";
    uint32_t iter, sizeIncrement = 50000;
    dpiData *intColValue, *longColValue;
    dpiVar *intColVar, *longColVar;
    char *longValue;
    dpiStmt *stmt;

    if (dpiTest__execStatement(testCase, conn, dropSql, -1) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            arraySize, 0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_LONG_VARCHAR,
            DPI_NATIVE_TYPE_BYTES, arraySize, 0, 0, 0, NULL, &longColVar,
            &longColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert the requested number of rows
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (iter = 1; iter <= numOfRows; iter++) {
        if (dpiStmt_bindByPos(stmt, 1, intColVar) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_bindByPos(stmt, 2, longColVar) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        longValueLength = iter * sizeIncrement;
        longValue = malloc(longValueLength);
        if (!longValue)
            return dpiTestCase_setFailed(testCase, "Out of memory!");
        memset(longValue, 'A', longValueLength);
        intColValue->isNull = 0;
        intColValue->value.asInt64 = iter;
        if (dpiVar_setFromBytes(longColVar, 0, longValue, longValueLength) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        free(longValue);
        if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    dpiStmt_release(stmt);
    dpiVar_release(intColVar);
    dpiVar_release(longColVar);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_700_checkFetchArraySize()
//   Prepare any statement; call dpiStmt_getFetchArraySize() and verify the
// value returned is the default value of 100; call dpiStmt_setFetchArraySize()
// with any value except zero and then call dpiStmt_getFetchArraySize() to
// verify the value returned matches the value that was set; call
// dpiStmt_setFetchArraySize() with the value 0 and then call
// dpiStmt_getFetchArraySize() and verify the value returned is the default
// value of 100 (no error).
//-----------------------------------------------------------------------------
int dpiTest_700_checkFetchArraySize(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs order by IntCol";
    uint32_t arraySize, setArraySize = 3;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getFetchArraySize(stmt, &arraySize)< 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, arraySize, 100) < 0)
        return DPI_FAILURE;
    if (dpiStmt_setFetchArraySize(stmt, setArraySize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getFetchArraySize(stmt, &arraySize)< 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, arraySize, setArraySize) < 0)
        return DPI_FAILURE;
    setArraySize = 0;
    if (dpiStmt_setFetchArraySize(stmt, setArraySize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getFetchArraySize(stmt, &arraySize)< 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, arraySize, 100) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_701_fetchArraySizeTooLarge()
//   Prepare and execute any query; create and define variables needed to
// support the query; call dpiStmt_setFetchArraySize() with a value that is
// larger than the array size used to create at least one of the variables
// (error DPI-1015).
//-----------------------------------------------------------------------------
int dpiTest_701_fetchArraySizeTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs order by IntCol";
    uint32_t arraySize = 3, setArraySize = 3333;
    uint32_t numQueryColumns;
    dpiData *intColValue;
    dpiVar *intColVar;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            arraySize, 0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_define(stmt, 1, intColVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_setFetchArraySize(stmt, setArraySize);
    if (dpiTestCase_expectError(testCase,
            "DPI-1015: array size of 3333 is too large") < 0)
        return DPI_FAILURE;
    dpiVar_release(intColVar);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_702_fetchArraySizeTooSmall()
//   Prepare and execute any query; call dpiStmt_setFetchArraySize() with any
// value; create and define a variable with an array size smaller than the
// value used to call dpiStmt_setFetchArraySize(); call dpiStmt_fetch()
// (error  DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_702_fetchArraySizeTooSmall(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs order by IntCol";
    uint32_t numQueryColumns, bufferRowIndex;
    uint32_t arraySize = 3, setArraySize = 5;
    dpiData *intColValue;
    dpiVar *intColVar;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            arraySize, 0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, setArraySize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_define(stmt, 1, intColVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_fetch(stmt, &found, &bufferRowIndex);
    if (dpiTestCase_expectError(testCase,
            "DPI-1018: array size of 3 is too small") < 0)
        return DPI_FAILURE;
    dpiVar_release(intColVar);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_703_getQueryValueNoDefine()
//   Prepare and execute any query but do not create any variables or perform
// any defines; call dpiStmt_fetch() followed by dpiStmt_getQueryValue() and
// verify that the values returned match the expected values (no error).
//-----------------------------------------------------------------------------
int dpiTest_703_getQueryValueNoDefine(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiNativeTypeNum nativeTypeNum;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiData *data;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__insertIntoTestLongs(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, nativeTypeNum,
            DPI_NATIVE_TYPE_INT64) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_704_getQueryValueDiffPos()
//   Prepare and execute any query; call dpiStmt_fetch(), then call
// dpiStmt_getQueryValue() with position 0 as well as a position that exceeds
// the number of columns being fetched (error DPI-1028).
//-----------------------------------------------------------------------------
int dpiTest_704_getQueryValueDiffPos(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiNativeTypeNum nativeTypeNum;
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getQueryValue(stmt, 0, &nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase,
            "DPI-1028: query position 0 is invalid") < 0)
        return DPI_FAILURE;
    dpiStmt_getQueryValue(stmt, 3, &nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase,
            "DPI-1028: query position 3 is invalid") < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_705_getQueryValueNoQuery()
//   Prepare any query; call dpiStmt_getQueryValue() with any position
// (error DPI-1007).
//-----------------------------------------------------------------------------
int dpiTest_705_getQueryValueNoQuery(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    dpiNativeTypeNum nativeTypeNum;
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getQueryValue(stmt, 0, &nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase,
            "DPI-1007: no query has been executed") < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_706_getQueryValueAfterFetch()
//   Prepare and execute any query; call dpiStmt_fetch() until no rows are
// found; call dpiStmt_getQueryValue() with any valid position
// (error DPI-1029).
//-----------------------------------------------------------------------------
int dpiTest_706_getQueryValueAfterFetch(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *querySql = "select * from TestLongs";
    const char *deleteSql = "delete from TestLongs";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiNativeTypeNum nativeTypeNum;
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__execStatement(testCase, conn, deleteSql, -1) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, querySql, strlen(querySql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase,
            "DPI-1029: no row currently fetched") < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_707_getQueryInfo()
//   Prepare and execute any query; call dpiStmt_getQueryInfo() and verify
// that the type information returned matches that expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_707_getQueryInfo(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiQueryInfo info;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryInfo(stmt, 1, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase,
            info.typeInfo.defaultNativeTypeNum, DPI_NATIVE_TYPE_INT64) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_708_queryAfterDropColumn()
//   Prepare and execute a "select *" type query and fetch from it, then close
// the statement; execute an alter statement that removes a column from the
// table or view being queried; prepare, execute and fetch from the first query
// again and confirm the number of columns has changed and no error has been
// raised (no error).
//-----------------------------------------------------------------------------
int dpiTest_708_queryAfterDropColumn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *querySql = "select * from TestLongsAlter";
    const char *alterDropSql =
            "alter table TestLongsAlter drop column IntCol";
    const char *dropSql = "drop table TestLongsAlter";
    const char *createSql =
            "create table TestLongsAlter (IntCol number(9), LongCol long)";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__execStatement(testCase, conn, dropSql, -1) < 0)
        return DPI_FAILURE;
    if (dpiTest__execStatement(testCase, conn, createSql, -1) < 0)
        return DPI_FAILURE;
    if (dpiTest__execStatement(testCase, conn, querySql, 2) < 0)
        return DPI_FAILURE;
    if (dpiTest__execStatement(testCase, conn, alterDropSql, -1) < 0)
        return DPI_FAILURE;
    if (dpiTest__execStatement(testCase, conn, querySql, 1) < 0)
        return DPI_FAILURE;
    if (dpiTest__execStatement(testCase, conn, querySql, -1) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_709_fetchRowCheckCount()
//   Prepare and execute any query; call dpiStmt_fetch() multiple times and
// confirm that after each call a call to dpiStmt_getRowCount() results in a
// row count that is one larger than the previous iteration (no error).
//-----------------------------------------------------------------------------
int dpiTest_709_fetchRowCheckCount(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t numQueryColumns, bufferRowIndex, iter, numOfRows = 2;
    const char *sql = "select * from TestLongs";
    uint64_t rowCount;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (iter = 1; iter <= numOfRows; iter++) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, rowCount, iter) < 0)
            return DPI_FAILURE;
        if (!found)
            return DPI_FAILURE;
    }
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_710_fetchRowsCheckCount()
//   Prepare and execute any query; call dpiStmt_fetchRows() and confirm that
// the value returned by dpiStmt_getRowCount() increases by the number of rows
// that were returned (no error).
//-----------------------------------------------------------------------------
int dpiTest_710_fetchRowsCheckCount(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t numQueryColumns, bufferRowIndex, numRowsFetched, maxRows = 2;
    const char *sql = "select * from TestLongs";
    uint64_t rowCount;
    dpiConn *conn;
    dpiStmt *stmt;
    int moreRows;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__insertIntoTestLongs(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetchRows(stmt, maxRows, &bufferRowIndex, &numRowsFetched,
            &moreRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, rowCount, maxRows) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_711_verifyStmtFuncs()
//   Prepare any query but do not execute it; call dpiStmt_fetch(),
// dpiStmt_getQueryInfo() and dpiStmt_define() and confirm each receives
// an error (error ORA-24338).
//-----------------------------------------------------------------------------
int dpiTest_711_verifyStmtFuncs(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "ORA-24338: statement handle not executed";
    const char *sql = "select * from TestLongs";
    uint32_t bufferRowIndex;
    dpiQueryInfo info;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_fetch(stmt, &found, &bufferRowIndex);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getQueryInfo(stmt, 1, &info);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_define(stmt, 1, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_712_fetchDataToSmallLenStrVar()
//   Create a string variable with a size that is smaller than at least some
// of the data that is subsequently fetched (error DPI-1037)
//-----------------------------------------------------------------------------
int dpiTest_712_fetchDataToSmallLenStrVar(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestTempTable values(1,'TestString')";
    const char *selectSql = "select StringCol from TestTempTable";
    const char *truncateSql = "truncate table TestTempTable";
    uint32_t bufferRowIndex;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    // insert a row with a string of known size into the table
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    // perform fetch into string of smaller size
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_defineValue(stmt, 1, DPI_ORACLE_TYPE_VARCHAR,
            DPI_NATIVE_TYPE_BYTES, 2, 1, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_fetch(stmt, &found, &bufferRowIndex);
    if (dpiTestCase_expectError(testCase, "DPI-1037: column at array position "
            "0 fetched with error 1406") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(700);
    dpiTestSuite_addCase(dpiTest_700_checkFetchArraySize,
            "check get / set array size with various values");
    dpiTestSuite_addCase(dpiTest_701_fetchArraySizeTooLarge,
            "dpiStmt_setFetchArraySize() with value too large");
    dpiTestSuite_addCase(dpiTest_702_fetchArraySizeTooSmall,
            "dpiStmt_setFetchArraySize() with value too small");
    dpiTestSuite_addCase(dpiTest_703_getQueryValueNoDefine,
            "dpiStmt_getQueryValue() without define");
    dpiTestSuite_addCase(dpiTest_704_getQueryValueDiffPos,
            "dpiStmt_getQueryValue() with invalid position values");
    dpiTestSuite_addCase(dpiTest_705_getQueryValueNoQuery,
            "dpiStmt_getQueryValue() called when no query has been executed");
    dpiTestSuite_addCase(dpiTest_706_getQueryValueAfterFetch,
            "dpiStmt_getQueryValue() called after all rows fetched");
    dpiTestSuite_addCase(dpiTest_707_getQueryInfo,
            "dpiStmt_getQueryInfo() information is correct");
    dpiTestSuite_addCase(dpiTest_708_queryAfterDropColumn,
            "query all columns from table before and after column dropped");
    dpiTestSuite_addCase(dpiTest_709_fetchRowCheckCount,
            "dpiStmt_fetch() increments rowcount");
    dpiTestSuite_addCase(dpiTest_710_fetchRowsCheckCount,
            "dpiStmt_fetchRows() increments rowcount");
    dpiTestSuite_addCase(dpiTest_711_verifyStmtFuncs,
            "dpiStmt_fetchRows() increments rowcount");
    dpiTestSuite_addCase(dpiTest_712_fetchDataToSmallLenStrVar,
            "fetch data to a string variable which is smaller and verify");
    return dpiTestSuite_run();
}

