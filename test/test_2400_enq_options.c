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
// test_2400_enq_options.c
//   Test suite for testing all the Enqueue Options related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2400_releaseEnqOptionsTwice()
//   Call dpiConn_newEnqOptions(); call dpiEnqOptions_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2400_releaseEnqOptionsTwice(dpiTestCase *testCase,
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
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_2401_verifyPubFuncsOfEnqOptWithNULL()
//   Call each of the dpiEnqOptions public functions with the options parameter
// set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2401_verifyPubFuncsOfEnqOptWithNULL(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";
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
// dpiTest_2402_verifyVisIsSetAsExp()
//   Call dpiConn_newEnqOptions(), call dpiEnqOptions_setVisibility(),
// call dpiEnqOptions_getVisibility() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2402_verifyVisIsSetAsExp(dpiTestCase *testCase,
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
    dpiTestSuite_initialize(2400);
    dpiTestSuite_addCase(dpiTest_2400_releaseEnqOptionsTwice,
            "call dpiEnqOptions_release() twice");
    dpiTestSuite_addCase(dpiTest_2401_verifyPubFuncsOfEnqOptWithNULL,
            "call all dpiEnqOptions functions with options param as NULL");
    dpiTestSuite_addCase(dpiTest_2402_verifyVisIsSetAsExp,
            "verify visibility is set as expected");
    return dpiTestSuite_run();
}
