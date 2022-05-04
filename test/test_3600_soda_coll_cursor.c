//-----------------------------------------------------------------------------
// Copyright (c) 2018, 2022, Oracle and/or its affiliates.
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
// test_3600_soda_coll_cursor.c
//   Tests suite for testing SODA Collection Cursor Functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_3600_verifySodaCollCursorFuncsWithNull()
//   Call all public collection cursor functions with NULL handle and verify
// that the correct error is returned in each case.
//-----------------------------------------------------------------------------
int dpiTest_3600_verifySodaCollCursorFuncsWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    dpiSodaCollCursor_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaCollCursor_getNext(NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaCollCursor_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3601_verifySodaCollCursoraddRef()
//   Verify dpiSodaCollCursor_addRef() works as expected.
//-----------------------------------------------------------------------------
int dpiTest_3601_verifySodaCollCursoraddRef(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiSodaCollCursor *cursor;
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_getCollections(db, NULL, 0, DPI_SODA_FLAGS_DEFAULT,
            &cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaCollCursor_addRef(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaCollCursor_release(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaCollCursor_release(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiSodaCollCursor_release(cursor);
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3600);
    dpiTestSuite_addCase(dpiTest_3600_verifySodaCollCursorFuncsWithNull,
            "call collection cursor functions with NULL handle");
    dpiTestSuite_addCase(dpiTest_3601_verifySodaCollCursoraddRef,
            "dpiSodaCollCursor_addRef() works with valid parameters");
    return dpiTestSuite_run();
}
