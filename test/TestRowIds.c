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
// dpiTest_2000_convRowIdToStrAndVerifyOnRegTab()
//   Prepare and execute any query that selects rowid on a regular table.
// Convert one of these rowids to string by calling dpiRowid_getStringValue()
// and perform a second query specifically for the row matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_2000_convRowIdToStrAndVerifyOnRegTab(dpiTestCase *testCase,
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
// dpiTest_2001_convRowIdToStrAndVerifyOnIndexTab()
//   Prepare and execute any query that selects rowid on an index organized
// table. convert one of these rowids to string by calling
// dpiRowid_getStringValue() and perform a second query specifically
// for the row matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_2001_convRowIdToStrAndVerifyOnIndexTab(dpiTestCase *testCase,
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
// dpiTest_2002_releaseRowIdTwice()
//   Fetch any rowid; close statement that fetched that rowid; call
// dpiRowid_release() twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2002_releaseRowIdTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select rowid from TestOrgIndex where IntCol = 6";
    const char *expectedError = "DPI-1002: invalid dpiRowid handle";
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
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2003_verifyGetStrValWithNullRowId()
//   Call dpiRowid_getStringValue() with a NULL rowid (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2003_verifyGetStrValWithNullRowId(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiRowid handle";
    uint32_t rowidAsStringLength;
    const char *rowidAsString;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiRowid_getStringValue(NULL, &rowidAsString, &rowidAsStringLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2004_verifySpecificRowIdOnRegTab()
//   Prepare and execute any query which selects a rowid from a regular table.
// Use one of these rowids to perform a second query specifically for the row
// matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_2004_verifySpecificRowIdOnRegTab(dpiTestCase *testCase,
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
// dpiTest_2005_verifySpecificRowIdOnIndexTab()
//   prepare and execute any query which selects rowid on an index organized
// table. use one of these rowids to perform a second query specifically
// for the row matching that rowid.
//-----------------------------------------------------------------------------
int dpiTest_2005_verifySpecificRowIdOnIndexTab(dpiTestCase *testCase,
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
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2000);
    dpiTestSuite_addCase(dpiTest_2000_convRowIdToStrAndVerifyOnRegTab,
            "fetch rowid and refetch row via string rep (normal table)");
    dpiTestSuite_addCase(dpiTest_2001_convRowIdToStrAndVerifyOnIndexTab,
            "fetch rowid and refetch row via string rep (index org table)");
    dpiTestSuite_addCase(dpiTest_2002_releaseRowIdTwice,
            "call dpiRowid_release() twice");
    dpiTestSuite_addCase(dpiTest_2003_verifyGetStrValWithNullRowId,
            "verify dpiRowid_getStringValue() with NULL rowid");
    dpiTestSuite_addCase(dpiTest_2004_verifySpecificRowIdOnRegTab,
            "fetch rowid and refetch row (normal table)");
    dpiTestSuite_addCase(dpiTest_2005_verifySpecificRowIdOnIndexTab,
            "fetch rowid and refetch row (index org table)");
    return dpiTestSuite_run();
}

