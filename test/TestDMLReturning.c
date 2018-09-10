//-----------------------------------------------------------------------------
// Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestDMLReturning.c
//   Test suite for testing all the possible combinations of DML Returning
// statements.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__prepareTable() [INTERNAL]
//   Prepare table for test. This truncates the table and then optionally
// populates it with a number of rows.
//-----------------------------------------------------------------------------
int dpiTest__prepareTable(dpiTestCase *testCase, dpiConn *conn,
        uint32_t numRows)
{
    const char *truncateSql = "truncate table TestTempTable";
    const char *insertSql =
            "begin "
            "    for i in 1..:count loop "
            "        insert into TestTempTable "
            "        values (i, 'String value ' || i); "
            "    end loop; "
            "end;";
    dpiData tempData;
    dpiStmt *stmt;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate table with some rows, if applicable
    if (numRows > 0) {
        if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
                &stmt) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setInt64(&tempData, numRows);
        if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64,
                &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_execute(stmt, 0, NULL) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_release(stmt) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2400_verifyExecuteReturningOneRow()
//   Test call to dpiStmt_execute() of a DML returning statement that returns a
// single row.
//-----------------------------------------------------------------------------
int dpiTest_2400_verifyExecuteReturningOneRow(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "insert into TestTempTable "
            "values (:intIn, :strIn) "
            "returning IntCol + 4, substr(StringCol, 1, 4) "
            "into :intOut, :strOut";
    dpiData *intData, *strData, tempData;
    const char *expectedString = "Test";
    uint32_t numReturnedRows;
    dpiVar *intVar, *strVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare table for use
    if (dpiTest__prepareTable(testCase, conn, 0) < 0)
        return DPI_FAILURE;

    // prepare statement to insert row with DML returning
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform binds
    dpiData_setInt64(&tempData, 1);
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&tempData, "Test data", strlen("Test data"));
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &intVar, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 3, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            100, 1, 0, NULL, &strVar, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 4, strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and check return values
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getReturnedData(intVar, 0, &numReturnedRows, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, intData->value.asUint64, 5) < 0)
        return DPI_FAILURE;
    if (dpiVar_getReturnedData(strVar, 0, &numReturnedRows, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, strData->value.asBytes.ptr,
            strData->value.asBytes.length, expectedString,
            strlen(expectedString)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2401_verifyExecuteReturningNoRows()
//   Test call to dpiStmt_execute() of a DML returning statement that returns
// no rows.
//-----------------------------------------------------------------------------
int dpiTest_2401_verifyExecuteReturningNoRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "update TestTempTable set StringCol = 'X' "
                "returning IntCol, StringCol into :intVar, :strVar";
    dpiData *intData, *strData;
    uint32_t numReturnedRows;
    dpiVar *intVar, *strVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare table for use
    if (dpiTest__prepareTable(testCase, conn, 0) < 0)
        return DPI_FAILURE;

    // prepare statement to update row with DML returning
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform binds
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &intVar, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            100, 1, 0, NULL, &strVar, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and check return values
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getReturnedData(intVar, 0, &numReturnedRows, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 0) < 0)
        return DPI_FAILURE;
    if (dpiVar_getReturnedData(strVar, 0, &numReturnedRows, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 0) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2402_verifyExecuteReturningMultipleRows()
//   Test call to dpiStmt_execute() of a DML returning statement that returns
// multiple rows.
//-----------------------------------------------------------------------------
int dpiTest_2402_verifyExecuteReturningMultipleRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "update TestTempTable set "
            "    IntCol = IntCol + 15, "
            "    StringCol = 'The final value of string ' || to_char(IntCol) "
            "returning IntCol, StringCol into :intVar, :strVar";
    uint32_t numRows = 10, numReturnedRows;
    dpiData *intData, *strData;
    dpiVar *intVar, *strVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare table for use
    if (dpiTest__prepareTable(testCase, conn, numRows) < 0)
        return DPI_FAILURE;

    // prepare statement to update row with DML returning
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform binds
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &intVar, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 1, 0, NULL, &strVar, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and check return values
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getReturnedData(intVar, 0, &numReturnedRows, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, numRows) < 0)
        return DPI_FAILURE;
    if (dpiVar_getReturnedData(strVar, 0, &numReturnedRows, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, numRows) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2403_verifyExecuteManyReturningOneRow()
//   Test inserting multiple rows with DML returning of one row for each row
// inserted.
//-----------------------------------------------------------------------------
int dpiTest_2403_verifyExecuteManyReturningOneRow(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "insert into TestTempTable "
            "values (:intVal, :strVal) "
            "returning IntCol + 3, substr(StringCol, 11) "
            "into :intVar, :strVar";
    uint32_t numCols = 4, numRows = 10, i, numReturnedRows;
    dpiData *colData[4];
    dpiVar *colVar[4];
    char buffer[30];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare table for use
    if (dpiTest__prepareTable(testCase, conn, 0) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &colVar[0], &colData[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 0, 0, NULL, &colVar[1], &colData[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &colVar[2], &colData[2]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 0, 0, NULL, &colVar[3], &colData[3]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate some dummy data
    for (i = 0; i < numRows; i++) {
        dpiData_setInt64(&colData[0][i], i + 1);
        sprintf(buffer, "Test data %d", i + 1);
        if (dpiVar_setFromBytes(colVar[1], i, buffer, strlen(buffer)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // prepare and bind statement to insert rows with DML returning
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numCols; i++) {
        if (dpiStmt_bindByPos(stmt, i + 1, colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // execute and check return values
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numRows; i++) {
        if (dpiVar_getReturnedData(colVar[2], i, &numReturnedRows,
                &colData[2]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 1) < 0)
            return DPI_FAILURE;
        if (dpiTestCase_expectUintEqual(testCase, colData[2]->value.asUint64,
                i + 4) < 0)
            return DPI_FAILURE;
        if (dpiVar_getReturnedData(colVar[3], i, &numReturnedRows,
                &colData[3]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 1) < 0)
            return DPI_FAILURE;
        sprintf(buffer, "%d", i + 1);
        if (dpiTestCase_expectStringEqual(testCase,
                colData[3]->value.asBytes.ptr,
                colData[3]->value.asBytes.length, buffer, strlen(buffer)) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    for (i = 0; i < numCols; i++) {
        if (dpiVar_release(colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2404_verifyExecuteManyReturningNoRows()
//   Test updating of multiple rows with DML returning of no rows for each
// iteration.
//-----------------------------------------------------------------------------
int dpiTest_2404_verifyExecuteManyReturningNoRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "update TestTempTable set StringCol = 'X' "
            "where IntCol = :intVal "
            "returning IntCol, StringCol into :intVar, :strVar";
    uint32_t numCols = 3, numRows = 10, i, numReturnedRows;
    dpiData *colData[3];
    dpiVar *colVar[3];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare table for use
    if (dpiTest__prepareTable(testCase, conn, numRows) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &colVar[0], &colData[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &colVar[1], &colData[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 0, 0, NULL, &colVar[2], &colData[2]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare and bind statement to update rows with DML returning
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numRows; i++)
        dpiData_setInt64(&colData[0][i], i + 100);
    for (i = 0; i < numCols; i++) {
        if (dpiStmt_bindByPos(stmt, i + 1, colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // execute and check return values
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numRows; i++) {
        if (dpiVar_getReturnedData(colVar[1], i, &numReturnedRows,
                &colData[1]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 0) < 0)
            return DPI_FAILURE;
        if (dpiVar_getReturnedData(colVar[2], i, &numReturnedRows,
                &colData[2]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 0) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    for (i = 0; i < numCols; i++) {
        if (dpiVar_release(colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2405_verifyExecuteManyReturningMultipleRows()
//   Test deleting of multiple rows with DML returning of multiple differing
// number of rows for each iteration.
//-----------------------------------------------------------------------------
int dpiTest_2405_verifyExecuteManyReturningMultipleRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "delete from TestTempTable "
            "where IntCol <= :intVal returning IntCol into :intVar";
    uint32_t numRows = 3, i, numReturnedRows, intValues[3] = { 3, 8, 10 };
    dpiData *intValData, *intVarData;
    dpiVar *intVal, *intVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare table for use
    if (dpiTest__prepareTable(testCase, conn, 10) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &intVal, &intValData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &intVar, &intVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare and bind statement to delete rows with DML returning
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numRows; i++)
        dpiData_setInt64(&intValData[i], intValues[i]);
    if (dpiStmt_bindByPos(stmt, 1, intVal) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // check first iteration
    if (dpiVar_getReturnedData(intVar, 0, &numReturnedRows, &intVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 3) < 0)
        return DPI_FAILURE;
    for (i = 0; i < 3; i++) {
        if (dpiTestCase_expectUintEqual(testCase, intVarData[i].value.asUint64,
                i + 1) < 0)
            return DPI_FAILURE;
    }

    // check second iteration
    if (dpiVar_getReturnedData(intVar, 1, &numReturnedRows, &intVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 5) < 0)
        return DPI_FAILURE;
    for (i = 0; i < 5; i++) {
        if (dpiTestCase_expectUintEqual(testCase, intVarData[i].value.asUint64,
                i + 4) < 0)
            return DPI_FAILURE;
    }

    // check third iteration
    if (dpiVar_getReturnedData(intVar, 2, &numReturnedRows, &intVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 2) < 0)
        return DPI_FAILURE;
    for (i = 0; i < 2; i++) {
        if (dpiTestCase_expectUintEqual(testCase, intVarData[i].value.asUint64,
                i + 9) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiVar_release(intVal) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2406_verifyReturningMultipleRowsThenNoRows()
//   Test deleting multiple rows with DML returning and then attempting to
// delete the same rows which should result in no rows being returned.
//-----------------------------------------------------------------------------
int dpiTest_2406_verifyReturningMultipleRowsThenNoRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "delete from TestTempTable "
            "where IntCol <= :intVal returning IntCol into :intVar";
    dpiData *intVarData, tempData;
    uint32_t numReturnedRows;
    dpiVar *intVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare table for use
    if (dpiTest__prepareTable(testCase, conn, 10) < 0)
        return DPI_FAILURE;

    // prepare statement to perform delete with DML returning
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform binds
    dpiData_setInt64(&tempData, 4);
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &intVar, &intVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and check return values
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getReturnedData(intVar, 0, &numReturnedRows, &intVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 4) < 0)
        return DPI_FAILURE;

    // execute statement a second time and check return values
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getReturnedData(intVar, 0, &numReturnedRows, &intVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numReturnedRows, 0) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2400);
    dpiTestSuite_addCase(dpiTest_2400_verifyExecuteReturningOneRow,
            "verify execute() of statement returning single row");
    dpiTestSuite_addCase(dpiTest_2401_verifyExecuteReturningNoRows,
            "verify execute() of statement returning no rows");
    dpiTestSuite_addCase(dpiTest_2402_verifyExecuteReturningMultipleRows,
            "verify execute() of statement returning multiple rows");
    dpiTestSuite_addCase(dpiTest_2403_verifyExecuteManyReturningOneRow,
            "verify executeMany() returning one row per iteration");
    dpiTestSuite_addCase(dpiTest_2404_verifyExecuteManyReturningNoRows,
            "verify executeMany() returning no rows per iteration");
    dpiTestSuite_addCase(dpiTest_2405_verifyExecuteManyReturningMultipleRows,
            "verify executeMany() returning multiple rows per iteration");
    dpiTestSuite_addCase(dpiTest_2406_verifyReturningMultipleRowsThenNoRows,
            "verify execute() of statement returning multiple, then no rows");
    return dpiTestSuite_run();
}

