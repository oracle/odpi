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
// test_2900_implicit_results.c
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
// dpiTest_2900_verifyNullIsReturnedIfNoImpResults()
//   Prepare and execute statement that does not produce implicit results;
// call dpiStmt_getImplicitResult() and confirm that NULL is returned (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_2900_verifyNullIsReturnedIfNoImpResults(dpiTestCase *testCase,
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
// dpiTest_2901_verifyImplicitResultsAsExpected()
//   Prepare and execute statements that return a varying number of implicit
// results and call dpiStmt_getImplicitResult() as many times as are needed
// to verify that the expected number of implicit results is returned;
// perform fetches from the implicit results and confirm results are
// as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_2901_verifyImplicitResultsAsExpected(dpiTestCase *testCase,
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
    dpiTestSuite_initialize(2900);
    dpiTestSuite_addCase(dpiTest_2900_verifyNullIsReturnedIfNoImpResults,
            "dpiStmt_getImplicitResult() without implicit results");
    dpiTestSuite_addCase(dpiTest_2901_verifyImplicitResultsAsExpected,
            "dpiStmt_getImplicitResult() returns expected results");
    return dpiTestSuite_run();
}
