//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2023, Oracle and/or its affiliates.
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
// test_3200_batch_errors.c
//   Test suite for testing all the Batch Errors related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define NUM_ROWS                        3
#define NUM_ERR                         2

//-----------------------------------------------------------------------------
// dpiTest__prepareInsertWithErrors()
//   Prepare insert statement for execution with known errors.
//-----------------------------------------------------------------------------
int dpiTest__prepareInsertWithErrors(dpiTestCase *testCase, dpiConn *conn,
        dpiStmt **stmt)
{
    const char *stringValues[NUM_ROWS] = { "TEST 1", "TEST 2", "TEST 3" };
    const char *sql = "insert into TestTempTable values (:1, :2)";
    int64_t intValues[NUM_ROWS] = { 3, 3, 71113434343434 };
    dpiData *intColValue, *stringColValue;
    dpiVar *intColVar, *stringColVar;
    int i;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            NUM_ROWS, 0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(*stmt, 1, intColVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            NUM_ROWS, 30, 0, 0, NULL, &stringColVar, &stringColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(*stmt, 2, stringColVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_ROWS; i++) {
        dpiData_setInt64(&intColValue[i], intValues[i]);
        if (dpiVar_setFromBytes(stringColVar, i, stringValues[i],
                strlen(stringValues[i])) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(intColVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(stringColVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__truncateTable()
//   Truncate test table.
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
// dpiTest_3200_callExeManyWithArrDataAndVerifyErrAsExp()
//   Prepare array of data to insert that will result in errors (such as
// ORA-00001: unique constraint violations and ORA-01438: value larger than
// specified precision allowed for this column); call dpiStmt_executeMany()
// with mode set to DPI_MODE_EXEC_DEFAULT and confirm that the error returned
// matches the first error in the array of data that was prepared.
//-----------------------------------------------------------------------------
int dpiTest_3200_callExeManyWithArrDataAndVerifyErrAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__prepareInsertWithErrors(testCase, conn, &stmt) < 0)
        return DPI_FAILURE;
    dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, NUM_ROWS);
    if (dpiTestCase_expectError(testCase, "ORA-00001:") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3201_verifyBatchErrsAndOffsetAsExpected()
//   Prepare array of data to insert that will result in errors. Call
// dpiStmt_executeMany() with mode set to DPI_MODE_EXEC_BATCH_ERRORS; call
// dpiStmt_getBatchErrorCount() and confirm the count returned matches the
// number of errors expected; call dpiStmt_getBatchErrors() and confirm that
// the errors and row offsets returned match expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_3201_verifyBatchErrsAndOffsetAsExpected(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiErrorInfo errorInfo[NUM_ERR];
    uint32_t count;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__prepareInsertWithErrors(testCase, conn, &stmt) < 0)
        return DPI_FAILURE;
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_BATCH_ERRORS, NUM_ROWS) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBatchErrorCount(stmt, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, count, NUM_ERR) < 0)
        return DPI_FAILURE;
    if (dpiStmt_getBatchErrors(stmt, NUM_ERR, errorInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectErrorInfo(testCase, &errorInfo[0], "ORA-01438:") < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectErrorInfo(testCase, &errorInfo[1], "ORA-00001:") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3202_verifyGetBatchErrorsWithLesserNumErrVal()
//   Prepare array of data to insert that will result in errors. Call
// dpiStmt_executeMany() with mode set to DPI_MODE_EXEC_BATCH_ERRORS; call
// dpiStmt_getBatchErrors() with a value for numErrors that is less than the
// number of errors expected (error DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_3202_verifyGetBatchErrorsWithLesserNumErrVal(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__prepareInsertWithErrors(testCase, conn, &stmt) < 0)
        return DPI_FAILURE;
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_BATCH_ERRORS, NUM_ROWS) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getBatchErrors(stmt, 1, &errorInfo);
    if (dpiTestCase_expectError(testCase, "DPI-1018:") < 0)
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
    dpiTestSuite_initialize(3200);
    dpiTestSuite_addCase(dpiTest_3200_callExeManyWithArrDataAndVerifyErrAsExp,
            "dpiStmt_executeMany() with batch errors");
    dpiTestSuite_addCase(dpiTest_3201_verifyBatchErrsAndOffsetAsExpected,
            "dpiStmt_getBatchErrors() returns expected results");
    dpiTestSuite_addCase(dpiTest_3202_verifyGetBatchErrorsWithLesserNumErrVal,
            "dpiStmt_getBatchErrors() with numErrors less than required");
    return dpiTestSuite_run();
}
