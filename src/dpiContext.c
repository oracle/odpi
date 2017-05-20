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
// dpiContext.c
//   Implementation of context. Each context uses a specific version of the
// ODPI-C library, which is checked for compatibility before allowing its use.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

// define check integer for dpiContext structure
#define DPI_CONTEXT_CHECK_INT           0xd81b9181

// maintain major and minor versions compiled into the library
static const unsigned int dpiMajorVersion = DPI_MAJOR_VERSION;
static const unsigned int dpiMinorVersion = DPI_MINOR_VERSION;


//-----------------------------------------------------------------------------
// dpiContext__initCommonCreateParams() [INTERNAL]
//   Initialize the common connection/pool creation parameters to default
// values.
//-----------------------------------------------------------------------------
int dpiContext__initCommonCreateParams(const dpiContext *context,
        dpiCommonCreateParams *params, dpiError *error)
{
    memset(params, 0, sizeof(dpiCommonCreateParams));
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext__initConnCreateParams() [INTERNAL]
//   Initialize the connection creation parameters to default values.
//-----------------------------------------------------------------------------
int dpiContext__initConnCreateParams(const dpiContext *context,
        dpiConnCreateParams *params, dpiError *error)
{
    memset(params, 0, sizeof(dpiConnCreateParams));
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext__initPoolCreateParams() [INTERNAL]
//   Initialize the pool creation parameters to default values.
//-----------------------------------------------------------------------------
int dpiContext__initPoolCreateParams(const dpiContext *context,
        dpiPoolCreateParams *params, dpiError *error)
{
    memset(params, 0, sizeof(dpiPoolCreateParams));
    params->minSessions = 1;
    params->maxSessions = 1;
    params->sessionIncrement = 0;
    params->homogeneous = 1;
    params->getMode = DPI_MODE_POOL_GET_NOWAIT;
    params->pingInterval = DPI_DEFAULT_PING_INTERVAL;
    params->pingTimeout = DPI_DEFAULT_PING_TIMEOUT;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext__initSubscrCreateParams() [INTERNAL]
//   Initialize the subscription creation parameters to default values.
//-----------------------------------------------------------------------------
int dpiContext__initSubscrCreateParams(const dpiContext *context,
        dpiSubscrCreateParams *params, dpiError *error)
{
    memset(params, 0, sizeof(dpiSubscrCreateParams));
    params->subscrNamespace = DPI_SUBSCR_NAMESPACE_DBCHANGE;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext__startPublicFn() [INTERNAL]
//   Create a new context for interaction with the library. The major versions
// must match and the minor version of the caller must be less than or equal to
// the minor version compiled into the library.
//-----------------------------------------------------------------------------
int dpiContext__startPublicFn(const dpiContext *context, const char *fnName,
        dpiError *error)
{
#if DPI_DEBUG_LEVEL & DPI_DEBUG_LEVEL_FNS
    fprintf(stderr, "FN: %s(%p)\n", fnName, context);
#endif
    if (dpiGlobal__initError(fnName, error) < 0)
        return DPI_FAILURE;
    if (!context || context->checkInt != DPI_CONTEXT_CHECK_INT)
        return dpiError__set(error, "check context", DPI_ERR_INVALID_HANDLE,
                "dpiContext");

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext_create() [PUBLIC]
//   Create a new context for interaction with the library. The major versions
// must match and the minor version of the caller must be less than or equal to
// the minor version compiled into the library.
//-----------------------------------------------------------------------------
int dpiContext_create(unsigned int majorVersion, unsigned int minorVersion,
        dpiContext **context, dpiErrorInfo *errorInfo)
{
    dpiContext *tempContext;
    dpiError error;

    // get error structure first (populates global environment if needed)
    if (dpiGlobal__initError(__func__, &error) < 0)
        return dpiError__getInfo(&error, errorInfo);

    // validate context handle
    if (!context) {
        dpiError__set(&error, "check context handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "context");
        return dpiError__getInfo(&error, errorInfo);
    }

    // verify that the supplied version is supported by the library
    if (dpiMajorVersion != majorVersion || minorVersion > dpiMinorVersion) {
        dpiError__set(&error, "check version", DPI_ERR_VERSION_NOT_SUPPORTED,
                majorVersion, minorVersion, dpiMajorVersion, dpiMinorVersion);
        return dpiError__getInfo(&error, errorInfo);
    }

    // allocate memory for the context and initialize it
    tempContext = calloc(1, sizeof(dpiContext));
    if (!tempContext) {
        dpiError__set(&error, "allocate memory", DPI_ERR_NO_MEMORY);
        return dpiError__getInfo(&error, errorInfo);
    }
    tempContext->checkInt = DPI_CONTEXT_CHECK_INT;
    dpiOci__clientVersion(tempContext);

    *context = tempContext;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext_destroy() [PUBLIC]
//   Destroy an existing context. The structure will be checked for validity
// first.
//-----------------------------------------------------------------------------
int dpiContext_destroy(dpiContext *context)
{
    dpiError error;

    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;
    dpiUtils__clearMemory(&context->checkInt, sizeof(context->checkInt));
    free(context);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext_getClientVersion() [PUBLIC]
//   Return the version of the Oracle client that is in use.
//-----------------------------------------------------------------------------
int dpiContext_getClientVersion(const dpiContext *context,
        dpiVersionInfo *versionInfo)
{
    dpiError error;

    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;
    memcpy(versionInfo, context->versionInfo, sizeof(dpiVersionInfo));
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiContext_getError() [PUBLIC]
//   Return information about the error that was last populated.
//-----------------------------------------------------------------------------
void dpiContext_getError(const dpiContext *context, dpiErrorInfo *info)
{
    dpiError error;

    dpiGlobal__initError(NULL, &error);
    if (!context || context->checkInt != DPI_CONTEXT_CHECK_INT)
        dpiError__set(&error, "check check integer", DPI_ERR_INVALID_HANDLE,
                "dpiContext");
    dpiError__getInfo(&error, info);
}


//-----------------------------------------------------------------------------
// dpiContext_initCommonCreateParams() [PUBLIC]
//   Initialize the common connection/pool creation parameters to default
// values.
//-----------------------------------------------------------------------------
int dpiContext_initCommonCreateParams(const dpiContext *context,
        dpiCommonCreateParams *params)
{
    dpiError error;

    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiContext__initCommonCreateParams(context, params, &error);
}


//-----------------------------------------------------------------------------
// dpiContext_initConnCreateParams() [PUBLIC]
//   Initialize the connection creation parameters to default values.
//-----------------------------------------------------------------------------
int dpiContext_initConnCreateParams(const dpiContext *context,
        dpiConnCreateParams *params)
{
    dpiError error;

    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiContext__initConnCreateParams(context, params, &error);
}


//-----------------------------------------------------------------------------
// dpiContext_initPoolCreateParams() [PUBLIC]
//   Initialize the pool creation parameters to default values.
//-----------------------------------------------------------------------------
int dpiContext_initPoolCreateParams(const dpiContext *context,
        dpiPoolCreateParams *params)
{
    dpiError error;

    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiContext__initPoolCreateParams(context, params, &error);
}


//-----------------------------------------------------------------------------
// dpiContext_initSubscrCreateParams() [PUBLIC]
//   Initialize the subscription creation parameters to default values.
//-----------------------------------------------------------------------------
int dpiContext_initSubscrCreateParams(const dpiContext *context,
        dpiSubscrCreateParams *params)
{
    dpiError error;

    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiContext__initSubscrCreateParams(context, params, &error);
}

