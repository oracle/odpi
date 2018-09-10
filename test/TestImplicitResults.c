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
// TestImpResults.c
//   Test suite for testing all the Implicit Results related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define SQL_NOIMP_RES       "begin null; end;"

#define SQL_IMP_RES            "declare " \
                            "    c1 sys_refcursor; " \
                            "    c2 sys_refcursor; " \
                            "begin " \
                            " " \
                            "    open c1 for " \
                            "    select NumberCol " \
                            "    from TestNumbers " \
                            "    where IntCol between 3 and 4; " \
                            " " \
                            "    dbms_sql.return_result(c1); " \
                            " " \
                            "    open c2 for " \
                            "    select NumberCol " \
                            "    from TestNumbers " \
                            "    where IntCol between 7 and 8; " \
                            " " \
                            "    dbms_sql.return_result(c2); " \
                            "end;"

//-----------------------------------------------------------------------------
// dpiTest_2000_verifyNullIsReturnedIfNoImpResults()
//   Prepare and execute statement that does not produce implicit results;
// call dpiStmt_getImplicitResult() and confirm that NULL is returned (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_2000_verifyNullIsReturnedIfNoImpResults(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiStmt *stmt, *impResult;
    uint32_t numQueryColumns;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, SQL_NOIMP_RES, strlen(SQL_NOIMP_RES),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getImplicitResult(stmt, &impResult) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (impResult)
        return dpiTestCase_setFailed(testCase,
                "expected NULL implicit result for query");
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2001_verifyImplicitResultsAsExpected()
//   Prepare and execute statements that return a varying number of implicit
// results and call dpiStmt_getImplicitResult() as many times as are needed
// to verify that the expected number of implicit results is returned;
// perform fetches from the implicit results and confirm results are
// as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_2001_verifyImplicitResultsAsExpected(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t numQueryColumns, bufferRowIndex;
    double result[4] = {3.75, 5, 8.75, 10};
    dpiNativeTypeNum nativeTypeNum;
    dpiStmt *stmt, *impResult;
    dpiData *doubleValue;
    dpiConn *conn;
    int found, i;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, SQL_IMP_RES, strlen(SQL_IMP_RES), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    i = 0;
    while (1) {
        if (dpiStmt_getImplicitResult(stmt, &impResult) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!impResult)
            break;
        while (1) {
            if (dpiStmt_fetch(impResult, &found, &bufferRowIndex) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (!found)
                break;
            if (dpiStmt_getQueryValue(impResult, 1, &nativeTypeNum,
                    &doubleValue) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (dpiTestCase_expectDoubleEqual(testCase,
                    doubleValue->value.asDouble, result[i++]) < 0)
                return dpiTestCase_setFailedFromError(testCase);
        }
        if (dpiStmt_release(impResult) < 0)
            return DPI_FAILURE;
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2000);
    dpiTestSuite_addCase(dpiTest_2000_verifyNullIsReturnedIfNoImpResults,
            "dpiStmt_getImplicitResult() without implicit results");
    dpiTestSuite_addCase(dpiTest_2001_verifyImplicitResultsAsExpected,
            "dpiStmt_getImplicitResult() returns expected results");
    return dpiTestSuite_run();
}

