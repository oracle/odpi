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
// dpiPool.c
//   Implementation of session pools.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

//-----------------------------------------------------------------------------
// dpiPool__checkConnected() [INTERNAL]
//   Determine if the session pool is connected to the database. If not, an
// error is raised.
//-----------------------------------------------------------------------------
static int dpiPool__checkConnected(dpiPool *pool, const char *fnName,
        dpiError *error)
{
    if (dpiGen__startPublicFn(pool, DPI_HTYPE_POOL, fnName, error) < 0)
        return DPI_FAILURE;
    if (!pool->handle)
        return dpiError__set(error, "check pool", DPI_ERR_NOT_CONNECTED);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool__create() [INTERNAL]
//   Internal method for creating a session pool.
//-----------------------------------------------------------------------------
static int dpiPool__create(dpiPool *pool, const char *userName,
        uint32_t userNameLength, const char *password, uint32_t passwordLength,
        const char *connectString, uint32_t connectStringLength,
        const dpiCommonCreateParams *commonParams,
        dpiPoolCreateParams *createParams, dpiError *error)
{
    OCIAuthInfo *authInfo;
    uint32_t poolMode;
    uint8_t getMode;
    sword status;

    // validate parameters
    if (createParams->externalAuth && (userName || password))
        return dpiError__set(error, "check mixed credentials",
                DPI_ERR_EXT_AUTH_WITH_CREDENTIALS);

    // create the session pool handle
    status = OCIHandleAlloc(pool->env->handle, (dvoid**) &pool->handle,
            OCI_HTYPE_SPOOL, 0, 0);
    if (dpiError__check(error, status, NULL, "allocate handle") < 0)
        return DPI_FAILURE;

    // prepare pool mode
    poolMode = OCI_SPC_STMTCACHE;
    if (createParams->homogeneous)
        poolMode |= OCI_SPC_HOMOGENEOUS;

    // create authorization handle
    status = OCIHandleAlloc(pool->env->handle, (dvoid*) &authInfo,
            OCI_HTYPE_AUTHINFO, 0, NULL);
    if (dpiError__check(error, status, NULL, "allocate authinfo handle") < 0)
        return DPI_FAILURE;

    // set context attributes
    if (dpiConn__setAttributesFromCommonCreateParams(pool->env->context,
            authInfo, OCI_HTYPE_AUTHINFO, commonParams, error) < 0)
        return DPI_FAILURE;

    // set authorization info on session pool
    status = OCIAttrSet(pool->handle, OCI_HTYPE_SPOOL,
            (void*) authInfo, 0, OCI_ATTR_SPOOL_AUTH, error->handle);
    if (dpiError__check(error, status, NULL, "set auth info") < 0)
        return DPI_FAILURE;

    // create pool
    status = OCISessionPoolCreate(pool->env->handle, error->handle,
            pool->handle, (OraText**) &pool->name, &pool->nameLength,
            (text*) connectString, connectStringLength,
            createParams->minSessions, createParams->maxSessions,
            createParams->sessionIncrement, (text*) userName,
            userNameLength, (text*) password, passwordLength, poolMode);
    if (dpiError__check(error, status, NULL, "create pool") < 0)
        return DPI_FAILURE;

    // set the get mode on the pool
    getMode = (uint8_t) createParams->getMode;
    status = OCIAttrSet(pool->handle, OCI_HTYPE_SPOOL, (dvoid*) &getMode, 0,
            OCI_ATTR_SPOOL_GETMODE, error->handle);
    if (dpiError__check(error, status, NULL, "set get mode") < 0)
        return DPI_FAILURE;

    // set reamining attributes directly
    pool->homogeneous = createParams->homogeneous;
    pool->externalAuth = createParams->externalAuth;
    pool->pingInterval = createParams->pingInterval;
    pool->pingTimeout = createParams->pingTimeout;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool__free() [INTERNAL]
//   Free any memory associated with the pool.
//-----------------------------------------------------------------------------
void dpiPool__free(dpiPool *pool, dpiError *error)
{
    if (pool->handle) {
        OCISessionPoolDestroy(pool->handle, error->handle, OCI_SPD_FORCE);
        OCIHandleFree(pool->handle, OCI_HTYPE_SPOOL);
        pool->handle = NULL;
    }
    if (pool->env) {
        dpiEnv__free(pool->env, error);
        pool->env = NULL;
    }
    free(pool);
}


//-----------------------------------------------------------------------------
// dpiPool__getAttributeUint() [INTERNAL]
//   Return the value of the attribute as an unsigned integer.
//-----------------------------------------------------------------------------
static int dpiPool__getAttributeUint(dpiPool *pool, uint32_t attribute,
        uint32_t *value, const char *fnName)
{
    uint8_t shortValue;
    dvoid *ociValue;
    dpiError error;
    sword status;

    // make sure session pool is connected
    if (dpiPool__checkConnected(pool, fnName, &error) < 0)
        return DPI_FAILURE;

    // determine pointer to pass (OCI uses different sizes)
    switch (attribute) {
        case OCI_ATTR_SPOOL_GETMODE:
            ociValue = &shortValue;
            break;
        case OCI_ATTR_SPOOL_BUSY_COUNT:
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
        case OCI_ATTR_SPOOL_MAX_LIFETIME_SESSION:
#endif
        case OCI_ATTR_SPOOL_OPEN_COUNT:
        case OCI_ATTR_SPOOL_STMTCACHESIZE:
        case OCI_ATTR_SPOOL_TIMEOUT:
            ociValue = value;
            break;
        default:
            return dpiError__set(&error, "get attribute",
                    DPI_ERR_NOT_SUPPORTED);
    }

    // acquire value from OCI
    status = OCIAttrGet(pool->handle, OCI_HTYPE_SPOOL, ociValue, 0,
            attribute, error.handle);
    if (dpiError__check(&error, status, NULL, "get attribute value") < 0)
        return DPI_FAILURE;

    // determine return value
    if (attribute == OCI_ATTR_SPOOL_GETMODE)
        *value = shortValue;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool__setAttributeUint() [INTERNAL]
//   Set the value of the OCI attribute as an unsigned integer.
//-----------------------------------------------------------------------------
static int dpiPool__setAttributeUint(dpiPool *pool, uint32_t attribute,
        uint32_t value, const char *fnName)
{
    uint8_t shortValue;
    dvoid *ociValue;
    dpiError error;
    sword status;

    // make sure session pool is connected
    if (dpiPool__checkConnected(pool, fnName, &error) < 0)
        return DPI_FAILURE;

    // determine pointer to pass (OCI uses different sizes)
    switch (attribute) {
        case OCI_ATTR_SPOOL_GETMODE:
            shortValue = (uint8_t) value;
            ociValue = &shortValue;
            break;
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
        case OCI_ATTR_SPOOL_MAX_LIFETIME_SESSION:
#endif
        case OCI_ATTR_SPOOL_STMTCACHESIZE:
        case OCI_ATTR_SPOOL_TIMEOUT:
            ociValue = &value;
            break;
        default:
            return dpiError__set(&error, "set attribute",
                    DPI_ERR_NOT_SUPPORTED);
    }

    // set value in the OCI
    status = OCIAttrSet(pool->handle, OCI_HTYPE_SPOOL, ociValue, 0, attribute,
            error.handle);
    if (dpiError__check(&error, status, NULL, "set attribute value") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool_acquireConnection() [PUBLIC]
//   Acquire a connection from the pool.
//-----------------------------------------------------------------------------
int dpiPool_acquireConnection(dpiPool *pool, const char *userName,
        uint32_t userNameLength, const char *password, uint32_t passwordLength,
        dpiConnCreateParams *params, dpiConn **conn)
{
    dpiConnCreateParams localParams;
    dpiConn *tempConn;
    dpiError error;

    // make sure session pool is connected
    *conn = NULL;
    if (dpiPool__checkConnected(pool, __func__, &error) < 0)
        return DPI_FAILURE;

    // validate pool handle
    if (!conn)
        return dpiError__set(&error, "check connection handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "conn");

    // use default parameters if none provided
    if (!params) {
        if (dpiContext__initConnCreateParams(pool->env->context, &localParams,
                &error) < 0)
            return DPI_FAILURE;
        params = &localParams;
    }

    // allocate new connection
    if (dpiGen__allocate(DPI_HTYPE_CONN, pool->env, (void**) &tempConn,
            &error) < 0)
        return DPI_FAILURE;

    // create the connection
    if (dpiConn__get(tempConn, userName, userNameLength, password,
            passwordLength, pool->name, pool->nameLength, params, pool,
            &error) < 0) {
        dpiConn__free(tempConn, &error);
        return DPI_FAILURE;
    }

    *conn = tempConn;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool_addRef() [PUBLIC]
//   Add a reference to the pool.
//-----------------------------------------------------------------------------
int dpiPool_addRef(dpiPool *pool)
{
    return dpiGen__addRef(pool, DPI_HTYPE_POOL, __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_close() [PUBLIC]
//   Destroy the pool now, not when the reference count reaches zero.
//-----------------------------------------------------------------------------
int dpiPool_close(dpiPool *pool, dpiPoolCloseMode mode)
{
    dpiError error;
    sword status;

    if (dpiPool__checkConnected(pool, __func__, &error) < 0)
        return DPI_FAILURE;
    status = OCISessionPoolDestroy(pool->handle, error.handle, mode);
    if (dpiError__check(&error, status, NULL, "destroy pool") < 0)
        return DPI_FAILURE;
    OCIHandleFree(pool->handle, OCI_HTYPE_SPOOL);
    pool->handle = NULL;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool_create() [PUBLIC]
//   Create a new session pool and return it.
//-----------------------------------------------------------------------------
int dpiPool_create(const dpiContext *context, const char *userName,
        uint32_t userNameLength, const char *password, uint32_t passwordLength,
        const char *connectString, uint32_t connectStringLength,
        const dpiCommonCreateParams *commonParams,
        dpiPoolCreateParams *createParams, dpiPool **pool)
{
    dpiCommonCreateParams localCommonParams;
    dpiPoolCreateParams localCreateParams;
    dpiPool *tempPool;
    dpiError error;

    // validate context
    if (dpiContext__startPublicFn(context, __func__, &error) < 0)
        return DPI_FAILURE;

    // validate pool handle
    if (!pool)
        return dpiError__set(&error, "check pool handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "pool");

    // use default parameters if none provided
    if (!commonParams) {
        if (dpiContext__initCommonCreateParams(context, &localCommonParams,
                &error) < 0)
            return DPI_FAILURE;
        commonParams = &localCommonParams;
    }
    if (!createParams) {
        if (dpiContext__initPoolCreateParams(context, &localCreateParams,
                &error) < 0)
            return DPI_FAILURE;
        createParams = &localCreateParams;
    }

    // allocate memory for pool
    if (dpiGen__allocate(DPI_HTYPE_POOL, NULL, (void**) &tempPool, &error) < 0)
        return DPI_FAILURE;

    // initialize environment
    if (dpiEnv__init(tempPool->env, context, commonParams, &error) < 0) {
        dpiPool__free(tempPool, &error);
        return DPI_FAILURE;
    }

    // perform remaining steps required to create pool
    if (dpiPool__create(tempPool, userName, userNameLength, password,
            passwordLength, connectString, connectStringLength, commonParams,
            createParams, &error) < 0) {
        dpiPool__free(tempPool, &error);
        return DPI_FAILURE;
    }

    createParams->outPoolName = tempPool->name;
    createParams->outPoolNameLength = tempPool->nameLength;
    *pool = tempPool;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool_getBusyCount() [PUBLIC]
//   Return the pool's busy count.
//-----------------------------------------------------------------------------
int dpiPool_getBusyCount(dpiPool *pool, uint32_t *value)
{
    return dpiPool__getAttributeUint(pool, OCI_ATTR_SPOOL_BUSY_COUNT, value,
            __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_getEncodingInfo() [PUBLIC]
//   Get the encoding information from the pool.
//-----------------------------------------------------------------------------
int dpiPool_getEncodingInfo(dpiPool *pool, dpiEncodingInfo *info)
{
    dpiError error;

    if (dpiGen__startPublicFn(pool, DPI_HTYPE_POOL, __func__, &error) < 0)
        return DPI_FAILURE;
    return dpiEnv__getEncodingInfo(pool->env, info);
}


//-----------------------------------------------------------------------------
// dpiPool_getGetMode() [PUBLIC]
//   Return the pool's "get" mode.
//-----------------------------------------------------------------------------
int dpiPool_getGetMode(dpiPool *pool, dpiPoolGetMode *value)
{
    uint32_t tempValue;

    if (dpiPool__getAttributeUint(pool, OCI_ATTR_SPOOL_GETMODE, &tempValue,
            __func__) < 0)
        return DPI_FAILURE;
    *value = tempValue;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiPool_getMaxLifetimeSession() [PUBLIC]
//   Return the pool's maximum lifetime session.
//-----------------------------------------------------------------------------
int dpiPool_getMaxLifetimeSession(dpiPool *pool, uint32_t *value)
{
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
    return dpiPool__getAttributeUint(pool, OCI_ATTR_SPOOL_MAX_LIFETIME_SESSION,
            value, __func__);
#else
    return dpiPool__getAttributeUint(pool, 0, value, __func__);
#endif
}


//-----------------------------------------------------------------------------
// dpiPool_getOpenCount() [PUBLIC]
//   Return the pool's open count.
//-----------------------------------------------------------------------------
int dpiPool_getOpenCount(dpiPool *pool, uint32_t *value)
{
    return dpiPool__getAttributeUint(pool, OCI_ATTR_SPOOL_OPEN_COUNT, value,
            __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_getStmtCacheSize() [PUBLIC]
//   Return the pool's default statement cache size.
//-----------------------------------------------------------------------------
int dpiPool_getStmtCacheSize(dpiPool *pool, uint32_t *value)
{
    return dpiPool__getAttributeUint(pool, OCI_ATTR_SPOOL_STMTCACHESIZE, value,
            __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_getTimeout() [PUBLIC]
//   Return the pool's timeout value.
//-----------------------------------------------------------------------------
int dpiPool_getTimeout(dpiPool *pool, uint32_t *value)
{
    return dpiPool__getAttributeUint(pool, OCI_ATTR_SPOOL_TIMEOUT, value,
            __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_release() [PUBLIC]
//   Release a reference to the pool.
//-----------------------------------------------------------------------------
int dpiPool_release(dpiPool *pool)
{
    return dpiGen__release(pool, DPI_HTYPE_POOL, __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_setGetMode() [PUBLIC]
//   Set the pool's "get" mode.
//-----------------------------------------------------------------------------
int dpiPool_setGetMode(dpiPool *pool, dpiPoolGetMode value)
{
    return dpiPool__setAttributeUint(pool, OCI_ATTR_SPOOL_GETMODE, value,
            __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_setMaxLifetimeSession() [PUBLIC]
//   Set the pool's maximum lifetime session.
//-----------------------------------------------------------------------------
int dpiPool_setMaxLifetimeSession(dpiPool *pool, uint32_t value)
{
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
    return dpiPool__setAttributeUint(pool, OCI_ATTR_SPOOL_MAX_LIFETIME_SESSION,
            value, __func__);
#else
    return dpiPool__setAttributeUint(pool, 0, value, __func__);
#endif
}


//-----------------------------------------------------------------------------
// dpiPool_setStmtCacheSize() [PUBLIC]
//   Set the pool's default statement cache size.
//-----------------------------------------------------------------------------
int dpiPool_setStmtCacheSize(dpiPool *pool, uint32_t value)
{
    return dpiPool__setAttributeUint(pool, OCI_ATTR_SPOOL_STMTCACHESIZE, value,
            __func__);
}


//-----------------------------------------------------------------------------
// dpiPool_setTimeout() [PUBLIC]
//   Set the pool's timeout value.
//-----------------------------------------------------------------------------
int dpiPool_setTimeout(dpiPool *pool, uint32_t value)
{
    return dpiPool__setAttributeUint(pool, OCI_ATTR_SPOOL_TIMEOUT, value,
            __func__);
}

