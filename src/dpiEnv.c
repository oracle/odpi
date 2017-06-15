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
// dpiEnv.c
//   Implementation of environment.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

//-----------------------------------------------------------------------------
// dpiEnv__free() [INTERNAL]
//   Free the memory associated with the environment.
//-----------------------------------------------------------------------------
void dpiEnv__free(dpiEnv *env, dpiError *error)
{
    if (env->threadKey) {
        dpiOci__threadKeyDestroy(env, env->threadKey, error);
        env->threadKey = NULL;
    }
    if (env->mutex) {
        dpiOci__threadMutexDestroy(env, env->mutex, error);
        env->mutex = NULL;
    }
    if (env->handle) {
        dpiOci__handleFree(env->handle, DPI_OCI_HTYPE_ENV);
        env->handle = NULL;
    }
    free(env);
}


//-----------------------------------------------------------------------------
// dpiEnv__getCharacterSetIdAndName() [INTERNAL]
//   Retrieve and store the IANA character set name for the attribute.
//-----------------------------------------------------------------------------
static int dpiEnv__getCharacterSetIdAndName(dpiEnv *env, uint16_t attribute,
        uint16_t *charsetId, char *encoding, dpiError *error)
{
    *charsetId = 0;
    dpiOci__attrGet(env->handle, DPI_OCI_HTYPE_ENV, charsetId, NULL, attribute,
            "get environment", error);
    return dpiGlobal__lookupEncoding(*charsetId, encoding, error);
}


//-----------------------------------------------------------------------------
// dpiEnv__getEncodingInfo() [INTERNAL]
//   Populate the structure with the encoding info.
//-----------------------------------------------------------------------------
int dpiEnv__getEncodingInfo(dpiEnv *env, dpiEncodingInfo *info)
{
    info->encoding = env->encoding;
    info->maxBytesPerCharacter = env->maxBytesPerCharacter;
    info->nencoding = env->nencoding;
    info->nmaxBytesPerCharacter = env->nmaxBytesPerCharacter;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiEnv__init() [INTERNAL]
//   Initialize the environment structure by creating the OCI environment and
// populating information about the environment.
//-----------------------------------------------------------------------------
int dpiEnv__init(dpiEnv *env, const dpiContext *context,
        const dpiCommonCreateParams *params, dpiError *error)
{
    char timezoneBuffer[20];
    size_t timezoneLength;

    // lookup encoding
    if (params->encoding && dpiGlobal__lookupCharSet(params->encoding,
            &env->charsetId, error) < 0)
        return DPI_FAILURE;

    // check for identical encoding before performing lookup
    if (params->nencoding && params->encoding &&
            strcmp(params->nencoding, params->encoding) == 0)
        env->ncharsetId = env->charsetId;
    else if (params->nencoding && dpiGlobal__lookupCharSet(params->nencoding,
            &env->ncharsetId, error) < 0)
        return DPI_FAILURE;

    // both charsetId and ncharsetId must be zero or both must be non-zero
    // use NLS routine to look up missing value, if needed
    if (env->charsetId && !env->ncharsetId) {
        if (dpiOci__nlsEnvironmentVariableGet(DPI_OCI_NLS_NCHARSET_ID,
                &env->ncharsetId, error) < 0)
            return DPI_FAILURE;
    } else if (!env->charsetId && env->ncharsetId) {
        if (dpiOci__nlsEnvironmentVariableGet(DPI_OCI_NLS_CHARSET_ID,
                &env->charsetId, error) < 0)
            return DPI_FAILURE;
    }

    // create the new environment handle
    env->context = context;
    env->versionInfo = context->versionInfo;
    if (dpiOci__envNlsCreate(env, params->createMode | DPI_OCI_OBJECT,
            error) < 0)
        return DPI_FAILURE;

    // create first error handle; this is used for all errors if the
    // environment is not threaded and for looking up the thread specific
    // error structure if is threaded
    if (dpiOci__handleAlloc(env, &env->errorHandle, DPI_OCI_HTYPE_ERROR,
            "allocate OCI error", error) < 0)
        return DPI_FAILURE;
    error->handle = env->errorHandle;

    // if threaded, create mutex and thread key
    if (params->createMode & DPI_OCI_THREADED) {
        if (dpiOci__threadMutexInit(env, &env->mutex, error) < 0)
            return DPI_FAILURE;
        if (dpiOci__threadKeyInit(env, &env->threadKey, NULL, error) < 0)
            return DPI_FAILURE;
    }

    // determine encodings in use
    if (dpiEnv__getCharacterSetIdAndName(env, DPI_OCI_ATTR_CHARSET_ID,
            &env->charsetId, env->encoding, error) < 0)
        return DPI_FAILURE;
    error->encoding = env->encoding;
    error->charsetId = env->charsetId;
    if (dpiEnv__getCharacterSetIdAndName(env, DPI_OCI_ATTR_NCHARSET_ID,
            &env->ncharsetId, env->nencoding, error) < 0)
        return DPI_FAILURE;

    // acquire max bytes per character
    if (dpiOci__nlsNumericInfoGet(env, &env->maxBytesPerCharacter,
            DPI_OCI_NLS_CHARSET_MAXBYTESZ, error) < 0)
        return DPI_FAILURE;

    // for NCHAR we have no idea of how many so we simply take the worst case
    // unless the charsets are identical
    if (env->ncharsetId == env->charsetId)
        env->nmaxBytesPerCharacter = env->maxBytesPerCharacter;
    else env->nmaxBytesPerCharacter = 4;

    // allocate base date descriptor (for converting to/from time_t)
    if (dpiOci__descriptorAlloc(env, &env->baseDate,
            DPI_OCI_DTYPE_TIMESTAMP_LTZ, "alloc base date descriptor",
            error) < 0)
        return DPI_FAILURE;

    // populate base date with January 1, 1970
    if (dpiOci__nlsCharSetConvert(env, env->charsetId, timezoneBuffer,
            sizeof(timezoneBuffer), DPI_CHARSET_ID_ASCII, "+00:00", 6,
            &timezoneLength, error) < 0)
        return DPI_FAILURE;
    if (dpiOci__dateTimeConstruct(env, env->baseDate, 1970, 1, 1, 0, 0, 0, 0,
            timezoneBuffer, timezoneLength, error) < 0)
        return DPI_FAILURE;

    // set whether or not we are threaded
    if (params->createMode & DPI_OCI_THREADED)
        env->threaded = 1;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiEnv__initError() [INTERNAL]
//   Retrieve the OCI error handle to use for error handling. This is stored in
// thread local storage if threading is enabled; otherwise the error handle
// that is stored directly on the environment is used. Note that in threaded
// mode the error handle stored directly on the environment is used solely for
// the purpose of getting thread local storage. No attempt is made in that case
// to get the error information since another thread may have used it in
// between; instead a ODPI-C error is raised. This should be exceedingly rare in
// any case!
//-----------------------------------------------------------------------------
int dpiEnv__initError(dpiEnv *env, dpiError *error)
{
    // the encoding for errors is the CHAR encoding
    // use the error handle stored on the environment itself
    error->encoding = env->encoding;
    error->charsetId = env->charsetId;
    error->handle = env->errorHandle;

    // if threaded, however, use thread-specified error handle
    if (env->threaded) {

        // get the thread specific error handle
        if (dpiOci__threadKeyGet(env, &error->handle, error) < 0)
            return dpiError__set(error, "get TLS error", DPI_ERR_TLS_ERROR);

        // if NULL, key has never been set before, create new one and set it
        if (!error->handle) {
            if (dpiOci__handleAlloc(env, &error->handle, DPI_OCI_HTYPE_ERROR,
                    "allocate OCI error", error) < 0)
                return DPI_FAILURE;
            if (dpiOci__threadKeySet(env, error->handle, error) < 0) {
                dpiOci__handleFree(error->handle, DPI_OCI_HTYPE_ERROR);
                error->handle = NULL;
                return dpiError__set(error, "set TLS error",
                        DPI_ERR_TLS_ERROR);
            }
        }

    }

    return DPI_SUCCESS;
}

