//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
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
// dpiTest_100_validMajorMinor()
//   Verify that dpiContext_create() succeeds when valid major and minor
// version numbers are passed.
//-----------------------------------------------------------------------------
int dpiTest_100_validMajorMinor(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_101_diffMajor()
//   Verify that dpiContext_create() returns error DPI-1020 when called with a
// major version that doesn't match the one with which DPI was compiled.
//-----------------------------------------------------------------------------
int dpiTest_101_diffMajor(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    dpiContext_create(DPI_MAJOR_VERSION + 1, DPI_MINOR_VERSION, &context,
            &errorInfo);
    return dpiTestCase_expectError(testCase, "DPI-1020:");
}


//-----------------------------------------------------------------------------
// dpiTest_102_diffMinor()
//   Verify that dpiContext_create() returns error DPI-1020 when called with a
// minor version that doesn't match the one with which DPI was compiled.
//-----------------------------------------------------------------------------
int dpiTest_102_diffMinor(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION + 1, &context,
            &errorInfo);
    return dpiTestCase_expectError(testCase, "DPI-1020:");
}


//-----------------------------------------------------------------------------
// dpiTest_103_createWithNull()
//   Verify that dpiContext_create() when passed a NULL pointer returns error
// DPI-1046.
//-----------------------------------------------------------------------------
int dpiTest_103_createWithNull(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;

    dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, NULL, &errorInfo);
    return dpiTestCase_expectError(testCase, "DPI-1046:");
}


//-----------------------------------------------------------------------------
// dpiTest_104_destroyWithNull()
//   Verify that dpiContext_destroy() when passed a NULL pointer returns error
// DPI-1002.
//-----------------------------------------------------------------------------
int dpiTest_104_destroyWithNull(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext_destroy(NULL);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_105_destroyTwice()
//   Verify that dpiContext_destroy() called twice returns error DPI-1002.
//-----------------------------------------------------------------------------
int dpiTest_105_destroyTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    if (dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, &context,
            &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_destroy(context) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    dpiContext_destroy(context);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(100);
    dpiTestSuite_addCase(dpiTest_100_validMajorMinor,
            "dpiContext_create() with valid major/minor versions");
    dpiTestSuite_addCase(dpiTest_101_diffMajor,
            "dpiContext_create() with invalid major version");
    dpiTestSuite_addCase(dpiTest_102_diffMinor,
            "dpiContext_create() with invalid minor version");
    dpiTestSuite_addCase(dpiTest_103_createWithNull,
            "dpiContext_create() with NULL pointer");
    dpiTestSuite_addCase(dpiTest_104_destroyWithNull,
            "dpiContext_destroy() with NULL pointer");
    dpiTestSuite_addCase(dpiTest_105_destroyTwice,
            "dpiContext_destroy() called twice on same pointer");
    return dpiTestSuite_run();
}

