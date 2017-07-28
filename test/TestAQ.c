//-----------------------------------------------------------------------------
// Copyright (c) 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestAQ.c
//   Test suite for testing all the Advanced Queuing related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define QUEUE_NAME          "BOOKS"
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
// dpiTest_1800_verifyEnqAndDeqWorksAsExp()
//   Call dpiConn_enqObject(); call dpiConn_deqObject() and verify that the
// object that was enqueued matches the object that was dequeued
// (separate objects should be used for this test) (no error).
//-----------------------------------------------------------------------------
int dpiTest_1800_verifyEnqAndDeqWorksAsExp(dpiTestCase *testCase,
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
// dpiTest_1801_verifyNumAttemptsFunWorksAsExp()
//   Call dpiConn_deqObject() on a queue and verify that a call to
// dpiMsgProps_getNumAttempts() returns a positive value (no error).
//-----------------------------------------------------------------------------
int dpiTest_1801_verifyNumAttemptsFunWorksAsExp(dpiTestCase *testCase,
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
// dpiTest_1802_verifyEnqTimeFunWorksAsExp()
//   Call dpiConn_deqObject() on a queue and verify that a call to
// dpiMsgProps_getEnqTime() returns a consistent value (no error).
//-----------------------------------------------------------------------------
int dpiTest_1802_verifyEnqTimeFunWorksAsExp(dpiTestCase *testCase,
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
// dpiTest_1803_verifyMsgIdIsNullOnEmptyQ()
//   Call dpiConn_deqObject() on an empty queue and verify that the message id
// returned is NULL (no error).
//-----------------------------------------------------------------------------
int dpiTest_1803_verifyMsgIdIsNullOnEmptyQ(dpiTestCase *testCase,
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
// dpiTest_1804_verifyMsgIdsOfEnqAndDeqAreAsExp()
//   Call dpiConn_enqObject() and dpiConn_deqObject() on any queue and verify
// that the message id returned is correct (no error).
//-----------------------------------------------------------------------------
int dpiTest_1804_verifyMsgIdsOfEnqAndDeqAreAsExp(dpiTestCase *testCase,
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
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1800);
    dpiTestSuite_addCase(dpiTest_1800_verifyEnqAndDeqWorksAsExp,
            "dpiConn_enqObject() and dpiConn_deqObject() object attributes");
    dpiTestSuite_addCase(dpiTest_1801_verifyNumAttemptsFunWorksAsExp,
            "dpiMsgProps_getNumAttempts() after successful dequeue");
    dpiTestSuite_addCase(dpiTest_1802_verifyEnqTimeFunWorksAsExp,
            "dpiMsgProps_getEnqTime() after successful dequeue");
    dpiTestSuite_addCase(dpiTest_1803_verifyMsgIdIsNullOnEmptyQ,
            "dpiConn_deqObject() on empty queue");
    dpiTestSuite_addCase(dpiTest_1804_verifyMsgIdsOfEnqAndDeqAreAsExp,
            "dpiConn_enqObject() and dpiConn_deqObject() message ids");
    return dpiTestSuite_run();
}

