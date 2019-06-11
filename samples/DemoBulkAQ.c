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
// DemoBulkAQ.c
//   Demos bulk enqueuing and dequeuing RAW data using advanced queuing.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define QUEUE_NAME          "DEMO_RAW_QUEUE"
#define NUM_MESSAGES        12
#define NUM_BATCH_ENQ       6
#define NUM_BATCH_DEQ       8

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


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiMsgProps *msgPropsEnq[NUM_BATCH_ENQ], *msgPropsDeq[NUM_BATCH_DEQ];
    uint32_t i, pos, numMessages;
    dpiDeqOptions *deqOptions;
    uint32_t payloadLength;
    const char *payload;
    dpiQueue *queue;
    dpiConn *conn;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // create queue and message properties
    if (dpiConn_newQueue(conn, QUEUE_NAME, strlen(QUEUE_NAME), NULL,
            &queue) < 0)
        return dpiSamples_showError();
    for (i = 0; i < NUM_BATCH_ENQ; i++) {
        if (dpiConn_newMsgProps(conn, &msgPropsEnq[i]) < 0)
            return dpiSamples_showError();
    }
    dpiConn_release(conn);

    // enqueue messages
    printf("Enqueuing messages...\n");
    for (i = 0, pos = 0; i < NUM_MESSAGES; i++) {
        printf("%s\n", payloads[i]);
        if (dpiMsgProps_setPayloadBytes(msgPropsEnq[pos], payloads[i],
                strlen(payloads[i])) < 0)
            return dpiSamples_showError();
        if (++pos == NUM_BATCH_ENQ) {
            if (dpiQueue_enqMany(queue, NUM_BATCH_ENQ, msgPropsEnq) < 0)
                return dpiSamples_showError();
            pos = 0;
        }
    }
    for (i = 0; i < NUM_BATCH_ENQ; i++)
        dpiMsgProps_release(msgPropsEnq[i]);

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
        numMessages = NUM_BATCH_DEQ;
        if (dpiQueue_deqMany(queue, &numMessages, msgPropsDeq) < 0)
            return dpiSamples_showError();
        if (numMessages == 0)
            break;
        for (i = 0; i < numMessages; i++) {
            if (dpiMsgProps_getPayload(msgPropsDeq[i], NULL, &payload,
                    &payloadLength) < 0)
                return dpiSamples_showError();
            printf("%.*s\n", payloadLength, payload);
            dpiMsgProps_release(msgPropsDeq[i]);
        }
    }

    printf("\nDone.\n");
    return 0;
}
