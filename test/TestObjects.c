//-----------------------------------------------------------------------------
// Copyright (c) 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestObjects.c
//   Test suite for testing all the object related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define SQL_TEXT   "begin " \
                        "for i in 1..3 loop " \
                            "proc_TestObjAttr(udt_SubObject(i, 'test')); " \
                        "end loop; " \
                    "end;"

//-----------------------------------------------------------------------------
// dpiTest__expectErrorNotACollection()
//   Verify that an error is raised and that it states that the error is not a
// collection.
//-----------------------------------------------------------------------------
static int dpiTest__expectErrorNotACollection(dpiTestCase *testCase,
        dpiTestParams *params, const char *objName)
{
    char expectedError[512];

    snprintf(expectedError, sizeof(expectedError),
            "DPI-1023: object %.*s.%s is not a collection",
            params->mainUserNameLength, params->mainUserName, objName);
    return dpiTestCase_expectError(testCase, expectedError);
}


//-----------------------------------------------------------------------------
// dpiTest_1400_releaseObjTwice()
//   Call dpiObjectType_createObject(); call dpiObject_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1400_releaseObjTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_release(obj);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiObject handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1401_verifyPubFuncsOfObjWithNull()
//   Call each of the dpiObject public functions with the obj parameter set to
// NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1401_verifyPubFuncsOfObjWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiObject handle";
    dpiNativeTypeNum nativeTypeNum = DPI_NATIVE_TYPE_INT64;
    int32_t index, prevIndex, nextIndex, size;
    dpiConn *conn;
    dpiObject *obj;
    dpiData data;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiObject_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiData_setInt64(&data, 1);
    dpiObject_appendElement(NULL, nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_copy(NULL, &obj);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_deleteElementByIndex(NULL, 1);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getAttributeValue(NULL, NULL, nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getElementExistsByIndex(NULL, 1, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getElementValueByIndex(NULL, 1, nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getFirstIndex(NULL, &index, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getLastIndex(NULL, &index, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getNextIndex(NULL, 1, &nextIndex, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getPrevIndex(NULL, 1, &prevIndex, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_getSize(NULL, &size);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_setAttributeValue(NULL, NULL, nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_setElementValueByIndex(NULL, 1, nativeTypeNum, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObject_trim(NULL, 2);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1402_verifyAppendElementWithNormalObject()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_appendElement() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_1402_verifyAppendElementWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj, *obj2;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj2);
    dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1403_verifyDeleteElementWithNormalObject()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_deleteElementByIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_1403_verifyDeleteElementWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_deleteElementByIndex(obj, 1);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1404_verifyGetElementExistsWithNormalObject()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getElementExistsByIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_1404_verifyGetElementExistsWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getElementExistsByIndex(obj, 1, &exists);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1405_verifyGetElementValueWithNormalObject()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getElementValueByIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_1405_verifyGetElementValueWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getElementValueByIndex(obj, 1, DPI_NATIVE_TYPE_OBJECT, &data);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1406_verifyGetFirstIndexWithNormalObject()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getFirstIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_1406_verifyGetFirstIndexWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    int32_t index;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getFirstIndex(obj, &index, &exists);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1407_verifyGetLastIndexWithNormalObject()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getLastIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_1407_verifyGetLastIndexWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    int32_t index;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getLastIndex(obj, &index, &exists);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1408_verifyGetNextIndexWithNormalObject()
//   call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getNextIndex() (error DPI-1023)
//-----------------------------------------------------------------------------
int dpiTest_1408_verifyGetNextIndexWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    int32_t nextIndex;
    dpiObject *obj;
    dpiConn *conn;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getNextIndex(obj, 1, &nextIndex, &exists);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1409_verifyGetPrevIndexWithNormalObject()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getPrevIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_1409_verifyGetPrevIndexWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    int32_t prevIndex;
    dpiObject *obj;
    dpiConn *conn;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getPrevIndex(obj, 2, &prevIndex, &exists);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1410_verifySetElementValueWithNormalObject()
//   call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_setElementValueByIndex() (error DPI-1023)
//-----------------------------------------------------------------------------
int dpiTest_1410_verifySetElementValueWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj, *obj2;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj2);
    dpiObject_setElementValueByIndex(obj, 1, DPI_NATIVE_TYPE_OBJECT, &data);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1411_verifyObjectTrimWithNormalObject()
//   call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_trim() (error DPI-1023)
//-----------------------------------------------------------------------------
int dpiTest_1411_verifyObjectTrimWithNormalObject(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_trim(obj, 2);
    if (dpiTest__expectErrorNotACollection(testCase, params, objName) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1412_verifyAppendElementWithDiffElementType()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() with a value that does not match
// the expected element type (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_1412_verifyAppendElementWithDiffElementType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1014: conversion between Oracle type "
            "2023 and native type 3000 is not implemented";
    const char *objName = "UDT_OBJECTARRAY";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 1);
    dpiObject_appendElement(obj, DPI_NATIVE_TYPE_INT64, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1413_verifyAppendElementWithCollectionObj()
//   call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() with a value that matches the
// expected element type; verify that the size reported by dpiObject_getSize()
// is now 1 (no error)
//-----------------------------------------------------------------------------
int dpiTest_1413_verifyAppendElementWithCollectionObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_OBJECTARRAY";
    dpiObjectType *objType, *objType2;
    dpiObject *obj, *obj2;
    dpiConn *conn;
    dpiData data;
    int32_t size;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(obj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, size, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1414_verifyDeleteElementWithIncorrectIndex()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_deleteElementByIndex() with any
// index value (error OCI-22160).
//-----------------------------------------------------------------------------
int dpiTest_1414_verifyDeleteElementWithIncorrectIndex(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_NESTEDARRAY";
    const char *expectedError = "OCI-22160: element at index [0] does not "
            "exist";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_deleteElementByIndex(obj, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1415_verifyDeleteElementWithCollectionObj()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_deleteElementByIndex() with a valid index and confirm that the
// element has been deleted by calling dpiObject_getElementExistsByIndex() (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_1415_verifyDeleteElementWithCollectionObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    dpiObjectType *objType, *objType2;
    dpiObject *obj, *obj2;
    dpiConn *conn;
    dpiData data;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_deleteElementByIndex(obj, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getElementExistsByIndex(obj, 0, &exists) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, exists, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1416_verifyGetElementExistsWithCollectionObj()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_getElementExistsByIndex() with an index that is known to not exist
// and verify the value returned is 0; call dpiObject_getElementExistsByIndex()
// with an index that is known to exist and verify the value returned is 1 (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_1416_verifyGetElementExistsWithCollectionObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    dpiObjectType *objType, *objType2;
    dpiObject *obj, *obj2;
    dpiConn *conn;
    dpiData data;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
                                &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getElementExistsByIndex(obj, 1, &exists) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, exists, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getElementExistsByIndex(obj, 0, &exists) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, exists, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1417_verifyGetElementValueWithInvalidElement()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_getElementValueByIndex() with a native type that is incompatible
// with the element type (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_1417_verifyGetElementValueWithInvalidElement(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1014: conversion between Oracle type "
            "2023 and native type 3000 is not implemented";
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    dpiObjectType *objType, *objType2;
    dpiObject *obj, *obj2;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
                                &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getElementValueByIndex(obj, 0, DPI_NATIVE_TYPE_INT64, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1418_verifyGetElementValueWithInvalidIndex()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_getElementValueByIndex() with an index that is known to not exist
// (error DPI-1024).
//-----------------------------------------------------------------------------
int dpiTest_1418_verifyGetElementValueWithInvalidIndex(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1024: element at index 1 does not exist";
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    dpiObjectType *objType, *objType2;
    dpiObject *obj, *obj2;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
                                &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getElementValueByIndex(obj, 1, DPI_NATIVE_TYPE_OBJECT, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1419_verifySetElementValueWithInvalidElement()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_setElementValue() with a native type that is
// incompatible with the element type (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_1419_verifySetElementValueWithInvalidElement(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1014: conversion between Oracle type "
            "2023 and native type 3000 is not implemented";
    const char *objName = "UDT_NESTEDARRAY";
    dpiObjectType *objType;
    dpiConn *conn;
    dpiObject *obj;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 1);
    dpiObject_setElementValueByIndex(obj, 0, DPI_NATIVE_TYPE_INT64, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1420_verifyObjectTrimWithInvalidNum()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_trim() with any value for numToTrim (error
// OCI-22167).
//-----------------------------------------------------------------------------
int dpiTest_1420_verifyObjectTrimWithInvalidNum(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "OCI-22167: given trim size [1] must be "
            "less than or equal to [0]";
    const char *objName = "UDT_OBJECTARRAY";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_trim(obj, 1);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1421_verifyObjectTrimWithCollectionObj()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; append any number of elements to the collection; call
// dpiObject_trim() with any value less than the number of elements appended
// and verify that the size of the collection is the expected size (no error).
//-----------------------------------------------------------------------------
int dpiTest_1421_verifyObjectTrimWithCollectionObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_OBJECTARRAY";
    dpiObjectType *objType, *subObjType;
    dpiObject *obj, *subObj1, *subObj2;
    dpiConn *conn;
    dpiData data;
    int32_t size;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(subObjType, &subObj1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(subObjType, &subObj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, subObj1);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, subObj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(obj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, size, 2) < 0)
        return DPI_FAILURE;
    if (dpiObject_trim(obj, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(obj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, size, 1) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(subObj1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(subObj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1422_verifyGetAttrValueWithDiffObj()
//   Call dpiObject_getAttributeValue() with an attribute that does not belong
// to the object type of the object (error DPI-1022).
//-----------------------------------------------------------------------------
int dpiTest_1422_verifyGetAttrValueWithDiffObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_SUBOBJECT", *objName2 = "UDT_OBJECT";
    dpiObjectType *objType, *objType2;
    dpiObjectAttr *attributes[7];
    dpiObjectTypeInfo typeInfo;
    char expectedError[512];
    dpiObject *obj;
    dpiConn *conn;
    uint32_t i;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, objName2, strlen(objName2), &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType2, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType2, typeInfo.numAttributes,
            attributes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getAttributeValue(obj, attributes[0], DPI_NATIVE_TYPE_DOUBLE,
            &data);
    snprintf(expectedError, sizeof(expectedError),
            "DPI-1022: attribute NUMBERVALUE is not part of object "
            "type %.*s.%s", params->mainUserNameLength, params->mainUserName,
            objName);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < typeInfo.numAttributes; i++) {
        if (dpiObjectAttr_release(attributes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1423_verifyGetAttrValueWithInvalidNativeType()
//   Call dpiObject_getAttributeValue() with a native type that does not
// correspond to the attribute value's type (error).
//-----------------------------------------------------------------------------
int dpiTest_1423_verifyGetAttrValueWithInvalidNativeType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1014: conversion between Oracle type "
            "2010 and native type 3009 is not implemented";
    const char *objName = "UDT_SUBOBJECT";
    dpiObjectAttr *attributes[2];
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    dpiData data;
    uint32_t i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, typeInfo.numAttributes,
            attributes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 5);
    if (dpiObject_setAttributeValue(obj, attributes[0], DPI_NATIVE_TYPE_INT64,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getAttributeValue(obj, attributes[0], DPI_NATIVE_TYPE_OBJECT,
            &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < typeInfo.numAttributes; i++) {
        if (dpiObjectAttr_release(attributes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1424_verifyGetAttrValueWithAttrAsNull()
//   Call PL/SQL procedure which populates an object of a type that is not
// a collection; call dpiObject_getAttributeValue() with a NULL value for the
// attribute parameter (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1424_verifyGetAttrValueWithAttrAsNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiObjectAttr handle";
    const char *objName = "UDT_SUBOBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObject_getAttributeValue(obj, NULL, DPI_NATIVE_TYPE_OBJECT, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1425_verifySetAttrValueWithDiffObj()
//   Call dpiObject_setAttributeValue() with an attribute that does not belong
// to the object type of the object (error DPI-1022).
//-----------------------------------------------------------------------------
int dpiTest_1425_verifySetAttrValueWithDiffObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objName = "UDT_SUBOBJECT", *objName2 = "UDT_OBJECT";
    dpiObjectType *objType, *objType2;
    dpiObjectAttr *attributes[7];
    dpiObjectTypeInfo typeInfo;
    char expectedError[512];
    dpiObject *obj;
    dpiConn *conn;
    dpiData data;
    uint32_t i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, objName2, strlen(objName2), &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType2, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType2, typeInfo.numAttributes,
            attributes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    snprintf(expectedError, sizeof(expectedError),
            "DPI-1022: attribute NUMBERVALUE is not part of object "
            "type %.*s.%s", params->mainUserNameLength, params->mainUserName,
            objName);
    dpiData_setDouble(&data, 12.25);
    dpiObject_setAttributeValue(obj, attributes[0], DPI_NATIVE_TYPE_DOUBLE,
            &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < typeInfo.numAttributes; i++) {
        if (dpiObjectAttr_release(attributes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1426_verifySetAttrValueWithDiffNativeType()
//   Call dpiObject_setAttributeValue() with a native type that does not
// correspond to the attribute value's type (error).
//-----------------------------------------------------------------------------
int dpiTest_1426_verifySetAttrValueWithDiffNativeType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1014: conversion between Oracle type "
            "2010 and native type 3007 is not implemented";
    const char *objName = "UDT_SUBOBJECT";
    dpiObjectAttr *attributes[2];
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiConn *conn;
    dpiObject *obj;
    dpiData data;
    uint32_t i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, typeInfo.numAttributes,
            attributes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setIntervalYM(&data, 1, 5);
    dpiObject_setAttributeValue(obj, attributes[0],
            DPI_NATIVE_TYPE_INTERVAL_YM, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < typeInfo.numAttributes; i++) {
        if (dpiObjectAttr_release(attributes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1427_verifySetAttrValueWithAttrAsNull()
//   Call dpiObject_setAttributeValue() with a NULL value for the attribute
// parameter (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1427_verifySetAttrValueWithAttrAsNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiObjectAttr handle";
    const char *objName = "UDT_SUBOBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 1);
    dpiObject_setAttributeValue(obj, NULL, DPI_NATIVE_TYPE_INT64, &data);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1400);
    dpiTestSuite_addCase(dpiTest_1400_releaseObjTwice,
            "dpiObjectType_createObject() with dpiObject_release() twice");
    dpiTestSuite_addCase(dpiTest_1401_verifyPubFuncsOfObjWithNull,
            "call all object functions with NULL object");
    dpiTestSuite_addCase(dpiTest_1402_verifyAppendElementWithNormalObject,
            "dpiObject_appendElement() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1403_verifyDeleteElementWithNormalObject,
            "dpiObject_deleteElementByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1404_verifyGetElementExistsWithNormalObject,
            "dpiObject_getElementExistsByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1405_verifyGetElementValueWithNormalObject,
            "dpiObject_getElementValueByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1406_verifyGetFirstIndexWithNormalObject,
            "dpiObject_getFirstIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1407_verifyGetLastIndexWithNormalObject,
            "dpiObject_getLastIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1408_verifyGetNextIndexWithNormalObject,
            "dpiObject_getNextIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1409_verifyGetPrevIndexWithNormalObject,
            "dpiObject_getPrevIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1410_verifySetElementValueWithNormalObject,
            "dpiObject_setElementValueByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1411_verifyObjectTrimWithNormalObject,
            "dpiObject_trim() with non-collection object");
    dpiTestSuite_addCase(dpiTest_1412_verifyAppendElementWithDiffElementType,
            "dpiObject_appendElement() with invalid element type");
    dpiTestSuite_addCase(dpiTest_1413_verifyAppendElementWithCollectionObj,
            "dpiObject_appendElement() with valid element type");
    dpiTestSuite_addCase(dpiTest_1414_verifyDeleteElementWithIncorrectIndex,
            "dpiObject_deleteElementByIndex() with invalid index");
    dpiTestSuite_addCase(dpiTest_1415_verifyDeleteElementWithCollectionObj,
            "dpiObject_deleteElementByIndex() with valid index");
    dpiTestSuite_addCase(dpiTest_1416_verifyGetElementExistsWithCollectionObj,
            "dpiObject_getElementExistsByIndex() with valid/invalid indices");
    dpiTestSuite_addCase(dpiTest_1417_verifyGetElementValueWithInvalidElement,
            "dpiObject_getElementValueByIndex() with invalid element type");
    dpiTestSuite_addCase(dpiTest_1418_verifyGetElementValueWithInvalidIndex,
            "dpiObject_getElementValueByIndex() with invalid index");
    dpiTestSuite_addCase(dpiTest_1419_verifySetElementValueWithInvalidElement,
            "dpiObject_setElementValueByIndex() with invalid element type");
    dpiTestSuite_addCase(dpiTest_1420_verifyObjectTrimWithInvalidNum,
            "dpiObject_trim() on empty collection");
    dpiTestSuite_addCase(dpiTest_1421_verifyObjectTrimWithCollectionObj,
            "dpiObject_trim() with valid number of elements");
    dpiTestSuite_addCase(dpiTest_1422_verifyGetAttrValueWithDiffObj,
            "dpiObject_getAttributeValue() with invalid attribute");
    dpiTestSuite_addCase(dpiTest_1423_verifyGetAttrValueWithInvalidNativeType,
            "dpiObject_getAttributeValue() with invalid native type");
    dpiTestSuite_addCase(dpiTest_1424_verifyGetAttrValueWithAttrAsNull,
            "dpiObject_getAttributeValue() with NULL attribute");
    dpiTestSuite_addCase(dpiTest_1425_verifySetAttrValueWithDiffObj,
            "dpiObject_setAttributeValue() with invalid attribute");
    dpiTestSuite_addCase(dpiTest_1426_verifySetAttrValueWithDiffNativeType,
            "dpiObject_setAttributeValue() with invalid native type");
    dpiTestSuite_addCase(dpiTest_1427_verifySetAttrValueWithAttrAsNull,
            "dpiObject_appendElement() with NULL attribute");
    return dpiTestSuite_run();
}

