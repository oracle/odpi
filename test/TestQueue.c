//-----------------------------------------------------------------------------
// Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestQueue.c
//   Test suite for testing all the features of AQ queues.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define RAW_QUEUE_NAME                  "TESTRAW"
#define NUM_MESSAGES                    12
#define NUM_BATCH_ENQ                   5
#define NUM_BATCH_DEQ                   8


//-----------------------------------------------------------------------------
// dpiTest__clearQueue()
//   Dequeue all messages from the queue to ensure that we are starting with
// an empty queue.
//-----------------------------------------------------------------------------
int dpiTest__clearQueue(dpiTestCase *testCase, dpiConn *conn,
        const char *name)
{
    dpiDeqOptions *deqOptions;
    dpiMsgProps *props;
    dpiQueue *queue;

    // create queue
    if (dpiConn_newQueue(conn, name, strlen(name), NULL, &queue) < 0)
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
// dpiTest_3200_verifyPubFuncsOfQueueWithNull()
//   Call each of the dpiQueue public functions with the queue parameter set to
// NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_3200_verifyPubFuncsOfQueueWithNull(dpiTestCase *testCase,
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
// dpiTest_3201_releaseQueueTwice()
//   Create a queue, then call dpiQueue_release() twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_3201_releaseQueueTwice(dpiTestCase *testCase,
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
// dpiTest_3202_enqManyWithVariousParameters()
//   Create a queue and then call dpiQueue_enqMany() without setting any
// payload (error DPI-1070), with the message properties parameter set to NULL
// (error DPI-1046) and with the number of properties set to zero (no error).
//-----------------------------------------------------------------------------
int dpiTest_3202_enqManyWithVariousParameters(dpiTestCase *testCase,
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
// dpiTest_3203_deqManyWithVariousParameters()
//   Create a queue and then call dpiQueue_deqMany() on an empty queue
// (no error), number of properties set to 0 (error ORA-25327) and with the
// different parameters set to NULL (error DPI-1046).
//-----------------------------------------------------------------------------
int dpiTest_3203_deqManyWithVariousParameters(dpiTestCase *testCase,
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
    if (dpiTest__clearQueue(testCase, conn, RAW_QUEUE_NAME) < 0)
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
// dpiTest_3204_enqOneWithVariousParams()
//   Create a queue and then call dpiQueue_enqOne() without setting a payload
// (error DPI-1070) and with the different parameters set to NULL (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_3204_enqOneWithVariousParams(dpiTestCase *testCase,
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
// dpiTest_3205_deqOneWithVariousParams()
//   Create a queue and then call dpiQueue_deqOne() on an empty queue (no
// error) and with the different parameters set to null (error DPI-1046).
//-----------------------------------------------------------------------------
int dpiTest_3205_deqOneWithVariousParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiDeqOptions *deqOptions;
    dpiMsgProps *props;
    dpiQueue *queue;
    dpiConn *conn;

    // create queue; ensure it is cleared so that errors don't cascade
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__clearQueue(testCase, conn, RAW_QUEUE_NAME) < 0)
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
// dpiTest_3206_bulkEnqDeq()
//   Create a queue and then enqueue some messages in batches. Verify that the
// messages can then be dequeued and match what was enqueued (no error).
//-----------------------------------------------------------------------------
int dpiTest_3206_bulkEnqDeq(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTest__clearQueue(testCase, conn, RAW_QUEUE_NAME) < 0)
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
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3200);
    dpiTestSuite_addCase(dpiTest_3200_verifyPubFuncsOfQueueWithNull,
            "call public functions with queue set to NULL");
    dpiTestSuite_addCase(dpiTest_3201_releaseQueueTwice,
            "call dpiQueue_release() twice");
    dpiTestSuite_addCase(dpiTest_3202_enqManyWithVariousParameters,
            "call dpiQueue_enqMany() with various parameters");
    dpiTestSuite_addCase(dpiTest_3203_deqManyWithVariousParameters,
            "call dpiQueue_deqMany() with various parameters");
    dpiTestSuite_addCase(dpiTest_3204_enqOneWithVariousParams,
            "call dpiQueue_enqOne() with various parameters");
    dpiTestSuite_addCase(dpiTest_3205_deqOneWithVariousParams,
            "call dpiQueue_deqOne() with various parameters");
    dpiTestSuite_addCase(dpiTest_3206_bulkEnqDeq,
            "verify bulk dequeue of messages match what was enqueued");
    return dpiTestSuite_run();
}
