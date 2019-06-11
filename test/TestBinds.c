//-----------------------------------------------------------------------------
// Copyright (c) 2019 Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestBinds.c
//   Test suite for all of the binding related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_3300_bindUnicodeArrayIn()
//   Verify that binding unicode (NVARCHAR2) arrays IN work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_3300_bindUnicodeArrayIn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin :1 := pkg_TestUnicodeArrays.TestInArrays(:2, :3); end;";
    dpiData *returnValue, *startingLenValue, *arrayValue;
    dpiVar *returnVar, *startingLenVar, *arrayVar;
    uint32_t numElements = 278, i;
    char buffer[300];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &startingLenVar, &startingLenValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            numElements, 100, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(startingLenValue, 65);
    for (i = 0; i < numElements; i++) {
        sprintf(buffer, "\xe3\x81\x82 %u", i + 1);
        if (dpiVar_setFromBytes(arrayVar, i, buffer, strlen(buffer)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setNumElementsInArray(arrayVar, numElements) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, startingLenVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 3, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(returnValue),
            1347) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(startingLenVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3301_bindUnicodeArrayInOut()
//   Verify that binding unicode (NVARCHAR2) arrays IN/OUT work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_3301_bindUnicodeArrayInOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestUnicodeArrays.TestInOutArrays(:1, :2); end;";
    const char *outValueFormat =
            "Converted element \xe3\x81\x82 # %u originally had length %u";
    const char *inValueFormat = "\xe3\x81\x82 in value %u";
    uint32_t numElements = 183, numElementsOut, i;
    dpiData *numElementsValue, *arrayValue;
    dpiVar *numElementsVar, *arrayVar;
    char buffer[300];
    size_t strLength;
    dpiBytes *bytes;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            numElements, 100, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    for (i = 0; i < numElements; i++) {
        sprintf(buffer, inValueFormat, i + 1);
        if (dpiVar_setFromBytes(arrayVar, i, buffer, strlen(buffer)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setNumElementsInArray(arrayVar, numElements) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform execution and verify return values match expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsOut) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, numElementsOut, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numElements; i++) {
        sprintf(buffer, inValueFormat, i + 1);
        strLength = strlen(buffer) - 2;
        sprintf(buffer, outValueFormat, i + 1, strLength);
        bytes = dpiData_getBytes(&arrayValue[i]);
        if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                buffer, strlen(buffer)) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3302_bindUnicodeArrayOut()
//   Verify that binding unicode (NVARCHAR2) arrays OUT work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_3302_bindUnicodeArrayOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestUnicodeArrays.TestOutArrays(:1, :2); end;";
    const char *outValueFormat = "Test out element \xe3\x81\x82 # %u";
    uint32_t numElements = 207, numElementsOut, i;
    dpiData *numElementsValue, *arrayValue;
    dpiVar *numElementsVar, *arrayVar;
    char buffer[300];
    dpiBytes *bytes;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            numElements, 100, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    if (dpiVar_setNumElementsInArray(arrayVar, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform execution and verify return values match expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsOut) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, numElementsOut, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numElements; i++) {
        sprintf(buffer, outValueFormat, i + 1);
        bytes = dpiData_getBytes(&arrayValue[i]);
        if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                buffer, strlen(buffer)) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3303_bindNumberArrayIn()
//   Verify that binding NUMBER arrays IN work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_3303_bindNumberArrayIn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin :1 := pkg_TestNumberArrays.TestInArrays(:2, :3); end;";
    dpiData *returnValue, *startingValue, *arrayValue;
    dpiVar *returnVar, *startingVar, *arrayVar;
    uint32_t numElements = 295, i;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &startingVar, &startingValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numElements, 0, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(startingValue, 25);
    for (i = 0; i < numElements; i++)
        dpiData_setInt64(&arrayValue[i], i);
    if (dpiVar_setNumElementsInArray(arrayVar, numElements) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, startingVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 3, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(returnValue),
            43390) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(startingVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3304_bindNumberArrayInOut()
//   Verify that binding NUMBER arrays IN/OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_3304_bindNumberArrayInOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestNumberArrays.TestInOutArrays(:1, :2); end;";
    uint32_t numElements = 179, i, numElementsOut;
    dpiData *numElementsValue, *arrayValue;
    dpiVar *numElementsVar, *arrayVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numElements, 0, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    for (i = 0; i < numElements; i++)
        dpiData_setInt64(&arrayValue[i], (i + 1) * 2);
    if (dpiVar_setNumElementsInArray(arrayVar, numElements) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsOut) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, numElementsOut, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numElements; i++) {
        if (dpiTestCase_expectIntEqual(testCase,
                dpiData_getInt64(&arrayValue[i]), (i + 1) * 20) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3305_bindNumberArrayOut()
//   Verify that binding NUMBER arrays IN/OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_3305_bindNumberArrayOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin pkg_TestNumberArrays.TestOutArrays(:1, :2); end;";
    uint32_t numElements = 243, i, numElementsOut;
    dpiData *numElementsValue, *arrayValue;
    dpiVar *numElementsVar, *arrayVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numElements, 0, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    if (dpiVar_setNumElementsInArray(arrayVar, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsOut) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, numElementsOut, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numElements; i++) {
        if (dpiTestCase_expectIntEqual(testCase,
                dpiData_getInt64(&arrayValue[i]), (i + 1) * 100) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3306_bindDateArrayIn()
//   Verify that binding DATE arrays IN work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_3306_bindDateArrayIn(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql =
            "begin :1 := pkg_TestDateArrays.TestInArrays(:2, :3, :4); end;";
    dpiData *returnValue, *startingValue, *baseDateValue, *arrayValue;
    dpiVar *returnVar, *startingVar, *baseDateVar, *arrayVar;
    uint32_t numElements = 279, i;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &startingVar, &startingValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP,
            1, 0, 0, 0, NULL, &baseDateVar, &baseDateValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP,
            numElements, 0, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(startingValue, 10);
    dpiData_setTimestamp(baseDateValue, 2020, 1, 1, 1, 2, 1, 0, 0, 0);
    for (i = 0; i < numElements; i++)
        dpiData_setTimestamp(&arrayValue[i], 2020, i % 12 + 1, 1, 1, 2, 1, 0,
                0, 0);
    if (dpiVar_setNumElementsInArray(arrayVar, numElements) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, startingVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 3, baseDateVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 4, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(returnValue),
            46285) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(startingVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(baseDateVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3307_bindDateArrayInOut()
//   Verify that binding DATE arrays IN/OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_3307_bindDateArrayInOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin pkg_TestDateArrays.TestInOutArrays(:1, :2); end;";
    dpiData *numElementsValue, *arrayValue, temp;
    uint32_t numElements = 185, i, numElementsOut;
    dpiVar *numElementsVar, *arrayVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP,
            numElements, 0, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    for (i = 0; i < numElements; i++)
        dpiData_setTimestamp(&arrayValue[i], 2021, i % 12 + 1, 4, 1, 2, 1, 0,
                0, 0);
    if (dpiVar_setNumElementsInArray(arrayVar, numElements) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsOut) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, numElementsOut, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numElements; i++) {
        dpiData_setTimestamp(&temp, 2021, i % 12 + 1, 11, 1, 2, 1, 0, 0, 0);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&arrayValue[i]),
                dpiData_getTimestamp(&temp)) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3308_bindDateArrayOut()
//   Verify that binding DATE arrays OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_3308_bindDateArrayOut(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "begin pkg_TestDateArrays.TestOutArrays(:1, :2); end;";
    dpiData *numElementsValue, *arrayValue, temp;
    uint32_t numElements = 14, i, numElementsOut;
    dpiVar *numElementsVar, *arrayVar;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP,
            numElements, 0, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    if (dpiVar_setNumElementsInArray(arrayVar, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and bind values
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsOut) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, numElementsOut, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numElements; i++) {
        dpiData_setTimestamp(&temp, 2002, 12, i + 13, 0, 0, 0, 0, 0, 0);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&arrayValue[i]),
                dpiData_getTimestamp(&temp)) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(3300);
    dpiTestSuite_addCase(dpiTest_3300_bindUnicodeArrayIn,
            "test PL/SQL bind of unicode array (IN)");
    dpiTestSuite_addCase(dpiTest_3301_bindUnicodeArrayInOut,
            "test PL/SQL bind of unicode array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_3302_bindUnicodeArrayOut,
            "test PL/SQL bind of unicode array (OUT)");
    dpiTestSuite_addCase(dpiTest_3303_bindNumberArrayIn,
            "test PL/SQL bind of number array (IN)");
    dpiTestSuite_addCase(dpiTest_3304_bindNumberArrayInOut,
            "test PL/SQL bind of number array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_3305_bindNumberArrayOut,
            "test PL/SQL bind of number array (OUT)");
    dpiTestSuite_addCase(dpiTest_3306_bindDateArrayIn,
            "test PL/SQL bind of date array (IN)");
    dpiTestSuite_addCase(dpiTest_3307_bindDateArrayInOut,
            "test PL/SQL bind of date array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_3308_bindDateArrayOut,
            "test PL/SQL bind of date array (OUT)");
    return dpiTestSuite_run();
}
