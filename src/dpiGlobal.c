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
// dpiGlobal.c
//   Global environment used for managing errors in a thread safe manner as
// well as for looking up encodings.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

// a global OCI environment is used for managing errors in a thread-safe
// manner; each thread is given its own error state; OCI error handles, though,
// are created within the OCI environment created for use by standalone
// connections and session pools
static dpiEnv *dpiGlobalEnv;
static dpiErrorBuffer dpiGlobalErrorBuffer;


//-----------------------------------------------------------------------------
// dpiGlobal__createEnv() [INTERNAL]
//   Create the global environment used for managing error buffers in a
// thread-safe manner. This environment is solely used for implementing thread
// local storage for the error buffers and for looking up encodings given an
// IANA or Oracle character set name. This routine is not thread safe and it is
// assumed that it will be called before any other routine in ODPI-C is called.
//-----------------------------------------------------------------------------
static int dpiGlobal__createEnv(const char *fnName, dpiError *error)
{
    dpiEnv *tempEnv;

    // initialize error
    error->handle = NULL;
    error->buffer->fnName = fnName;

    // allocate memory for global environment
    tempEnv = calloc(1, sizeof(dpiEnv));
    if (!tempEnv)
        return dpiError__set(error, "allocate global env", DPI_ERR_NO_MEMORY);

    // create threaded OCI environment for storing
    // use character set AL32UTF8 solely to avoid the overhead of processing
    // the environment variables; no error messages from this environment are
    // ever used (ODPI-C specific error messages are used)
    tempEnv->charsetId = DPI_CHARSET_ID_UTF8;
    tempEnv->ncharsetId = DPI_CHARSET_ID_UTF8;
    if (dpiOci__envNlsCreate(tempEnv, DPI_OCI_THREADED, error) < 0)
        return DPI_FAILURE;

    // create global error handle used for managing errors for each thread
    if (dpiOci__handleAlloc(tempEnv, &tempEnv->errorHandle,
            DPI_OCI_HTYPE_ERROR, "create global error", error) < 0) {
        dpiEnv__free(tempEnv, error);
        return DPI_FAILURE;
    }

    // create thread key
    error->handle = tempEnv->errorHandle;
    if (dpiOci__threadKeyInit(tempEnv, &tempEnv->threadKey, free, error) < 0) {
        dpiEnv__free(tempEnv, error);
        return DPI_FAILURE;
    }

    // store these in global state
    // NOTE: this is not thread safe; two threads could attempt to call this
    // function at the same time even though it is documented that they should
    // not do so; this check minimizes but does not eliminate the risk
    if (dpiGlobalEnv)
        dpiEnv__free(tempEnv, error);
    else dpiGlobalEnv = tempEnv;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiGlobal__initError() [INTERNAL]
//   Get the thread local error structure for use in all other functions. If
// an error structure cannot be determined for some reason, the global error
// buffer structure is returned instead.
//-----------------------------------------------------------------------------
int dpiGlobal__initError(const char *fnName, dpiError *error)
{
    dpiErrorBuffer *tempErrorBuffer;

    // initialize error buffer output to global error buffer structure; this is
    // the value that is used if an error takes place before the thread local
    // error structure can be returned
    error->buffer = &dpiGlobalErrorBuffer;

    // initialize global environment, if necessary
    // this should only ever be done once by the first thread to execute this
    if (!dpiGlobalEnv && dpiGlobal__createEnv(fnName, error) < 0)
        return DPI_FAILURE;

    // look up the error buffer specific to this thread
    error->handle = dpiGlobalEnv->errorHandle;
    if (dpiOci__threadKeyGet(dpiGlobalEnv, (void**) &tempErrorBuffer,
            error) < 0)
        return DPI_FAILURE;

    // if NULL, key has never been set for this thread, allocate new error
    // and set it
    if (!tempErrorBuffer) {
        tempErrorBuffer = calloc(1, sizeof(dpiErrorBuffer));
        if (!tempErrorBuffer)
            return dpiError__set(error, "allocate error buffer",
                    DPI_ERR_NO_MEMORY);
        if (dpiOci__threadKeySet(dpiGlobalEnv, tempErrorBuffer, error) < 0) {
            free(tempErrorBuffer);
            return DPI_FAILURE;
        }
    }

    // if a function name has been specified, clear error
    // the only time a function name is not specified is for
    // dpiContext_getError() when the error information is being retrieved
    if (fnName) {
        tempErrorBuffer->code = 0;
        tempErrorBuffer->offset = 0;
        tempErrorBuffer->errorNum = 0;
        tempErrorBuffer->isRecoverable = 0;
        tempErrorBuffer->messageLength = 0;
        tempErrorBuffer->fnName = fnName;
        tempErrorBuffer->action = "start";
        strcpy(tempErrorBuffer->encoding, DPI_CHARSET_NAME_UTF8);
    }

    error->buffer = tempErrorBuffer;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiGlobal__lookupCharSet() [INTERNAL]
//   Lookup the character set id that can be used in the call to
// OCINlsEnvCreate().
//-----------------------------------------------------------------------------
int dpiGlobal__lookupCharSet(const char *name, uint16_t *charsetId,
        dpiError *error)
{
    char oraCharsetName[DPI_OCI_NLS_MAXBUFSZ];

    // check for well-known encodings first
    if (strcmp(name, DPI_CHARSET_NAME_UTF8) == 0)
        *charsetId = DPI_CHARSET_ID_UTF8;
    else if (strcmp(name, DPI_CHARSET_NAME_UTF16) == 0)
        *charsetId = DPI_CHARSET_ID_UTF16;
    else if (strcmp(name, DPI_CHARSET_NAME_ASCII) == 0)
        *charsetId = DPI_CHARSET_ID_ASCII;
    else if (strcmp(name, DPI_CHARSET_NAME_UTF16LE) == 0 ||
            strcmp(name, DPI_CHARSET_NAME_UTF16BE) == 0)
        return dpiError__set(error, "check encoding", DPI_ERR_NOT_SUPPORTED);

    // perform lookup; check for the Oracle character set name first and if
    // that fails, lookup using the IANA character set name
    else {
        if (dpiOci__nlsCharSetNameToId(dpiGlobalEnv, name, charsetId,
                error) < 0)
            return DPI_FAILURE;
        if (!*charsetId) {
            if (dpiOci__nlsNameMap(dpiGlobalEnv, oraCharsetName,
                    sizeof(oraCharsetName), name, DPI_OCI_NLS_CS_IANA_TO_ORA,
                    error) < 0)
                return dpiError__set(error, "lookup charset",
                        DPI_ERR_INVALID_CHARSET, name);
            dpiOci__nlsCharSetNameToId(dpiGlobalEnv, oraCharsetName, charsetId,
                    error);
        }
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiGlobal__lookupEncoding() [INTERNAL]
//   Get the IANA character set name (encoding) given the Oracle character set
// id.
//-----------------------------------------------------------------------------
int dpiGlobal__lookupEncoding(uint16_t charsetId, char *encoding,
        dpiError *error)
{
    char oracleName[DPI_OCI_NLS_MAXBUFSZ];

    // check for well-known encodings first
    switch (charsetId) {
        case DPI_CHARSET_ID_UTF8:
            strcpy(encoding, DPI_CHARSET_NAME_UTF8);
            return DPI_SUCCESS;
        case DPI_CHARSET_ID_UTF16:
            strcpy(encoding, DPI_CHARSET_NAME_UTF16);
            return DPI_SUCCESS;
        case DPI_CHARSET_ID_ASCII:
            strcpy(encoding, DPI_CHARSET_NAME_ASCII);
            return DPI_SUCCESS;
    }

    // get character set name
    if (dpiOci__nlsCharSetIdToName(dpiGlobalEnv, oracleName,
            sizeof(oracleName), charsetId, error) < 0)
        return dpiError__set(error, "lookup Oracle character set name",
                DPI_ERR_INVALID_CHARSET_ID, charsetId);

    // get IANA character set name
    if (dpiOci__nlsNameMap(dpiGlobalEnv, encoding, DPI_OCI_NLS_MAXBUFSZ,
            oracleName, DPI_OCI_NLS_CS_ORA_TO_IANA, error) < 0)
        return dpiError__set(error, "lookup IANA name",
                DPI_ERR_INVALID_CHARSET_ID, charsetId);

    return DPI_SUCCESS;
}

