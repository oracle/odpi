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

// forward declarations of internal functions only used in this file
static int dpiEnv__initErrorForThread(dpiEnv *env,
        dpiErrorForThread **errorForThread, dpiError *error);


//-----------------------------------------------------------------------------
// dpiEnv__free() [INTERNAL]
//   Free the memory associated with the environment.
//-----------------------------------------------------------------------------
void dpiEnv__free(dpiEnv *env, dpiError *error)
{
    uint32_t i;

    if (env->threadKey) {

        // acquire mutex and clear environment on all thread error structures
        // this ensures that on Windows, the thread destructor function will
        // not attempt to actually free the error handle twice; on platforms
        // other than Windows, the structure must be freed since the destructor
        // function will no longer be called
        dpiOci__threadMutexAcquire(env, error);
        for (i = 0; i < env->numErrorsForThread; i++) {
            if (env->errorsForThread[i]) {
                env->errorsForThread[i]->env = NULL;
#ifndef _WIN32
                if (env->versionInfo->versionNum >= 12)
                    free(env->errorsForThread[i]);
#endif
                env->errorsForThread[i] = NULL;
            }
        }
        dpiOci__threadMutexRelease(env, error);
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
    if (env->errorsForThread) {
        free(env->errorsForThread);
        env->errorsForThread = NULL;
    }
    free(env);
}


//-----------------------------------------------------------------------------
// dpiEnv__freeErrorForThread() [INTERNAL]
//   Free the error handle associated with a particular thread. If the
// environment is NULL, however, the environment has already been freed and
// therefore so has the error handle, so don't attempt to do so a second time!
//-----------------------------------------------------------------------------
static void dpiEnv__freeErrorForThread(dpiErrorForThread *errorForThread)
{
    dpiError error;

    if (errorForThread->env) {
        dpiGlobal__initError(__func__, &error);
        error.handle = errorForThread->env->errorHandle;
        dpiOci__threadMutexAcquire(errorForThread->env, &error);
        errorForThread->env->errorsForThread[errorForThread->pos] = NULL;
        dpiOci__threadMutexRelease(errorForThread->env, &error);
        dpiOci__handleFree(errorForThread->handle, DPI_OCI_HTYPE_ERROR);
        errorForThread->env = NULL;
        errorForThread->handle = NULL;
        free(errorForThread);
    }
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
// dpiEnv__getErrorForThreadPos() [INTERNAL]
//   Return the position in the error for threads array that should be used for
// this thread.
//-----------------------------------------------------------------------------
static int dpiEnv__getErrorForThreadPos(dpiEnv *env, uint32_t *pos,
        dpiError *error)
{
    dpiErrorForThread **tempArray;
    uint32_t i;
    int found;

    // acquire the mutex to ensure the array is handled properly
    if (dpiOci__threadMutexAcquire(env, error) < 0)
        return DPI_FAILURE;

    // scan the array, looking for an empty entry
    for (i = 0, found = 0; i < env->numErrorsForThread; i++) {
        if (!env->errorsForThread[i]) {
            *pos = i;
            found = 1;
        }
    }

    // if not found, need to allocate more space in array
    if (!found) {
        *pos = env->numErrorsForThread;
        env->numErrorsForThread += 8;
        tempArray = calloc(env->numErrorsForThread,
                sizeof(dpiErrorForThread*));
        if (!tempArray) {
            dpiOci__threadMutexRelease(env, error);
            return dpiError__set(error, "allocate thread errors",
                    DPI_ERR_NO_MEMORY);
        }
        if (env->errorsForThread) {
            for (i = 0; i < *pos; i++)
                tempArray[i] = env->errorsForThread[i];
            free(env->errorsForThread);
        }
        env->errorsForThread = tempArray;
    }

    // release mutex
    if (dpiOci__threadMutexRelease(env, error) < 0)
        return DPI_FAILURE;

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
        if (dpiOci__threadKeyInit(env, &env->threadKey,
#ifdef DPI_DISABLE_THREAD_CLEANUP
                NULL,
#else
                dpiEnv__freeErrorForThread,
#endif
                error) < 0)
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
// between; instead an ODPI-C error is raised. This should be exceedingly rare
// in any case!
//-----------------------------------------------------------------------------
int dpiEnv__initError(dpiEnv *env, dpiError *error)
{
    dpiErrorForThread *errorForThread;

    // the encoding for errors is the CHAR encoding
    // use the error handle stored on the environment itself
    error->encoding = env->encoding;
    error->charsetId = env->charsetId;
    error->handle = env->errorHandle;

    // if threaded, however, use thread specific error handle
    if (env->threaded) {

        // get the thread specific error structure
        if (dpiOci__threadKeyGet(env, (void**) &errorForThread, error) < 0)
            return dpiError__set(error, "get TLS error", DPI_ERR_TLS_ERROR);

        // if NULL, key has never been set before, create new one and set it
        if (!errorForThread) {
            if (dpiEnv__initErrorForThread(env, &errorForThread, error) < 0)
                return DPI_FAILURE;
            if (dpiOci__threadKeySet(env, errorForThread, error) < 0) {
                dpiEnv__freeErrorForThread(errorForThread);
                return dpiError__set(error, "set TLS error",
                        DPI_ERR_TLS_ERROR);
            }
        }

        error->handle = errorForThread->handle;

    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiEnv__initErrorForThread() [INTERNAL]
//   Initialize the error structure for the thread. On platforms other than
// Windows, the thread key destructor function never gets called once the
// thread key has been destroyed, but on Windows it does. This means that
// effort must be taken to ensure that attempting to free the error handle
// twice does not take place on Windows, and that the memory allocated for the
// thread gets cleaned up on platforms other than Windows. Once this anomaly
// has been addressed, this code can be simplified to only store and free the
// error handle.
//-----------------------------------------------------------------------------
static int dpiEnv__initErrorForThread(dpiEnv *env,
        dpiErrorForThread **errorForThread, dpiError *error)
{
    dpiErrorForThread *tempErrorForThread;

    // allocate memory for the structure that is stored
    tempErrorForThread = malloc(sizeof(dpiErrorForThread));
    if (!tempErrorForThread)
        return dpiError__set(error, "init error for thread",
                DPI_ERR_NO_MEMORY);

    // get position in array to store structure
    if (dpiEnv__getErrorForThreadPos(env, &tempErrorForThread->pos,
            error) < 0) {
        free(tempErrorForThread);
        return DPI_FAILURE;
    }
    env->errorsForThread[tempErrorForThread->pos] = tempErrorForThread;
    tempErrorForThread->env = env;

    // get error handle
    if (dpiOci__handleAlloc(env, &tempErrorForThread->handle,
            DPI_OCI_HTYPE_ERROR, "allocate OCI error", error) < 0) {
        env->errorsForThread[tempErrorForThread->pos] = NULL;
        free(tempErrorForThread);
        return DPI_FAILURE;
    }

    *errorForThread = tempErrorForThread;
    return DPI_SUCCESS;
}

