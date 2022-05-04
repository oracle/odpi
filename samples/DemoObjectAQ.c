//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2022, Oracle and/or its affiliates.
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
// DemoObjectAQ.c
//   Demos simple enqueuing and dequeuing objects using advanced queuing.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define QUEUE_NAME          "DEMO_BOOK_QUEUE"
#define QUEUE_OBJECT_TYPE   "UDT_BOOK"
#define NUM_BOOKS           2
#define NUM_ATTRS           3

struct bookType {
    char *title;
    char *authors;
    double price;
};

struct bookType books[NUM_BOOKS] = {
    { "Oracle Call Interface Programmers Guide", "Oracle", 0 },
    { "Selecting Employees", "Scott Tiger", 7.99 }
};

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiDeqOptions *deqOptions;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    dpiData attrValue;
    dpiQueue *queue;
    dpiObject *book;
    dpiConn *conn;
    uint32_t i;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // look up object type
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiSamples_showError();
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiSamples_showError();

    // create queue
    if (dpiConn_newQueue(conn, QUEUE_NAME, strlen(QUEUE_NAME),
            objType, &queue) < 0)
        return dpiSamples_showError();
    dpiConn_release(conn);

    // create message properties and set payload
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiSamples_showError();
    if (dpiObjectType_createObject(objType, &book) < 0)
        return dpiSamples_showError();
    if (dpiMsgProps_setPayloadObject(msgProps, book) < 0)
        return dpiSamples_showError();

    // enqueue books
    printf("Enqueuing messages...\n");
    for (i = 0; i < NUM_BOOKS; i++) {
        printf("%s\n", books[i].title);

        // set title
        dpiData_setBytes(&attrValue, books[i].title, strlen(books[i].title));
        if (dpiObject_setAttributeValue(book, attrs[0], DPI_NATIVE_TYPE_BYTES,
                &attrValue) < 0)
            return dpiSamples_showError();

        // set authors
        dpiData_setBytes(&attrValue, books[i].authors,
                strlen(books[i].authors));
        if (dpiObject_setAttributeValue(book, attrs[1], DPI_NATIVE_TYPE_BYTES,
                &attrValue) < 0)
            return dpiSamples_showError();

        // set price
        dpiData_setDouble(&attrValue, books[i].price);
        if (dpiObject_setAttributeValue(book, attrs[2], DPI_NATIVE_TYPE_DOUBLE,
                &attrValue) < 0)
            return dpiSamples_showError();

        // enqueue book
        if (dpiQueue_enqOne(queue, msgProps) < 0)
            return dpiSamples_showError();
    }
    dpiObjectType_release(objType);
    dpiMsgProps_release(msgProps);
    dpiObject_release(book);

    // get dequeue options from queue and set some options
    if (dpiQueue_getDeqOptions(queue, &deqOptions) < 0)
        return dpiSamples_showError();
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiSamples_showError();
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiSamples_showError();

    // dequeue books
    printf("\nDequeuing messages...\n");
    while (1) {
        if (dpiQueue_deqOne(queue, &msgProps) < 0)
            return dpiSamples_showError();
        if (!msgProps)
            break;
        if (dpiMsgProps_getPayload(msgProps, &book, NULL, NULL) < 0)
            return dpiSamples_showError();
        if (dpiObject_getAttributeValue(book, attrs[0], DPI_NATIVE_TYPE_BYTES,
                &attrValue) < 0)
            return dpiSamples_showError();
        printf("%.*s\n", attrValue.value.asBytes.length,
                attrValue.value.asBytes.ptr);
        dpiMsgProps_release(msgProps);
    }
    for (i = 0; i < NUM_ATTRS; i++)
        dpiObjectAttr_release(attrs[i]);

    printf("\nDone.\n");
    return 0;
}
