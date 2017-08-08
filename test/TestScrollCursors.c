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
// TestScrollCursors.c
//   Test suite for testing all the Scrollable Cursors related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2100_verifyNonScrQueryWithDiffFetchModes()
//   Prepare and execute a non scrollable query; call dpiStmt_scroll() with the
// mode parameter set to any value except DPI_MODE_FETCH_NEXT (error
// ORA-24391).
//-----------------------------------------------------------------------------
int dpiTest_2100_verifyNonScrQueryWithDiffFetchModes(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "ORA-24391: invalid fetch operation";
    const char *sql = "select intcol from TestOrgIndex";
    uint32_t numQueryColumns;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_FIRST, 0, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_PRIOR, 0, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_ABSOLUTE, 3, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, 2, 3);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2101_verifyModesFirstAndLastWithNoRows()
//   Prepare and execute scrollable query which returns no rows; call
// dpiStmt_scroll() with mode set to DPI_MODE_FETCH_LAST; call dpiStmt_fetch()
// and verify that no rows are available; call dpiStmt_scroll() with mode set
// to DPI_MODE_FETCH_FIRST; call dpiStmt_fetch() and verify that no rows are
// available (no error).
//-----------------------------------------------------------------------------
int dpiTest_2101_verifyModesFirstAndLastWithNoRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select IntCol from TestOrgIndex where 1 = 0";
    uint32_t bufferRowIndex;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_LAST, 0, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (found)
        return dpiTestCase_setFailed(testCase,
                "last row should not be found!");
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_FIRST, 0, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (found)
        return dpiTestCase_setFailed(testCase,
                "first row should not be found!");
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2102_verifyModeAbsWithNoRows()
//   Prepare and execute scrollable query which returns no rows; call
// dpiStmt_scroll() with mode set to DPI_MODE_FETCH_ABSOLUTE and offset set to
// 1 (error DPI-1027).
//-----------------------------------------------------------------------------
int dpiTest_2102_verifyModeAbsWithNoRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select IntCol from TestOrgIndex where 1 = 0";
    const char *expectedError =
            "DPI-1027: scroll operation would go out of the result set";
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_ABSOLUTE, 1, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2103_verifyModeRelativeWithNegVal()
//   Prepare and execute scrollable query; call dpiStmt_scroll() with mode set
// to DPI_MODE_FETCH_RELATIVE and offset set to a negative value (error
// DPI-1027).
//-----------------------------------------------------------------------------
int dpiTest_2103_verifyModeRelativeWithNegVal(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select intcol from TestOrgIndex";
    const char *expectedError =
            "DPI-1027: scroll operation would go out of the result set";
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, -1, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2104_verifyModeRelativeWithCursorAtEnd()
//   Prepare and execute scrollable query; call dpiStmt_scroll() with mode set
// to DPI_MODE_FETCH_LAST; then call dpiStmt_scroll() with mode set to
// DPI_MODE_FETCH_RELATIVE and offset set to a positive value (error DPI-1027).
//-----------------------------------------------------------------------------
int dpiTest_2104_verifyModeRelativeWithCursorAtEnd(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select intcol from TestOrgIndex";
    const char *expectedError =
            "DPI-1027: scroll operation would go out of the result set";
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_LAST, 0, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, 2, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
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
    dpiTestSuite_initialize(2100);
    dpiTestSuite_addCase(dpiTest_2100_verifyNonScrQueryWithDiffFetchModes,
            "verify non scrollable query with different fetch modes");
    dpiTestSuite_addCase(dpiTest_2101_verifyModesFirstAndLastWithNoRows,
            "dpiStmt_scroll() with modes first and last with no rows");
    dpiTestSuite_addCase(dpiTest_2102_verifyModeAbsWithNoRows,
            "dpiStmt_scroll() with absolute mode with no rows");
    dpiTestSuite_addCase(dpiTest_2103_verifyModeRelativeWithNegVal,
            "dpiStmt_scroll() with relative mode, negative offset");
    dpiTestSuite_addCase(dpiTest_2104_verifyModeRelativeWithCursorAtEnd,
            "dpiStmt_scroll() with last mode and then relative mode");
    return dpiTestSuite_run();
}

