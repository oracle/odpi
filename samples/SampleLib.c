//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SampleLib.c
//   Common code used in all samples.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

static dpiContext *gContext = NULL;
static dpiSampleParams gParams;

//-----------------------------------------------------------------------------
// dpiSamples__fatalError() [INTERNAL]
//   Called when a fatal error is encountered from which recovery is not
// possible. This simply prints a message to stderr and exits the program with
// a non-zero exit code to indicate an error.
//-----------------------------------------------------------------------------
static void dpiSamples__fatalError(const char *message)
{
    fprintf(stderr, "FATAL: %s\n", message);
    exit(1);
}


//-----------------------------------------------------------------------------
// dpiSamples__finalize() [INTERNAL]
//   Destroy context upon process exit.
//-----------------------------------------------------------------------------
static void dpiSamples__finalize(void)
{
    dpiContext_destroy(gContext);
}


//-----------------------------------------------------------------------------
// dpiSamples__getEnvValue()
//   Get parameter value from the environment or use supplied default value if
// the value is not set in the environment. Memory is allocated to accommodate
// the value.
//-----------------------------------------------------------------------------
static void dpiSamples__getEnvValue(const char *envName,
        const char *defaultValue, const char **value, uint32_t *valueLength,
        int convertToUpper)
{
    const char *source;
    uint32_t i;
    char *ptr;

    source = getenv(envName);
    if (!source)
        source = defaultValue;
    *valueLength = strlen(source);
    *value = malloc(*valueLength);
    if (!*value)
        dpiSamples__fatalError("Out of memory!");
    memcpy((void*) *value, source, *valueLength);
    if (convertToUpper) {
        ptr = (char*) *value;
        for (i = 0; i < *valueLength; i++)
            ptr[i] = toupper(ptr[i]);
    }
}


//-----------------------------------------------------------------------------
// dpiSamples_getConn()
//   Connect to the database using the supplied parameters. The DPI library
// will also be initialized, if needed.
//-----------------------------------------------------------------------------
dpiConn *dpiSamples_getConn(int withPool, dpiCommonCreateParams *commonParams)
{
    dpiConn *conn;
    dpiPool *pool;

    // perform initialization
    dpiSamples_getParams();

    // create a pool and acquire a connection
    if (withPool) {
        if (dpiPool_create(gContext, gParams.mainUserName,
                gParams.mainUserNameLength, gParams.mainPassword,
                gParams.mainPasswordLength, gParams.connectString,
                gParams.connectStringLength, commonParams, NULL, &pool) < 0) {
            dpiSamples_showError();
            dpiSamples__fatalError("Unable to create pool.");
        }
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                    &conn) < 0) {
            dpiSamples_showError();
            dpiSamples__fatalError("Unable to acquire connection from pool.");
        }
        dpiPool_release(pool);

    // or create a standalone connection
    } else if (dpiConn_create(gContext, gParams.mainUserName,
            gParams.mainUserNameLength, gParams.mainPassword,
            gParams.mainPasswordLength, gParams.connectString,
            gParams.connectStringLength, commonParams, NULL, &conn) < 0) {
        dpiSamples_showError();
        dpiSamples__fatalError("Unable to create connection.");
    }

    return conn;
}


//-----------------------------------------------------------------------------
// dpiSamples_getParams()
//   Get parameters set in the environment. The DPI library will also be
// initialized if needed.
//-----------------------------------------------------------------------------
dpiSampleParams *dpiSamples_getParams(void)
{
    dpiErrorInfo errorInfo;

    if (!gContext) {
        if (dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, &gContext,
                &errorInfo) < 0) {
            fprintf(stderr, "ERROR: %.*s (%s : %s)\n", errorInfo.messageLength,
                    errorInfo.message, errorInfo.fnName, errorInfo.action);
            dpiSamples__fatalError("Cannot create DPI context.");
        }
        atexit(dpiSamples__finalize);
    }

    dpiSamples__getEnvValue("ODPIC_SAMPLES_MAIN_USER", "odpicdemo",
            &gParams.mainUserName, &gParams.mainUserNameLength, 1);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_MAIN_PASSWORD", "welcome",
            &gParams.mainPassword, &gParams.mainPasswordLength, 0);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_PROXY_USER", "odpicdemo_proxy",
            &gParams.proxyUserName, &gParams.proxyUserNameLength, 1);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_PROXY_PASSWORD", "welcome",
            &gParams.proxyPassword, &gParams.proxyPasswordLength, 0);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_CONNECT_STRING", "localhost/orclpdb",
            &gParams.connectString, &gParams.connectStringLength, 0);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_DIR_NAME", "odpicdemo_dir",
            &gParams.dirName, &gParams.dirNameLength, 1);
    gParams.context = gContext;

    return &gParams;
}


//-----------------------------------------------------------------------------
// dpiSamples_getSodaDb()
//   Connect to the database and then acquire the SODA database object.
//-----------------------------------------------------------------------------
dpiSodaDb *dpiSamples_getSodaDb(void)
{
    dpiSodaDb *db;
    dpiConn *conn;

    conn = dpiSamples_getConn(0, NULL);
    if (dpiConn_getSodaDb(conn, &db) < 0) {
        dpiSamples_showError();
        dpiSamples__fatalError("Unable to acquire SODA database.");
    }
    dpiConn_release(conn);

    return db;
}


//-----------------------------------------------------------------------------
// dpiSamples_showError()
//   Display the error to stderr.
//-----------------------------------------------------------------------------
int dpiSamples_showError(void)
{
    dpiErrorInfo info;

    dpiContext_getError(gContext, &info);
    fprintf(stderr, "ERROR: %.*s (%s: %s)\n", info.messageLength, info.message,
            info.fnName, info.action);
    return -1;
}

