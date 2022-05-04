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
// test_3000_scroll_cursors.c
//   Test suite for testing all the Scrollable Cursors related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"


//-----------------------------------------------------------------------------
// dpiTest__populateTable() [INTERNAL]
//   Populate table with a known set of data.
//-----------------------------------------------------------------------------
static int dpiTest__populateTable(dpiTestCase *testCase, dpiConn *conn)
{
    const char *truncateSql = "truncate table TestTempTable";
    const char *populateSql =
            "begin "
            "    for i in 1..30 loop "
            "        insert into TestTempTable values (i, 'Test Data ' || i); "
            "    end loop; "
            "end;";
    dpiStmt *stmt;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate table with known data
    if (dpiConn_prepareStmt(conn, 0, populateSql, strlen(populateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyFetchedRow() [INTERNAL]
//   Verify that the row fetched from the cursor is the expected row.
//-----------------------------------------------------------------------------
static int dpiTest__verifyFetchedRow(dpiTestCase *testCase, dpiStmt *stmt,
        uint64_t expectedValue)
{
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiData *data;
    int found;

    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, data->value.asInt64,
            expectedValue);
}


//-----------------------------------------------------------------------------
// dpiTest_3000_verifyNonScrQueryWithDiffFetchModes()
//   Prepare and execute a non scrollable query; call dpiStmt_scroll() with the
// mode parameter set to any value except DPI_MODE_FETCH_NEXT (error
// ORA-24391).
//-----------------------------------------------------------------------------
int dpiTest_3000_verifyNonScrQueryWithDiffFetchModes(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select intcol from TestOrgIndex";
    const char *expectedError = "ORA-24391:";
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
// dpiTest_3001_verifyModesFirstAndLastWithNoRows()
//   Prepare and execute scrollable query which returns no rows; call
// dpiStmt_scroll() with mode set to DPI_MODE_FETCH_LAST; call dpiStmt_fetch()
// and verify that no rows are available; call dpiStmt_scroll() with mode set
// to DPI_MODE_FETCH_FIRST; call dpiStmt_fetch() and verify that no rows are
// available (no error).
//-----------------------------------------------------------------------------
int dpiTest_3001_verifyModesFirstAndLastWithNoRows(dpiTestCase *testCase,
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
// dpiTest_3002_verifyModeAbsWithNoRows()
//   Prepare and execute scrollable query which returns no rows; call
// dpiStmt_scroll() with mode set to DPI_MODE_FETCH_ABSOLUTE and offset set to
// 1 (error DPI-1027).
//-----------------------------------------------------------------------------
int dpiTest_3002_verifyModeAbsWithNoRows(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select IntCol from TestOrgIndex where 1 = 0";
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_ABSOLUTE, 1, 0);
    if (dpiTestCase_expectError(testCase, "DPI-1027:") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3003_verifyModeRelativeWithNegVal()
//   Prepare and execute scrollable query; call dpiStmt_scroll() with mode set
// to DPI_MODE_FETCH_RELATIVE and offset set to a negative value (error
// DPI-1027).
//-----------------------------------------------------------------------------
int dpiTest_3003_verifyModeRelativeWithNegVal(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select intcol from TestOrgIndex";
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, -1, 0);
    if (dpiTestCase_expectError(testCase, "DPI-1027:") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3004_verifyModeRelativeWithCursorAtEnd()
//   Prepare and execute scrollable query; call dpiStmt_scroll() with mode set
// to DPI_MODE_FETCH_LAST; then call dpiStmt_scroll() with mode set to
// DPI_MODE_FETCH_RELATIVE and offset set to a positive value (error DPI-1027).
//-----------------------------------------------------------------------------
int dpiTest_3004_verifyModeRelativeWithCursorAtEnd(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select intcol from TestOrgIndex";
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
    if (dpiTestCase_expectError(testCase, "DPI-1027:") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3005_verifyModeRelativeWithDiffOffsets()
//   prepare and execute scrollable query which returns some rows; call
// dpiStmt_scroll() with mode set to DPI_MODE_FETCH_RELATIVE and offset set
// to values that exceed the array size (internal fetch required) and that
// do not exceed the array size (satisfied by internal buffers) (no error)
//-----------------------------------------------------------------------------
int dpiTest_3005_verifyModeRelativeWithDiffOffsets(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select IntCol from TestTempTable where IntCol < 21 "
            "order by IntCol";
    uint32_t numQueryColumns;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 5) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, 4, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyFetchedRow(testCase, stmt, 5) < 0)
        return DPI_FAILURE;
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, 10, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyFetchedRow(testCase, stmt, 15) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3006_verifyModesWithDiffOffsets()
//   prepare and execute scrollable query; call dpiStmt_scroll() with each
// possible mode from the enumeration dpiFetchMode, verifying that both
// fetches satisfied by the internal buffers and fetches not satisified by
// internal buffers provide the correct results (no error)
//-----------------------------------------------------------------------------
int dpiTest_3006_verifyModesWithDiffOffsets(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select IntCol from TestTempTable where IntCol < 22 "
            "order by IntCol";
    uint32_t numQueryColumns;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 1, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 5) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_ABSOLUTE, 2, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyFetchedRow(testCase, stmt, 2) < 0)
        return DPI_FAILURE;
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_ABSOLUTE, 15, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyFetchedRow(testCase, stmt, 15) < 0)
        return DPI_FAILURE;
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, 1, 4) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyFetchedRow(testCase, stmt, 20) < 0)
        return DPI_FAILURE;
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_FIRST, 0, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyFetchedRow(testCase, stmt, 1) < 0)
        return DPI_FAILURE;
    if (dpiStmt_scroll(stmt, DPI_MODE_FETCH_RELATIVE, 10, 10) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyFetchedRow(testCase, stmt, 21) < 0)
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
    dpiTestSuite_initialize(3000);
    dpiTestSuite_addCase(dpiTest_3000_verifyNonScrQueryWithDiffFetchModes,
            "verify non scrollable query with different fetch modes");
    dpiTestSuite_addCase(dpiTest_3001_verifyModesFirstAndLastWithNoRows,
            "dpiStmt_scroll() with modes first and last with no rows");
    dpiTestSuite_addCase(dpiTest_3002_verifyModeAbsWithNoRows,
            "dpiStmt_scroll() with absolute mode with no rows");
    dpiTestSuite_addCase(dpiTest_3003_verifyModeRelativeWithNegVal,
            "dpiStmt_scroll() with relative mode, negative offset");
    dpiTestSuite_addCase(dpiTest_3004_verifyModeRelativeWithCursorAtEnd,
            "dpiStmt_scroll() with last mode and then relative mode");
    dpiTestSuite_addCase(dpiTest_3005_verifyModeRelativeWithDiffOffsets,
            "dpiStmt_scroll() with relative mode, various offsets");
    dpiTestSuite_addCase(dpiTest_3006_verifyModesWithDiffOffsets,
            "dpiStmt_scroll() with all possible modes");
    return dpiTestSuite_run();
}
