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
// TestSodaCollCursor.c
//   Tests suite for testing SODA Collection Cursor Functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2700_verifySodaCollCursorFuncsWithNull()
//   Call all public collection cursor functions with NULL handle and verify
// that the correct error is returned in each case.
//-----------------------------------------------------------------------------
int dpiTest_2700_verifySodaCollCursorFuncsWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaCollCursor handle";

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
// dpiTest_2701_verifySodaCollCursoraddRef()
//   Verify dpiSodaCollCursor_addRef() works as expected.
//-----------------------------------------------------------------------------
int dpiTest_2701_verifySodaCollCursoraddRef(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaCollCursor handle";
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
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
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
    dpiTestSuite_initialize(2700);
    dpiTestSuite_addCase(dpiTest_2700_verifySodaCollCursorFuncsWithNull,
            "call collection cursor functions with NULL handle");
    dpiTestSuite_addCase(dpiTest_2701_verifySodaCollCursoraddRef,
            "dpiSodaCollCursor_addRef() works with valid parameters");
    return dpiTestSuite_run();
}

