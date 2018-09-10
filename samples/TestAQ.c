//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2017, Oracle and/or its affiliates. All rights reserved.
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
//   Tests enqueuing and dequeuing objects using advanced queuing.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define QUEUE_NAME          "BOOKS"
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
    dpiEnqOptions *enqOptions;
    dpiDeqOptions *deqOptions;
    uint32_t i, msgIdLength;
    dpiObjectType *objType;
    dpiMsgProps *msgProps;
    const char *msgId;
    dpiData attrValue;
    dpiObject *book;
    dpiConn *conn;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // look up object type and create object
    if (dpiConn_getObjectType(conn, QUEUE_OBJECT_TYPE,
            strlen(QUEUE_OBJECT_TYPE), &objType) < 0)
        return dpiSamples_showError();
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return dpiSamples_showError();
    if (dpiObjectType_createObject(objType, &book) < 0)
        return dpiSamples_showError();

    // create enqueue options and message properties
    if (dpiConn_newEnqOptions(conn, &enqOptions) < 0)
        return dpiSamples_showError();
    if (dpiConn_newMsgProps(conn, &msgProps) < 0)
        return dpiSamples_showError();

    // enqueue books
    attrValue.isNull = 0;
    for (i = 0; i < NUM_BOOKS; i++) {
        printf("Enqueuing book %s\n", books[i].title);

        // set title
        attrValue.value.asBytes.ptr = books[i].title;
        attrValue.value.asBytes.length = strlen(books[i].title);
        if (dpiObject_setAttributeValue(book, attrs[0], DPI_NATIVE_TYPE_BYTES,
                &attrValue) < 0)
            return dpiSamples_showError();

        // set authors
        attrValue.value.asBytes.ptr = books[i].authors;
        attrValue.value.asBytes.length = strlen(books[i].authors);
        if (dpiObject_setAttributeValue(book, attrs[1], DPI_NATIVE_TYPE_BYTES,
                &attrValue) < 0)
            return dpiSamples_showError();

        // set price
        attrValue.value.asDouble = books[i].price;
        if (dpiObject_setAttributeValue(book, attrs[2], DPI_NATIVE_TYPE_DOUBLE,
                &attrValue) < 0)
            return dpiSamples_showError();

        // enqueue book
        if (dpiConn_enqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME), enqOptions,
                msgProps, book, &msgId, &msgIdLength) < 0)
            return dpiSamples_showError();
    }

    // create dequeue options
    if (dpiConn_newDeqOptions(conn, &deqOptions) < 0)
        return dpiSamples_showError();
    if (dpiDeqOptions_setNavigation(deqOptions, DPI_DEQ_NAV_FIRST_MSG) < 0)
        return dpiSamples_showError();
    if (dpiDeqOptions_setWait(deqOptions, DPI_DEQ_WAIT_NO_WAIT) < 0)
        return dpiSamples_showError();

    // dequeue books
    while (1) {
        if (dpiConn_deqObject(conn, QUEUE_NAME, strlen(QUEUE_NAME), deqOptions,
                msgProps, book, &msgId, &msgIdLength) < 0)
            return dpiSamples_showError();
        if (!msgId)
            break;
        if (dpiObject_getAttributeValue(book, attrs[0], DPI_NATIVE_TYPE_BYTES,
                &attrValue) < 0)
            return dpiSamples_showError();
        printf("Dequeuing book %.*s\n", attrValue.value.asBytes.length,
                attrValue.value.asBytes.ptr);
    }

    // clean up
    dpiObjectType_release(objType);
    for (i = 0; i < NUM_ATTRS; i++)
        dpiObjectAttr_release(attrs[i]);
    dpiObject_release(book);
    dpiEnqOptions_release(enqOptions);
    dpiDeqOptions_release(deqOptions);
    dpiMsgProps_release(msgProps);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

