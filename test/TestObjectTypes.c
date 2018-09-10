//-----------------------------------------------------------------------------
// Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestObjectTypes.c
//   Test suite for testing all the object type related cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"
#define NUM_ATTRS           7

//-----------------------------------------------------------------------------
// dpiTest__verifyAttributeInfo() [INTERNAL]
//   Verifies each field of dpiObjectAttrInfo.
//-----------------------------------------------------------------------------
int dpiTest__verifyAttributeInfo(dpiTestCase *testCase,
        dpiObjectAttrInfo *attrInfo, const char *expectedName,
        dpiOracleTypeNum expectedOracleTypeNum,
        dpiNativeTypeNum expectedDefaultNativeTypeNum,
        const char *expectedObjectTypeName)
{
    dpiObjectTypeInfo typeInfo;

    if (dpiTestCase_expectStringEqual(testCase, attrInfo->name,
            attrInfo->nameLength, expectedName, strlen(expectedName)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, attrInfo->typeInfo.oracleTypeNum,
            expectedOracleTypeNum) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            attrInfo->typeInfo.defaultNativeTypeNum,
            expectedDefaultNativeTypeNum) < 0)
        return DPI_FAILURE;
    if (expectedObjectTypeName && !attrInfo->typeInfo.objectType)
        return dpiTestCase_setFailed(testCase,
                "Expected object type but got NULL");
    if (!expectedObjectTypeName && attrInfo->typeInfo.objectType)
        return dpiTestCase_setFailed(testCase,
                "Expected no object type but got one");
    if (expectedObjectTypeName && attrInfo->typeInfo.objectType) {
        if (dpiObjectType_getInfo(attrInfo->typeInfo.objectType,
                &typeInfo) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectStringEqual(testCase, typeInfo.name,
                typeInfo.nameLength, expectedObjectTypeName,
                strlen(expectedObjectTypeName)) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyObjectTypeInfo() [INTERNAL]
//   Verifies each field of dpiObjectTypeInfo.
//-----------------------------------------------------------------------------
int dpiTest__verifyObjectTypeInfo(dpiTestCase *testCase,
        dpiObjectTypeInfo *typeInfo, const char *expectedSchemaName,
        uint32_t expectedSchemaNameLength, const char * expectedName,
        int expectedIsCollection, dpiOracleTypeNum expectedOracleTypeNum,
        dpiNativeTypeNum expectedDefaultNativeTypeNum,
        dpiObjectType *expectedObjectType, uint16_t expectedNumAttrs)
{
    if (dpiTestCase_expectStringEqual(testCase, typeInfo->schema,
            typeInfo->schemaLength, expectedSchemaName,
            expectedSchemaNameLength) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, typeInfo->name,
            typeInfo->nameLength, expectedName, strlen(expectedName)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, typeInfo->isCollection,
            expectedIsCollection) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, typeInfo->numAttributes,
            expectedNumAttrs) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            typeInfo->elementTypeInfo.oracleTypeNum,
            expectedOracleTypeNum) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            typeInfo->elementTypeInfo.defaultNativeTypeNum,
            expectedDefaultNativeTypeNum) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1300_verifyGetObjTypeWithInvalidObj()
//   Call dpiConn_getObjectType() with an invalid object type name (error).
//-----------------------------------------------------------------------------
int dpiTest_1300_verifyGetObjTypeWithInvalidObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiVersionInfo *clientVersionInfo, *databaseVersionInfo;
    const char *objStr = "INVALID_OBJECT_TYPE";
    dpiObjectType *objType;
    dpiConn *conn;

    dpiTestSuite_getClientVersionInfo(&clientVersionInfo);
    if (dpiTestCase_getDatabaseVersionInfo(testCase, &databaseVersionInfo) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType);
    if (clientVersionInfo->versionNum < 12 ||
            databaseVersionInfo->versionNum < 12)
        return dpiTestCase_expectError(testCase,
                "ORA-04043: object INVALID_OBJECT_TYPE does not exist");
    return dpiTestCase_expectError(testCase,
            "OCI-22303: type \"\".\"INVALID_OBJECT_TYPE\" not found");
}


//-----------------------------------------------------------------------------
// dpiTest_1301_verifyGetObjTypeWithValidObj()
//   Call dpiConn_getObjectType() with a valid object type name (no error).
//-----------------------------------------------------------------------------
int dpiTest_1301_verifyGetObjTypeWithValidObj(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "UDT_OBJECTDATATYPES";
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1302_releaseObjTypeTwice()
//   Call dpiConn_getObjectType() with a valid object type name; call
// dpiObjectType_release() twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1302_releaseObjTypeTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "UDT_OBJECTDATATYPES";
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObjectType_release(objType);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiObjectType handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1303_verifyPubFuncsOfObjTypeWithNull()
//   Call each public function for dpiObjectType with the objType parameter
// set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1303_verifyPubFuncsOfObjTypeWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiObjectType handle";
    dpiObjectAttr *attributes[NUM_ATTRS];
    dpiObjectTypeInfo info;
    dpiObject *obj;

    dpiObjectType_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObjectType_createObject(NULL, &obj);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObjectType_getAttributes(NULL, 0, attributes);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObjectType_getInfo(NULL, &info);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiObjectType_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1304_verifyGetAttrsFunWithInvalidNumAttrs()
//   Call dpiConn_getObjectType() with any valid object type name that has
// attributes; call dpiObjectType_getAttributes() with the numAttributes
// parameter set to a value smaller than the actual number of
// parameters (error DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_1304_verifyGetAttrsFunWithInvalidNumAttrs(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiObjectAttr *attributes[NUM_ATTRS];
    const char *objStr = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiObjectType_getAttributes(objType, 5, attributes);
    if (dpiTestCase_expectError(testCase,
            "DPI-1018: array size of 5 is too small") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1305_verifyMetaDataOfAttributes()
//   Call dpiConn_getObjectType() with any valid object type name that has
// attributes; call dpiObjectType_getAttributes() and call
// dpiObjectAttr_getInfo() on each attribute returned, verifying that the
// metadata matches expectations; ensure that at least one of the attributes is
// also an object type (no error).
//-----------------------------------------------------------------------------
int dpiTest_1305_verifyMetaDataOfAttributes(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiObjectAttr *attributes[NUM_ATTRS];
    const char *objStr = "UDT_OBJECT";
    dpiObjectAttrInfo attrInfo;
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiConn *conn;
    uint32_t i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, typeInfo.numAttributes,
            attributes) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiObjectAttr_getInfo(attributes[0], &attrInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyAttributeInfo(testCase, &attrInfo, "NUMBERVALUE",
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, NULL) < 0)
        return DPI_FAILURE;
    if (dpiObjectAttr_getInfo(attributes[1], &attrInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyAttributeInfo(testCase, &attrInfo, "STRINGVALUE",
            DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, NULL) < 0)
        return DPI_FAILURE;
    if (dpiObjectAttr_getInfo(attributes[2], &attrInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyAttributeInfo(testCase, &attrInfo, "FIXEDCHARVALUE",
            DPI_ORACLE_TYPE_CHAR, DPI_NATIVE_TYPE_BYTES, NULL) < 0)
        return DPI_FAILURE;
    if (dpiObjectAttr_getInfo(attributes[3], &attrInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyAttributeInfo(testCase, &attrInfo, "DATEVALUE",
            DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP, NULL) < 0)
        return DPI_FAILURE;
    if (dpiObjectAttr_getInfo(attributes[4], &attrInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyAttributeInfo(testCase, &attrInfo, "TIMESTAMPVALUE",
            DPI_ORACLE_TYPE_TIMESTAMP, DPI_NATIVE_TYPE_TIMESTAMP, NULL) < 0)
        return DPI_FAILURE;
    if (dpiObjectAttr_getInfo(attributes[5], &attrInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyAttributeInfo(testCase, &attrInfo, "SUBOBJECTVALUE",
            DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT,
            "UDT_SUBOBJECT") < 0)
        return DPI_FAILURE;
    if (dpiObjectAttr_getInfo(attributes[6], &attrInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyAttributeInfo(testCase, &attrInfo, "SUBOBJECTARRAY",
            DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT,
            "UDT_OBJECTARRAY") < 0)
        return DPI_FAILURE;

    for (i = 0; i < NUM_ATTRS; i++) {
        if (dpiObjectAttr_release(attributes[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1306_verifyTypeInfoOfObjCollection()
//   Call dpiConn_getObjectType() with a valid object type name that refers
// to a collection containing an object; call dpiObjectType_getInfo()
// and confirm that in the dpiObjectTypeInfo structure the isCollection
// attribute is set to 1 and that the numAttributes attribute is set to 0;
// verify the remaining attributes match expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_1306_verifyTypeInfoOfObjCollection(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "UDT_OBJECTARRAY";
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyObjectTypeInfo(testCase, &typeInfo,
            params->mainUserName, params->mainUserNameLength, objStr, 1,
            DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, NULL, 0) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1307_verifyTypeInfoOfScalarCollection()
//   Call dpiConn_getObjectType() with a valid object type name that refers
// to a collection containing scalar values; call dpiObjectType_getInfo()
// and confirm that in the dpiObjectTypeInfo structure the isCollection
// attribute is set to 1 and that the numAttributes attribute is set to 0;
// verify the remaining attributes match expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_1307_verifyTypeInfoOfScalarCollection(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "UDT_ARRAY";
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyObjectTypeInfo(testCase, &typeInfo,
            params->mainUserName, params->mainUserNameLength, objStr, 1,
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, NULL, 0) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1308_verifyTypeInfoOfNonCollection()
//   Call dpiConn_getObjectType() with a valid object type name that refers to
// a type that is not a collection; call dpiObjectType_getInfo() and confirm
// that in the dpiObjectTypeInfo structure the isCollection attribute is set to
// 0; verify the remaining attributes match expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_1308_verifyTypeInfoOfNonCollection(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "UDT_OBJECT";
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyObjectTypeInfo(testCase, &typeInfo,
            params->mainUserName, params->mainUserNameLength, objStr, 0, 0, 0,
            NULL, NUM_ATTRS) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1309_verifyTypeInfoOfIndexedTable()
//   Call dpiConn_getObjectType() with a PL/SQL indexed-by-binary-integer table
// and validate the results from the call to dpiObjectType_getInfo() (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_1309_verifyTypeInfoOfIndexedTable(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "PKG_TESTNUMBERARRAYS.UDT_NUMBERLIST";
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyObjectTypeInfo(testCase, &typeInfo,
            params->mainUserName, params->mainUserNameLength, "UDT_NUMBERLIST",
            1, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, NULL, 0) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1310_verifyTypeInfoOfRecordType()
//   Call dpiConn_getObjectType() with a PL/SQL record type and validate the
// results from the call to dpiObjectType_getInfo() (no error).
//-----------------------------------------------------------------------------
int dpiTest_1310_verifyTypeInfoOfRecordType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "PKG_TESTRECORDS.UDT_RECORD";
    dpiObjectTypeInfo typeInfo;
    dpiObjectType *objType;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getInfo(objType, &typeInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyObjectTypeInfo(testCase, &typeInfo,
            params->mainUserName, params->mainUserNameLength, "UDT_RECORD", 0,
            0, 0, NULL, 5) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1300);
    dpiTestSuite_addCase(dpiTest_1300_verifyGetObjTypeWithInvalidObj,
            "dpiConn_getObjectType() with invalid name");
    dpiTestSuite_addCase(dpiTest_1301_verifyGetObjTypeWithValidObj,
            "dpiConn_getObjectType() with valid name");
    dpiTestSuite_addCase(dpiTest_1302_releaseObjTypeTwice,
            "dpiObjectType_release() called twice");
    dpiTestSuite_addCase(dpiTest_1303_verifyPubFuncsOfObjTypeWithNull,
            "call object type functions with NULL object type");
    dpiTestSuite_addCase(dpiTest_1304_verifyGetAttrsFunWithInvalidNumAttrs,
            "dpiObjectType_getAttributes() with too small numAttributes");
    dpiTestSuite_addCase(dpiTest_1305_verifyMetaDataOfAttributes,
            "dpiObjectAttr_getInfo() and verify metadata");
    dpiTestSuite_addCase(dpiTest_1306_verifyTypeInfoOfObjCollection,
            "dpiObjectType_getInfo() of collection containing an object");
    dpiTestSuite_addCase(dpiTest_1307_verifyTypeInfoOfScalarCollection,
            "dpiObjectType_getInfo() of collection containing scalar values");
    dpiTestSuite_addCase(dpiTest_1308_verifyTypeInfoOfNonCollection,
            "dpiObjectType_getInfo() of object type with attributes");
    dpiTestSuite_addCase(dpiTest_1309_verifyTypeInfoOfIndexedTable,
            "dpiObjectType_getInfo() of indexed-by binary integer table");
    dpiTestSuite_addCase(dpiTest_1310_verifyTypeInfoOfRecordType,
            "dpiObjectType_getInfo() of PL/SQL record type");
    return dpiTestSuite_run();
}

