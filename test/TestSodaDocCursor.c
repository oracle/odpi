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
// TestSodaDocCursor.c
//   Test suite for testing SODA Document Cursor Functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__dropCollection()
// Drops the collection  and verifies that it was actually dropped.
//-----------------------------------------------------------------------------
int dpiTest__dropCollection(dpiTestCase *testCase, dpiSodaDb *db,
        const char *collName)
{
    dpiSodaColl *coll;
    int isDropped;

    if (dpiSodaDb_openCollection(db, collName, strlen(collName),
                DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_drop(coll, DPI_SODA_FLAGS_DEFAULT, &isDropped) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isDropped, 1) < 0)
        return DPI_FAILURE;
    if (dpiSodaColl_release(coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}



//-----------------------------------------------------------------------------
// dpiTest_2900_nullHandle()
// Call all public document cursor functions with NULL argument and verify.
//-----------------------------------------------------------------------------
int dpiTest_2900_nullHandle(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDocCursor handle";

    dpiSodaDocCursor_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDocCursor_getNext(NULL, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDocCursor_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2901_addRef()
//   verify dpiSodaDocCursor_addRef() increments the reference count as
// expected.
//-----------------------------------------------------------------------------
int dpiTest_2901_addRef(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDocCursor handle";
    const char *collName = "ODPIC_COLL_2901";
    dpiSodaDocCursor *cursor;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_find(coll, NULL, DPI_SODA_FLAGS_DEFAULT, &cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDocCursor_addRef(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDocCursor_release(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDocCursor_release(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiSodaDocCursor_release(cursor);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
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
    dpiTestSuite_initialize(2900);
    dpiTestSuite_addCase(dpiTest_2900_nullHandle,
            "call all functions with NULL handle");
    dpiTestSuite_addCase(dpiTest_2901_addRef,
            "dpiSodaDocCursor_addRef() with valid parameters");
    return dpiTestSuite_run();
}

