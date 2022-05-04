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
// test_1000_context.c
//   Test suite for testing dpiContext functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_1000_validMajorMinor()
//   Verify that dpiContext_createWithParams() succeeds when valid major and
// minor version numbers are passed.
//-----------------------------------------------------------------------------
int dpiTest_1000_validMajorMinor(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, NULL,
            &context, &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_destroy(context) < 0) {
        dpiContext_getError(context, &errorInfo);
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    }
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1001_diffMajor()
//   Verify that dpiContext_createWithParams() returns error DPI-1020 when
// called with a major version that doesn't match the one with which DPI was
// compiled.
//-----------------------------------------------------------------------------
int dpiTest_1001_diffMajor(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    dpiContext_createWithParams(DPI_MAJOR_VERSION + 1, DPI_MINOR_VERSION, NULL,
            &context, &errorInfo);
    return dpiTestCase_expectError(testCase, "DPI-1020:");
}


//-----------------------------------------------------------------------------
// dpiTest_1002_diffMinor()
//   Verify that dpiContext_createWithParams() returns error DPI-1020 when
// called with a minor version that doesn't match the one with which DPI was
// compiled.
//-----------------------------------------------------------------------------
int dpiTest_1002_diffMinor(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION + 1, NULL,
            &context, &errorInfo);
    return dpiTestCase_expectError(testCase, "DPI-1020:");
}


//-----------------------------------------------------------------------------
// dpiTest_1003_createWithNull()
//   Verify that dpiContext_createWithParams() when passed a NULL pointer
// returns error DPI-1046.
//-----------------------------------------------------------------------------
int dpiTest_1003_createWithNull(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;

    dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, NULL,
            NULL, &errorInfo);
    return dpiTestCase_expectError(testCase, "DPI-1046:");
}


//-----------------------------------------------------------------------------
// dpiTest_1004_destroyWithNull()
//   Verify that dpiContext_destroy() when passed a NULL pointer returns error
// DPI-1002.
//-----------------------------------------------------------------------------
int dpiTest_1004_destroyWithNull(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContext_destroy(NULL);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_1005_destroyTwice()
//   Verify that dpiContext_destroy() called twice returns error DPI-1002.
//-----------------------------------------------------------------------------
int dpiTest_1005_destroyTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiErrorInfo errorInfo;
    dpiContext *context;

    if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, NULL,
            &context, &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_destroy(context) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    dpiContext_destroy(context);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_1006_validCtxParams()
//   Verify that dpiContext_createWithParams() succeeds when valid
// dpiContextCreateParams is passed.
//-----------------------------------------------------------------------------
int dpiTest_1006_validCtxParams(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiContextCreateParams ctxParams = {0};
    dpiErrorInfo errorInfo;
    dpiContext *context;

    ctxParams.defaultEncoding = "ASCII";
    ctxParams.defaultDriverName = "dummy : 0.0.1";
    if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION,
            &ctxParams, &context, &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_destroy(context) < 0) {
        dpiContext_getError(context, &errorInfo);
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    }
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1007_multipleContexts()
//   Verify that dpiContext_createWithParams() can be called twice and the same
// version information is provided in both cases.
//-----------------------------------------------------------------------------
int dpiTest_1007_multipleContexts(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiVersionInfo versionInfo1, versionInfo2;
    dpiContext *context1, *context2;
    dpiErrorInfo errorInfo;

    // create first context
    if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION,
            NULL, &context1, &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_getClientVersion(context1, &versionInfo1) < 0) {
        dpiContext_getError(context1, &errorInfo);
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    }

    // create second context
    if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION,
            NULL, &context2, &errorInfo) < 0)
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    if (dpiContext_getClientVersion(context2, &versionInfo2) < 0) {
        dpiContext_getError(context2, &errorInfo);
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    }

    // verify version information is the same
    if (dpiTestCase_expectUintEqual(testCase, versionInfo1.versionNum,
            versionInfo2.versionNum) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiContext_destroy(context1) < 0) {
        dpiContext_getError(context1, &errorInfo);
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    }
    if (dpiContext_destroy(context2) < 0) {
        dpiContext_getError(context2, &errorInfo);
        return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1000);
    dpiTestSuite_addCase(dpiTest_1000_validMajorMinor,
            "dpiContext_createWithParams() with valid major/minor versions");
    dpiTestSuite_addCase(dpiTest_1001_diffMajor,
            "dpiContext_createWithParams() with invalid major version");
    dpiTestSuite_addCase(dpiTest_1002_diffMinor,
            "dpiContext_createWithParams() with invalid minor version");
    dpiTestSuite_addCase(dpiTest_1003_createWithNull,
            "dpiContext_createWithParams() with NULL pointer");
    dpiTestSuite_addCase(dpiTest_1004_destroyWithNull,
            "dpiContext_destroy() with NULL pointer");
    dpiTestSuite_addCase(dpiTest_1005_destroyTwice,
            "dpiContext_destroy() called twice on same pointer");
    dpiTestSuite_addCase(dpiTest_1006_validCtxParams,
            "dpiContext_createWithParams() with creation parameters");
    dpiTestSuite_addCase(dpiTest_1007_multipleContexts,
            "dpiContext_createWithParams() twice");
    return dpiTestSuite_run();
}
