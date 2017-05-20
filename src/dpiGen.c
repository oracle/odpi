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
// dpiGen.c
//   Generic routines for managing the types available through public APIs.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

//-----------------------------------------------------------------------------
// definition of handle types
//-----------------------------------------------------------------------------
static const dpiTypeDef dpiAllTypeDefs[DPI_HTYPE_MAX - DPI_HTYPE_NONE - 1] = {
    {
        "dpiConn",                      // name
        sizeof(dpiConn),                // size of structure
        0x49DC600C,                     // check integer
        (dpiTypeFreeProc) dpiConn__free
    },
    {
        "dpiPool",                      // name
        sizeof(dpiPool),                // size of structure
        0x18E1AA4B,                     // check integer
        (dpiTypeFreeProc) dpiPool__free
    },
    {
        "dpiStmt",                      // name
        sizeof(dpiStmt),                // size of structure
        0x31B02B2E,                     // check integer
        (dpiTypeFreeProc) dpiStmt__free
    },
    {
        "dpiVar",                       // name
        sizeof(dpiVar),                 // size of structure
        0x2AE8C6DC,                     // check integer
        (dpiTypeFreeProc) dpiVar__free
    },
    {
        "dpiLob",                       // name
        sizeof(dpiLob),                 // size of structure
        0xD8F31746,                     // check integer
        (dpiTypeFreeProc) dpiLob__free
    },
    {
        "dpiObject",                    // name
        sizeof(dpiObject),              // size of structure
        0x38616080,                     // check integer
        (dpiTypeFreeProc) dpiObject__free
    },
    {
        "dpiObjectType",                // name
        sizeof(dpiObjectType),          // size of structure
        0x86036059,                     // check integer
        (dpiTypeFreeProc) dpiObjectType__free
    },
    {
        "dpiObjectAttr",                // name
        sizeof(dpiObjectAttr),          // size of structure
        0xea6d5dde,                     // check integer
        (dpiTypeFreeProc) dpiObjectAttr__free
    },
    {
        "dpiSubscr",                    // name
        sizeof(dpiSubscr),              // size of structure
        0xa415a1c0,                     // check integer
        (dpiTypeFreeProc) dpiSubscr__free
    },
    {
        "dpiDeqOptions",                // name
        sizeof(dpiDeqOptions),          // size of structure
        0x70ee498d,                     // check integer
        (dpiTypeFreeProc) dpiDeqOptions__free
    },
    {
        "dpiEnqOptions",                // name
        sizeof(dpiEnqOptions),          // size of structure
        0x682f3946,                     // check integer
        (dpiTypeFreeProc) dpiEnqOptions__free
    },
    {
        "dpiMsgProps",                  // name
        sizeof(dpiMsgProps),            // size of structure
        0xa2b75506,                     // check integer
        (dpiTypeFreeProc) dpiMsgProps__free
    },
    {
        "dpiRowid",                     // name
        sizeof(dpiRowid),               // size of structure
        0x6204fa04,                     // check integer
        (dpiTypeFreeProc) dpiRowid__free
    }
};


//-----------------------------------------------------------------------------
// dpiGen__addRef() [INTERNAL]
//   Add a reference to the specified handle.
//-----------------------------------------------------------------------------
int dpiGen__addRef(void *ptr, dpiHandleTypeNum typeNum, const char *fnName)
{
    dpiError error;

    if (dpiGen__startPublicFn(ptr, typeNum, fnName, &error) < 0)
        return DPI_FAILURE;
    return dpiGen__setRefCount(ptr, &error, 1);
}


//-----------------------------------------------------------------------------
// dpiGen__allocate() [INTERNAL]
//   Allocate memory for the specified type and initialize the base fields. The
// type specified is assumed to be valid. If the environment is specified, use
// it; otherwise, create a new one. No additional initialization is performed.
//-----------------------------------------------------------------------------
int dpiGen__allocate(dpiHandleTypeNum typeNum, dpiEnv *env, void **handle,
        dpiError *error)
{
    const dpiTypeDef *typeDef;
    dpiBaseType *value;

    typeDef = &dpiAllTypeDefs[typeNum - DPI_HTYPE_NONE - 1];
    value = calloc(1, typeDef->size);
    if (!value)
        return dpiError__set(error, "allocate memory", DPI_ERR_NO_MEMORY);
    value->typeDef = typeDef;
    value->checkInt = typeDef->checkInt;
    value->refCount = 1;
    if (!env) {
        env = (dpiEnv*) calloc(1, sizeof(dpiEnv));
        if (!env) {
            free(value);
            return dpiError__set(error, "allocate env memory",
                    DPI_ERR_NO_MEMORY);
        }
    }
    value->env = env;
#if DPI_DEBUG_LEVEL & DPI_DEBUG_LEVEL_REFS
    fprintf(stderr, "REF: %p (%s) -> 1 [NEW]\n", value, typeDef->name);
#endif

    *handle = value;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiGen__checkHandle() [INTERNAL]
//   Check that the specific handle is valid, that it matches the type
// requested and that the check integer is still in place.
//-----------------------------------------------------------------------------
int dpiGen__checkHandle(void *ptr, dpiHandleTypeNum typeNum,
        const char *action, dpiError *error)
{
    dpiBaseType *value = (dpiBaseType*) ptr;
    const dpiTypeDef *typeDef;

    typeDef = &dpiAllTypeDefs[typeNum - DPI_HTYPE_NONE - 1];
    if (!ptr || value->typeDef != typeDef ||
            value->checkInt != typeDef->checkInt)
        return dpiError__set(error, action, DPI_ERR_INVALID_HANDLE,
                typeDef->name);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiGen__release() [INTERNAL]
//   Release a reference to the specified handle. If the reference count
// reaches zero, the resources associated with the handle are released and
// the memory associated with the handle is freed. Any internal references
// held to other handles are also released.
//-----------------------------------------------------------------------------
int dpiGen__release(void *ptr, dpiHandleTypeNum typeNum, const char *fnName)
{
    dpiError error;

    if (dpiGen__startPublicFn(ptr, typeNum, fnName, &error) < 0)
        return DPI_FAILURE;
    return dpiGen__setRefCount(ptr, &error, -1);
}


//-----------------------------------------------------------------------------
// dpiGen__setRefCount() [INTERNAL]
//   Increase or decrease the reference count by the given amount. The handle
// is assumed to be valid at this point. If the environment is in threaded
// mode, acquire the mutex first before making any adjustments to the reference
// count. If the operation sets the reference count to zero, release all
// resources and free the memory associated with the structure.
//-----------------------------------------------------------------------------
int dpiGen__setRefCount(void *ptr, dpiError *error, int increment)
{
    dpiBaseType *value = (dpiBaseType*) ptr;
    unsigned localRefCount;

    // if threaded need to protect modification of the refCount with a mutex
    if (value->env->threaded) {
        if (dpiOci__threadMutexAcquire(value->env, error) < 0)
            return DPI_FAILURE;
        value->refCount += increment;
        localRefCount = value->refCount;
        if (dpiOci__threadMutexRelease(value->env, error) < 0)
            return DPI_FAILURE;

    // otherwise the count can be incremented normally
    } else {
        value->refCount += increment;
        localRefCount = value->refCount;
    }

#if DPI_DEBUG_LEVEL & DPI_DEBUG_LEVEL_REFS
    fprintf(stderr, "REF: %p (%s) -> %d\n", ptr, value->typeDef->name,
            localRefCount);
#endif

    // if the refCount has reached zero, call the free routine
    if (localRefCount == 0) {
        dpiUtils__clearMemory(&value->checkInt, sizeof(value->checkInt));
        (*value->typeDef->freeProc)(value, error);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiGen__startPublicFn() [INTERNAL]
//   Check that the specific handle is valid and acquire an error handle to use
// for all subsequent function calls. This method should be the first call made
// in any public method on a ODPI-C handle (other than dpiContext which is handled
// differently).
//-----------------------------------------------------------------------------
int dpiGen__startPublicFn(void *ptr, dpiHandleTypeNum typeNum,
        const char *fnName, dpiError *error)
{
    dpiBaseType *value = (dpiBaseType*) ptr;

#if DPI_DEBUG_LEVEL & DPI_DEBUG_LEVEL_FNS
    fprintf(stderr, "FN: %s(%p)\n", fnName, ptr);
#endif
    if (dpiGlobal__initError(fnName, error) < 0)
        return DPI_FAILURE;
    if (dpiGen__checkHandle(ptr, typeNum, "check main handle", error) < 0)
        return DPI_FAILURE;
    if (dpiEnv__initError(value->env, error) < 0)
        return DPI_FAILURE;
    return DPI_SUCCESS;
}

