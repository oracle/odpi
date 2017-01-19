//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Test.h
//   Common code used in all tests.
//-----------------------------------------------------------------------------

#include <dpi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#if _MSC_VER < 1900
#define PRId64                  "I64d"
#define PRIu64                  "I64u"
#endif
#endif

#ifndef PRIu64
#include <inttypes.h>
#endif

#define CONN_USERNAME           "dpic"
#define CONN_PASSWORD           "dev"
#define CONN_CONNECT_STRING     ""

static dpiContext *gContext = NULL;

//-----------------------------------------------------------------------------
// ShowError()
//   Display the error to stderr.
//-----------------------------------------------------------------------------
int ShowError(void)
{
    dpiErrorInfo info;

    dpiContext_getError(gContext, &info);
    fprintf(stderr, "ERROR: %.*s (%s: %s)\n", info.messageLength, info.message,
            info.fnName, info.action);
    return -1;
}


//-----------------------------------------------------------------------------
// InitializeDPI()
//   Initialize the ODPI-C library.
//-----------------------------------------------------------------------------
int InitializeDPI(void)
{
    dpiErrorInfo errorInfo;

    if (dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, &gContext,
            &errorInfo) < 0) {
        fprintf(stderr, "ERROR: %.*s (%s : %s)\n", errorInfo.messageLength,
                errorInfo.message, errorInfo.fnName, errorInfo.action);
        return -1;
    }
    return 0;
}


//-----------------------------------------------------------------------------
// GetConnection()
//   Connect to the database using the connection parameters above.
//-----------------------------------------------------------------------------
dpiConn *GetConnection(int withPool, dpiCommonCreateParams *commonParams)
{
    dpiConn *conn;
    dpiPool *pool;

    // perform initialization
    if (!gContext && InitializeDPI() < 0)
        return NULL;

    // create a pool and acquire a connection
    if (withPool) {
        if (dpiPool_create(gContext, CONN_USERNAME, strlen(CONN_USERNAME),
                CONN_PASSWORD, strlen(CONN_PASSWORD), CONN_CONNECT_STRING,
                strlen(CONN_CONNECT_STRING), commonParams, NULL, &pool) < 0) {
            ShowError();
            return NULL;
        }
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                    &conn) < 0) {
            ShowError();
            return NULL;
        }
        dpiPool_release(pool);

    // or create a standalone connection
    } else if (dpiConn_create(gContext, CONN_USERNAME, strlen(CONN_USERNAME),
            CONN_PASSWORD, strlen(CONN_PASSWORD), CONN_CONNECT_STRING,
            strlen(CONN_CONNECT_STRING), commonParams, NULL, &conn) < 0) {
        ShowError();
        return NULL;
    }

    return conn;
}

