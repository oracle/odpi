//-----------------------------------------------------------------------------
// Copyright (c) 2018 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestSodaDoc.c
//   Test suite for testing SODA Document Functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2800_nullHandle()
//   Call all public document functions with a NULL handle and verify the
// expected error is returned.
//-----------------------------------------------------------------------------
int dpiTest_2800_nullHandle(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDoc handle";

    dpiSodaDoc_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getContent(NULL, NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getCreatedOn(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getKey(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getLastModified(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getMediaType(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getVersion(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2801_addRef()
//   Verify dpiSodaDoc_addRef() manages reference counts as expected.
//-----------------------------------------------------------------------------
int dpiTest_2801_addRef(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDoc handle";
    dpiSodaDoc *doc;
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_createDocument(db, NULL, 0, NULL, 0, NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
       return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_addRef(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiSodaDoc_release(doc);
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
    dpiTestSuite_initialize(2800);
    dpiTestSuite_addCase(dpiTest_2800_nullHandle,
            "verify public document functions with NULL");
    dpiTestSuite_addCase(dpiTest_2801_addRef,
            "verify dpiSodaDoc_addRef() works as expected");
    return dpiTestSuite_run();
}

