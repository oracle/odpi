//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2025, Oracle and/or its affiliates.
//
// This software is dual-licensed to you under the Universal Permissive License
// (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
// 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
// either license.
//
// If you elect to accept the software under the Apache License, Version 2.0,
// the following applies:
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// test_2300_objects.c
//   Test suite for testing all the object related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define TYPE_NAME "PKG_TESTSTRINGARRAYS.UDT_STRINGLIST"
#define SQL_TEXT  "begin pkg_TestStringArrays.TestIndexBy(:1); end;"

//-----------------------------------------------------------------------------
// dpiTest__createNestedData() [INTERNAL]
//   Function to create nested data.
//-----------------------------------------------------------------------------
int dpiTest__createNestedData(dpiTestCase *testCase, dpiConn *conn,
        dpiObjectType *objType, dpiObjectType *ntType,
        dpiObjectType *ntOfNtType, dpiObject **outObj, uint32_t rowNum,
        dpiObjectAttr **attrs)
{
    dpiObject *outerList, *innerList, *subObj;
    dpiData tempData, objData;
    char name[60];
    int i, j;

    // create outer list object
    if (dpiObjectType_createObject(ntOfNtType, &outerList) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate outer list object with elements
    for (i = 0; i < 2; i++) {
        if (i == 1) {
            dpiData_setNull(&objData);
            innerList = NULL;
        } else {

            // create list of sub-objects
            if (dpiObjectType_createObject(ntType, &innerList) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            for (j = 0; j < 2; j++) {

                // create sub-object
                if (dpiObjectType_createObject(objType, &subObj) < 0)
                    return dpiTestCase_setFailedFromError(testCase);

                // set sub-object attribute for SubNumberValue
                dpiData_setInt64(&tempData, rowNum * 100 + i * 10 + j);
                if (dpiObject_setAttributeValue(subObj, attrs[0],
                       DPI_NATIVE_TYPE_INT64, &tempData) < 0)
                   return dpiTestCase_setFailedFromError(testCase);

                // set sub-object attribute for SubStringValue
                snprintf(name, sizeof(name), "Member%d%d", i + 1, j + 1);
                dpiData_setBytes(&tempData, name, strlen(name));
                if (dpiObject_setAttributeValue(subObj, attrs[1],
                       DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
                   return dpiTestCase_setFailedFromError(testCase);

                // append element
                dpiData_setObject(&tempData, subObj);
                if (dpiObject_appendElement(innerList, DPI_NATIVE_TYPE_OBJECT,
                        &tempData) < 0)
                   return dpiTestCase_setFailedFromError(testCase);
                if (dpiObject_release(subObj) < 0)
                    return dpiTestCase_setFailedFromError(testCase);

            }
            dpiData_setObject(&objData, innerList);
        }

        // append element
        if (dpiObject_appendElement(outerList, DPI_NATIVE_TYPE_OBJECT,
                &objData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (innerList && dpiObject_release(innerList) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    *outObj = outerList;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__fetchRows() [INTERNAL]
//   Function to fetch rows from nested table.
//-----------------------------------------------------------------------------
int dpiTest__fetchRows(dpiTestCase *testCase, dpiConn *conn,
             dpiObjectType *objType, dpiObjectType *ntType,
             dpiObjectType *ntOfNtType, dpiObjectAttr **attrs)
{
    const char *sql =
        "select IntCol, StringCol, NestedCol "
        "from TestNestedCollections order by IntCol";
    dpiObject *outerNested, *innerNested, *subObj;
    uint32_t bufferRowIndex, rowNum, numCols;
    int32_t i, j, outerCount, innerCount;
    dpiNativeTypeNum nativeTypeNum;
    dpiData *fetchedData;
    dpiData tempData;
    dpiBytes *bytes;
    dpiStmt *stmt;
    char name[60];
    int found;

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numCols) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_defineValue(stmt, 1, DPI_ORACLE_TYPE_NUMBER,
            DPI_NATIVE_TYPE_INT64, 0, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch all rows
    rowNum = 0;
    while (1) {

        // get next row from the result set
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!found)
            break;

        // validate value of IntCol
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &fetchedData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(fetchedData),
                rowNum + 1) < 0)
            return DPI_FAILURE;

        // validate value of StringCol
        if (dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum, &fetchedData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        snprintf(name, sizeof(name), "Team %d", rowNum + 1);
        bytes = dpiData_getBytes(fetchedData);
        if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                name, strlen(name)) < 0)
            return DPI_FAILURE;

        // validate value of NestedCol
        if (dpiStmt_getQueryValue(stmt, 3, &nativeTypeNum, &fetchedData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        outerNested = dpiData_getObject(fetchedData);
        if (dpiObject_getSize(outerNested, &outerCount) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        for (i = 0; i < outerCount; i++) {

            // check element of outer nested table
            if (dpiObject_getElementValueByIndex(outerNested, i,
                    DPI_NATIVE_TYPE_OBJECT, &tempData) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            if (tempData.isNull)
                continue;

            // check elements of inner nested table
            innerNested = dpiData_getObject(&tempData);
            if (dpiObject_getSize(innerNested, &innerCount) < 0)
                return dpiTestCase_setFailedFromError(testCase);
            for (j = 0; j < innerCount; j++) {

                // get element of inner nested table
                if (dpiObject_getElementValueByIndex(innerNested, j,
                        DPI_NATIVE_TYPE_OBJECT, &tempData) < 0)
                    return dpiTestCase_setFailedFromError(testCase);
                subObj = dpiData_getObject(&tempData);

                // check NumberValue of sub-object
                if (dpiObject_getAttributeValue(subObj, attrs[0],
                        DPI_NATIVE_TYPE_INT64, &tempData) < 0)
                    return dpiTestCase_setFailedFromError(testCase);
                if (dpiTestCase_expectIntEqual(testCase,
                        dpiData_getInt64(&tempData),
                        rowNum * 100 + i * 10 + j) < 0)
                    return DPI_FAILURE;

                // check StringValue of sub-object
                if (dpiObject_getAttributeValue(subObj, attrs[1],
                        DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
                    return dpiTestCase_setFailedFromError(testCase);
                bytes = dpiData_getBytes(&tempData);
                snprintf(name, sizeof(name), "Member%d%d", i + 1, j + 1);
                if (dpiTestCase_expectStringEqual(testCase, bytes->ptr,
                        bytes->length, name, strlen(name)) < 0)
                    return DPI_FAILURE;

                // cleanup
                if (dpiObject_release(subObj) < 0)
                    return dpiTestCase_setFailedFromError(testCase);

            }

            // cleanup
            if (dpiObject_release(innerNested) < 0)
                return dpiTestCase_setFailedFromError(testCase);

        }

        // cleanup
        if (dpiObject_release(outerNested) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        rowNum++;

    }

    // final cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__insertNtRows() [INTERNAL]
//   Function to insert rows into the nested table.
//-----------------------------------------------------------------------------
int dpiTest__insertNtRows(dpiTestCase *testCase, dpiConn *conn,
        dpiObjectType *objType, dpiObjectType *ntType,
        dpiObjectType *ntOfNtType, dpiObjectAttr **attrs, uint32_t numRows)
{
    const char *insertSql =
        "insert into TestNestedCollections (IntCol, StringCol, NestedCol) "
        "values (:1, :2, :3)";
    dpiData intData, strData, objData;
    dpiObject *nested;
    char label[60];
    dpiStmt *stmt;
    uint32_t i;

    for (i = 0; i < numRows; i++) {
        snprintf(label, sizeof(label), "Team %d", i + 1);
        if (dpiTest__createNestedData(testCase, conn, objType, ntType,
                ntOfNtType, &nested, i, attrs) < 0)
            return DPI_FAILURE;
        dpiData_setInt64(&intData, i + 1);
        dpiData_setBytes(&strData, label, strlen(label));
        dpiData_setObject(&objData, nested);
        if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
                &stmt) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64,
                &intData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES,
                &strData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_bindValueByPos(stmt, 3, DPI_NATIVE_TYPE_OBJECT,
                &objData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_release(stmt) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiObject_release(nested) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2300()
//   Call dpiObjectType_createObject(); call dpiObject_release() twice (error
// DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2300(dpiTestCase *testCase, dpiTestParams *params)
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
    return dpiTestCase_expectError(testCase, "DPI-1002:");
}


//-----------------------------------------------------------------------------
// dpiTest_2301()
//   Call each of the dpiObject public functions with the obj parameter set to
// NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2301(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";
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
// dpiTest_2302()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_appendElement() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_2302(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
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
// dpiTest_2303()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_deleteElementByIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_2303(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2304()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getElementExistsByIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_2304(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2305()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getElementValueByIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_2305(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2306()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getFirstIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_2306(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2307()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getLastIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_2307(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2308()
//   call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getNextIndex() (error DPI-1023)
//-----------------------------------------------------------------------------
int dpiTest_2308(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2309()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_getPrevIndex() (error DPI-1023).
//-----------------------------------------------------------------------------
int dpiTest_2309(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2310()
//   call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_setElementValueByIndex() (error DPI-1023)
//-----------------------------------------------------------------------------
int dpiTest_2310(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
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
// dpiTest_2311()
//   call dpiObjectType_createObject() with an object type that is not a
// collection; call dpiObject_trim() (error DPI-1023)
//-----------------------------------------------------------------------------
int dpiTest_2311(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase, "DPI-1023:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2312()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() with a value that does not match
// the expected element type (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_2312(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1014:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2313()
//   call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() with a value that matches the
// expected element type; verify that the size reported by dpiObject_getSize()
// is now 1 (no error)
//-----------------------------------------------------------------------------
int dpiTest_2313(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_2314()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_deleteElementByIndex() with any
// index value (error OCI-22160).
//-----------------------------------------------------------------------------
int dpiTest_2314(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *objName = "UDT_NESTEDARRAY";
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
    if (dpiTestCase_expectError(testCase, "OCI-22160:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2315()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_deleteElementByIndex() with a valid index and confirm that the
// element has been deleted by calling dpiObject_getElementExistsByIndex() (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_2315(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_2316()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_getElementExistsByIndex() with an index that is known to not exist
// and verify the value returned is 0; call dpiObject_getElementExistsByIndex()
// with an index that is known to exist and verify the value returned is 1 (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_2316(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_2317()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_getElementValueByIndex() with a native type that is incompatible
// with the element type (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_2317(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1014:") < 0)
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
// dpiTest_2318()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_getElementValueByIndex() with an index that is known to not exist
// (error DPI-1024).
//-----------------------------------------------------------------------------
int dpiTest_2318(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1024:") < 0)
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
// dpiTest_2319()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_setElementValue() with a native type that is
// incompatible with the element type (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_2319(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1014:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2320()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_trim() with any value for numToTrim (error
// OCI-22167).
//-----------------------------------------------------------------------------
int dpiTest_2320(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "OCI-22167:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2321()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; append any number of elements to the collection; call
// dpiObject_trim() with any value less than the number of elements appended
// and verify that the size of the collection is the expected size (no error).
//-----------------------------------------------------------------------------
int dpiTest_2321(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_2322()
//   Call dpiObject_getAttributeValue() with an attribute that does not belong
// to the object type of the object (error DPI-1022).
//-----------------------------------------------------------------------------
int dpiTest_2322(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *objName = "UDT_SUBOBJECT", *objName2 = "UDT_OBJECT";
    dpiObjectType *objType, *objType2;
    dpiObjectAttr *attributes[7];
    dpiObjectTypeInfo typeInfo;
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
    if (dpiTestCase_expectError(testCase, "DPI-1022:") < 0)
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
// dpiTest_2323()
//   Call dpiObject_getAttributeValue() with a native type that does not
// correspond to the attribute value's type (error).
//-----------------------------------------------------------------------------
int dpiTest_2323(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1014:") < 0)
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
// dpiTest_2324()
//   Call PL/SQL procedure which populates an object of a type that is not
// a collection; call dpiObject_getAttributeValue() with a NULL value for the
// attribute parameter (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2324(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2325()
//   Call dpiObject_setAttributeValue() with an attribute that does not belong
// to the object type of the object (error DPI-1022).
//-----------------------------------------------------------------------------
int dpiTest_2325(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *objName = "UDT_SUBOBJECT", *objName2 = "UDT_OBJECT";
    dpiObjectType *objType, *objType2;
    dpiObjectAttr *attributes[7];
    dpiObjectTypeInfo typeInfo;
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
    dpiData_setDouble(&data, 12.25);
    dpiObject_setAttributeValue(obj, attributes[0], DPI_NATIVE_TYPE_DOUBLE,
            &data);
    if (dpiTestCase_expectError(testCase, "DPI-1022:") < 0)
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
// dpiTest_2326()
//   Call dpiObject_setAttributeValue() with a native type that does not
// correspond to the attribute value's type (error).
//-----------------------------------------------------------------------------
int dpiTest_2326(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1014:") < 0)
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
// dpiTest_2327()
//   Call dpiObject_setAttributeValue() with a NULL value for the attribute
// parameter (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2327(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2328()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() at least once; call
// dpiObject_getElementValueByIndex() with an index that is known to exist
// and verify the value returned is the expected value (no error).
//-----------------------------------------------------------------------------
int dpiTest_2328(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    dpiData outData, attrValue, getValue;
    dpiObjectType *objType, *objType2;
    double testDouble = 1234.567999;
    char *testStr = "Test String";
    dpiObjectAttr *attrs[2];
    dpiObject *obj, *obj2;
    uint32_t numAttr = 2;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType2, numAttr, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&attrValue, testDouble);
    if (dpiObject_setAttributeValue(obj2, attrs[0], DPI_NATIVE_TYPE_DOUBLE,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&attrValue, testStr, strlen(testStr));
    if (dpiObject_setAttributeValue(obj2, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&attrValue, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getElementValueByIndex(obj, 0, DPI_NATIVE_TYPE_OBJECT,
            &outData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getAttributeValue(outData.value.asObject, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, getValue.value.asDouble,
            testDouble) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(outData.value.asObject, attrs[1],
            DPI_NATIVE_TYPE_BYTES, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue.value.asBytes.ptr,
            getValue.value.asBytes.length, testStr, strlen(testStr)) < 0)
        return DPI_FAILURE;
    if (dpiObject_release(outData.value.asObject) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttr; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2329()
//   Call dpiObject_getFirstIndex() and dpiObject_getLastIndex() and verify the
// values returned are the expected values (no error).
//-----------------------------------------------------------------------------
int dpiTest_2329(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *stringData[2] = { "First element", "Fourth element" };
    uint32_t numQueryColumns;
    dpiObjectType *objType;
    int32_t elementIndex;
    dpiData *objectValue;
    dpiData elementValue;
    dpiVar *objectVar;
    dpiStmt *stmt;
    dpiConn *conn;
    int exists;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, TYPE_NAME, strlen(TYPE_NAME),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getFirstIndex(objectValue->value.asObject, &elementIndex,
            &exists) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!exists)
        return dpiTestCase_setFailed(testCase, "No elements exist!");
    if (dpiObject_getElementValueByIndex(objectValue->value.asObject,
            elementIndex, DPI_NATIVE_TYPE_BYTES, &elementValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, elementValue.value.asBytes.ptr,
            elementValue.value.asBytes.length, stringData[0],
            strlen(stringData[0])) < 0)
        return DPI_FAILURE;
    if (dpiObject_getLastIndex(objectValue->value.asObject,
            &elementIndex, &exists) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!exists)
        return dpiTestCase_setFailed(testCase, "No elements exist!");
    if (dpiObject_getElementValueByIndex(objectValue->value.asObject,
            elementIndex, DPI_NATIVE_TYPE_BYTES, &elementValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, elementValue.value.asBytes.ptr,
            elementValue.value.asBytes.length, stringData[1],
            strlen(stringData[1])) < 0)
        return DPI_FAILURE;
    if (dpiVar_release(objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2330()
//   Call dpiObject_getFirstIndex() followed by dpiObject_getNextIndex()
// repeatedly to iterate through the collection and verify the indices
// returned match the expected values (no error).
//-----------------------------------------------------------------------------
int dpiTest_2330(dpiTestCase *testCase, dpiTestParams *params)
{
    int32_t indexes[4] = { -1048576, -576, 284, 8388608 };
    uint32_t numQueryColumns, numElements = 4;
    int32_t elementIndex, nextElementIndex;
    dpiObjectType *objType;
    dpiData *objectValue;
    dpiVar *objectVar;
    dpiStmt *stmt;
    dpiConn *conn;
    int exists, i;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, TYPE_NAME, strlen(TYPE_NAME),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getFirstIndex(objectValue->value.asObject, &elementIndex,
            &exists) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!exists)
        return dpiTestCase_setFailed(testCase, "No elements exist!");
    for (i = 0; i < numElements; i++) {
        if (!exists)
            return dpiTestCase_setFailed(testCase,
                    "Missing expected element!");
        if (dpiTestCase_expectIntEqual(testCase, elementIndex, indexes[i]) < 0)
            return DPI_FAILURE;
        if (dpiObject_getNextIndex(objectValue->value.asObject, elementIndex,
                &nextElementIndex, &exists) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        elementIndex = nextElementIndex;
    }
    if (exists)
        return dpiTestCase_setFailed(testCase, "Unexpected element found!");
    if (dpiVar_release(objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2331()
//   Call dpiObject_getLastIndex() followed by dpiObject_getPrevIndex()
// repeatedly to iterate through the collection and verify the indices returned
// match the expected values (no error).
//-----------------------------------------------------------------------------
int dpiTest_2331(dpiTestCase *testCase, dpiTestParams *params)
{
    int32_t indexes[4] = { 8388608, 284, -576, -1048576 };
    uint32_t numQueryColumns, numElements = 4;
    int32_t elementIndex, prevElementIndex;
    dpiObjectType *objType;
    dpiData *objectValue;
    dpiVar *objectVar;
    dpiStmt *stmt;
    dpiConn *conn;
    int exists, i;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, TYPE_NAME, strlen(TYPE_NAME),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getLastIndex(objectValue->value.asObject,
            &elementIndex, &exists) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numElements; i++) {
        if (!exists)
            return dpiTestCase_setFailed(testCase,
                    "Missing expected element!");
        if (dpiTestCase_expectIntEqual(testCase, elementIndex, indexes[i]) < 0)
            return DPI_FAILURE;
        if (dpiObject_getPrevIndex(objectValue->value.asObject, elementIndex,
                &prevElementIndex, &exists) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        elementIndex = prevElementIndex;
    }
    if (exists)
        return dpiTestCase_setFailed(testCase, "Unexpected element found!");
    if (dpiVar_release(objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2332()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_setElementValueByIndex() with a native type that
// is compatible with the element type; verify that the element was set
// properly by calling dpiObject_getElementValueByIndex() and verifying the
// values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_2332(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    dpiData outData, attrValue, getValue;
    dpiObjectType *objType, *objType2;
    double testDouble = 1234.5679999;
    char *testStr = "Test String";
    dpiObjectAttr *attrs[2];
    dpiObject *obj, *obj2;
    uint32_t numAttr = 2;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType2, numAttr, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&attrValue, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&attrValue, testDouble);
    if (dpiObject_setAttributeValue(obj2, attrs[0], DPI_NATIVE_TYPE_DOUBLE,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&attrValue, testStr, strlen(testStr));
    if (dpiObject_setAttributeValue(obj2, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&attrValue, obj2);
    if (dpiObject_setElementValueByIndex(obj, 0, DPI_NATIVE_TYPE_OBJECT,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getElementValueByIndex(obj, 0,
            DPI_NATIVE_TYPE_OBJECT, &outData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getAttributeValue(outData.value.asObject, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, getValue.value.asDouble,
            testDouble) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(outData.value.asObject, attrs[1],
            DPI_NATIVE_TYPE_BYTES, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue.value.asBytes.ptr,
            getValue.value.asBytes.length, testStr, strlen(testStr)) < 0)
        return DPI_FAILURE;
    if (dpiObject_release(outData.value.asObject) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttr; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2333()
//   Call dpiObject_copy() and verify that the copy is a true copy of the
// original object; append or delete an element of one of the objects and
// verify that only the one object has been changed (no error).
//-----------------------------------------------------------------------------
int dpiTest_2333(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    double testDouble = 1234.5679999;
    char *testStr = "Test String";
    dpiData outData, attrValue, getValue;
    dpiObjectType *objType, *objType2;
    dpiObject *obj, *obj2, *copiedObj;
    dpiObjectAttr *attrs[2];
    uint32_t numAttr = 2;
    dpiConn *conn;
    int32_t size;
    int i;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType2, numAttr, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&attrValue, testDouble);
    if (dpiObject_setAttributeValue(obj2, attrs[0], DPI_NATIVE_TYPE_DOUBLE,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&attrValue, testStr, strlen(testStr));
    if (dpiObject_setAttributeValue(obj2, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&attrValue, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_copy(obj, &copiedObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getElementValueByIndex(copiedObj, 0,
            DPI_NATIVE_TYPE_OBJECT, &outData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getAttributeValue(outData.value.asObject, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, getValue.value.asDouble,
            testDouble) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(outData.value.asObject, attrs[1],
            DPI_NATIVE_TYPE_BYTES, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue.value.asBytes.ptr,
            getValue.value.asBytes.length, testStr, strlen(testStr)) < 0)
        return DPI_FAILURE;
    if (dpiObject_release(outData.value.asObject) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&attrValue, obj2);
    if (dpiObject_appendElement(copiedObj, DPI_NATIVE_TYPE_OBJECT,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(obj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, size, 1) < 0)
        return DPI_FAILURE;
    if (dpiObject_getSize(copiedObj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, size, 2) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(copiedObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttr; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2334()
//   Call PL/SQL procedure which populates an object of a type that is not a
// collection; call dpiObject_copy() and verify that the copy is a true copy of
// the original object; set one or more of the attributes of one of the objects
// and verify that only the one object has been changed (no error).
//-----------------------------------------------------------------------------
int dpiTest_2334(dpiTestCase *testCase, dpiTestParams *params)
{
    double testDouble1 = 1234.5679999, testDouble2 = 5678999.1234;
    const char *objName = "UDT_SUBOBJECT";
    char *testStr = "Test String";
    dpiData attrValue, getValue;
    dpiObject *obj, *copiedObj;
    dpiObjectType *objType;
    dpiObjectAttr *attrs[2];
    uint32_t numAttr = 2;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, numAttr, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&attrValue, testDouble1);
    if (dpiObject_setAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_DOUBLE,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&attrValue, testStr, strlen(testStr));
    if (dpiObject_setAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_copy(obj, &copiedObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getAttributeValue(copiedObj, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, getValue.value.asDouble,
            testDouble1) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(copiedObj, attrs[1],
            DPI_NATIVE_TYPE_BYTES, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue.value.asBytes.ptr,
            getValue.value.asBytes.length, testStr, strlen(testStr)) < 0)
        return DPI_FAILURE;
    dpiData_setDouble(&attrValue, testDouble2);
    if (dpiObject_setAttributeValue(copiedObj, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getAttributeValue(copiedObj, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, getValue.value.asDouble,
            testDouble2) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, getValue.value.asDouble,
            testDouble1) < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(copiedObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttr; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2335()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_appendElement() with an object that does not
// match the expected type (error DPI-1056).
//-----------------------------------------------------------------------------
int dpiTest_2335(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *collectionObjTypeName = "UDT_OBJECTARRAY";
    dpiObjectType *collectionObjType, *elementObjType;
    const char *elementObjTypeName = "UDT_OBJECT";
    dpiObject *collectionObj, *elementObj;
    dpiConn *conn;
    dpiData data;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, collectionObjTypeName,
            strlen(collectionObjTypeName), &collectionObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, elementObjTypeName,
            strlen(elementObjTypeName), &elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(collectionObjType, &collectionObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(elementObjType, &elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, elementObj);
    dpiObject_appendElement(collectionObj, DPI_NATIVE_TYPE_OBJECT, &data);
    if (dpiTestCase_expectError(testCase, "DPI-1056:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(collectionObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(collectionObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2336()
//   Call dpiObjectType_createObject() with an object type that is not a
// collection; set the attribute of an object to an object of the wrong type
// (error DPI-1056).
//-----------------------------------------------------------------------------
int dpiTest_2336(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *valueObjTypeName = "UDT_SUBOBJECT";
    const char *objTypeName = "UDT_OBJECT";
    dpiObjectType *objType, *valueObjType;
    dpiObject *obj, *valueObj;
    dpiObjectAttr *attrs[7];
    uint32_t numAttrs = 7;
    dpiConn *conn;
    dpiData data;
    int i;

    // get object types and attributes
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objTypeName, strlen(objTypeName),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, valueObjTypeName, strlen(valueObjTypeName),
            &valueObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create object and attempt to set attribute
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(valueObjType, &valueObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, valueObj);
    dpiObject_setAttributeValue(obj, attrs[6], DPI_NATIVE_TYPE_OBJECT, &data);
    if (dpiTestCase_expectError(testCase, "DPI-1056:") < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(valueObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(valueObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2337()
//   Create a variable with one object type and attempt to set its value with
// an object of another type (error DPI-1056).
//-----------------------------------------------------------------------------
int dpiTest_2337(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedObjTypeName = "UDT_SUBOBJECT";
    const char *varObjTypeName = "UDT_OBJECT";
    dpiObjectType *objType, *varObjType;
    dpiObject *obj;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, varObjTypeName, strlen(varObjTypeName),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, expectedObjTypeName,
            strlen(expectedObjTypeName), &varObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, varObjType, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromObject(var, 0, obj);
    if (dpiTestCase_expectError(testCase, "DPI-1056:") < 0)
        return DPI_FAILURE;
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(varObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2338()
//   Call dpiObjectType_createObject() with an object type that is a
// collection; call dpiObject_setElementValueByIndex() with a native type that
// is incompatible with the element type (error).
//-----------------------------------------------------------------------------
int dpiTest_2338(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *subObjName = "UDT_SUBOBJECT";
    const char *objName = "UDT_NESTEDARRAY";
    dpiObjectType *objType, *objType2;
    double testDouble = 1234.5679999;
    char *testStr = "Test String";
    dpiObjectAttr *attrs[2];
    dpiObject *obj, *obj2;
    uint32_t numAttr = 2;
    dpiData attrValue;
    dpiConn *conn;
    int i;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType2, numAttr, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType2, &obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&attrValue, obj2);
    if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_OBJECT, &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&attrValue, testDouble);
    if (dpiObject_setAttributeValue(obj2, attrs[0], DPI_NATIVE_TYPE_DOUBLE,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&attrValue, testStr, strlen(testStr));
    if (dpiObject_setAttributeValue(obj2, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &attrValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&attrValue, obj2);
    dpiObject_setElementValueByIndex(obj, 0, DPI_NATIVE_TYPE_INT64,
            &attrValue);
    if (dpiTestCase_expectError(testCase, "DPI-1014:") < 0)
        return DPI_FAILURE;

    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttr; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2339()
//   Call dpiConn_getObjectType() with objName as NULL (error DPI-1046). call
// dpiObjectType_createObject() with an object type that is NULL
// (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_2339(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiObjectType *objType = NULL;
    const char *objName = NULL;
    dpiObject *obj;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_getObjectType(conn, objName, 0, &objType);
    if (dpiTestCase_expectError(testCase, "DPI-1046:") < 0)
        return DPI_FAILURE;
    dpiObjectType_createObject(objType, &obj);
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2340()
//  Test inserts, fetches, deletes works as expected with nested collections.
//-----------------------------------------------------------------------------
int dpiTest_2340(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiObjectType *objType, *ntType, *ntOfNtType;
    uint32_t numRows = 100, numAttr = 2;
    dpiObjectAttr *attrs[2];
    dpiConn *conn;
    int i;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object types
    if (dpiConn_getObjectType(conn, "UDT_SUBOBJECT", strlen("UDT_SUBOBJECT"),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, "UDT_NESTTAB", strlen("UDT_NESTTAB"),
            &ntType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, "UDT_NESTTABOFNESTTAB",
            strlen("UDT_NESTTABOFNESTTAB"), &ntOfNtType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, numAttr, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert rows into nested table
    if (dpiTest__insertNtRows(testCase, conn, objType, ntType, ntOfNtType,
            attrs, numRows) < 0)
        return DPI_FAILURE;

    // fetch rows from the nested table
    if (dpiTest__fetchRows(testCase, conn, objType, ntType, ntOfNtType,
            attrs) < 0)
        return DPI_FAILURE;

    // cleanup
    for (i = 0; i < numAttr; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(ntType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(ntOfNtType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2300);
    dpiTestSuite_addCase(dpiTest_2300,
            "dpiObjectType_createObject() with dpiObject_release() twice");
    dpiTestSuite_addCase(dpiTest_2301,
            "call all object functions with NULL object");
    dpiTestSuite_addCase(dpiTest_2302,
            "dpiObject_appendElement() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2303,
            "dpiObject_deleteElementByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2304,
            "dpiObject_getElementExistsByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2305,
            "dpiObject_getElementValueByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2306,
            "dpiObject_getFirstIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2307,
            "dpiObject_getLastIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2308,
            "dpiObject_getNextIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2309,
            "dpiObject_getPrevIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2310,
            "dpiObject_setElementValueByIndex() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2311,
            "dpiObject_trim() with non-collection object");
    dpiTestSuite_addCase(dpiTest_2312,
            "dpiObject_appendElement() with invalid element type");
    dpiTestSuite_addCase(dpiTest_2313,
            "dpiObject_appendElement() with valid element type");
    dpiTestSuite_addCase(dpiTest_2314,
            "dpiObject_deleteElementByIndex() with invalid index");
    dpiTestSuite_addCase(dpiTest_2315,
            "dpiObject_deleteElementByIndex() with valid index");
    dpiTestSuite_addCase(dpiTest_2316,
            "dpiObject_getElementExistsByIndex() with valid/invalid indices");
    dpiTestSuite_addCase(dpiTest_2317,
            "dpiObject_getElementValueByIndex() with invalid element type");
    dpiTestSuite_addCase(dpiTest_2318,
            "dpiObject_getElementValueByIndex() with invalid index");
    dpiTestSuite_addCase(dpiTest_2319,
            "dpiObject_setElementValueByIndex() with invalid element type");
    dpiTestSuite_addCase(dpiTest_2320,
            "dpiObject_trim() on empty collection");
    dpiTestSuite_addCase(dpiTest_2321,
            "dpiObject_trim() with valid number of elements");
    dpiTestSuite_addCase(dpiTest_2322,
            "dpiObject_getAttributeValue() with invalid attribute");
    dpiTestSuite_addCase(dpiTest_2323,
            "dpiObject_getAttributeValue() with invalid native type");
    dpiTestSuite_addCase(dpiTest_2324,
            "dpiObject_getAttributeValue() with NULL attribute");
    dpiTestSuite_addCase(dpiTest_2325,
            "dpiObject_setAttributeValue() with invalid attribute");
    dpiTestSuite_addCase(dpiTest_2326,
            "dpiObject_setAttributeValue() with invalid native type");
    dpiTestSuite_addCase(dpiTest_2327,
            "dpiObject_appendElement() with NULL attribute");
    dpiTestSuite_addCase(dpiTest_2328,
            "call dpiObject_getElementValueByIndex() and verify the value");
    dpiTestSuite_addCase(dpiTest_2329,
            "call getFirstIndex and getLastIndex and verify the values");
    dpiTestSuite_addCase(dpiTest_2330,
            "call getNextIndex repeatedly and verify Indexes are as expected");
    dpiTestSuite_addCase(dpiTest_2331,
            "call getPrevIndex repeatedly and verify Indexes are as expected");
    dpiTestSuite_addCase(dpiTest_2332,
            "setElement at specific index and verify it is set properly");
    dpiTestSuite_addCase(dpiTest_2333,
            "copy collection object and verify copies are independent");
    dpiTestSuite_addCase(dpiTest_2334,
            "copy object with attributes and verify copies are independent");
    dpiTestSuite_addCase(dpiTest_2335,
            "append object of wrong type to a collection");
    dpiTestSuite_addCase(dpiTest_2336,
            "set object attribute value of wrong type on an object");
    dpiTestSuite_addCase(dpiTest_2337,
            "set object variable value with wrong type");
    dpiTestSuite_addCase(dpiTest_2338,
            "call dpiObject_setElementValueByIndex() with wrong type");
    dpiTestSuite_addCase(dpiTest_2339,
            "call dpiObjectType_createObject() with object type as NULL");
    dpiTestSuite_addCase(dpiTest_2340,
            "test inserts/fetches works with nested collections");
    return dpiTestSuite_run();
}
