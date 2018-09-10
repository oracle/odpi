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
// TestEnqOptions.c
//   Test suite for testing all the Enqueue Options related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_1500_releaseEnqOptionsTwice()
//   Call dpiConn_newEnqOptions(); call dpiEnqOptions_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1500_releaseEnqOptionsTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiEnqOptions *enqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiEnqOptions_release(enqOptions);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiEnqOptions handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1501_verifyPubFuncsOfEnqOptWithNULL()
//   Call each of the dpiEnqOptions public functions with the options parameter
// set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1501_verifyPubFuncsOfEnqOptWithNULL(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiEnqOptions handle";
    dpiVisibility visibility;
    uint32_t valueLength;
    const char *value;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiEnqOptions_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiEnqOptions_getTransformation(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiEnqOptions_getVisibility(NULL, &visibility);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiEnqOptions_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiEnqOptions_setDeliveryMode(NULL, DPI_MODE_MSG_PERSISTENT);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiEnqOptions_setTransformation(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiEnqOptions_setVisibility(NULL, DPI_VISIBILITY_IMMEDIATE);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1502_verifyVisIsSetAsExp()
//   Call dpiConn_newEnqOptions(), call dpiEnqOptions_setVisibility(),
// call dpiEnqOptions_getVisibility() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1502_verifyVisIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiVisibility visModes[] = {DPI_VISIBILITY_ON_COMMIT,
            DPI_VISIBILITY_IMMEDIATE, -1}, getValue;
    dpiEnqOptions *enqOptions;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; visModes[i] != -1; i++) {
        if (dpiEnqOptions_setVisibility(enqOptions, visModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiEnqOptions_getVisibility(enqOptions, &getValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, getValue, visModes[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1500);
    dpiTestSuite_addCase(dpiTest_1500_releaseEnqOptionsTwice,
            "call dpiEnqOptions_release() twice");
    dpiTestSuite_addCase(dpiTest_1501_verifyPubFuncsOfEnqOptWithNULL,
            "call all dpiEnqOptions functions with options param as NULL");
    dpiTestSuite_addCase(dpiTest_1502_verifyVisIsSetAsExp,
            "verify visibility is set as expected");
    return dpiTestSuite_run();
}

