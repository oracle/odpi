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
        OCIThreadKeyDestroy(env->handle, error->handle, &env->threadKey);
        env->threadKey = NULL;
    }
    if (env->mutex) {
        OCIThreadMutexDestroy(env->handle, error->handle, &env->mutex);
        env->mutex = NULL;
    }
    if (env->handle) {
        OCIHandleFree(env->handle, OCI_HTYPE_ENV);
        env->handle = NULL;
    }
    free(env);
}


//-----------------------------------------------------------------------------
// dpiEnv__freeErrorHandle() [INTERNAL]
//   This routine is called by the thread key destructor and ensures that the
// error handle allocated for thread local storage is cleaned up when the
// thread terminates.
//-----------------------------------------------------------------------------
static void dpiEnv__freeErrorHandle(OCIEnv *handle)
{
    OCIHandleFree(handle, OCI_HTYPE_ERROR);
}


//-----------------------------------------------------------------------------
// dpiEnv__getCharacterSetIdAndName() [INTERNAL]
//   Retrieve and store the IANA character set name for the attribute.
//-----------------------------------------------------------------------------
static int dpiEnv__getCharacterSetIdAndName(dpiEnv *env, uint16_t attribute,
        uint16_t *charsetId, char *encoding, dpiError *error)
{
    *charsetId = 0;
    OCIAttrGet(env->handle, OCI_HTYPE_ENV, charsetId, NULL, attribute,
            error->handle);
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
    sword status;

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

    // create the new environment handle
    env->context = context;
    status = OCIEnvNlsCreate(&env->handle, params->createMode | OCI_OBJECT,
            NULL, NULL, NULL, NULL, 0, NULL, env->charsetId, env->ncharsetId);
    if (!env->handle ||
            (status != OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO))
        return dpiError__set(error, "create environment", DPI_ERR_CREATE_ENV);

    // create first error handle; this is used for all errors if the
    // environment is not threaded and for looking up the thread specific
    // error structure if is threaded
    status = OCIHandleAlloc(env->handle, (dvoid**) &env->errorHandle,
            OCI_HTYPE_ERROR, 0, NULL);
    if (status != OCI_SUCCESS)
        return dpiError__set(error, "allocate OCI error", DPI_ERR_NO_MEMORY);
    error->handle = env->errorHandle;

    // if threaded, create mutex and thread key
    if (params->createMode & OCI_THREADED) {
        status = OCIThreadMutexInit(env->handle, error->handle, &env->mutex);
        if (dpiError__check(error, status, NULL, "initialize mutex") < 0)
            return DPI_FAILURE;
        status = OCIThreadKeyInit(env->handle, error->handle, &env->threadKey,
                (OCIThreadKeyDestFunc) dpiEnv__freeErrorHandle);
        if (dpiError__check(error, status, NULL, "initialize thread key") < 0)
            return DPI_FAILURE;
    }

    // determine encodings in use
    if (dpiEnv__getCharacterSetIdAndName(env, OCI_ATTR_ENV_CHARSET_ID,
            &env->charsetId, env->encoding, error) < 0)
        return DPI_FAILURE;
    error->encoding = env->encoding;
    error->charsetId = env->charsetId;
    if (dpiEnv__getCharacterSetIdAndName(env, OCI_ATTR_ENV_NCHARSET_ID,
            &env->ncharsetId, env->nencoding, error) < 0)
        return DPI_FAILURE;

    // acquire max bytes per character
    status = OCINlsNumericInfoGet(env->handle, error->handle,
            &env->maxBytesPerCharacter, OCI_NLS_CHARSET_MAXBYTESZ);
    if (dpiError__check(error, status, NULL,
            "get max bytes per character") < 0)
        return DPI_FAILURE;

    // for NCHAR we have no idea of how many so we simply take the worst case
    // unless the charsets are identical
    if (env->ncharsetId == env->charsetId)
        env->nmaxBytesPerCharacter = env->maxBytesPerCharacter;
    else env->nmaxBytesPerCharacter = 4;

    // allocate base date descriptor (for converting to/from time_t)
    status = OCIDescriptorAlloc(env->handle, (dvoid**) &env->baseDate,
            OCI_DTYPE_TIMESTAMP_LTZ, 0, 0);
    if (dpiError__check(error, status, NULL,
            "allocate base date descriptor") < 0)
        return DPI_FAILURE;

    // populate base date with January 1, 1970
    status = OCINlsCharSetConvert(env->handle, error->handle, env->charsetId,
            timezoneBuffer, sizeof(timezoneBuffer), DPI_CHARSET_ID_ASCII,
            "+00:00", 6, &timezoneLength);
    if (dpiError__check(error, status,  NULL, "convert timezone text") < 0)
        return DPI_FAILURE;
    status = OCIDateTimeConstruct(env->handle, error->handle, env->baseDate,
            1970, 1, 1, 0, 0, 0, 0, (OraText*) timezoneBuffer, timezoneLength);
    if (dpiError__check(error, status, NULL, "construct base date") < 0)
        return DPI_FAILURE;

    // set whether or not we are threaded
    if (params->createMode & OCI_THREADED)
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
    sword status;

    // the encoding for errors is the CHAR encoding
    error->encoding = env->encoding;
    error->charsetId = env->charsetId;

    // if not threaded, use the error handle stored on the environment itself
    if (!env->threaded)
        error->handle = env->errorHandle;

    // if threaded, however, use thread-specified error handle
    else {

        // get the thread specific error handle
        status = OCIThreadKeyGet(env->handle, env->errorHandle, env->threadKey,
                (void**) &error->handle);
        if (status != OCI_SUCCESS)
            return dpiError__set(error, "get TLS error", DPI_ERR_TLS_ERROR);

        // if NULL, key has never been set before, create new one and set it
        if (!error->handle) {
            status = OCIHandleAlloc(env->handle, (dvoid**) &error->handle,
                    OCI_HTYPE_ERROR, 0, NULL);
            if (status != OCI_SUCCESS)
                return dpiError__set(error, "allocate OCI error",
                        DPI_ERR_NO_MEMORY);
            status = OCIThreadKeySet(env->handle, env->errorHandle,
                    env->threadKey, error->handle);
            if (status != OCI_SUCCESS) {
                OCIHandleFree(error->handle, OCI_HTYPE_ERROR);
                error->handle = NULL;
                return dpiError__set(error, "set TLS error",
                        DPI_ERR_TLS_ERROR);
            }
        }

    }

    return DPI_SUCCESS;
}

