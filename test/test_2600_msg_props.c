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
// test_2600_msg_props.c
//   Test suite for testing all the Message Properties related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2600_releaseMsgPropsTwice()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2600_releaseMsgPropsTwice(dpiTestCase *testCase,
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
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_2601_verifyPubFuncsOfMsgPropsWithNull()
//   Call each of the dpiMsgProps public functions with the props parameter set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2601_verifyPubFuncsOfMsgPropsWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";
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
// dpiTest_2602_verifyCorrelationIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setCorrelation(); call
// dpiMsgProps_getCorrelation() and verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2602_verifyCorrelationIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_2603_verifyDelayIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setDelay(); call
// dpiMsgProps_getDelay() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2603_verifyDelayIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_2604_verifyExpirationIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setExpiration(); call
// dpiMsgProps_getExpiration() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2604_verifyExpirationIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_2605_verifyPriorityIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setPriority(); call
// dpiMsgProps_getPriority() and verify that the value returned matches the
// value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2605_verifyPriorityIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_2606_verifyMsgIdIsSetAsExp()
//   Call dpiConn_newMsgProps(); call dpiMsgProps_setOriginalMsgId(); call
// dpiMsgProps_getOriginalMsgId() and verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_2606_verifyMsgIdIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_2607_verifyExcQIsSetAsExp()
//   call dpiConn_newMsgProps(); call dpiMsgProps_setExceptionQ(); call
// dpiMsgProps_getExceptionQ() and verify that the value returned matches
// the value that was set (no error)
//-----------------------------------------------------------------------------
int dpiTest_2607_verifyExcQIsSetAsExp(dpiTestCase *testCase,
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
    dpiTestSuite_initialize(2600);
    dpiTestSuite_addCase(dpiTest_2600_releaseMsgPropsTwice,
            "call dpiMsgProps_release() twice");
    dpiTestSuite_addCase(dpiTest_2601_verifyPubFuncsOfMsgPropsWithNull,
            "call all dpiMsgProps public functions with props param as NULL");
    dpiTestSuite_addCase(dpiTest_2602_verifyCorrelationIsSetAsExp,
            "verify correlation is set as expected");
    dpiTestSuite_addCase(dpiTest_2603_verifyDelayIsSetAsExp,
            "verify delay is set as expected");
    dpiTestSuite_addCase(dpiTest_2604_verifyExpirationIsSetAsExp,
            "verify expiration is set as expected");
    dpiTestSuite_addCase(dpiTest_2605_verifyPriorityIsSetAsExp,
            "verify priority is set as expected");
    dpiTestSuite_addCase(dpiTest_2606_verifyMsgIdIsSetAsExp,
            "verify msgId is set as expected");
    dpiTestSuite_addCase(dpiTest_2607_verifyExcQIsSetAsExp,
            "verify exceptionQ is set as expected");
    return dpiTestSuite_run();
}
