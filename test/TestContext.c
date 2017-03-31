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
// TestContext.c
//   Test suite for testing dpiContext functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// Test_ValidMajorMinor() [PRIVATE]
//   Verify that dpiContext_create() succeeds when valid major and minor
// version numbers are passed.
//-----------------------------------------------------------------------------
int Test_ValidMajorMinor(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    if (dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, &context,
            &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_destroy(context) < 0) {
        dpiContext_getError(context, &errorInfo);
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    }
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_DiffMajor() [PRIVATE]
//   Verify that dpiContext_create() returns error DPI-1020 when called with a
// major version that doesn't match the one with which DPI was compiled.
//-----------------------------------------------------------------------------
int Test_DiffMajor(dpiTestCase *testCase, dpiTestParams *params)
{
    char expectedMessage[200];
    dpiErrorInfo errorInfo;
    dpiContext *context;

    dpiContext_create(DPI_MAJOR_VERSION + 1, DPI_MINOR_VERSION, &context,
            &errorInfo);
    sprintf(expectedMessage, "DPI-1020: version %d.%d is not supported by "
            "ODPI-C library version %d.%d", DPI_MAJOR_VERSION + 1,
            DPI_MINOR_VERSION, DPI_MAJOR_VERSION, DPI_MINOR_VERSION);
    return dpiTestCase_expectError(testCase, expectedMessage);
}


//-----------------------------------------------------------------------------
// Test_DiffMinor() [PRIVATE]
//   Verify that dpiContext_create() returns error DPI-1020 when called with a
// minor version that doesn't match the one with which DPI was compiled.
//-----------------------------------------------------------------------------
int Test_DiffMinor(dpiTestCase *testCase, dpiTestParams *params)
{
    char expectedMessage[200];
    dpiErrorInfo errorInfo;
    dpiContext *context;

    dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION + 1, &context,
            &errorInfo);
    sprintf(expectedMessage, "DPI-1020: version %d.%d is not supported by "
            "ODPI-C library version %d.%d", DPI_MAJOR_VERSION,
            DPI_MINOR_VERSION + 1, DPI_MAJOR_VERSION, DPI_MINOR_VERSION);
    return dpiTestCase_expectError(testCase, expectedMessage);
}


//-----------------------------------------------------------------------------
// Test_CreateWithNullPointer() [PRIVATE]
//   Verify that dpiContext_create() when passed a NULL pointer returns error
// DPI-1046.
//-----------------------------------------------------------------------------
int Test_CreateWithNullPointer(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;

    dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, NULL, &errorInfo);
    return dpiTestCase_expectError(testCase,
            "DPI-1046: parameter context cannot be a NULL pointer");
}


//-----------------------------------------------------------------------------
// Test_DestroyWithNullPointer() [PRIVATE]
//   Verify that dpiContext_destroy() when passed a NULL pointer returns error
// DPI-1002.
//-----------------------------------------------------------------------------
int Test_DestroyWithNullPointer(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext_destroy(NULL);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiContext handle");
}


//-----------------------------------------------------------------------------
// Test_ContextDestroyTwice() [PRIVATE]
//   Verify that dpiContext_destroy() called twice returns error DPI-1002.
//-----------------------------------------------------------------------------
int Test_ContextDestroyTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    if (dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, &context,
            &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_destroy(context) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    dpiContext_destroy(context);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiContext handle");
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(100);
    dpiTestSuite_addCase(Test_ValidMajorMinor,
            "dpiContext_create() with valid major/minor versions");
    dpiTestSuite_addCase(Test_DiffMajor,
            "dpiContext_create() with invalid major version");
    dpiTestSuite_addCase(Test_DiffMinor,
            "dpiContext_create() with invalid minor version");
    dpiTestSuite_addCase(Test_CreateWithNullPointer,
            "dpiContext_create() with NULL pointer");
    dpiTestSuite_addCase(Test_DestroyWithNullPointer,
            "dpiContext_destroy() with NULL pointer");
    dpiTestSuite_addCase(Test_ContextDestroyTwice,
            "dpiContext_destroy() called twice on same pointer");
    return dpiTestSuite_run();
}

