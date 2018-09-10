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
// TestSubscriptions.c
//   Test suite for testing all the Subscriptions related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2200_verifyNewSubscriptionWithCallBkNULL()
//   Call dpiConn_subscribe() with protocol DPI_SUBSCR_PROTO_CALLBACK but
// with the callback attribute left NULL (error).
//-----------------------------------------------------------------------------
int dpiTest_2200_verifyNewSubscriptionWithCallBkNULL(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError =
            "ORA-24904: invalid callback attribute passed into OCI call";
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
    subParams.callback = NULL;
    subParams.protocol = DPI_SUBSCR_PROTO_CALLBACK;
    dpiConn_subscribe(conn, &subParams, &subscr);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_release(conn);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2201_verifyPubFuncsOfSubscrWithNull()
//   Call each of the dpiSubscr public functions with the subscr parameter set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2201_verifyPubFuncsOfSubscrWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSubscr handle";

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
    dpiTestSuite_initialize(2200);
    dpiTestSuite_addCase(dpiTest_2200_verifyNewSubscriptionWithCallBkNULL,
            "dpiConn_subscribe() without callback");
    dpiTestSuite_addCase(dpiTest_2201_verifyPubFuncsOfSubscrWithNull,
            "call all dpiSubscr public functions with subscr param as NULL");
    return dpiTestSuite_run();
}

