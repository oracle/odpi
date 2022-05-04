//-----------------------------------------------------------------------------
// Copyright (c) 2019, 2022, Oracle and/or its affiliates.
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
// test_4000_queue.c
//   Test suite for testing all the features of AQ queues.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define RAW_QUEUE_NAME                  "RAW_QUEUE"
#define NUM_MESSAGES                    12
#define NUM_BATCH_ENQ                   5
#define NUM_BATCH_DEQ                   8

#define OBJ_QUEUE_NAME                  "BOOK_QUEUE"
#define QUEUE_OBJECT_TYPE               "UDT_BOOK"
#define NUM_ATTRS                       3

struct bookType {
    char *title;
    char *authors;
    double price;
};


//-----------------------------------------------------------------------------
// dpiTest__clearQueue()
//   Dequeue all messages from the queue to ensure that we are starting with
// an empty queue.
//-----------------------------------------------------------------------------
int dpiTest__clearQueue(dpiTestCase *testCase, dpiConn *conn,
        const char *name, dpiObjectType *objType)
{
    dpiDeqOptions *deqOptions;
    dpiMsgProps *props;
    dpiQueue *queue;

    // create queue
    if (dpiConn_newQueue(conn, name, strlen(name), objType, &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get dequeue options from queue and specify that waiting should not be
    // done for messages
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform dequeue until all messages have been dequeued
    while (1) {
        if (dpiQueue_deqOne(queue, &props) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!props)
            break;
        if (dpiMsgProps_release(props) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4000_verifyPubFuncsOfQueueWithNull()
//   Call each of the dpiQueue public functions with the queue parameter set to
// NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_4000_verifyPubFuncsOfQueueWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    dpiQueue_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiQueue_deqMany(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiQueue_deqOne(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiQueue_enqMany(NULL, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiQueue_enqOne(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiQueue_getDeqOptions(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiQueue_getEnqOptions(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiQueue_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4001_releaseQueueTwice()
//   Create a queue, then call dpiQueue_release() twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_4001_releaseQueueTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiQueue *queue;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, RAW_QUEUE_NAME, strlen(RAW_QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiQueue_release(queue);
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_4002_enqManyWithVariousParameters()
//   Create a queue and then call dpiQueue_enqMany() without setting any
// payload (error DPI-1070), with the message properties parameter set to NULL
// (error DPI-1046) and with the number of properties set to zero (no error).
//-----------------------------------------------------------------------------
int dpiTest_4002_enqManyWithVariousParameters(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiMsgProps *enqProps[NUM_BATCH_ENQ];
    dpiQueue *queue;
    dpiConn *conn;
    uint32_t i;

    // create queue
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, RAW_QUEUE_NAME, strlen(RAW_QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create some message properties to enqueue but don't set any payloads
    for (i = 0; i < NUM_BATCH_ENQ; i++) {
        if (dpiConn_newMsgProps(conn, &enqProps[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // perform various tests
    dpiQueue_enqMany(queue, NUM_BATCH_ENQ, enqProps);
    if (dpiTestCase_expectError(testCase, "DPI-1070:") < 0)
        return DPI_FAILURE;
    dpiQueue_enqMany(queue, NUM_BATCH_ENQ, NULL);
    if (dpiTestCase_expectError(testCase, "DPI-1046:") < 0)
        return DPI_FAILURE;
    if (dpiQueue_enqMany(queue, 0, enqProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    for (i = 0; i < NUM_BATCH_ENQ; i++) {
        if (dpiMsgProps_release(enqProps[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4003_deqManyWithVariousParameters()
//   Create a queue and then call dpiQueue_deqMany() on an empty queue
// (no error), number of properties set to 0 (error ORA-25327) and with the
// different parameters set to NULL (error DPI-1046).
//-----------------------------------------------------------------------------
int dpiTest_4003_deqManyWithVariousParameters(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiMsgProps *deqProps[NUM_BATCH_DEQ];
    dpiDeqOptions *deqOptions;
    uint32_t numMessages;
    dpiQueue *queue;
    dpiConn *conn;

    // create queue; ensure it is cleared so that errors don't cascade
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__clearQueue(testCase, conn, RAW_QUEUE_NAME, NULL) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, RAW_QUEUE_NAME, strlen(RAW_QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get dequeue options from queue and set some options
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform various tests
    numMessages = NUM_BATCH_DEQ;
    if (dpiQueue_deqMany(queue, &numMessages, deqProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numMessages, 0) < 0)
        return DPI_FAILURE;
    dpiQueue_deqMany(queue, &numMessages, deqProps);
    if (dpiTestCase_expectError(testCase, "ORA-25327:") < 0)
        return DPI_FAILURE;
    dpiQueue_deqMany(queue, NULL, deqProps);
    if (dpiTestCase_expectError(testCase, "DPI-1046:") < 0)
        return DPI_FAILURE;
    dpiQueue_deqMany(queue, &numMessages, NULL);
    if (dpiTestCase_expectError(testCase, "DPI-1046:") < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4004_enqOneWithVariousParams()
//   Create a queue and then call dpiQueue_enqOne() without setting a payload
// (error DPI-1070) and with the different parameters set to NULL (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_4004_enqOneWithVariousParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiMsgProps *msgProps;
    dpiQueue *queue;
    dpiConn *conn;

    // create queue
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, RAW_QUEUE_NAME, strlen(RAW_QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform various tests
    dpiQueue_enqOne(queue, msgProps);
    if (dpiTestCase_expectError(testCase, "DPI-1070:") < 0)
        return DPI_FAILURE;
    dpiQueue_enqOne(queue, NULL);
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4005_deqOneWithVariousParams()
//   Create a queue and then call dpiQueue_deqOne() on an empty queue (no
// error) and with the different parameters set to null (error DPI-1046).
//-----------------------------------------------------------------------------
int dpiTest_4005_deqOneWithVariousParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiDeqOptions *deqOptions;
    dpiMsgProps *props;
    dpiQueue *queue;
    dpiConn *conn;

    // create queue; ensure it is cleared so that errors don't cascade
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__clearQueue(testCase, conn, RAW_QUEUE_NAME, NULL) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, RAW_QUEUE_NAME, strlen(RAW_QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get dequeue options from queue and set some options
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform various tests
    if (dpiQueue_deqOne(queue, &props) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (props)
        return dpiTestCase_setFailed(testCase,
                "dequeue on empty queue should have NULL result");
    dpiQueue_deqOne(queue, NULL);
    if (dpiTestCase_expectError(testCase, "DPI-1046:") < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4006_bulkEnqDeq()
//   Create a queue and then enqueue some messages in batches. Verify that the
// messages can then be dequeued and match what was enqueued (no error).
//-----------------------------------------------------------------------------
int dpiTest_4006_bulkEnqDeq(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiMsgProps *enqProps[NUM_BATCH_ENQ], *deqProps[NUM_BATCH_DEQ];
    uint32_t i, pos, numMessages, payloadLength;
    const char *payloads[NUM_MESSAGES] = {
        "The first message",
        "The second message",
        "The third message",
        "The fourth message",
        "The fifth message",
        "The sixth message",
        "The seventh message",
        "The eighth message",
        "The ninth message",
        "The tenth message",
        "The eleventh message",
        "The twelfth and final message"
    };
    dpiDeqOptions *deqOptions;
    const char *payload;
    dpiQueue *queue;
    dpiConn *conn;

    // create queue; ensure it is cleared so that errors don't cascade
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__clearQueue(testCase, conn, RAW_QUEUE_NAME, NULL) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, RAW_QUEUE_NAME, strlen(RAW_QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create some messages to be used for enqueuing
    for (i = 0; i < NUM_BATCH_ENQ; i++) {
        if (dpiConn_newMsgProps(conn, &enqProps[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // enqueue messages in batches
    for (i = 0, pos = 0; i < NUM_MESSAGES; i++) {
        if (dpiMsgProps_setPayloadBytes(enqProps[pos], payloads[i],
                strlen(payloads[i])) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (++pos == NUM_BATCH_ENQ || i == NUM_MESSAGES - 1) {
            if (dpiQueue_enqMany(queue, pos, enqProps) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            pos = 0;
        }
    }
    for (i = 0; i < NUM_BATCH_ENQ; i++) {
        if (dpiMsgProps_release(enqProps[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // get dequeue options from queue and set some options
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // dequeue messages and verify they match what was enqueued
    pos = 0;
    while (1) {
        numMessages = NUM_BATCH_DEQ;
        if (dpiQueue_deqMany(queue, &numMessages, deqProps) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (numMessages == 0)
            break;
        for (i = 0; i < numMessages; i++, pos++) {
            if (dpiMsgProps_getPayload(deqProps[i], NULL, &payload,
                    &payloadLength) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (dpiTestCase_expectStringEqual(testCase, payload, payloadLength,
                    payloads[pos], strlen(payloads[pos])) < 0)
                return DPI_FAILURE;
            if (dpiMsgProps_release(deqProps[i]) < 0)
                return dpiTestCase_setFailedFromError(testCase);
        }
    }
    if (dpiTestCase_expectUintEqual(testCase, pos, NUM_MESSAGES) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4007_bulkEnqDeqObjects()
//   Create a queue and then enqueue some objects in batches. Verify that the
// objects can then be dequeued and match what was enqueued (no error).
//-----------------------------------------------------------------------------
int dpiTest_4007_bulkEnqDeqObjects(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiMsgProps *enqProps[NUM_BATCH_ENQ], *deqProps[NUM_BATCH_DEQ];
    struct bookType books[NUM_MESSAGES] = {
        { "Oracle Call Interface Programmers Guide", "Oracle", 0 },
        { "Oracle Call Interface Programmers Guide 1", "Oracle 1", 1.23 },
        { "Oracle Call Interface Programmers Guide 2", "Oracle 2", 2.34 },
        { "Oracle Call Interface Programmers Guide 3", "Oracle 3", 3.34 },
        { "Oracle Call Interface Programmers Guide 4", "Oracle 4", 4.34 },
        { "Oracle Call Interface Programmers Guide 5", "Oracle 5", 5.34 },
        { "Oracle Call Interface Programmers Guide 6", "Oracle 6", 6.66 },
        { "Oracle Call Interface Programmers Guide 7", "Oracle 7", 7.34 },
        { "Oracle Call Interface Programmers Guide 8", "Oracle 8", 8.99 },
        { "Oracle Call Interface Programmers Guide 9", "Oracle 9", 9.99 },
        { "Oracle Call Interface Programmers Guide 10", "Oracle 10", 10.11 },
        { "Selecting Employees", "Scott Tiger", 7.99 }
    };
    dpiObjectAttr *attrs[NUM_ATTRS];
    uint32_t i, pos, numMessages;
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiObject *bookObj;
    dpiData attrValue;
    dpiQueue *queue;
    dpiConn *conn;

    // create queue; ensure it is cleared so that errors don't cascade
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__clearQueue(testCase, conn, OBJ_QUEUE_NAME, objType) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, OBJ_QUEUE_NAME, strlen(OBJ_QUEUE_NAME), objType,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create some messages to be used for enqueuing
    for (i = 0; i < NUM_BATCH_ENQ; i++) {
        if (dpiConn_newMsgProps(conn, &enqProps[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // enqueue messages in batches
    for (i = 0, pos = 0; i < NUM_MESSAGES; i++) {

        // create new object
        if (dpiObjectType_createObject(objType, &bookObj) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // set title
        dpiData_setBytes(&attrValue, books[i].title, strlen(books[i].title));
        if (dpiObject_setAttributeValue(bookObj, attrs[0],
                DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // set authors
        dpiData_setBytes(&attrValue, books[i].authors,
                strlen(books[i].authors));
        if (dpiObject_setAttributeValue(bookObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // set price
        dpiData_setDouble(&attrValue, books[i].price);
        if (dpiObject_setAttributeValue(bookObj, attrs[2],
                DPI_NATIVE_TYPE_DOUBLE, &attrValue) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // set payload
        if (dpiMsgProps_setPayloadObject(enqProps[pos], bookObj) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiObject_release(bookObj) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // perform enqueue once the batch has been filled or there are no more
        // messages
        if (++pos == NUM_BATCH_ENQ || i == NUM_MESSAGES - 1) {
            if (dpiQueue_enqMany(queue, pos, enqProps) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            pos = 0;
        }

    }

    // cleanup enqueue
    for (i = 0; i < NUM_BATCH_ENQ; i++) {
        if (dpiMsgProps_release(enqProps[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get dequeue options from queue and set some options
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // dequeue messages and verify they match what was enqueued
    pos = 0;
    while (1) {
        numMessages = NUM_BATCH_DEQ;
        if (dpiQueue_deqMany(queue, &numMessages, deqProps) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (numMessages == 0)
            break;
        for (i = 0; i < numMessages; i++, pos++) {

            // get payload
            if (dpiMsgProps_getPayload(deqProps[i], &bookObj, NULL, NULL) < 0)
                return dpiTestCase_setFailedFromError(testCase);

            // verify title matches
            if (dpiObject_getAttributeValue(bookObj, attrs[0],
                    DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (dpiTestCase_expectStringEqual(testCase,
                    attrValue.value.asBytes.ptr,
                    attrValue.value.asBytes.length, books[pos].title,
                    strlen(books[pos].title)) < 0)
                return DPI_FAILURE;

            // verify authors match
            if (dpiObject_getAttributeValue(bookObj, attrs[1],
                    DPI_NATIVE_TYPE_BYTES, &attrValue) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (dpiTestCase_expectStringEqual(testCase,
                attrValue.value.asBytes.ptr, attrValue.value.asBytes.length,
                books[pos].authors, strlen(books[pos].authors)) < 0)
                return DPI_FAILURE;

            // verify price matches
            if (dpiObject_getAttributeValue(bookObj, attrs[2],
                    DPI_NATIVE_TYPE_DOUBLE, &attrValue) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (dpiTestCase_expectDoubleEqual(testCase,
                    attrValue.value.asDouble, books[pos].price) < 0)
                return DPI_FAILURE;

            // cleanup
            if (dpiMsgProps_release(deqProps[i]) < 0)
                return dpiTestCase_setFailedFromError(testCase);
        }
    }
    if (dpiTestCase_expectUintEqual(testCase, pos, NUM_MESSAGES) < 0)
        return DPI_FAILURE;

    // cleanup
    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiQueue_release(queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4008_verifyDeqUsingMsgId()
//   Create a queue and then enqueue some messages. Verify that the
// message can then be dequeued using msgId(no error).
//-----------------------------------------------------------------------------
int dpiTest_4008_verifyDeqUsingMsgId(dpiTestCase *testCase,
    dpiTestParams *params)
{
    const char *payloads[NUM_MESSAGES] = {
        "The first message",
        "The second message",
        "The third message",
        "The fourth message",
        "The fifth message",
        "The sixth message",
        "The seventh message",
        "The eighth message",
        "The ninth message",
        "The tenth message",
        "The eleventh message",
        "The twelfth and final message"
    };
    dpiDeqOptions *deqOptions;
    uint32_t payloadLength;
    dpiMsgProps *msgProps;
    const char *payload;
    const char *msg;
    uint32_t msgLen;
    dpiQueue *queue;
    dpiConn *conn;
    uint32_t i;

    // connect to database
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__clearQueue(testCase, conn, RAW_QUEUE_NAME, NULL) < 0)
        return DPI_FAILURE;
    if (dpiConn_newQueue(conn, RAW_QUEUE_NAME, strlen(RAW_QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // enqueue messages
    for (i = 0; i < NUM_MESSAGES; i++) {
        if (dpiMsgProps_setPayloadBytes(msgProps, payloads[i],
                strlen(payloads[i])) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiQueue_enqOne(queue, msgProps) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    if (dpiMsgProps_getMsgId(msgProps, &msg, &msgLen) < 0)
            return dpiTestCase_setFailedFromError(testCase);

    // get dequeue options from queue and set some options
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiDeqOptions_setMsgId(deqOptions, msg, msgLen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_release(msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiQueue_deqOne(queue, &msgProps) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiMsgProps_getPayload(msgProps, NULL, &payload,
            &payloadLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, payload, payloadLength,
            payloads[NUM_MESSAGES - 1],
            strlen(payloads[NUM_MESSAGES - 1])) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiQueue_release(queue) < 0)
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
    dpiTestSuite_initialize(4000);
    dpiTestSuite_addCase(dpiTest_4000_verifyPubFuncsOfQueueWithNull,
            "call public functions with queue set to NULL");
    dpiTestSuite_addCase(dpiTest_4001_releaseQueueTwice,
            "call dpiQueue_release() twice");
    dpiTestSuite_addCase(dpiTest_4002_enqManyWithVariousParameters,
            "call dpiQueue_enqMany() with various parameters");
    dpiTestSuite_addCase(dpiTest_4003_deqManyWithVariousParameters,
            "call dpiQueue_deqMany() with various parameters");
    dpiTestSuite_addCase(dpiTest_4004_enqOneWithVariousParams,
            "call dpiQueue_enqOne() with various parameters");
    dpiTestSuite_addCase(dpiTest_4005_deqOneWithVariousParams,
            "call dpiQueue_deqOne() with various parameters");
    dpiTestSuite_addCase(dpiTest_4006_bulkEnqDeq,
            "bulk dequeue of raw data matches what was enqueued");
    dpiTestSuite_addCase(dpiTest_4007_bulkEnqDeqObjects,
            "bulk dequeue of objects matches what was enqueued");
    dpiTestSuite_addCase(dpiTest_4008_verifyDeqUsingMsgId,
            "verify dequeue by message id");

    return dpiTestSuite_run();
}
