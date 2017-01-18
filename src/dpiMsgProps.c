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
// dpiMsgProps.c
//   Implementation of AQ message properties.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

//-----------------------------------------------------------------------------
// dpiMsgProps__create() [INTERNAL]
//   Create a new subscription structure and return it. In case of error NULL
// is returned.
//-----------------------------------------------------------------------------
int dpiMsgProps__create(dpiMsgProps *options, dpiConn *conn, dpiError *error)
{
    sword status;

    // retain a reference to the connection
    if (dpiGen__setRefCount(conn, error, 1) < 0)
        return DPI_FAILURE;
    options->conn = conn;

    // create the OCI handle
    status = OCIDescriptorAlloc(conn->env->handle, (dvoid**) &options->handle,
            OCI_DTYPE_AQMSG_PROPERTIES, 0, 0);
    return dpiError__check(error, status, options->conn,
            "allocate descriptor");
}


//-----------------------------------------------------------------------------
// dpiMsgProps__free() [INTERNAL]
//   Free the memory for a message properties structure.
//-----------------------------------------------------------------------------
void dpiMsgProps__free(dpiMsgProps *props, dpiError *error)
{
    if (props->handle) {
        OCIDescriptorFree(props->handle, OCI_DTYPE_AQMSG_PROPERTIES);
        props->handle = NULL;
    }
    if (props->conn) {
        dpiGen__setRefCount(props->conn, error, -1);
        props->conn = NULL;
    }
    free(props);
}


//-----------------------------------------------------------------------------
// dpiMsgProps__getAttrValue() [INTERNAL]
//   Get the attribute value in OCI.
//-----------------------------------------------------------------------------
static int dpiMsgProps__getAttrValue(dpiMsgProps *props, uint32_t attribute,
        const char *fnName, dvoid *value, uint32_t *valueLength)
{
    dpiError error;
    sword status;

    if (dpiGen__startPublicFn(props, DPI_HTYPE_MSG_PROPS, fnName, &error) < 0)
        return DPI_FAILURE;
    status = OCIAttrGet(props->handle, OCI_DTYPE_AQMSG_PROPERTIES, value,
            valueLength, attribute, error.handle);
    return dpiError__check(&error, status, props->conn, "get attribute value");
}


//-----------------------------------------------------------------------------
// dpiMsgProps__setAttrValue() [INTERNAL]
//   Set the attribute value in OCI.
//-----------------------------------------------------------------------------
static int dpiMsgProps__setAttrValue(dpiMsgProps *props, uint32_t attribute,
        const char *fnName, const void *value, uint32_t valueLength)
{
    dpiError error;
    sword status;

    if (dpiGen__startPublicFn(props, DPI_HTYPE_MSG_PROPS, fnName, &error) < 0)
        return DPI_FAILURE;
    status = OCIAttrSet(props->handle, OCI_DTYPE_AQMSG_PROPERTIES,
            (dvoid*) value, valueLength, attribute, error.handle);
    return dpiError__check(&error, status, props->conn, "set attribute value");
}


//-----------------------------------------------------------------------------
// dpiMsgProps_addRef() [PUBLIC]
//   Add a reference to the message properties.
//-----------------------------------------------------------------------------
int dpiMsgProps_addRef(dpiMsgProps *props)
{
    return dpiGen__addRef(props, DPI_HTYPE_MSG_PROPS, __func__);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getCorrelation() [PUBLIC]
//   Return correlation associated with the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_getCorrelation(dpiMsgProps *props, const char **value,
        uint32_t *valueLength)
{
    return dpiMsgProps__getAttrValue(props, OCI_ATTR_CORRELATION, __func__,
            (void*) value, valueLength);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getDelay() [PUBLIC]
//   Return the number of seconds the message was delayed.
//-----------------------------------------------------------------------------
int dpiMsgProps_getDelay(dpiMsgProps *props, int32_t *value)
{
    return dpiMsgProps__getAttrValue(props, OCI_ATTR_DELAY, __func__, value,
            NULL);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getDeliveryMode() [PUBLIC]
//   Return the mode used for delivering the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_getDeliveryMode(dpiMsgProps *props,
        dpiMessageDeliveryMode *value)
{
    uint16_t ociValue;

    if (dpiMsgProps__getAttrValue(props, OCI_ATTR_MSG_DELIVERY_MODE, __func__,
            &ociValue, NULL) < 0)
        return DPI_FAILURE;
    *value = ociValue;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getEnqTime() [PUBLIC]
//   Return the time the message was enqueued.
//-----------------------------------------------------------------------------
int dpiMsgProps_getEnqTime(dpiMsgProps *props, dpiTimestamp *value)
{
    OCIDate ociValue;

    if (dpiMsgProps__getAttrValue(props, OCI_ATTR_ENQ_TIME, __func__,
            &ociValue, NULL) < 0)
        return DPI_FAILURE;
    OCIDateGetDate(&ociValue, &value->year, &value->month, &value->day);
    OCIDateGetTime(&ociValue, &value->hour, &value->minute, &value->second);
    value->fsecond = 0;
    value->tzHourOffset = 0;
    value->tzMinuteOffset = 0;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getExceptionQ() [PUBLIC]
//   Return the name of the exception queue associated with the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_getExceptionQ(dpiMsgProps *props, const char **value,
        uint32_t *valueLength)
{
    return dpiMsgProps__getAttrValue(props, OCI_ATTR_EXCEPTION_QUEUE, __func__,
            (void*) value, valueLength);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getExpiration() [PUBLIC]
//   Return the number of seconds until the message expires.
//-----------------------------------------------------------------------------
int dpiMsgProps_getExpiration(dpiMsgProps *props, int32_t *value)
{
    return dpiMsgProps__getAttrValue(props, OCI_ATTR_EXPIRATION, __func__,
            value, NULL);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getNumAttempts() [PUBLIC]
//   Return the number of attempts made to deliver the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_getNumAttempts(dpiMsgProps *props, int32_t *value)
{
    return dpiMsgProps__getAttrValue(props, OCI_ATTR_ATTEMPTS, __func__, value,
            NULL);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getOriginalMsgId() [PUBLIC]
//   Return the original message id for the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_getOriginalMsgId(dpiMsgProps *props, const char **value,
        uint32_t *valueLength)
{
    OCIRaw *rawValue;

    if (dpiMsgProps__getAttrValue(props, OCI_ATTR_ORIGINAL_MSGID, __func__,
            &rawValue, NULL) < 0)
        return DPI_FAILURE;
    *value = (const char*) OCIRawPtr(props->env->handle, rawValue);
    *valueLength = OCIRawSize(props->env->handle, rawValue);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getPriority() [PUBLIC]
//   Return the priority of the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_getPriority(dpiMsgProps *props, int32_t *value)
{
    return dpiMsgProps__getAttrValue(props, OCI_ATTR_PRIORITY, __func__, value,
            NULL);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_getState() [PUBLIC]
//   Return the state of the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_getState(dpiMsgProps *props, dpiMessageState *value)
{
    uint32_t ociValue;

    if (dpiMsgProps__getAttrValue(props, OCI_ATTR_MSG_STATE, __func__,
            &ociValue, NULL) < 0)
        return DPI_FAILURE;
    *value = ociValue;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiMsgProps_release() [PUBLIC]
//   Release a reference to the message properties.
//-----------------------------------------------------------------------------
int dpiMsgProps_release(dpiMsgProps *props)
{
    return dpiGen__release(props, DPI_HTYPE_MSG_PROPS, __func__);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_setCorrelation() [PUBLIC]
//   Set correlation associated with the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_setCorrelation(dpiMsgProps *props, const char *value,
        uint32_t valueLength)
{
    return dpiMsgProps__setAttrValue(props, OCI_ATTR_CORRELATION, __func__,
            value, valueLength);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_setDelay() [PUBLIC]
//   Set the number of seconds to delay the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_setDelay(dpiMsgProps *props, int32_t value)
{
    return dpiMsgProps__setAttrValue(props, OCI_ATTR_DELAY, __func__, &value,
            0);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_setExceptionQ() [PUBLIC]
//   Set the name of the exception queue associated with the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_setExceptionQ(dpiMsgProps *props, const char *value,
        uint32_t valueLength)
{
    return dpiMsgProps__setAttrValue(props, OCI_ATTR_EXCEPTION_QUEUE, __func__,
            value, valueLength);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_setExpiration() [PUBLIC]
//   Set the number of seconds until the message expires.
//-----------------------------------------------------------------------------
int dpiMsgProps_setExpiration(dpiMsgProps *props, int32_t value)
{
    return dpiMsgProps__setAttrValue(props, OCI_ATTR_EXPIRATION, __func__,
            &value, 0);
}


//-----------------------------------------------------------------------------
// dpiMsgProps_setOriginalMsgId() [PUBLIC]
//   Set the original message id for the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_setOriginalMsgId(dpiMsgProps *props, const char *value,
        uint32_t valueLength)
{
    OCIRaw *rawValue = NULL;
    dpiError error;
    sword status;

    if (dpiGen__startPublicFn(props, DPI_HTYPE_MSG_PROPS, __func__,
            &error) < 0)
        return DPI_FAILURE;
    status = OCIRawAssignBytes(props->env->handle, error.handle,
            (const ub1*) value, valueLength, &rawValue);
    if (dpiError__check(&error, status, props->conn, "set raw buffer") < 0)
        return DPI_FAILURE;
    status = OCIAttrSet(props->handle, OCI_DTYPE_AQMSG_PROPERTIES,
            (dvoid*) rawValue, valueLength, OCI_ATTR_ORIGINAL_MSGID,
            error.handle);
    OCIRawResize(props->env->handle, error.handle, 0, &rawValue);
    return dpiError__check(&error, status, props->conn, "set value");
}


//-----------------------------------------------------------------------------
// dpiMsgProps_setPriority() [PUBLIC]
//   Set the priority of the message.
//-----------------------------------------------------------------------------
int dpiMsgProps_setPriority(dpiMsgProps *props, int32_t value)
{
    return dpiMsgProps__setAttrValue(props, OCI_ATTR_PRIORITY, __func__,
            &value, 0);
}

