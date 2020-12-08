//-----------------------------------------------------------------------------
// Copyright (c) 2020, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestJson.c
//   Test suite for all the JSON related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__truncateJsonTable()
//   Truncate the JSON table.
//-----------------------------------------------------------------------------
int dpiTest__truncateJsonTable(dpiTestCase *testCase, dpiConn *conn)
{
    const char *sql = "truncate table TestJson";
    dpiStmt *stmt;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return 0;
}


//-----------------------------------------------------------------------------
// dpiTest_3500_verifyPubFuncsOfJsonWithNull()
//   Call each of the dpiJson public functions with the json parameter set to
// NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_3500_verifyPubFuncsOfJsonWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    dpiJson_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiJson_getValue(NULL, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiJson_setValue(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiJson_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3501_bindJsonScalarValue()
//   Insert scalar JSON value into the table and then fetch it back again,
// ensuring that it is identical.
//-----------------------------------------------------------------------------
int dpiTest_3501_bindJsonScalarValue(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestJson values(:1)";
    const char *selectSql = "select jsoncol from TestJson";
    dpiNativeTypeNum nativeTypeNum;
    dpiJsonNode inNode, *topNode;
    int64_t valueToPass = 99;
    dpiDataBuffer inNodeData;
    dpiData *data, *outValue;
    uint32_t bufferRowIndex;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;
    int found;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateJsonTable(testCase, conn) < 0)
        return DPI_FAILURE;

    // setup node
    inNode.value = &inNodeData;
    inNode.oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    inNode.nativeTypeNum = DPI_NATIVE_TYPE_INT64;
    inNodeData.asInt64 = valueToPass;

    // create variable and populate it with the node
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON, 1, 0,
            0, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    data->isNull = 0;
    if (dpiJson_setValue(data->value.asJson, &inNode) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch row
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_getValue(outValue->value.asJson, DPI_JSON_OPT_DEFAULT,
            &topNode) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, topNode->value->asDouble,
            valueToPass) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3502_bindJsonObjectValue()
//   Bind JSON values into the database by building a JSON node tree and
// verify.
//-----------------------------------------------------------------------------
int dpiTest_3502_bindJsonObjectValue(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestJson values(:1)";
    const char *selectSql = "select jsoncol from TestJson";
    uint32_t bufferRowIndex, fieldNameLengths[2];
    dpiJsonNode inNodes[11], *topNode;
    dpiNativeTypeNum nativeTypeNum;
    dpiDataBuffer inNodeData[11];
    dpiData *data, *outValue;
    char *fieldNames[2];
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;
    int found, i;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateJsonTable(testCase, conn) < 0)
        return DPI_FAILURE;

    memset(inNodes, 0, sizeof(inNodes));
    memset(inNodeData, 0, sizeof(inNodeData));

    // set value for each of the nodes
    for (i = 0; i < 11; i++)
        inNodes[i].value = &inNodeData[i];

    inNodes[0].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    inNodes[0].nativeTypeNum = DPI_NATIVE_TYPE_INT64;
    inNodeData[0].asInt64 = -99;

    inNodes[1].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    inNodes[1].nativeTypeNum = DPI_NATIVE_TYPE_INT64;
    inNodeData[1].asUint64 = 99;

    inNodes[2].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    inNodes[2].nativeTypeNum = DPI_NATIVE_TYPE_DOUBLE;
    inNodeData[2].asDouble = 1.23456;

    inNodes[3].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    inNodes[3].nativeTypeNum = DPI_NATIVE_TYPE_DOUBLE;
    inNodeData[3].asDouble = 9397.111114444;

    inNodes[4].oracleTypeNum = DPI_ORACLE_TYPE_VARCHAR;
    inNodes[4].nativeTypeNum = DPI_NATIVE_TYPE_BYTES;
    inNodeData[4].asBytes.ptr = "Test String";
    inNodeData[4].asBytes.length = strlen(inNodeData[4].asBytes.ptr);

    inNodes[5].oracleTypeNum = DPI_ORACLE_TYPE_VARCHAR;
    inNodes[5].nativeTypeNum = DPI_NATIVE_TYPE_BYTES;
    inNodeData[5].asBytes.ptr = "Test String2";
    inNodeData[5].asBytes.length = strlen(inNodeData[5].asBytes.ptr);

    inNodes[6].oracleTypeNum = DPI_ORACLE_TYPE_RAW;
    inNodes[6].nativeTypeNum = DPI_NATIVE_TYPE_BYTES;
    inNodeData[6].asBytes.ptr = "A Raw";
    inNodeData[6].asBytes.length = strlen(inNodeData[6].asBytes.ptr);

    inNodes[7].oracleTypeNum = DPI_ORACLE_TYPE_DATE;
    inNodes[7].nativeTypeNum = DPI_NATIVE_TYPE_TIMESTAMP;
    inNodeData[7].asTimestamp.year = 2020;
    inNodeData[7].asTimestamp.month = 10;
    inNodeData[7].asTimestamp.day = 15;
    inNodeData[7].asTimestamp.hour = 13;
    inNodeData[7].asTimestamp.minute = 54;
    inNodeData[7].asTimestamp.second = 12;

    inNodes[8].oracleTypeNum = DPI_ORACLE_TYPE_TIMESTAMP;
    inNodes[8].nativeTypeNum = DPI_NATIVE_TYPE_TIMESTAMP;
    inNodeData[8].asTimestamp.year = 2020;
    inNodeData[8].asTimestamp.month = 10;
    inNodeData[8].asTimestamp.day = 15;
    inNodeData[8].asTimestamp.hour = 13;
    inNodeData[8].asTimestamp.minute = 58;
    inNodeData[8].asTimestamp.second = 59;

    // JSON object
    fieldNames[0] = "Fred";
    fieldNameLengths[0] = strlen(fieldNames[0]);
    fieldNames[1] = "George";
    fieldNameLengths[1] = strlen(fieldNames[1]);
    inNodes[9].oracleTypeNum = DPI_ORACLE_TYPE_JSON_OBJECT;
    inNodes[9].nativeTypeNum = DPI_NATIVE_TYPE_JSON_OBJECT;
    inNodeData[9].asJsonObject.numFields = 2;
    inNodeData[9].asJsonObject.fieldNames = fieldNames;
    inNodeData[9].asJsonObject.fieldNameLengths = fieldNameLengths;
    inNodeData[9].asJsonObject.fields = &inNodes[0];
    inNodeData[9].asJsonObject.fieldValues = &inNodeData[0];

    inNodes[10].oracleTypeNum = DPI_ORACLE_TYPE_JSON_ARRAY;
    inNodes[10].nativeTypeNum = DPI_NATIVE_TYPE_JSON_ARRAY;
    inNodeData[10].asJsonArray.numElements = 10;
    inNodeData[10].asJsonArray.elements = &inNodes[0];
    inNodeData[10].asJsonArray.elementValues = &inNodeData[0];

    // create variable and populate it with the JSON value
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON, 1, 0,
            0, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    data->isNull = 0;
    if (dpiJson_setValue(data->value.asJson, &inNodes[10]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform insert
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch row
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_getValue(outValue->value.asJson, DPI_JSON_OPT_DEFAULT,
            &topNode) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase,
            topNode->value->asJsonArray.numElements, 10) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3503_bindJsonArrayIntValues()
//   Verify that integer values can be stored in a JSON array -- and when
// fetched, that they retain the same values.
//-----------------------------------------------------------------------------
int dpiTest_3503_bindJsonArrayIntValues(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestJson values (:1)";
    const char *selectSql = "select jsoncol from TestJson";
    int64_t valueToPass[] = {9, 255, 999, 65535, 0, 4294967295,
            -9223372036854775, 9223372036854775, 0, 1};
    uint32_t numElements = 10, bufferRowIndex, i;
    dpiNativeTypeNum nativeTypeNum;
    dpiJsonNode inNodes[11], *node;
    dpiDataBuffer inNodeData[11];
    dpiData *data, *outValue;
    dpiJsonArray *array;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;
    int found;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateJsonTable(testCase, conn) < 0)
        return DPI_FAILURE;

    // prepare node structure
    memset(inNodes, 0, sizeof(inNodes));
    memset(inNodeData, 0, sizeof(inNodeData));
    for (i = 0; i < numElements; i++) {
        inNodeData[i].asInt64 = valueToPass[i];
        inNodes[i].value = &inNodeData[i];
        inNodes[i].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
        inNodes[i].nativeTypeNum = DPI_NATIVE_TYPE_INT64;
    }
    inNodes[numElements].value = &inNodeData[numElements];
    inNodes[numElements].oracleTypeNum = DPI_ORACLE_TYPE_JSON_ARRAY;
    inNodes[numElements].nativeTypeNum = DPI_NATIVE_TYPE_JSON_ARRAY;
    inNodeData[numElements].asJsonArray.numElements = numElements;
    inNodeData[numElements].asJsonArray.elements = &inNodes[0];
    inNodeData[numElements].asJsonArray.elementValues = &inNodeData[0];

    // create variable and populate it with the JSON value
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON, 1, 0,
            0, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    data->isNull = 0;
    if (dpiJson_setValue(data->value.asJson, &inNodes[numElements]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch row
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_getValue(outValue->value.asJson, DPI_JSON_OPT_DEFAULT,
            &node) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase,
            node->value->asJsonArray.numElements, numElements) < 0)
        return DPI_FAILURE;

    // verify value matches
    array = &node->value->asJsonArray;
    for (i = 0; i < array->numElements; i++) {
        node = &array->elements[i];
        if (dpiTestCase_expectDoubleEqual(testCase, node->value->asDouble,
                valueToPass[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3504_bindJsonArrayDoubleValues()
//   Verify that double values can be stored in a JSON array -- and when
// fetched, that they retain the same values.
//-----------------------------------------------------------------------------
int dpiTest_3504_bindJsonArrayDoubleValues(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestJson values (:1)";
    const char *selectSql = "select jsoncol from TestJson";
    double valueToPass[] = {-0.123456789101112, 0.123456789101112,
            -1234567.12345678, 9999999.99999999};
    uint32_t numElements = 4, bufferRowIndex, i;
    dpiNativeTypeNum nativeTypeNum;
    dpiJsonNode inNodes[5], *node;
    dpiDataBuffer inNodeData[5];
    dpiData *data, *outValue;
    dpiJsonArray *array;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;
    int found;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateJsonTable(testCase, conn) < 0)
        return DPI_FAILURE;

    // prepare node structure
    memset(inNodes, 0, sizeof(inNodes));
    memset(inNodeData, 0, sizeof(inNodeData));
    for (i = 0; i < numElements; i++) {
        inNodes[i].value = &inNodeData[i];
        inNodes[i].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
        inNodes[i].nativeTypeNum = DPI_NATIVE_TYPE_DOUBLE;
        inNodeData[i].asDouble = valueToPass[i];
    }
    inNodes[numElements].value = &inNodeData[numElements];
    inNodes[numElements].oracleTypeNum = DPI_ORACLE_TYPE_JSON_ARRAY;
    inNodes[numElements].nativeTypeNum = DPI_NATIVE_TYPE_JSON_ARRAY;
    inNodeData[numElements].asJsonArray.numElements = numElements;
    inNodeData[numElements].asJsonArray.elements = &inNodes[0];
    inNodeData[numElements].asJsonArray.elementValues = &inNodeData[0];

    // create variable and populate it with the JSON value
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON, 1, 0,
            0, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    data->isNull = 0;
    if (dpiJson_setValue(data->value.asJson, &inNodes[numElements]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch row
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_getValue(outValue->value.asJson, DPI_JSON_OPT_DEFAULT,
            &node) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify value
    if (dpiTestCase_expectIntEqual(testCase,
            node->value->asJsonArray.numElements, numElements) < 0)
        return DPI_FAILURE;
    array = &node->value->asJsonArray;
    for (i = 0; i < array->numElements; i++) {
        node = &array->elements[i];
        if (dpiTestCase_expectDoubleEqual(testCase, node->value->asDouble,
                valueToPass[i]) < 0)
            return DPI_FAILURE;
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3505_bindJsonArrayStringValues()
//   Verify that strings and raw bytes can be stored in a JSON array -- and
// when fetched, that they retain the same values.
//-----------------------------------------------------------------------------
int dpiTest_3505_bindJsonArrayStringValues(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestJson values (:1)";
    const char *selectSql = "select jsoncol from TestJson";
    char valueToPass[][100] = {"Test String aaaaaa\naabbbbcccdddeeefffgxyzzzz",
            "a", "12ABCD\\EFGHIJ\nKLMNOPQRSTUVWXYZ", "abcdeadcdasdf"};
    uint32_t numElements = 4, bufferRowIndex, i;
    dpiNativeTypeNum nativeTypeNum;
    dpiJsonNode inNodes[5], *node;
    dpiDataBuffer inNodeData[5];
    dpiData *data, *outValue;
    dpiJsonArray *array;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;
    int found;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__truncateJsonTable(testCase, conn) < 0)
        return DPI_FAILURE;

    // prepare node structure
    memset(inNodes, 0, sizeof(inNodes));
    memset(inNodeData, 0, sizeof(inNodeData));
    for (i = 0; i < numElements; i++) {
        inNodes[i].value = &inNodeData[i];
        inNodes[i].oracleTypeNum = (i < 2) ? DPI_ORACLE_TYPE_VARCHAR :
                DPI_ORACLE_TYPE_RAW;
        inNodes[i].nativeTypeNum = DPI_NATIVE_TYPE_BYTES;
        inNodeData[i].asBytes.ptr = valueToPass[i];
        inNodeData[i].asBytes.length = strlen(inNodeData[i].asBytes.ptr);
    }
    inNodes[numElements].value = &inNodeData[numElements];
    inNodes[numElements].oracleTypeNum = DPI_ORACLE_TYPE_JSON_ARRAY;
    inNodes[numElements].nativeTypeNum = DPI_NATIVE_TYPE_JSON_ARRAY;
    inNodeData[numElements].asJsonArray.numElements = numElements;
    inNodeData[numElements].asJsonArray.elements = &inNodes[0];
    inNodeData[numElements].asJsonArray.elementValues = &inNodeData[0];

    // create variable and populate it with the JSON value
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON, 1, 0,
            0, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    data->isNull = 0;
    if (dpiJson_setValue(data->value.asJson, &inNodes[numElements]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch row
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_getValue(outValue->value.asJson, DPI_JSON_OPT_DEFAULT,
            &node) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify value
    if (dpiTestCase_expectIntEqual(testCase,
            node->value->asJsonArray.numElements, numElements) < 0)
        return DPI_FAILURE;
    array = &node->value->asJsonArray;
    for (i = 0; i < array->numElements; i++) {
        node = &array->elements[i];
        if (dpiTestCase_expectStringEqual(testCase, node->value->asBytes.ptr,
                node->value->asBytes.length, valueToPass[i],
                strlen(valueToPass[i])) < 0)
            return DPI_FAILURE;
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3500);
    dpiTestSuite_addCase(dpiTest_3500_verifyPubFuncsOfJsonWithNull,
            "call public functions with Json set to NULL");
    dpiTestSuite_addCase(dpiTest_3501_bindJsonScalarValue,
            "insert and fetch JSON scalar value");
    dpiTestSuite_addCase(dpiTest_3502_bindJsonObjectValue,
            "insert and fetch JSON object value");
    dpiTestSuite_addCase(dpiTest_3503_bindJsonArrayIntValues,
            "insert and fetch JSON array int values");
    dpiTestSuite_addCase(dpiTest_3504_bindJsonArrayDoubleValues,
            "insert and fetch JSON array double values");
    dpiTestSuite_addCase(dpiTest_3505_bindJsonArrayStringValues,
            "insert and fetch JSON array string values");
    return dpiTestSuite_run();
}
