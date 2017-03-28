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
// dpiLob.c
//   Implementation of LOB data.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

//-----------------------------------------------------------------------------
// dpiLob__allocate() [INTERNAL]
//   Allocate and initialize LOB object.
//-----------------------------------------------------------------------------
int dpiLob__allocate(dpiConn *conn, const dpiOracleType *type, dpiLob **lob,
        dpiError *error)
{
    dpiLob *tempLob;

    if (dpiGen__allocate(DPI_HTYPE_LOB, conn->env, (void**) &tempLob,
            error) < 0)
        return DPI_FAILURE;
    if (dpiGen__setRefCount(conn, error, 1) < 0) {
        dpiLob__free(tempLob, error);
        return DPI_FAILURE;
    }
    tempLob->conn = conn;
    tempLob->type = type;
    if (dpiOci__descriptorAlloc(conn->env, &tempLob->locator,
            DPI_OCI_DTYPE_LOB, "allocate descriptor", error) < 0) {
        dpiLob__free(tempLob, error);
        return DPI_FAILURE;
    }

    *lob = tempLob;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiLob__check() [INTERNAL]
//   Check that the LOB is valid and get an error handle for subsequent calls.
//-----------------------------------------------------------------------------
int dpiLob__check(dpiLob *lob, const char *fnName, dpiError *error)
{
    if (dpiGen__startPublicFn(lob, DPI_HTYPE_LOB, fnName, error) < 0)
        return DPI_FAILURE;
    if (!lob->locator)
        return dpiError__set(error, "check closed", DPI_ERR_LOB_CLOSED);
    if (!lob->conn->handle)
        return dpiError__set(error, "check connection", DPI_ERR_NOT_CONNECTED);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiLob__close() [INTERNAL]
//   Internal method used for closing the LOB.
//-----------------------------------------------------------------------------
static int dpiLob__close(dpiLob *lob, int propagateErrors, dpiError *error)
{
    int isTemporary;

    if (lob->locator) {
        if (dpiOci__lobIsTemporary(lob, &isTemporary, propagateErrors,
                error) < 0)
            return DPI_FAILURE;
        if (isTemporary) {
            if (dpiOci__lobFreeTemporary(lob, propagateErrors, error) < 0)
                return DPI_FAILURE;
        }
        dpiOci__descriptorFree(lob->locator, DPI_OCI_DTYPE_LOB);
        lob->locator = NULL;
    }
    if (lob->conn) {
        dpiGen__setRefCount(lob->conn, error, -1);
        lob->conn = NULL;
    }
    if (lob->buffer) {
        free(lob->buffer);
        lob->buffer = NULL;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiLob__free() [INTERNAL]
//   Free the memory for a LOB.
//-----------------------------------------------------------------------------
void dpiLob__free(dpiLob *lob, dpiError *error)
{
    dpiLob__close(lob, 0, error);
    free(lob);
}


//-----------------------------------------------------------------------------
// dpiLob__readBytes() [INTERNAL]
//   Return a portion (or all) of the data in the LOB.
//-----------------------------------------------------------------------------
int dpiLob__readBytes(dpiLob *lob, uint64_t offset, uint64_t amount,
        char *value, uint64_t *valueLength, dpiError *error)
{
    uint64_t lengthInBytes = 0, lengthInChars = 0;
    int isOpen;

    // amount is in characters for character LOBs and bytes for binary LOBs
    if (lob->type->isCharacterData)
        lengthInChars = amount;
    else lengthInBytes = amount;

    // for files, open the file if needed
    if (lob->type->oracleTypeNum == DPI_ORACLE_TYPE_BFILE) {
        if (dpiOci__lobIsOpen(lob, &isOpen, error) < 0)
            return DPI_FAILURE;
        if (!isOpen) {
            if (dpiOci__lobOpen(lob, error) < 0)
                return DPI_FAILURE;
        }
    }

    // read the bytes from the LOB
    if (dpiOci__lobRead2(lob, offset, &lengthInBytes, &lengthInChars,
            value, *valueLength, error) < 0)
        return DPI_FAILURE;
    *valueLength = lengthInBytes;

    // if file was opened in this routine, close it again
    if (lob->type->oracleTypeNum == DPI_ORACLE_TYPE_BFILE && !isOpen) {
        if (dpiOci__lobClose(lob, error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiLob__setFromBytes() [INTERNAL]
//   Clear the LOB completely and then write the specified bytes to it.
//-----------------------------------------------------------------------------
int dpiLob__setFromBytes(dpiLob *lob, const char *value, uint64_t valueLength,
        dpiError *error)
{
    if (dpiOci__lobTrim2(lob, 0, error) < 0)
        return DPI_FAILURE;
    if (valueLength == 0)
        return DPI_SUCCESS;
    return dpiOci__lobWrite2(lob, 1, value, valueLength, error);
}


//-----------------------------------------------------------------------------
// dpiLob_addRef() [PUBLIC]
//   Add a reference to the LOB.
//-----------------------------------------------------------------------------
int dpiLob_addRef(dpiLob *lob)
{
    return dpiGen__addRef(lob, DPI_HTYPE_LOB, __func__);
}


//-----------------------------------------------------------------------------
// dpiLob_close() [PUBLIC]
//   Close the LOB and make it unusable for further operations.
//-----------------------------------------------------------------------------
int dpiLob_close(dpiLob *lob)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiLob__close(lob, 1, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_closeResource() [PUBLIC]
//   Close the LOB's resources.
//-----------------------------------------------------------------------------
int dpiLob_closeResource(dpiLob *lob)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobClose(lob, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_copy() [PUBLIC]
//   Create a copy of the LOB and return it.
//-----------------------------------------------------------------------------
int dpiLob_copy(dpiLob *lob, dpiLob **copiedLob)
{
    dpiLob *tempLob;
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    if (!copiedLob)
        return dpiError__set(&error, "check copied LOB handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "copiedLob");
    if (dpiLob__allocate(lob->conn, lob->type, &tempLob, &error) < 0)
        return DPI_FAILURE;
    if (dpiOci__lobLocatorAssign(lob, &tempLob->locator, &error) < 0) {
        dpiLob__free(tempLob, &error);
        return DPI_FAILURE;
    }
    *copiedLob = tempLob;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiLob_flushBuffer() [PUBLIC]
//   Flush the buffers.
//-----------------------------------------------------------------------------
int dpiLob_flushBuffer(dpiLob *lob)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobFlushBuffer(lob, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_getBufferSize() [PUBLIC]
//   Get the required size of a buffer given the number of characters. If the
// LOB does not refer to a character LOB the value is returned unchanged.
//-----------------------------------------------------------------------------
int dpiLob_getBufferSize(dpiLob *lob, uint64_t sizeInChars,
        uint64_t *sizeInBytes)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    if (lob->type->oracleTypeNum == DPI_ORACLE_TYPE_CLOB)
        *sizeInBytes = sizeInChars * lob->env->maxBytesPerCharacter;
    else if (lob->type->oracleTypeNum == DPI_ORACLE_TYPE_NCLOB)
        *sizeInBytes = sizeInChars * lob->env->nmaxBytesPerCharacter;
    else *sizeInBytes = sizeInChars;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiLob_getChunkSize() [PUBLIC]
//   Return the chunk size associated with the LOB.
//-----------------------------------------------------------------------------
int dpiLob_getChunkSize(dpiLob *lob, uint32_t *size)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobGetChunkSize(lob, size, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_getDirectoryAndFileName() [PUBLIC]
//   Return the directory alias and file name for the BFILE lob.
//-----------------------------------------------------------------------------
int dpiLob_getDirectoryAndFileName(dpiLob *lob, const char **directoryAlias,
        uint32_t *directoryAliasLength, const char **fileName,
        uint32_t *fileNameLength)
{
    uint16_t ociDirectoryAliasLength, ociFileNameLength;
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    ociDirectoryAliasLength = 30;
    ociFileNameLength = 255;
    if (!lob->buffer) {
        lob->buffer = malloc(ociDirectoryAliasLength + ociFileNameLength);
        if (!lob->buffer)
            return dpiError__set(&error, "allocate memory", DPI_ERR_NO_MEMORY);
    }
    *directoryAlias = lob->buffer;
    *fileName = lob->buffer + ociDirectoryAliasLength;
    if (dpiOci__lobFileGetName(lob, (char*) *directoryAlias,
            &ociDirectoryAliasLength, (char*) *fileName, &ociFileNameLength,
            &error) < 0)
        return DPI_FAILURE;
    *directoryAliasLength = ociDirectoryAliasLength;
    *fileNameLength = ociFileNameLength;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiLob_getFileExists() [PUBLIC]
//   Return whether or not the file pointed to by the locator exists.
//-----------------------------------------------------------------------------
int dpiLob_getFileExists(dpiLob *lob, int *exists)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobFileExists(lob, exists, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_getIsResourceOpen() [PUBLIC]
//   Return whether or not the LOB' resources are open.
//-----------------------------------------------------------------------------
int dpiLob_getIsResourceOpen(dpiLob *lob, int *isOpen)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobIsOpen(lob, isOpen, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_getSize() [PUBLIC]
//   Returns the size of the LOB.
//-----------------------------------------------------------------------------
int dpiLob_getSize(dpiLob *lob, uint64_t *size)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobGetLength2(lob, size, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_openResource() [PUBLIC]
//   Open the LOB's resources to speed further accesses.
//-----------------------------------------------------------------------------
int dpiLob_openResource(dpiLob *lob)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobOpen(lob, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_readBytes() [PUBLIC]
//   Return a portion (or all) of the data in the LOB.
//-----------------------------------------------------------------------------
int dpiLob_readBytes(dpiLob *lob, uint64_t offset, uint64_t amount,
        char *value, uint64_t *valueLength)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiLob__readBytes(lob, offset, amount, value, valueLength, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_release() [PUBLIC]
//   Release a reference to the LOB.
//-----------------------------------------------------------------------------
int dpiLob_release(dpiLob *lob)
{
    return dpiGen__release(lob, DPI_HTYPE_LOB, __func__);
}


//-----------------------------------------------------------------------------
// dpiLob_setDirectoryAndFileName() [PUBLIC]
//   Set the directory alias and file name for the BFILE LOB.
//-----------------------------------------------------------------------------
int dpiLob_setDirectoryAndFileName(dpiLob *lob, const char *directoryAlias,
        uint32_t directoryAliasLength, const char *fileName,
        uint32_t fileNameLength)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobFileSetName(lob, directoryAlias,
            (uint16_t) directoryAliasLength, fileName,
            (uint16_t) fileNameLength, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_setFromBytes() [PUBLIC]
//   Clear the LOB completely and then write the specified bytes to it.
//-----------------------------------------------------------------------------
int dpiLob_setFromBytes(dpiLob *lob, const char *value, uint64_t valueLength)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiLob__setFromBytes(lob, value, valueLength, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_trim() [PUBLIC]
//   Trim the LOB to the specified length.
//-----------------------------------------------------------------------------
int dpiLob_trim(dpiLob *lob, uint64_t newSize)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobTrim2(lob, newSize, &error);
}


//-----------------------------------------------------------------------------
// dpiLob_writeBytes() [PUBLIC]
//   Write the data to the LOB at the offset specified.
//-----------------------------------------------------------------------------
int dpiLob_writeBytes(dpiLob *lob, uint64_t offset, const char *value,
        uint64_t valueLength)
{
    dpiError error;

    if (dpiLob__check(lob, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiOci__lobWrite2(lob, offset, value, valueLength, &error);
}

