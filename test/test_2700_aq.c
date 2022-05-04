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
// test_2700_aq.c
//   Test suite for testing all the Advanced Queuing related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define QUEUE_NAME          "BOOK_QUEUE"
#define QUEUE_OBJECT_TYPE   "UDT_BOOK"
#define MAX_ATTEMPTS        10
#define NUM_BOOKS           1
#define NUM_ATTRS           3

struct bookType {
    char *title;
    char *authors;
    double price;
};

struct bookType books[NUM_BOOKS] = {
    { "Oracle Call Interface Programmers Guide", "Oracle", 10.34 }
};

//-----------------------------------------------------------------------------
// dpiTest__enqObj() [INTERNAL]
//   Sets attribute values and enqueues the object.
//-----------------------------------------------------------------------------
int dpiTest__enqObj(dpiTestCase *testCase, dpiObject *bookObj,
        dpiObjectAttr **attrs, dpiEnqOptions *enqOptions,
        dpiMsgProps *msgProps, const char **msgId, uint32_t *msgIdLength,
        dpiConn *conn)
{
    dpiData attrValue;
    int i;

    for (i = 0; i < NUM_BOOKS; i++) {
        dpiData_setBytes(&attrValue, books[i].title, strlen(books[i].title));
        if (dpiObject_setAttributeValue(bookObj, attrs[0],
                DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setBytes(&attrValue, books[i].authors,
                strlen(books[i].authors));
        if (dpiObject_setAttributeValue(bookObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setDouble(&attrValue, books[i].price);
        if (dpiObject_setAttributeValue(bookObj, attrs[2],
                DPI_NATIVE_TYPE_DOUBLE, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiConn_enqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME),
                enqOptions, msgProps, bookObj, &msgId[i], &msgIdLength[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2700_verifyEnqAndDeqWorksAsExp()
//   Call dpiConn_enqObject(); call dpiConn_deqObject() and verify that the
// object that was enqueued matches the object that was dequeued
// (separate objects should be used for this test) (no error).
//-----------------------------------------------------------------------------
int dpiTest_2700_verifyEnqAndDeqWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t i, enqMsgIdLength[NUM_BOOKS], deqMsgIdLen;
    const char *enqMsgId[NUM_BOOKS], *deqMsgId;
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiObject *enqBook, *deqBook;
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    dpiData attrValue;
    dpiConn *conn;

    // initialization
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // enqueue objects
    if (dpiTest__enqObj(testCase, enqBook, attrs, enqOptions, msgProps,
            enqMsgId, enqMsgIdLength, conn) < 0)
        return DPI_FAILURE;

    // dequeue objects
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_BOOKS; i++) {
        if (dpiConn_deqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME),
                deqOptions, msgProps, deqBook, &deqMsgId, &deqMsgIdLen) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!deqMsgId) {
            return dpiTestCase_setFailedFromError(testCase);
        }
        if (dpiObject_getAttributeValue(deqBook, attrs[0],
                DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectStringEqual(testCase,
                attrValue.value.asBytes.ptr,
                attrValue.value.asBytes.length, books[i].title,
                strlen(books[i].title)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiObject_getAttributeValue(deqBook, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectStringEqual(testCase,
                attrValue.value.asBytes.ptr,
                attrValue.value.asBytes.length, books[i].authors,
                strlen(books[i].authors)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiObject_getAttributeValue(deqBook, attrs[2],
                DPI_NATIVE_TYPE_DOUBLE, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectDoubleEqual(testCase, attrValue.value.asDouble,
                books[i].price) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    }

    // cleanup
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiObject_release(enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2701_verifyNumAttemptsFunWorksAsExp()
//   Call dpiConn_deqObject() on a queue and verify that a call to
// dpiMsgProps_getNumAttempts() returns a positive value (no error).
//-----------------------------------------------------------------------------
int dpiTest_2701_verifyNumAttemptsFunWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t i, enqMsgIdLength[NUM_BOOKS], deqMsgIdLen;
    const char *enqMsgId[NUM_BOOKS], *deqMsgId;
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiObject *enqBook, *deqBook;
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    int32_t numAttempts;
    dpiConn *conn;

    // initialization
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // enqueue objects
    if (dpiTest__enqObj(testCase, enqBook, attrs, enqOptions, msgProps,
            enqMsgId, enqMsgIdLength, conn) < 0)
        return DPI_FAILURE;

    // dequeue objects
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_BOOKS; i++) {
        if (dpiConn_deqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME),
                deqOptions, msgProps, deqBook, &deqMsgId, &deqMsgIdLen) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiMsgProps_getNumAttempts(msgProps, &numAttempts) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase, numAttempts, 0) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiObject_release(enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2702_verifyEnqTimeFunWorksAsExp()
//   Call dpiConn_deqObject() on a queue and verify that a call to
// dpiMsgProps_getEnqTime() returns a consistent value (no error).
//-----------------------------------------------------------------------------
int dpiTest_2702_verifyEnqTimeFunWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t i, enqMsgIdLength[NUM_BOOKS], deqMsgIdLen;
    const char *enqMsgId[NUM_BOOKS], *deqMsgId;
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiObject *enqBook, *deqBook;
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    dpiTimestamp timestamp;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    // initialization
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // enqueue objects
    if (dpiTest__enqObj(testCase, enqBook, attrs, enqOptions, msgProps,
            enqMsgId, enqMsgIdLength, conn) < 0)
        return DPI_FAILURE;

    // dequeue objects
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_BOOKS; i++) {
        if (dpiConn_deqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME), deqOptions,
                msgProps, deqBook, &deqMsgId, &deqMsgIdLen) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiMsgProps_getEnqTime(msgProps, &timestamp) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // cleanup
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiObject_release(enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2703_verifyMsgIdIsNullOnEmptyQ()
//   Call dpiConn_deqObject() on an empty queue and verify that the message id
// returned is NULL (no error).
//-----------------------------------------------------------------------------
int dpiTest_2703_verifyMsgIdIsNullOnEmptyQ(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    uint32_t msgIdLength;
    dpiObject *enqBook;
    const char *msgId;
    dpiConn *conn;
    int i;

    // initialization
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // dequeue until queue is empty or max attempts has been reached
    for (i = 0; i < MAX_ATTEMPTS; i++) {
        if (dpiConn_deqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME),
                deqOptions, msgProps, enqBook, &msgId, &msgIdLength) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!msgId)
            break;
    }

    // at this point we should have a NULL message id
    if (msgId)
        return dpiTestCase_setFailed(testCase,
                "expecting NULL message for empty queue");

    // cleanup
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2704_verifyMsgIdsOfEnqAndDeqAreAsExp()
//   Call dpiConn_enqObject() and dpiConn_deqObject() on any queue and verify
// that the message id returned is correct (no error).
//-----------------------------------------------------------------------------
int dpiTest_2704_verifyMsgIdsOfEnqAndDeqAreAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t i, enqMsgIdLen[NUM_BOOKS], deqMsgIdLen;
    const char *engMsgId[NUM_BOOKS], *deqMsgId;
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiObject *enqBook, *deqBook;
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    // initialization
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // enqueue objects
    if (dpiTest__enqObj(testCase, enqBook, attrs, enqOptions, msgProps,
            engMsgId, enqMsgIdLen, conn) < 0)
        return DPI_FAILURE;

    // dequeue objects
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_BOOKS; i++) {
        if (dpiConn_deqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME),
                deqOptions, msgProps, deqBook, &deqMsgId, &deqMsgIdLen) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectStringEqual(testCase, deqMsgId,
                deqMsgIdLen, engMsgId[i], enqMsgIdLen[i]) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiObject_release(enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2705_verifyDeqProperties()
//   Set each of the properties that can be set on dequeue options and verify
// that they are set correctly (no error).
//-----------------------------------------------------------------------------
int dpiTest_2705_verifyDeqProperties(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *condition = "TEST_CONDITION", *consumerName = "TEST_CONSUMER",
               *correlation = "TEST_CORRELATION", *msgId = "TEST_MSGID";
    uint32_t waitModes[] = {DPI_DEQ_WAIT_NO_WAIT, DPI_DEQ_WAIT_FOREVER, 9, -1};
    int deqModes[] = {DPI_MODE_DEQ_BROWSE, DPI_MODE_DEQ_LOCKED,
        DPI_MODE_DEQ_REMOVE, DPI_MODE_DEQ_REMOVE_NO_DATA, 55, -1};
    int navModes[] = {DPI_DEQ_NAV_NEXT_TRANSACTION,
        DPI_DEQ_NAV_FIRST_MSG, DPI_DEQ_NAV_NEXT_MSG, 20, -1};
    int visModes[] = {DPI_VISIBILITY_IMMEDIATE,
        DPI_VISIBILITY_ON_COMMIT, 99, -1};
    uint32_t getValueInt, getValueLength;
    dpiDeqNavigation expNavMode;
    dpiDeqOptions *deqOptions;
    dpiVisibility expVisMode;
    dpiDeqMode expDeqMode;
    const char *getValue;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; waitModes[i] != -1; i++) {
        if (dpiDeqOptions_setWait(deqOptions, waitModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiDeqOptions_getWait(deqOptions, &getValueInt) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, getValueInt,
                waitModes[i]) < 0)
            return DPI_FAILURE;
    }

    if (dpiDeqOptions_setCondition(deqOptions, condition,
            strlen(condition)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCondition(deqOptions, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue,
            getValueLength, condition, strlen(condition)) < 0)
        return DPI_FAILURE;

    if (dpiDeqOptions_setConsumerName(deqOptions, consumerName,
            strlen(consumerName)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getConsumerName(deqOptions, &getValue,
            &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue,
            getValueLength, consumerName, strlen(consumerName)) < 0)
        return DPI_FAILURE;

    if (dpiDeqOptions_setCorrelation(deqOptions, correlation,
            strlen(correlation)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getCorrelation(deqOptions, &getValue,
            &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue,
            getValueLength, correlation, strlen(correlation)) < 0)
        return DPI_FAILURE;

    for (i = 0; deqModes[i] != -1; i++) {
        if (dpiDeqOptions_setMode(deqOptions, deqModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiDeqOptions_getMode(deqOptions, &expDeqMode) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, expDeqMode, deqModes[i]) < 0)
            return DPI_FAILURE;
    }

    if (dpiDeqOptions_setMsgId(deqOptions, msgId, strlen(msgId)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_getMsgId(deqOptions, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue,
            getValueLength, msgId, strlen(msgId)) < 0)
        return DPI_FAILURE;

    for (i = 0; navModes[i] != -1; i++) {
        if (dpiDeqOptions_setNavigation(deqOptions, navModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiDeqOptions_getNavigation(deqOptions, &expNavMode) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, expNavMode, navModes[i]) < 0)
            return DPI_FAILURE;
    }

    for (i = 0; visModes[i] != -1; i++) {
        if (dpiDeqOptions_setVisibility(deqOptions, visModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiDeqOptions_getVisibility(deqOptions, &expVisMode) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, expVisMode, visModes[i]) < 0)
            return DPI_FAILURE;
    }

    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2706_verifyEnqProperties()
//   Set each of the properties that can be set on enqueue options and verify
// that they are set correctly (no error).
//-----------------------------------------------------------------------------
int dpiTest_2706_verifyEnqProperties(dpiTestCase *testCase,
        dpiTestParams *params)
{
    int visModes[] = {DPI_VISIBILITY_IMMEDIATE, DPI_VISIBILITY_ON_COMMIT, 99,
            -1};
    dpiEnqOptions *enqOptions;
    dpiVisibility expVisMode;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    for (i = 0; visModes[i] != -1; i++) {
        if (dpiEnqOptions_setVisibility(enqOptions, visModes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiEnqOptions_getVisibility(enqOptions, &expVisMode) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, expVisMode, visModes[i]) < 0)
            return DPI_FAILURE;
    }

    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2707_verifyMsgProperties()
//   Set each of the properties that can be set on message properties and
// verify that they are set correctly (no error).
//-----------------------------------------------------------------------------
int dpiTest_2707_verifyMsgProperties(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *correlation = "TEST_CORRELATION";
    const char *exceptionq = "TEST_EXCEPTION";
    const char *origMsgId = "TEST_ORGMSGID";
    uint32_t getValueLength;
    dpiMsgProps *msgProps;
    const char *getValue;
    int32_t getValueInt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiMsgProps_setCorrelation(msgProps, correlation,
            strlen(correlation)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getCorrelation(msgProps, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue,
            getValueLength, correlation, strlen(correlation)) < 0)
        return DPI_FAILURE;

    if (dpiMsgProps_setDelay(msgProps, 10) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getDelay(msgProps, &getValueInt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getValueInt, 10) < 0)
        return DPI_FAILURE;

    if (dpiMsgProps_setExceptionQ(msgProps, exceptionq,
            strlen(exceptionq)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getExceptionQ(msgProps, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue,
            getValueLength, exceptionq, strlen(exceptionq)) < 0)
        return DPI_FAILURE;

    if (dpiMsgProps_setExpiration(msgProps, 30) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getExpiration(msgProps, &getValueInt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getValueInt, 30) < 0)
        return DPI_FAILURE;

    if (dpiMsgProps_setOriginalMsgId(msgProps, origMsgId,
            strlen(origMsgId)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getOriginalMsgId(msgProps, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue,
            getValueLength, origMsgId, strlen(origMsgId)) < 0)
        return DPI_FAILURE;

    if (dpiMsgProps_setPriority(msgProps, -4) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getPriority(msgProps, &getValueInt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getValueInt, -4) < 0)
        return DPI_FAILURE;

    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2708_verifyDpiMsgPersistentMode()
//   Set delivery mode to DPI_MODE_MSG_PERSISTENT and verify
// dpiMsgProps_getDeliveryMode() returns the expected value (no error).
//-----------------------------------------------------------------------------
int dpiTest_2708_verifyDpiMsgPersistentMode(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t enqMsgIdLength[NUM_BOOKS], deqMsgIdLen, i;
    const char *enqMsgId[NUM_BOOKS], *deqMsgId;
    dpiMessageDeliveryMode getMsgDelMode;
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiObject *enqBook, *deqBook;
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiEnqOptions_setDeliveryMode(enqOptions, DPI_MODE_MSG_PERSISTENT) < 0)
         return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__enqObj(testCase, enqBook, attrs, enqOptions, msgProps,
            enqMsgId, enqMsgIdLength, conn) < 0)
        return DPI_FAILURE;
    if (dpiDeqOptions_setDeliveryMode(deqOptions, DPI_MODE_MSG_PERSISTENT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_deqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME),
            deqOptions, msgProps, deqBook, &deqMsgId, &deqMsgIdLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!deqMsgId)
        return dpiTestCase_setFailed(testCase, "no message dequeued!");
    if (dpiMsgProps_getDeliveryMode(msgProps, &getMsgDelMode) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getMsgDelMode,
            DPI_MODE_MSG_PERSISTENT) < 0)
        return DPI_FAILURE;

    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2709_verifyDpiMsgBufferedMode()
//   Set delivery mode to DPI_MODE_MSG_BUFFERED and verify
// dpiMsgProps_getDeliveryMode() returns the expected value (no error).
//-----------------------------------------------------------------------------
int dpiTest_2709_verifyDpiMsgBufferedMode(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t enqMsgIdLength[NUM_BOOKS], deqMsgIdLen, i;
    const char *enqMsgId[NUM_BOOKS], *deqMsgId;
    dpiMessageDeliveryMode getMsgDelMode;
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiObject *enqBook, *deqBook;
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    dpiConn *conn1, *conn2;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;

    if (dpiTestCase_getConnection(testCase, &conn1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn2) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn1, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiConn_newEnqOptions(conn1, &enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn1, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_setVisibility(enqOptions, DPI_VISIBILITY_IMMEDIATE) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_setDeliveryMode(enqOptions, DPI_MODE_MSG_BUFFERED) < 0)
         return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__enqObj(testCase, enqBook, attrs, enqOptions, msgProps,
            enqMsgId, enqMsgIdLength, conn1) < 0)
        return DPI_FAILURE;
    if (dpiMsgProps_release(msgProps) < 0)
        return DPI_FAILURE;

    if (dpiObjectType_createObject(objType, &deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn2, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newDeqOptions(conn2, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setVisibility(deqOptions, DPI_VISIBILITY_IMMEDIATE) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setDeliveryMode(deqOptions, DPI_MODE_MSG_BUFFERED) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_deqObject(conn2, QUEUE_NAME, strlen(QUEUE_NAME),
            deqOptions, msgProps, deqBook, &deqMsgId, &deqMsgIdLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!deqMsgId)
        return dpiTestCase_setFailed(testCase, "no message dequeued!");
    if (dpiMsgProps_getDeliveryMode(msgProps, &getMsgDelMode) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, getMsgDelMode,
            DPI_MODE_MSG_BUFFERED) < 0)
        return DPI_FAILURE;

    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(enqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(deqBook) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiEnqOptions_release(enqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_release(deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn1) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2700);
    dpiTestSuite_addCase(dpiTest_2700_verifyEnqAndDeqWorksAsExp,
            "dpiConn_enqObject() and dpiConn_deqObject() object attributes");
    dpiTestSuite_addCase(dpiTest_2701_verifyNumAttemptsFunWorksAsExp,
            "dpiMsgProps_getNumAttempts() after successful dequeue");
    dpiTestSuite_addCase(dpiTest_2702_verifyEnqTimeFunWorksAsExp,
            "dpiMsgProps_getEnqTime() after successful dequeue");
    dpiTestSuite_addCase(dpiTest_2703_verifyMsgIdIsNullOnEmptyQ,
            "dpiConn_deqObject() on empty queue");
    dpiTestSuite_addCase(dpiTest_2704_verifyMsgIdsOfEnqAndDeqAreAsExp,
            "dpiConn_enqObject() and dpiConn_deqObject() message ids");
    dpiTestSuite_addCase(dpiTest_2705_verifyDeqProperties,
            "verify properties of dequeue options");
    dpiTestSuite_addCase(dpiTest_2706_verifyEnqProperties,
            "verify properties of enque options");
    dpiTestSuite_addCase(dpiTest_2707_verifyMsgProperties,
            "verify properties of message options");
    dpiTestSuite_addCase(dpiTest_2708_verifyDpiMsgPersistentMode,
            "set delivery mode to DPI_MODE_MSG_PERSISTENT and verify");
    dpiTestSuite_addCase(dpiTest_2709_verifyDpiMsgBufferedMode,
            "set delivery mode to DPI_MODE_MSG_BUFFERED and verify");
    return dpiTestSuite_run();
}
