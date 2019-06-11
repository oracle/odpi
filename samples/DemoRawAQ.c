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
// DemoRawAQ.c
//   Demos simple enqueuing and dequeuing RAW data using advanced queuing.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define QUEUE_NAME          "DEMO_RAW_QUEUE"
#define NUM_MESSAGES        4

const char *payloads[NUM_MESSAGES] = {
    "The first message",
    "The second message",
    "The third message",
    "The fourth and final message"
};


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiDeqOptions *deqOptions;
    uint32_t payloadLength;
    dpiMsgProps *msgProps;
    const char *payload;
    dpiQueue *queue;
    dpiConn *conn;
    uint32_t i;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // create queue and message properties
    if (dpiConn_newQueue(conn, QUEUE_NAME, strlen(QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiSamples_showError();
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiSamples_showError();
    dpiConn_release(conn);

    // enqueue messages
    printf("Enqueuing messages...\n");
    for (i = 0; i < NUM_MESSAGES; i++) {
        printf("%s\n", payloads[i]);
        if (dpiMsgProps_setPayloadBytes(msgProps, payloads[i],
                strlen(payloads[i])) < 0)
            return dpiSamples_showError();
        if (dpiQueue_enqOne(queue, msgProps) < 0)
            return dpiSamples_showError();
    }
    dpiMsgProps_release(msgProps);

    // get dequeue options from queue and set some options
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiSamples_showError();
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiSamples_showError();
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiSamples_showError();

    // dequeue messages
    printf("\nDequeuing messages...\n");
    while (1) {
        if (dpiQueue_deqOne(queue, &msgProps) < 0)
            return dpiSamples_showError();
        if (!msgProps)
            break;
        if (dpiMsgProps_getPayload(msgProps, NULL, &payload,
                &payloadLength) < 0)
            return dpiSamples_showError();
        printf("%.*s\n", payloadLength, payload);
        dpiMsgProps_release(msgProps);
    }

    printf("\nDone.\n");
    return 0;
}
