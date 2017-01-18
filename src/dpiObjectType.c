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
// dpiObjectType.c
//   Implementation of object types.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

// forward declarations of internal functions only used in this file
static int dpiObjectType__init(dpiObjectType *objType, OCIParam *param,
        uint32_t nameAttribute, dpiError *error);


//-----------------------------------------------------------------------------
// dpiObjectType__allocate() [INTERNAL]
//   Allocate and initialize an object type structure.
//-----------------------------------------------------------------------------
int dpiObjectType__allocate(dpiConn *conn, OCIParam *param,
        uint32_t nameAttribute, dpiObjectType **objType, dpiError *error)
{
    dpiObjectType *tempObjType;

    // create structure and retain reference to connection
    *objType = NULL;
    if (dpiGen__allocate(DPI_HTYPE_OBJECT_TYPE, conn->env,
            (void**) &tempObjType, error) < 0)
        return DPI_FAILURE;
    if (dpiGen__setRefCount(conn, error, 1) < 0) {
        dpiObjectType__free(tempObjType, error);
        return DPI_FAILURE;
    }
    tempObjType->conn = conn;

    // perform initialization
    if (dpiObjectType__init(tempObjType, param, nameAttribute, error) < 0) {
        dpiObjectType__free(tempObjType, error);
        return DPI_FAILURE;
    }

    *objType = tempObjType;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiObjectType__describe() [INTERNAL]
//   Describe the object type and store information about it. Note that a
// separate call to OCIDescribeAny() is made in order to support nested types;
// an illegal attribute value is returned if this is not done.
//-----------------------------------------------------------------------------
static int dpiObjectType__describe(dpiObjectType *objType,
        OCIDescribe *describeHandle, dpiError *error)
{
    OCIParam *collectionParam, *param;
    OCITypeCode typeCode;
    sword status;

    // describe the type
    status = OCIDescribeAny(objType->conn->handle, error->handle,
            (dvoid*) objType->tdo, 0, OCI_OTYPE_PTR, OCI_DEFAULT,
            OCI_PTYPE_TYPE, describeHandle);
    if (dpiError__check(error, status, objType->conn, "describe type") < 0)
        return DPI_FAILURE;

    // get top level parameter descriptor
    status = OCIAttrGet(describeHandle, OCI_HTYPE_DESCRIBE, &param, 0,
            OCI_ATTR_PARAM, error->handle);
    if (dpiError__check(error, status, objType->conn,
            "get top level parameter") < 0)
        return DPI_FAILURE;

    // determine type code
    status = OCIAttrGet(param, OCI_DTYPE_PARAM, &typeCode, 0,
            OCI_ATTR_TYPECODE, error->handle);
    if (dpiError__check(error, status, objType->conn, "get type code") < 0)
        return DPI_FAILURE;
    objType->typeCode = typeCode;

    // determine the number of attributes
    status = OCIAttrGet(param, OCI_DTYPE_PARAM,
            (dvoid*) &objType->numAttributes, 0, OCI_ATTR_NUM_TYPE_ATTRS,
            error->handle);
    if (dpiError__check(error, status, objType->conn,
            "get number of attributes") < 0)
        return DPI_FAILURE;

    // if a collection, need to determine the element type
    if (typeCode == OCI_TYPECODE_NAMEDCOLLECTION) {
        objType->isCollection = 1;

        // acquire collection parameter descriptor
        status = OCIAttrGet(param, OCI_DTYPE_PARAM, &collectionParam, 0,
                OCI_ATTR_COLLECTION_ELEMENT, error->handle);
        if (dpiError__check(error, status, objType->conn,
                "get collection descriptor") < 0)
            return DPI_FAILURE;

        // determine type of element
        status = OCIAttrGet(collectionParam, OCI_DTYPE_PARAM, &typeCode, 0,
                OCI_ATTR_TYPECODE, error->handle);
        if (dpiError__check(error, status, objType->conn,
                "get element type code") < 0)
            return DPI_FAILURE;
        objType->elementOracleType =
                dpiOracleType__getFromObjectTypeInfo(typeCode, error);
        if (!objType->elementOracleType)
            return DPI_FAILURE;

        // if element type is an object type get its type
        if (typeCode == OCI_TYPECODE_OBJECT ||
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
                typeCode == OCI_TYPECODE_RECORD ||
#endif
                typeCode == OCI_TYPECODE_NAMEDCOLLECTION) {
            if (dpiObjectType__allocate(objType->conn,
                    collectionParam, OCI_ATTR_TYPE_NAME, &objType->elementType,
                    error) < 0)
                return DPI_FAILURE;
        }

    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiObjectType__free() [INTERNAL]
//   Free the memory for an object type.
//-----------------------------------------------------------------------------
void dpiObjectType__free(dpiObjectType *objType, dpiError *error)
{
    if (objType->conn) {
        dpiGen__setRefCount(objType->conn, error, -1);
        objType->conn = NULL;
    }
    if (objType->elementType) {
        dpiGen__setRefCount(objType->elementType, error, -1);
        objType->elementType = NULL;
    }
    if (objType->schema) {
        free((void*) objType->schema);
        objType->schema = NULL;
    }
    if (objType->name) {
        free((void*) objType->name);
        objType->name = NULL;
    }
    free(objType);
}


//-----------------------------------------------------------------------------
// dpiObjectType__init() [INTERNAL]
//   Initialize the object type.
//-----------------------------------------------------------------------------
static int dpiObjectType__init(dpiObjectType *objType, OCIParam *param,
        uint32_t nameAttribute, dpiError *error)
{
    OCIDescribe *describeHandle;
    OCIRef *tdoReference;
    sword status;

    // determine the schema of the type
    if (dpiUtils__getAttrStringWithDup(error, "get schema", param,
            OCI_DTYPE_PARAM, OCI_ATTR_SCHEMA_NAME, &objType->schema,
            &objType->schemaLength) < 0)
        return DPI_FAILURE;

    // determine the name of the type
    if (dpiUtils__getAttrStringWithDup(error, "get name", param,
            OCI_DTYPE_PARAM, nameAttribute, &objType->name,
            &objType->nameLength) < 0)
        return DPI_FAILURE;

    // retrieve TDO of the parameter and pin it in the cache
    status = OCIAttrGet(param, OCI_DTYPE_PARAM, (dvoid*) &tdoReference, 0,
            OCI_ATTR_REF_TDO, error->handle);
    if (dpiError__check(error, status, objType->conn, "get TDO reference") < 0)
        return DPI_FAILURE;
    status = OCIObjectPin(objType->env->handle, error->handle, tdoReference,
            NULL, OCI_PIN_ANY, OCI_DURATION_SESSION, OCI_LOCK_NONE,
            (dvoid**) &objType->tdo);
    if (dpiError__check(error, status, objType->conn, "pin TDO reference") < 0)
        return DPI_FAILURE;

    // acquire a describe handle
    status = OCIHandleAlloc(objType->env->handle, (dvoid**) &describeHandle,
            OCI_HTYPE_DESCRIBE, 0, 0);
    if (dpiError__check(error, status, objType->conn,
            "allocate describe handle") < 0)
        return DPI_FAILURE;

    // describe the type
    if (dpiObjectType__describe(objType, describeHandle, error) < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }

    // free the describe handle
    status = OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
    return dpiError__check(error, status, objType->conn,
            "free describe handle");
}


//-----------------------------------------------------------------------------
// dpiObjectType_addRef() [PUBLIC]
//   Add a reference to the object type.
//-----------------------------------------------------------------------------
int dpiObjectType_addRef(dpiObjectType *objType)
{
    return dpiGen__addRef(objType, DPI_HTYPE_OBJECT_TYPE, __func__);
}


//-----------------------------------------------------------------------------
// dpiObjectType_createObject() [PUBLIC]
//   Create a new object of the specified type and return it. Return NULL on
// error.
//-----------------------------------------------------------------------------
int dpiObjectType_createObject(dpiObjectType *objType, dpiObject **obj)
{
    dpiObject *tempObj;
    dpiError error;
    sword status;

    // validate object type
    if (dpiGen__startPublicFn(objType, DPI_HTYPE_OBJECT_TYPE, __func__,
            &error) < 0)
        return DPI_FAILURE;

    // validate object handle
    if (!obj)
        return dpiError__set(&error, "check object handle",
                DPI_ERR_NULL_POINTER_PARAMETER, "obj");

    // create the object
    if (dpiObject__allocate(objType, NULL, NULL, 0, &tempObj, &error) < 0)
        return DPI_FAILURE;

    // create the object instance data
    status = OCIObjectNew(objType->env->handle, error.handle,
            objType->conn->handle, objType->typeCode, objType->tdo, NULL,
            OCI_DURATION_SESSION, TRUE, &tempObj->instance);
    if (dpiError__check(&error, status, objType->conn,
            "create object instance") < 0) {
        dpiGen__setRefCount(tempObj, &error, -1);
        return DPI_FAILURE;
    }

    // get the null indicator structure
    status = OCIObjectGetInd(objType->env->handle, error.handle,
            tempObj->instance, &tempObj->indicator);
    if (dpiError__check(&error, status, objType->conn,
            "get indicator structure") < 0) {
        dpiGen__setRefCount(tempObj, &error, -1);
        return DPI_FAILURE;
    }

    *obj = tempObj;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiObjectType_getAttributes() [PUBLIC]
//   Get the attributes for the object type in the provided array.
//-----------------------------------------------------------------------------
int dpiObjectType_getAttributes(dpiObjectType *objType, uint16_t numAttributes,
        dpiObjectAttr **attributes)
{
    OCIParam *topLevelParam, *attrListParam, *attrParam;
    OCIDescribe *describeHandle;
    dpiError error;
    sword status;
    uint16_t i;

    // validate object type and the number of attributes
    if (dpiGen__startPublicFn(objType, DPI_HTYPE_OBJECT_TYPE, __func__,
            &error) < 0)
        return DPI_FAILURE;
    if (numAttributes < objType->numAttributes)
        return dpiError__set(&error, "get attributes",
                DPI_ERR_ARRAY_SIZE_TOO_SMALL, numAttributes);
    if (numAttributes == 0)
        return DPI_SUCCESS;
    if (!attributes)
        return dpiError__set(&error, "check attributes array",
                DPI_ERR_NULL_POINTER_PARAMETER, "attributes");

    // acquire a describe handle
    status = OCIHandleAlloc(objType->env->handle, (dvoid**) &describeHandle,
            OCI_HTYPE_DESCRIBE, 0, 0);
    if (dpiError__check(&error, status, objType->conn,
            "allocate describe handle") < 0)
        return DPI_FAILURE;

    // describe the type
    status = OCIDescribeAny(objType->conn->handle, error.handle,
            (dvoid*) objType->tdo, 0, OCI_OTYPE_PTR, OCI_DEFAULT,
            OCI_PTYPE_TYPE, describeHandle);
    if (dpiError__check(&error, status, objType->conn, "describe type") < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }

    // get the top level parameter descriptor
    status = OCIAttrGet(describeHandle, OCI_HTYPE_DESCRIBE, &topLevelParam, 0,
            OCI_ATTR_PARAM, error.handle);
    if (dpiError__check(&error, status, objType->conn,
            "get top level param") < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }

    // get the attribute list parameter descriptor
    status = OCIAttrGet(topLevelParam, OCI_DTYPE_PARAM,
            (dvoid*) &attrListParam, 0, OCI_ATTR_LIST_TYPE_ATTRS,
            error.handle);
    if (dpiError__check(&error, status, objType->conn,
            "get attribute list param") < 0) {
        OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
        return DPI_FAILURE;
    }

    // create attribute structure for each attribute
    for (i = 0; i < objType->numAttributes; i++) {
        status = OCIParamGet(attrListParam, OCI_DTYPE_PARAM, error.handle,
                (dvoid**) &attrParam, (uint32_t) i + 1);
        if (dpiError__check(&error, status, objType->conn,
                "get attribute param") < 0) {
            OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
            return DPI_FAILURE;
        }
        if (dpiObjectAttr__allocate(objType, attrParam, &attributes[i],
                &error) < 0) {
            OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
            return DPI_FAILURE;
        }
    }

    // free the describe handle
    status = OCIHandleFree(describeHandle, OCI_HTYPE_DESCRIBE);
    if (dpiError__check(&error, status, objType->conn,
            "free describe handle") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiObjectType_getInfo() [PUBLIC]
//   Return information about the object type.
//-----------------------------------------------------------------------------
int dpiObjectType_getInfo(dpiObjectType *objType, dpiObjectTypeInfo *info)
{
    dpiError error;

    if (dpiGen__startPublicFn(objType, DPI_HTYPE_OBJECT_TYPE, __func__,
            &error) < 0)
        return DPI_FAILURE;
    info->name = objType->name;
    info->nameLength = objType->nameLength;
    info->schema = objType->schema;
    info->schemaLength = objType->schemaLength;
    info->isCollection = objType->isCollection;
    info->elementObjectType = objType->elementType;
    if (objType->elementOracleType) {
        info->elementOracleTypeNum = objType->elementOracleType->oracleTypeNum;
        info->elementDefaultNativeTypeNum =
                objType->elementOracleType->defaultNativeTypeNum;
    } else {
        info->elementOracleTypeNum = 0;
        info->elementDefaultNativeTypeNum = 0;
    }
    info->numAttributes = objType->numAttributes;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiObjectType_release() [PUBLIC]
//   Release a reference to the object type.
//-----------------------------------------------------------------------------
int dpiObjectType_release(dpiObjectType *objType)
{
    return dpiGen__release(objType, DPI_HTYPE_OBJECT_TYPE, __func__);
}

