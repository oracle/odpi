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
// TestMsgProps.c
//   Test suite for testing all the Message Properties related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_1700_releaseMsgPropsTwice()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1700_releaseMsgPropsTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiMsgProps_release(msgProps);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiMsgProps handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1701_verifyPubFuncsOfMsgPropsWithNull()
//   Call each of the dpiMsgProps public functions with the props parameter set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1701_verifyPubFuncsOfMsgPropsWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiMsgProps handle";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiMsgProps_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getNumAttempts(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getCorrelation(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getDelay(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getDeliveryMode(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getEnqTime(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getExceptionQ(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getExpiration(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getOriginalMsgId(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getPriority(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_getState(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_setCorrelation(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_setDelay(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_setExceptionQ(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_setExpiration(NULL, 2);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_setOriginalMsgId(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiMsgProps_setPriority(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1702_verifyCorrelationIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setCorrelation(); call
// dpiMsgProps_getCorrelation() and verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1702_verifyCorrelationIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *setValue = "_", *getValue;
    uint32_t getValueLength;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_setCorrelation(msgProps, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getCorrelation(msgProps, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1703_verifyDelayIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setDelay(); call
// dpiMsgProps_getDelay() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1703_verifyDelayIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    int32_t setValue = 5, getValue;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_setDelay(msgProps, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getDelay(msgProps, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1704_verifyExpirationIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setExpiration(); call
// dpiMsgProps_getExpiration() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1704_verifyExpirationIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    int32_t setValue = 8, getValue;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_setExpiration(msgProps, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getExpiration(msgProps, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1705_verifyPriorityIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setPriority(); call
// dpiMsgProps_getPriority() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1705_verifyPriorityIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    int32_t setValue = 2, getValue;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_setPriority(msgProps, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getPriority(msgProps, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1706_verifyMsgIdIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setOriginalMsgId(); call
// dpiMsgProps_getOriginalMsgId() and verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1706_verifyMsgIdIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *setValue = "id1", *getValue;
    uint32_t getValueLength;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_setOriginalMsgId(msgProps, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getOriginalMsgId(msgProps, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1707_verifyExcQIsSetAsExp()
//   call dpiConn_newMsgProps(); call dpiMsgProps_setExceptionQ(); call
// dpiMsgProps_getExceptionQ() and verify that the value returned matches
// the value that was set (no error)
//-----------------------------------------------------------------------------
int dpiTest_1707_verifyExcQIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *setValue = "excq", *getValue;
    uint32_t getValueLength;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_setExceptionQ(msgProps, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getExceptionQ(msgProps, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1700);
    dpiTestSuite_addCase(dpiTest_1700_releaseMsgPropsTwice,
            "call dpiMsgProps_release() twice");
    dpiTestSuite_addCase(dpiTest_1701_verifyPubFuncsOfMsgPropsWithNull,
            "call all dpiMsgProps public functions with props param as NULL");
    dpiTestSuite_addCase(dpiTest_1702_verifyCorrelationIsSetAsExp,
            "verify correlation is set as expected");
    dpiTestSuite_addCase(dpiTest_1703_verifyDelayIsSetAsExp,
            "verify delay is set as expected");
    dpiTestSuite_addCase(dpiTest_1704_verifyExpirationIsSetAsExp,
            "verify expiration is set as expected");
    dpiTestSuite_addCase(dpiTest_1705_verifyPriorityIsSetAsExp,
            "verify priority is set as expected");
    dpiTestSuite_addCase(dpiTest_1706_verifyMsgIdIsSetAsExp,
            "verify msgId is set as expected");
    dpiTestSuite_addCase(dpiTest_1707_verifyExcQIsSetAsExp,
            "verify exceptionQ is set as expected");
    return dpiTestSuite_run();
}
