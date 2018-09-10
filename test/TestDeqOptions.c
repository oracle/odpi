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
// TestDeqOptions.c
//   Test suite for testing all the Dequeue Options related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_1600_releaseDeqOptionsTwice()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1600_releaseDeqOptionsTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiDeqOptions_release(deqOptions);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiDeqOptions handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1601_verifyPubFuncsOfDeqOptWithNull()
//   Call each of the dpiDeqOptions public functions with the options parameter
// set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1601_verifyPubFuncsOfDeqOptWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiDeqOptions handle";
    dpiDeqNavigation navigation;
    uint32_t valueLength, time;
    dpiVisibility visibility;
    const char *value;
    dpiDeqMode mode;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getCondition(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getConsumerName(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getCorrelation(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getMode(NULL, &mode);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getMsgId(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getNavigation(NULL, &navigation);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getTransformation(NULL, &value, &valueLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getVisibility(NULL, &visibility);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_getWait(NULL, &time);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setCondition(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setConsumerName(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setCorrelation(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setDeliveryMode(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setMode(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setMsgId(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setNavigation(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setTransformation(NULL, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setVisibility(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiDeqOptions_setWait(NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1602_verifyNavigationIsSetAsExp()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_setNavigation(); call
// dpiDeqOptions_getNavigation() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1602_verifyNavigationIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiDeqNavigation setValue = DPI_DEQ_NAV_FIRST_MSG, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getNavigation(deqOptions, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1603_verifyWaitTimeIsSetAsExp()
//   call dpiConn_newDeqOptions(); call dpiDeqOptions_setWait(); call
// dpiDeqOptions_getWait() and verify that the value returned matches
// the value that was set (no error)
//-----------------------------------------------------------------------------
int dpiTest_1603_verifyWaitTimeIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t setValue = 4, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getWait(deqOptions, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1604_verifyModeIsSetAsExp()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_setMode(); call
// dpiDeqOptions_getMode() and verify that the value returned matches the value
// that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1604_verifyModeIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiDeqMode setValue = DPI_MODE_DEQ_BROWSE, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setMode(deqOptions, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getMode(deqOptions, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, getValue, setValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1605_verifyCorrelationIsSetAsExp()
//   Call dpiConn_newDeqOptions(); call dpiDeqOptions_setCorrelation(); call
// dpiDeqOptions_getCorrelation() and verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1605_verifyCorrelationIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *setValue = "_", *getValue;
    dpiDeqOptions *deqOptions;
    uint32_t getValueLength;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setCorrelation(deqOptions, setValue,
            strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCorrelation(deqOptions, &getValue,
            &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1606_verifyVisIsSetAsExp()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setVisibility(),
// call dpiDeqOptions_getVisibility() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1606_verifyVisIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiVisibility visModes[] = {DPI_VISIBILITY_ON_COMMIT,
            DPI_VISIBILITY_IMMEDIATE, -1}, getValue;
    dpiDeqOptions *deqOptions;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; visModes[i] != -1; i++) {
        if (dpiDeqOptions_setVisibility(deqOptions, visModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiDeqOptions_getVisibility(deqOptions, &getValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, getValue, visModes[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1607_verifyMsgIdIsSetAsExp()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setMsgId(),
// call dpiDeqOptions_getMsgId() and verify that the value returned matches
// the value that was set (no error)
//-----------------------------------------------------------------------------
int dpiTest_1607_verifyMsgIdIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *setMsgId = "TEST_MSGID", *getMsgId;
    dpiDeqOptions *deqOptions;
    uint32_t getMsgIdLen;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getMsgId(deqOptions, &getMsgId, &getMsgIdLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getMsgId, getMsgIdLen, NULL,
            0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setMsgId(deqOptions, setMsgId, strlen(setMsgId)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getMsgId(deqOptions, &getMsgId, &getMsgIdLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getMsgId, getMsgIdLen,
            setMsgId, strlen(setMsgId)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1608_verifyCondIsSetAsExp()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setCondition(),
// call dpiDeqOptions_getCondition() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1608_verifyCondIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *setCond = "tab.priority between 2 and 4", *getCond;
    dpiDeqOptions *deqOptions;
    uint32_t getCondLen;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCondition(deqOptions, &getCond, &getCondLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getCond, getCondLen,
            NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setCondition(deqOptions, setCond, strlen(setCond)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCondition(deqOptions, &getCond, &getCondLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getCond, getCondLen,
            setCond, strlen(setCond)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1609_verifyConsNameIsSetAsExp()
//   Call dpiConn_newDeqOptions(), call dpiDeqOptions_setConsumerName(),
// call dpiDeqOptions_getConsumerName() and verify that the value returned
// matches the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1609_verifyConsNameIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *setConsName = "TEST_CONSUMER!", *getConsName;
    dpiDeqOptions *deqOptions;
    uint32_t getConsNameLen;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getConsumerName(deqOptions, &getConsName,
            &getConsNameLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getConsName, getConsNameLen,
            NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setConsumerName(deqOptions, setConsName,
            strlen(setConsName)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getConsumerName(deqOptions, &getConsName,
            &getConsNameLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getConsName, getConsNameLen,
            setConsName, strlen(setConsName)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1600);
    dpiTestSuite_addCase(dpiTest_1600_releaseDeqOptionsTwice,
            "call dpiDeqOptions_release() twice");
    dpiTestSuite_addCase(dpiTest_1601_verifyPubFuncsOfDeqOptWithNull,
            "call all dpiDeqOptions functions with options param as NULL");
    dpiTestSuite_addCase(dpiTest_1602_verifyNavigationIsSetAsExp,
            "verify Navigation is set as expected");
    dpiTestSuite_addCase(dpiTest_1603_verifyWaitTimeIsSetAsExp,
            "verify wait time is set as expected");
    dpiTestSuite_addCase(dpiTest_1604_verifyModeIsSetAsExp,
            "verify mode is set as expected");
    dpiTestSuite_addCase(dpiTest_1605_verifyCorrelationIsSetAsExp,
            "verify correlation is set as expected");
    dpiTestSuite_addCase(dpiTest_1606_verifyVisIsSetAsExp,
            "verify visibility mode is set as expected");
    dpiTestSuite_addCase(dpiTest_1607_verifyMsgIdIsSetAsExp,
            "verify message id is set as expected");
    dpiTestSuite_addCase(dpiTest_1608_verifyCondIsSetAsExp,
            "verify condition is set as expected");
    dpiTestSuite_addCase(dpiTest_1609_verifyConsNameIsSetAsExp,
            "verify consumer name is set as expected");
    return dpiTestSuite_run();
}
