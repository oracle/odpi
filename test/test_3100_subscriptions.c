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
// test_3100_subscriptions.c
//   Test suite for testing all the Subscriptions related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_3100_verifyNewSubscriptionWithCallBkNULL()
//   Call dpiConn_subscribe() with protocol DPI_SUBSCR_PROTO_CALLBACK but
// with the callback attribute left NULL (error).
//-----------------------------------------------------------------------------
int dpiTest_3100_verifyNewSubscriptionWithCallBkNULL(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiCommonCreateParams commonParams;
    dpiSubscrCreateParams subParams;
    dpiContext *context;
    dpiSubscr *subscr;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    commonParams.createMode = DPI_MODE_CREATE_EVENTS;
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiContext_initSubscrCreateParams(context, &subParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    subParams.protocol = DPI_SUBSCR_PROTO_CALLBACK;
    dpiConn_subscribe(conn, &subParams, &subscr);
    if (dpiTestCase_expectError(testCase, "ORA-24904:") < 0)
        return DPI_FAILURE;
    dpiConn_release(conn);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3101_verifyPubFuncsOfSubscrWithNull()
//   Call each of the dpiSubscr public functions with the subscr parameter set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_3101_verifyPubFuncsOfSubscrWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    dpiSubscr_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSubscr_prepareStmt(NULL, NULL, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSubscr_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3100);
    dpiTestSuite_addCase(dpiTest_3100_verifyNewSubscriptionWithCallBkNULL,
            "dpiConn_subscribe() without callback");
    dpiTestSuite_addCase(dpiTest_3101_verifyPubFuncsOfSubscrWithNull,
            "call all dpiSubscr public functions with subscr param as NULL");
    return dpiTestSuite_run();
}
