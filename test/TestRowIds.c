//-----------------------------------------------------------------------------
// Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestRowIds.c
//   Test suite for testing all the rowid related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_3400_convRowIdToStrAndVerifyOnRegTab()
//   Prepare and execute any query that selects rowid on a regular table.
// Convert one of these rowids to string by calling dpiRowid_getStringValue()
// and perform a second query specifically for the row matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_3400_convRowIdToStrAndVerifyOnRegTab(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sqlQuery1 = "select rowid from TestStrings where IntCol = 7";
    const char *sqlQuery2 = "select IntCol from TestStrings where rowid = :1";
    uint32_t bufferRowIndex, rowidAsStringLength;
    dpiData *queryValue, bindValue;
    dpiNativeTypeNum nativeTypeNum;
    const char *rowidAsString;
    dpiStmt *stmt1, *stmt2;
    dpiConn *conn;
    int found;

    // perform first query to get rowid
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sqlQuery1, strlen(sqlQuery1), NULL, 0,
            &stmt1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt1, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt1, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for first query!");
    if (dpiStmt_getQueryValue(stmt1, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_getStringValue(queryValue->value.asRowid, &rowidAsString,
            &rowidAsStringLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform second query to get row using rowid
    if (dpiConn_prepareStmt(conn, 0, sqlQuery2, strlen(sqlQuery2), NULL, 0,
            &stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&bindValue, (char*) rowidAsString, rowidAsStringLength);
    if (dpiStmt_bindValueByPos(stmt2, 1, DPI_NATIVE_TYPE_BYTES,
            &bindValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt2, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt2, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for second query!");
    if (dpiStmt_getQueryValue(stmt2, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, queryValue->value.asInt64,
            7) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiStmt_release(stmt1) < 0)
        dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3401_convRowIdToStrAndVerifyOnIndexTab()
//   Prepare and execute any query that selects rowid on an index organized
// table. convert one of these rowids to string by calling
// dpiRowid_getStringValue() and perform a second query specifically
// for the row matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_3401_convRowIdToStrAndVerifyOnIndexTab(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sqlQuery1 = "select rowid from TestOrgIndex where IntCol = 3";
    const char *sqlQuery2 = "select IntCol from TestOrgIndex where rowid = :1";
    uint32_t bufferRowIndex, rowidAsStringLength;
    dpiData *queryValue, bindValue;
    dpiNativeTypeNum nativeTypeNum;
    const char *rowidAsString;
    dpiStmt *stmt1, *stmt2;
    dpiConn *conn;
    int found;

    // perform first query to get rowid
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sqlQuery1, strlen(sqlQuery1), NULL, 0,
            &stmt1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt1, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt1, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for first query!");
    if (dpiStmt_getQueryValue(stmt1, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_getStringValue(queryValue->value.asRowid, &rowidAsString,
            &rowidAsStringLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform second query to get row using rowid
    if (dpiConn_prepareStmt(conn, 0, sqlQuery2, strlen(sqlQuery2), NULL, 0,
            &stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&bindValue, (char*) rowidAsString, rowidAsStringLength);
    if (dpiStmt_bindValueByPos(stmt2, 1, DPI_NATIVE_TYPE_BYTES,
            &bindValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt2, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt2, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for second query!");
    if (dpiStmt_getQueryValue(stmt2, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, queryValue->value.asInt64,
            3) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiStmt_release(stmt1) < 0)
        dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3402_releaseRowIdTwice()
//   Fetch any rowid; close statement that fetched that rowid; call
// dpiRowid_release() twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_3402_releaseRowIdTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select rowid from TestOrgIndex where IntCol = 6";
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiData *rowidValue;
    dpiRowid *rowid;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase, "row not found!");
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &rowidValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    rowid = rowidValue->value.asRowid;
    if (dpiRowid_addRef(rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_release(rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiRowid_release(rowid);
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3403_verifyGetStrValWithNullRowId()
//   Call dpiRowid_getStringValue() with a NULL rowid (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_3403_verifyGetStrValWithNullRowId(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t rowidAsStringLength;
    const char *rowidAsString;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiRowid_getStringValue(NULL, &rowidAsString, &rowidAsStringLength);
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3404_verifySpecificRowIdOnRegTab()
//   Prepare and execute any query which selects a rowid from a regular table.
// Use one of these rowids to perform a second query specifically for the row
// matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_3404_verifySpecificRowIdOnRegTab(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sqlQuery1 = "select rowid from TestStrings where IntCol = 9";
    const char *sqlQuery2 = "select IntCol from TestStrings where rowid = :1";
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiStmt *stmt1, *stmt2;
    dpiData *queryValue;
    dpiConn *conn;
    int found;

    // perform first query to get rowid
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sqlQuery1, strlen(sqlQuery1), NULL, 0,
            &stmt1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt1, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt1, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for first query!");
    if (dpiStmt_getQueryValue(stmt1, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform second query to get row using rowid
    if (dpiConn_prepareStmt(conn, 0, sqlQuery2, strlen(sqlQuery2), NULL, 0,
            &stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByPos(stmt2, 1, DPI_NATIVE_TYPE_ROWID,
            queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt2, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt2, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for second query!");
    if (dpiStmt_getQueryValue(stmt2, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, queryValue->value.asInt64,
            9) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiStmt_release(stmt1) < 0)
        dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3405_verifySpecificRowIdOnIndexTab()
//   prepare and execute any query which selects rowid on an index organized
// table. use one of these rowids to perform a second query specifically
// for the row matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_3405_verifySpecificRowIdOnIndexTab(dpiTestCase *testCase,
                                        dpiTestParams *params)
{
    const char *sqlQuery1 = "select rowid from TestOrgIndex where IntCol = 8";
    const char *sqlQuery2 = "select IntCol from TestOrgIndex where rowid = :1";
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiStmt *stmt1, *stmt2;
    dpiData *queryValue;
    dpiConn *conn;
    int found;

    // perform first query to get rowid
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sqlQuery1, strlen(sqlQuery1), NULL, 0,
            &stmt1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt1, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt1, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for first query!");
    if (dpiStmt_getQueryValue(stmt1, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform second query to get row using rowid
    if (dpiConn_prepareStmt(conn, 0, sqlQuery2, strlen(sqlQuery2), NULL, 0,
            &stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByPos(stmt2, 1, DPI_NATIVE_TYPE_ROWID,
            queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt2, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt2, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase,
                "row not found for second query!");
    if (dpiStmt_getQueryValue(stmt2, 1, &nativeTypeNum, &queryValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, queryValue->value.asInt64,
            8) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiStmt_release(stmt1) < 0)
        dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3406_verifyGetLastRowidBeforeStmtExec()
//   Call dpiStmt_getLastRowid() before calling dpiStmt_execute() and verify
// that it returns NULL (no error).
//-----------------------------------------------------------------------------
int dpiTest_3406_verifyGetLastRowidBeforeStmtExec(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select IntCol from TestOrgIndex";
    dpiRowid *rowid;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (rowid)
        return dpiTestCase_setFailed(testCase, "Non-NULL rowid returned.");
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3407_verifyGetLastRowidForNonDMLStmt()
//   Call dpiStmt_getLastRowid() after calling dpiStmt_execute() on a non-DML
// statement and verify it returns NULL (no error).
//-----------------------------------------------------------------------------
int dpiTest_3407_verifyGetLastRowidForNonDMLStmt(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "truncate table TestTempTable";
    dpiRowid *rowid;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (rowid)
        return dpiTestCase_setFailed(testCase, "Non-NULL rowid returned.");
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3408_verifyGetLastRowidForDMLStmtWithNoRows()
//   Call dpiStmt_getLastRowid() after calling dpiStmt_execute() with a DML
// statement that doesn't affect any rows and verify it returns NULL (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_3408_verifyGetLastRowidForDMLStmtWithNoRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select IntCol from TestOrgIndex where rownum < 1";
    dpiRowid *rowid;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (rowid)
        return dpiTestCase_setFailed(testCase, "Non-NULL rowid returned.");
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3409_verifyGetLastRowidForDMLStmtWithOneRow()
//   Call dpiStmt_getLastRowid() after calling dpiStmt_execute() with a DML
// statement that affects one row and verify it returns a rowid (no error).
//-----------------------------------------------------------------------------
int dpiTest_3409_verifyGetLastRowidForDMLStmtWithOneRow(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "insert into TestTempTable values (1, 'test1')";
    uint32_t rowidAsStringLength;
    const char *rowidAsString;
    dpiRowid *rowid;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_getStringValue(rowid, &rowidAsString,
            &rowidAsStringLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3410_verifyGetLastRowidForDMLStmtWithMulRows()
//   Call dpiStmt_getLastRowid() after calling dpiStmt_execute() with a DML
// statement that affects multiple rows and verify it returns a rowid (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_3410_verifyGetLastRowidForDMLStmtWithMulRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "insert into TestTempTable values (:1, :2)";
    uint32_t rowidAsStringLength, numRows = 5, i;
    dpiData *intData, *strData;
    const char *rowidAsString;
    dpiVar *intVar, *strVar;
    char buffer[100];
    dpiRowid *rowid;
    dpiConn *conn;
    dpiStmt *stmt;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare and bind insert statement
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
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

    // populate some dummy data
    for (i = 0; i < numRows; i++) {
        dpiData_setInt64(&intData[i], i + 1);
        sprintf(buffer, "Dummy data %d", i + 1);
        if (dpiVar_setFromBytes(strVar, i, buffer, strlen(buffer)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_getStringValue(rowid, &rowidAsString,
            &rowidAsStringLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3411_verifyGetLastRowidWithaddRef()
//   Call dpiStmt_execute() with a DML statement that affects one row, verify
// dpiStmt_getLastRowid() returns a rowid and call dpiRowid_addRef() to acquire
// an independent reference, then release the statement and verify the rowid
// can still be used (no error).
//-----------------------------------------------------------------------------
int dpiTest_3411_verifyGetLastRowidWithaddRef(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "insert into TestTempTable values (1, 'test1')";
    uint32_t rowidAsStringLength;
    const char *rowidAsString;
    dpiRowid *rowid;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_addRef(rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_getStringValue(rowid, &rowidAsString,
            &rowidAsStringLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiRowid_release(rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3412_verifyGetLastRowidForInsertAllStmt()
//   Call dpiStmt_getLastRowid() after calling dpiStmt_execute() with an INSERT
// ALL statement that affects many rows and verify it returns NULL (no error).
//-----------------------------------------------------------------------------
int dpiTest_3412_verifyGetLastRowidForInsertAllStmt(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "insert all "
            "into TestTempTable(IntCol, StringCol) values (1, 'test1') "
            "into TestTempTable(IntCol, StringCol) values (2, 'test2') "
            "into TestTempTable(IntCol, StringCol) values (3, 'test3') "
            "select * from dual";
    dpiRowid *rowid;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (rowid)
        return dpiTestCase_setFailed(testCase, "Non-NULL rowid returned.");
    if (dpiStmt_release(stmt) < 0)
        dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3400);
    dpiTestSuite_addCase(dpiTest_3400_convRowIdToStrAndVerifyOnRegTab,
            "fetch rowid and refetch row via string rep (normal table)");
    dpiTestSuite_addCase(dpiTest_3401_convRowIdToStrAndVerifyOnIndexTab,
            "fetch rowid and refetch row via string rep (index org table)");
    dpiTestSuite_addCase(dpiTest_3402_releaseRowIdTwice,
            "call dpiRowid_release() twice");
    dpiTestSuite_addCase(dpiTest_3403_verifyGetStrValWithNullRowId,
            "verify dpiRowid_getStringValue() with NULL rowid");
    dpiTestSuite_addCase(dpiTest_3404_verifySpecificRowIdOnRegTab,
            "fetch rowid and refetch row (normal table)");
    dpiTestSuite_addCase(dpiTest_3405_verifySpecificRowIdOnIndexTab,
            "fetch rowid and refetch row (index org table)");
    dpiTestSuite_addCase(dpiTest_3406_verifyGetLastRowidBeforeStmtExec,
            "call dpiStmt_getLastRowid() before calling dpiStmt_execute");
    dpiTestSuite_addCase(dpiTest_3407_verifyGetLastRowidForNonDMLStmt,
            "call dpiStmt_getLastRowid() after executing non-DML");
    dpiTestSuite_addCase(dpiTest_3408_verifyGetLastRowidForDMLStmtWithNoRows,
            "call dpiStmt_getLastRowid() after executing DML (no rows)");
    dpiTestSuite_addCase(dpiTest_3409_verifyGetLastRowidForDMLStmtWithOneRow,
            "call dpiStmt_getLastRowid() after executing DML (one row)");
    dpiTestSuite_addCase(dpiTest_3410_verifyGetLastRowidForDMLStmtWithMulRows,
            "call dpiStmt_getLastRowid() after executing DML (many rows)");
    dpiTestSuite_addCase(dpiTest_3411_verifyGetLastRowidWithaddRef,
            "call dpiRowid_addRef() to verify independent reference");
    dpiTestSuite_addCase(dpiTest_3412_verifyGetLastRowidForInsertAllStmt,
            "call dpiStmt_getLastRowid() after INSERT ALL statement");
    return dpiTestSuite_run();
}
