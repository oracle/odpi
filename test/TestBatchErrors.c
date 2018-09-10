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
// TestBatchErrors.c
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
// dpiTest_2300_callExeManyWithArrDataAndVerifyErrAsExp()
//   Prepare array of data to insert that will result in errors (such as
// ORA-00001: unique constraint violations and ORA-01438: value larger than
// specified precision allowed for this column); call dpiStmt_executeMany()
// with mode set to DPI_MODE_EXEC_DEFAULT and confirm that the error returned
// matches the first error in the array of data that was prepared.
//-----------------------------------------------------------------------------
int dpiTest_2300_callExeManyWithArrDataAndVerifyErrAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    char expectedError[512];
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateTable(testCase, conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__prepareInsertWithErrors(testCase, conn, &stmt) < 0)
        return DPI_FAILURE;
    snprintf(expectedError, sizeof(expectedError),
            "ORA-00001: unique constraint (%.*s.TESTTEMPTABLE_PK) violated",
            params->mainUserNameLength, params->mainUserName);
    dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, NUM_ROWS);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2301_verifyBatchErrsAndOffsetAsExpected()
//   Prepare array of data to insert that will result in errors. Call
// dpiStmt_executeMany() with mode set to DPI_MODE_EXEC_BATCH_ERRORS; call
// dpiStmt_getBatchErrorCount() and confirm the count returned matches the
// number of errors expected; call dpiStmt_getBatchErrors() and confirm that
// the errors and row offsets returned match expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_2301_verifyBatchErrsAndOffsetAsExpected(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiErrorInfo errorInfo[NUM_ERR];
    char expectedError[512];
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
    strcpy(expectedError, "ORA-01438: value larger than specified precision "
            "allowed for this column");
    if (dpiTestCase_expectStringEqual(testCase, errorInfo[0].message,
            errorInfo[0].messageLength, expectedError,
            strlen(expectedError)) < 0)
        return DPI_FAILURE;
    snprintf(expectedError, sizeof(expectedError),
            "ORA-00001: unique constraint (%.*s.TESTTEMPTABLE_PK) violated",
            params->mainUserNameLength, params->mainUserName);
    if (dpiTestCase_expectStringEqual(testCase, errorInfo[1].message,
            errorInfo[1].messageLength, expectedError,
            strlen(expectedError)) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2302_verifyGetBatchErrorsWithLesserNumErrVal()
//   Prepare array of data to insert that will result in errors. Call
// dpiStmt_executeMany() with mode set to DPI_MODE_EXEC_BATCH_ERRORS; call
// dpiStmt_getBatchErrors() with a value for numErrors that is less than the
// number of errors expected (error DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_2302_verifyGetBatchErrorsWithLesserNumErrVal(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1018: array size of 1 is too small";
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
    dpiTestSuite_initialize(2300);
    dpiTestSuite_addCase(dpiTest_2300_callExeManyWithArrDataAndVerifyErrAsExp,
            "dpiStmt_executeMany() with batch errors");
    dpiTestSuite_addCase(dpiTest_2301_verifyBatchErrsAndOffsetAsExpected,
            "dpiStmt_getBatchErrors() returns expected results");
    dpiTestSuite_addCase(dpiTest_2302_verifyGetBatchErrorsWithLesserNumErrVal,
            "dpiStmt_getBatchErrors() with numErrors less than required");
    return dpiTestSuite_run();
}

