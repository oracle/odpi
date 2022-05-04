//-----------------------------------------------------------------------------
// Copyright (c) 2019, 2022, Oracle and/or its affiliates.
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
// test_4100_binds.c
//   Test suite for all of the binding related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__bindLobIn() [INTERNAL]
//   Verify that binding LOBs IN  works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest__bindLobIn(dpiTestCase *testCase, dpiOracleTypeNum oracleTypeNum,
        const char *lobType, const char *lobStr)
{
    dpiData *returnValue, *lobValue;
    dpiVar *returnVar, *lobVar;
    char sql[100];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // generate SQL statement to execute
    sprintf(sql, "begin :1 := pkg_TestLobs.TestIn%s(:2); end;", lobType);

    // create variables and populate IN value
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, oracleTypeNum, DPI_NATIVE_TYPE_LOB, 1, 0, 0, 0,
            NULL, &lobVar, &lobValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(lobVar, 0, lobStr, strlen(lobStr)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, lobVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(returnValue),
            strlen(lobStr)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(lobVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__bindLobInOut() [INTERNAL]
//   Verify that binding LOBs IN/OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest__bindLobInOut(dpiTestCase *testCase,
        dpiOracleTypeNum oracleTypeNum, const char *lobType,
        const char *lobInStr)
{
    char expectedLobValue[200], actualLobValue[200], sql[100];
    uint64_t numBytes;
    dpiData *lobValue;
    dpiVar *lobVar;
    dpiStmt *stmt;
    dpiConn *conn;
    dpiLob *lob;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // generate SQL statement to execute
    sprintf(sql, "begin pkg_TestLobs.TestInOut%s(:1); end;", lobType);

    // create variable and populate it
    if (dpiConn_newVar(conn, oracleTypeNum, DPI_NATIVE_TYPE_LOB, 1, 0, 0, 0,
            NULL, &lobVar, &lobValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(lobVar, 0, lobInStr, strlen(lobInStr)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, lobVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    sprintf(expectedLobValue, "%s (Modified)", lobInStr);
    lob = dpiData_getLOB(lobValue);
    numBytes = sizeof(actualLobValue);
    if (dpiLob_readBytes(lob, 1, numBytes, actualLobValue, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, actualLobValue, numBytes,
            expectedLobValue, strlen(expectedLobValue)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiVar_release(lobVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__bindLobOut() [INTERNAL]
//   Verify that binding LOBs OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest__bindLobOut(dpiTestCase *testCase, dpiOracleTypeNum oracleTypeNum,
        const char *lobType, const char *expectedLobValue)
{
    char actualLobValue[200], sql[100];
    uint64_t numBytes;
    dpiData *lobValue;
    dpiVar *lobVar;
    dpiStmt *stmt;
    dpiConn *conn;
    dpiLob *lob;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // generate SQL statement to execute
    sprintf(sql, "begin pkg_TestLobs.TestOut%s(:1); end;", lobType);

    // create variable
    if (dpiConn_newVar(conn, oracleTypeNum, DPI_NATIVE_TYPE_LOB, 1, 0, 0, 0,
            NULL, &lobVar, &lobValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, lobVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    lob = dpiData_getLOB(lobValue);
    numBytes = sizeof(actualLobValue);
    if (dpiLob_readBytes(lob, 1, numBytes, actualLobValue, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, actualLobValue, numBytes,
            expectedLobValue, strlen(expectedLobValue)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(lobVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4100_bindUnicodeArrayIn()
//   Verify that binding unicode (NVARCHAR2) arrays IN work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_4100_bindUnicodeArrayIn(dpiTestCase *testCase,
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
// dpiTest_4101_bindUnicodeArrayInOut()
//   Verify that binding unicode (NVARCHAR2) arrays IN/OUT work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_4101_bindUnicodeArrayInOut(dpiTestCase *testCase,
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
// dpiTest_4102_bindUnicodeArrayOut()
//   Verify that binding unicode (NVARCHAR2) arrays OUT work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_4102_bindUnicodeArrayOut(dpiTestCase *testCase,
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
// dpiTest_4103_bindNumberArrayIn()
//   Verify that binding NUMBER arrays IN work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4103_bindNumberArrayIn(dpiTestCase *testCase,
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
// dpiTest_4104_bindNumberArrayInOut()
//   Verify that binding NUMBER arrays IN/OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4104_bindNumberArrayInOut(dpiTestCase *testCase,
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
// dpiTest_4105_bindNumberArrayOut()
//   Verify that binding NUMBER arrays IN/OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4105_bindNumberArrayOut(dpiTestCase *testCase,
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
// dpiTest_4106_bindDateArrayIn()
//   Verify that binding DATE arrays IN work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4106_bindDateArrayIn(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_4107_bindDateArrayInOut()
//   Verify that binding DATE arrays IN/OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4107_bindDateArrayInOut(dpiTestCase *testCase,
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
// dpiTest_4108_bindDateArrayOut()
//   Verify that binding DATE arrays OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4108_bindDateArrayOut(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_4109_bindObjArrayIn()
//   Verify that binding object arrays IN works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4109_bindObjArrayIn(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql =
            "begin :1 := pkg_TestObjectArrays.TestInObjArray(:2, :3); end;";
    dpiData *returnValue, *startingLenValue, *arrayValue, tempData;
    dpiVar *returnVar, *startingLenVar, *arrayVar;
    const char *elementObjName = "UDT_SUBOBJECT";
    const char *arrayObjName = "UDT_OBJECTARRAY";
    dpiObjectType *arrayObjType, *elementObjType;
    dpiObject *arrayObj, *elementObj;
    uint32_t i, numElements = 10;
    dpiObjectAttr *attrs[2];
    char buffer[300];
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object types and attributes
    if (dpiConn_getObjectType(conn, arrayObjName, strlen(arrayObjName),
            &arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, elementObjName, strlen(elementObjName),
            &elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(elementObjType, 2, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create objects
    if (dpiObjectType_createObject(arrayObjType, &arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(elementObjType, &elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &startingLenVar, &startingLenValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, arrayObjType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(startingLenValue, numElements);
    for (i = 0; i < numElements; i++) {
        dpiData_setDouble(&tempData, i + 1);
        if (dpiObject_setAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_DOUBLE, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        sprintf(buffer, "Test Data In %u", i + 1);
        dpiData_setBytes(&tempData, buffer, strlen(buffer));
        if (dpiObject_setAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setObject(&tempData, elementObj);
        if (dpiObject_appendElement(arrayObj, DPI_NATIVE_TYPE_OBJECT,
                &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setFromObject(arrayVar, 0, arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(arrayObj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;

    // prepare statement and perform binds
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
            206) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < 2; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(startingLenVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4110_bindObjArrayInOut()
//   Verify that binding object arrays IN/OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4110_bindObjArrayInOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestObjectArrays.TestInOutObjArray(:1, :2); end;";
    const char *outValueFormat = "Test in/out element number %u";
    const char *inValueFormat = "Test In/Out Data %u";
    dpiData *numElementsValue, *arrayValue, tempData;
    const char *elementObjName = "UDT_SUBOBJECT";
    const char *arrayObjName = "UDT_OBJECTARRAY";
    dpiObjectType *arrayObjType, *elementObjType;
    dpiVar *numElementsVar, *arrayVar;
    dpiObject *arrayObj, *elementObj;
    uint32_t i, numElements = 8;
    dpiObjectAttr *attrs[2];
    char buffer[300];
    dpiBytes *bytes;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object types and attributes
    if (dpiConn_getObjectType(conn, arrayObjName, strlen(arrayObjName),
            &arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, elementObjName, strlen(elementObjName),
            &elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(elementObjType, 2, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create objects
    if (dpiObjectType_createObject(arrayObjType, &arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(elementObjType, &elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, arrayObjType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    for (i = 0; i < numElements; i++) {
        dpiData_setDouble(&tempData, i);
        if (dpiObject_setAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_DOUBLE, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        sprintf(buffer, inValueFormat, i + 1);
        dpiData_setBytes(&tempData, buffer, strlen(buffer));
        if (dpiObject_setAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setObject(&tempData, elementObj);
        if (dpiObject_appendElement(arrayObj, DPI_NATIVE_TYPE_OBJECT,
                &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setFromObject(arrayVar, 0, arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(arrayObj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;
    if (dpiObject_release(arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return values matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    arrayObj = dpiData_getObject(arrayValue);
    if (dpiObject_getSize(arrayObj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < size; i++) {
        if (dpiObject_getElementValueByIndex(arrayObj, i,
                DPI_NATIVE_TYPE_OBJECT, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        elementObj = dpiData_getObject(&tempData);
        if (dpiObject_getAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_DOUBLE, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectDoubleEqual(testCase,
                dpiData_getDouble(&tempData), i + 1) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        sprintf(buffer, outValueFormat, i + 1);
        bytes = dpiData_getBytes(&tempData);
        if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                buffer, strlen(buffer)) < 0)
            return DPI_FAILURE;
        if (dpiObject_release(elementObj) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < 2; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4111_bindObjArrayOut()
//   Verify that binding object arrays OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4111_bindObjArrayOut(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestObjectArrays.TestOutObjArray(:1, :2); end;";
    const char *outValueFormat = "Test out element number %u";
    dpiData *numElementsValue, *arrayValue, tempData;
    const char *elementObjName = "UDT_SUBOBJECT";
    const char *arrayObjName = "UDT_OBJECTARRAY";
    dpiObjectType *arrayObjType, *elementObjType;
    dpiVar *numElementsVar, *arrayVar;
    dpiObject *arrayObj, *elementObj;
    uint32_t i, numElements = 7;
    dpiObjectAttr *attrs[2];
    char buffer[300];
    dpiBytes *bytes;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object types and attributes
    if (dpiConn_getObjectType(conn, arrayObjName, strlen(arrayObjName),
            &arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, elementObjName, strlen(elementObjName),
            &elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(elementObjType, 2, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, arrayObjType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return values matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    arrayObj = dpiData_getObject(arrayValue);
    if (dpiObject_getSize(arrayObj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < size; i++) {
        if (dpiObject_getElementValueByIndex(arrayObj, i,
                DPI_NATIVE_TYPE_OBJECT, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        elementObj = dpiData_getObject(&tempData);
        if (dpiObject_getAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_DOUBLE, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectDoubleEqual(testCase,
                dpiData_getDouble(&tempData), i + 1) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        sprintf(buffer, outValueFormat, i + 1);
        bytes = dpiData_getBytes(&tempData);
        if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                buffer, strlen(buffer)) < 0)
            return DPI_FAILURE;
        if (dpiObject_release(elementObj) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < 2; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4112_bindNumberVarrayIn()
//   Verify that binding Number Varray IN work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4112_bindNumberVarrayIn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin :1 := pkg_TestNumberVarrays.TestInArrays(:2, :3); end;";
    dpiData *returnValue, *startingLenValue, *arrayValue, tempData;
    dpiVar *returnVar, *startingLenVar, *arrayVar;
    const char *objName = "UDT_ARRAY";
    uint32_t i, numElements = 9;
    dpiObjectType *objType;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create object
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &startingLenVar, &startingLenValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(startingLenValue, 10);
    for (i = 0; i < numElements; i++) {
        dpiData_setInt64(&tempData, i + 1);
        if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setFromObject(arrayVar, 0, obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(obj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;

    // prepare statement and perform binds
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
            55) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
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
// dpiTest_4113_bindNumberVarrayInOut()
//   Verify that binding Number Varray IN/OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4113_bindNumberVarrayInOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestNumberVarrays.TestInOutArrays(:1, :2); end;";
    dpiData *numElementsValue, *arrayValue, tempData;
    dpiVar *numElementsVar,*arrayVar;
    const char *objName = "UDT_ARRAY";
    uint32_t i, numElements = 6;
    dpiObjectType *objType;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create object
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);
    for (i = 0; i < numElements; i++) {
        dpiData_setInt64(&tempData, i);
        if (dpiObject_appendElement(obj, DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setFromObject(arrayVar, 0, obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(obj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getSize(dpiData_getObject(arrayValue), &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < size; i++) {
        if (dpiObject_getElementValueByIndex(dpiData_getObject(arrayValue), i,
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase,
                dpiData_getInt64(&tempData), i * 20) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4114_bindNumberVarrayOut()
//   Verify that binding Number Varray OUT work as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4114_bindNumberVarrayOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestNumberVarrays.TestOutArrays(:1, :2); end;";
    dpiData *numElementsValue, *arrayValue, tempData;
    const char *objName = "UDT_ARRAY";
    dpiVar *numElementsVar,*arrayVar;
    uint32_t i, numElements = 9;
    dpiObjectType *objType;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object type
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return values matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    obj = dpiData_getObject(arrayValue);
    if (dpiObject_getSize(obj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;

    // compare results
    for (i = 0; i < size; i++) {
        if (dpiObject_getElementValueByIndex(obj, i, DPI_NATIVE_TYPE_INT64,
                &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
                (i + 1) * 200) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4115_bindStringArrayIn()
//   Verify that binding string (VARCHAR2) arrays IN work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_4115_bindStringArrayIn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin :1 := pkg_TestStringArrays.TestInArrays(:2, :3); end;";
    const char alphaNum[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    dpiData *returnValue, *startingLenValue, *arrayValue;
    dpiVar *returnVar, *startingLenVar, *arrayVar;
    uint32_t numElements = 2500, i, strSize = 100;
    char buffer[100];
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
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numElements, strSize, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(startingLenValue, 65);
    for (i = 0; i < strSize; i++)
        buffer[i] = alphaNum[rand() % (sizeof(alphaNum) - 1)];
    for (i = 0; i < numElements; i++) {
        if (dpiVar_setFromBytes(arrayVar, i, buffer, strSize) < 0)
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
            250065) < 0)
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
// dpiTest_4116_bindStringArrayInOut()
//   Verify that binding string (VARCHAR2) arrays IN/OUT work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_4116_bindStringArrayInOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestStringArrays.TestInOutArrays(:1, :2); end;";
    const char *outValueFormat =
            "Converted element # %u originally had length %u";
    const char *inValueFormat = "IN String in value %u";
    uint32_t numElements = 20000, numElementsOut, i;
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
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
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
        strLength = strlen(buffer);
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
// dpiTest_4117_bindStringArrayOut()
//   Verify that binding string (VARCHAR2) arrays OUT work as expected (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_4117_bindStringArrayOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "begin pkg_TestStringArrays.TestOutArrays(:1, :2); end;";
    const char *outValueFormat = "Test out element # %u";
    uint32_t numElements = 20007, numElementsOut, i;
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
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
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
// dpiTest_4118_bindRecordIn()
//   Verify that binding record IN works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4118_bindRecordIn(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedValue =
            "udt_Record(99, 'Test Record', "
            "to_date('2017-06-01', 'YYYY-MM-DD'), "
            "to_timestamp('2018-08-02 03:02:01', 'YYYY-MM-DD HH24:MI:SS'),"
            " true, -2147483647, 2147483647)";
    const char *sql =
            "begin :1 := pkg_TestRecords.GetStringRep(:2); end;";
    const char *objectName = "PKG_TESTRECORDS.UDT_RECORD";
    dpiData *stringRepValue, *objectValue, tempData;
    dpiVar *stringRepVar, *objectVar;
    uint32_t i, numAttrs = 7;
    dpiObjectAttr *attrs[7];
    dpiObjectType *objType;
    dpiBytes *bytes;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object type and attributes
    if (dpiConn_getObjectType(conn, objectName, strlen(objectName),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create object
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            200, 0, 0, NULL, &stringRepVar, &stringRepValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(&tempData, 99);
    if (dpiObject_setAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&tempData, "Test Record", strlen("Test Record"));
    if (dpiObject_setAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData, 2017, 6, 1, 1, 2, 1, 1, 0, 0);
    if (dpiObject_setAttributeValue(obj, attrs[2], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData, 2018, 8, 2, 3, 2, 1, 1, 0, 0);
    if (dpiObject_setAttributeValue(obj, attrs[3], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBool(&tempData, 1);
    if (dpiObject_setAttributeValue(obj, attrs[4],
            DPI_NATIVE_TYPE_BOOLEAN, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromObject(objectVar, 0, obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&tempData, -2147483647);
    if (dpiObject_setAttributeValue(obj, attrs[5], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&tempData, 2147483647);
    if (dpiObject_setAttributeValue(obj, attrs[6], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, stringRepVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    bytes = dpiData_getBytes(stringRepValue);
    if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                expectedValue, strlen(expectedValue)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(stringRepVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4119_bindRecordInOut()
//   Verify that binding record IN/OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4119_bindRecordInOut(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "begin pkg_TestRecords.TestInOut(:1); end;";
    const char *objectName = "PKG_TESTRECORDS.UDT_RECORD";
    const char *expectedValue = "String in/out record";
    dpiData *objectValue, tempData, tempData2;
    uint32_t i, numAttrs = 7;
    dpiObjectAttr *attrs[7];
    dpiObjectType *objType;
    dpiVar *objectVar;
    dpiBytes *bytes;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object type and attribute
    if (dpiConn_getObjectType(conn, objectName, strlen(objectName),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create object
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(&tempData, 99);
    if (dpiObject_setAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&tempData, "Test Record", strlen("Test Record"));
    if (dpiObject_setAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData, 2017, 6, 5, 0, 0, 0, 0, 0, 0);
    if (dpiObject_setAttributeValue(obj, attrs[2], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData, 2018, 8, 9, 3, 2, 1, 0, 0, 0);
    if (dpiObject_setAttributeValue(obj, attrs[3], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBool(&tempData, 1);
    if (dpiObject_setAttributeValue(obj, attrs[4], DPI_NATIVE_TYPE_BOOLEAN,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&tempData, -214748);
    if (dpiObject_setAttributeValue(obj, attrs[5], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&tempData, 214748);
    if (dpiObject_setAttributeValue(obj, attrs[6], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromObject(objectVar, 0, obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    obj = dpiData_getObject(objectValue);
    if (dpiObject_getAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
            990) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    bytes = dpiData_getBytes(&tempData);
    if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
            expectedValue, strlen(expectedValue)) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[2], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData2, 2017, 6, 9, 0, 0, 0, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&tempData),
            dpiData_getTimestamp(&tempData2)) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[3], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData2, 2018, 8, 8, 3, 2, 1, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&tempData),
            dpiData_getTimestamp(&tempData2)) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[4], DPI_NATIVE_TYPE_BOOLEAN,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, dpiData_getBool(&tempData),
            0) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[5], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
            -2147480) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[6], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
            2147480) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}



//-----------------------------------------------------------------------------
// dpiTest_4120_bindRecordOut()
//   Verify that binding record OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4120_bindRecordOut(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "begin pkg_TestRecords.TestOut(:1); end;";
    const char *objectName = "PKG_TESTRECORDS.UDT_RECORD";
    const char *expectedValue = "String in record";
    dpiData *objectValue, tempData, tempData2;
    uint32_t i, numAttrs = 7;
    dpiObjectAttr *attrs[7];
    dpiObjectType *objType;
    dpiVar *objectVar;
    dpiBytes *bytes;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object type and attribute
    if (dpiConn_getObjectType(conn, objectName, strlen(objectName),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    obj = dpiData_getObject(objectValue);
    if (dpiObject_getAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
            25) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    bytes = dpiData_getBytes(&tempData);
    if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
            expectedValue, strlen(expectedValue)) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[2], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData2, 2016, 2, 16, 0, 0, 0, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&tempData),
            dpiData_getTimestamp(&tempData2)) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[3], DPI_NATIVE_TYPE_TIMESTAMP,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData2, 2016, 2, 16, 18, 23, 55, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&tempData),
            dpiData_getTimestamp(&tempData2)) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[4], DPI_NATIVE_TYPE_BOOLEAN,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, dpiData_getBool(&tempData),
            1) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[5], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
            -214748) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(obj, attrs[6], DPI_NATIVE_TYPE_INT64,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
            214748) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(objectVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4121_bindRecordArrayIn()
//   Verify that binding Record array IN works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4121_bindRecordArrayIn(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin :1 := pkg_TestRecords.TestInArrays(:2); end;";
    const char *arrayObjName = "PKG_TESTRECORDS.UDT_RECORDARRAY";
    const char *elementObjName = "PKG_TESTRECORDS.UDT_RECORD";
    const char *inValueFormat = "Test Record In %u";
    dpiObjectType *arrayObjType, *elementObjType;
    dpiData *returnValue, *arrayValue, tempData;
    uint32_t i, numAttrs = 7, numElements = 10;
    dpiObject *arrayObj, *elementObj;
    dpiVar *returnVar, *arrayVar;
    dpiObjectAttr *attrs[7];
    char buffer[300];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object types and attributes
    if (dpiConn_getObjectType(conn, arrayObjName, strlen(arrayObjName),
            &arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, elementObjName, strlen(elementObjName),
            &elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(elementObjType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create objects
    if (dpiObjectType_createObject(arrayObjType, &arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(elementObjType, &elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, arrayObjType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    for (i = 0; i < numElements; i++) {
        dpiData_setInt64(&tempData, i + 1);
        if (dpiObject_setAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        sprintf(buffer, inValueFormat, i + 1);
        dpiData_setBytes(&tempData, buffer, strlen(buffer));
        if (dpiObject_setAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData, 2017, 6, i + 1, 0, 0, 0, 0, 0, 0);
        if (dpiObject_setAttributeValue(elementObj, attrs[2],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData, 2018, 8, i + 1, 3, 2, 1, 0, 0, 0);
        if (dpiObject_setAttributeValue(elementObj, attrs[3],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setBool(&tempData, 1);
        if (dpiObject_setAttributeValue(elementObj, attrs[4],
                DPI_NATIVE_TYPE_BOOLEAN, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setInt64(&tempData, i + -21478);
        if (dpiObject_setAttributeValue(elementObj, attrs[5],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setInt64(&tempData, i + 21478);
        if (dpiObject_setAttributeValue(elementObj, attrs[6],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setObject(&tempData, elementObj);
        if (dpiObject_appendElement(arrayObj, DPI_NATIVE_TYPE_OBJECT,
                &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setFromObject(arrayVar, 0, arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(returnValue),
            55) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(returnVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4122_bindRecordArrayInOut()
//   Verify that binding Record array IN/OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4122_bindRecordArrayInOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin pkg_TestRecords.TestInOutArrays(:1); end;";
    const char *outValueFormat = "Converted in/out record # %u";
    const char *arrayObjName = "PKG_TESTRECORDS.UDT_RECORDARRAY";
    const char *elementObjName = "PKG_TESTRECORDS.UDT_RECORD";
    const char *inValueFormat = "Test Record In %u";
    dpiObjectType *arrayObjType, *elementObjType;
    uint32_t i, numAttrs = 7, numElements = 10;
    dpiData *arrayValue, tempData, tempData2;
    dpiObject *arrayObj, *elementObj;
    dpiObjectAttr *attrs[7];
    dpiVar *arrayVar;
    char buffer[300];
    dpiBytes *bytes;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object types and attributes
    if (dpiConn_getObjectType(conn, arrayObjName, strlen(arrayObjName),
            &arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, elementObjName, strlen(elementObjName),
            &elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(elementObjType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create objects
    if (dpiObjectType_createObject(arrayObjType, &arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(elementObjType, &elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, arrayObjType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    for (i = 0; i < numElements; i++) {
        dpiData_setInt64(&tempData, i);
        if (dpiObject_setAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        sprintf(buffer, inValueFormat, i);
        dpiData_setBytes(&tempData, buffer, strlen(buffer));
        if (dpiObject_setAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData, 2017, 6, i + 1, 0, 0, 0, 0, 0, 0);
        if (dpiObject_setAttributeValue(elementObj, attrs[2],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData, 2018, 8, i + 1, 3, 2, 1, 0, 0, 0);
        if (dpiObject_setAttributeValue(elementObj, attrs[3],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setBool(&tempData, 1);
        if (dpiObject_setAttributeValue(elementObj, attrs[4],
                DPI_NATIVE_TYPE_BOOLEAN, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setInt64(&tempData, i);
        if (dpiObject_setAttributeValue(elementObj, attrs[5],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setInt64(&tempData, i);
        if (dpiObject_setAttributeValue(elementObj, attrs[6],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setObject(&tempData, elementObj);
        if (dpiObject_appendElement(arrayObj, DPI_NATIVE_TYPE_OBJECT,
                &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setFromObject(arrayVar, 0, arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    arrayObj = dpiData_getObject(arrayValue);
    if (dpiObject_getSize(arrayObj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numElements; i++) {
        if (dpiObject_getElementValueByIndex(arrayObj, i,
                DPI_NATIVE_TYPE_OBJECT, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        elementObj = dpiData_getObject(&tempData);
        if (dpiObject_getAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
                i * 10) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        bytes = dpiData_getBytes(&tempData);
        sprintf(buffer, outValueFormat, i);
        if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                buffer, strlen(buffer)) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[2],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData2, 2017, 6, i * 2 + 1, 0, 0, 0, 0, 0, 0);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData),
                dpiData_getTimestamp(&tempData2)) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[3],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData2, 2018, 8, i * 2 + 1, 3, 2, 1, 0, 0, 0);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData),
                dpiData_getTimestamp(&tempData2)) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[4],
                DPI_NATIVE_TYPE_BOOLEAN, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase,
                dpiData_getBool(&tempData), i % 2) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[5],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
                i * 10) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[6],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
                i * 10) < 0)
            return DPI_FAILURE;
        if (dpiObject_release(elementObj) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4123_bindRecordArrayOut()
//   Verify that binding Record array OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4123_bindRecordArrayOut(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin pkg_TestRecords.TestOutArrays(:1, :2); end;";
    const char *arrayObjName = "PKG_TESTRECORDS.UDT_RECORDARRAY";
    dpiData *numElementsValue, *arrayValue, tempData, tempData2;
    const char *elementObjName = "PKG_TESTRECORDS.UDT_RECORD";
    const char *outValueFormat = "Test OUT record # %u";
    dpiObjectType *arrayObjType, *elementObjType;
    uint32_t i, numAttrs = 7, numElements = 10;
    dpiVar *numElementsVar, *arrayVar;
    dpiObject *arrayObj, *elementObj;
    dpiObjectAttr *attrs[7];
    char buffer[300];
    dpiBytes *bytes;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;

    // get connection
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // get object types and attributes
    if (dpiConn_getObjectType(conn, arrayObjName, strlen(arrayObjName),
            &arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, elementObjName, strlen(elementObjName),
            &elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(elementObjType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numElementsVar, &numElementsValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, arrayObjType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate bind variables with values
    dpiData_setInt64(numElementsValue, numElements);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    arrayObj = dpiData_getObject(arrayValue);
    if (dpiObject_getSize(arrayObj, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, size, numElements) < 0)
        return DPI_FAILURE;
    for (i = 0; i < size; i++) {
        if (dpiObject_getElementValueByIndex(arrayObj, i,
                DPI_NATIVE_TYPE_OBJECT, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        elementObj = dpiData_getObject(&tempData);
        if (dpiObject_getAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
                i * 10) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        bytes = dpiData_getBytes(&tempData);
        sprintf(buffer, outValueFormat, i);
        if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
                buffer, strlen(buffer)) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[2],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData2, 2016, 2, 16 + i, 0, 0, 0, 0, 0, 0);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData),
                dpiData_getTimestamp(&tempData2)) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[3],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setTimestamp(&tempData2, 2016, 2, 16 + i, 18, 23, 55, 0, 0, 0);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData),
                dpiData_getTimestamp(&tempData2)) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[4],
                DPI_NATIVE_TYPE_BOOLEAN, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectUintEqual(testCase,
                dpiData_getBool(&tempData), i%2) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[5],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
                i * 7) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[6],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectIntEqual(testCase, dpiData_getInt64(&tempData),
                i * 5) < 0)
            return DPI_FAILURE;
        if (dpiObject_release(elementObj) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(arrayObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(elementObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_release(numElementsVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4124_bindClobIn()
//   Verify that binding CLOB IN works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4124_bindClobIn(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobIn(testCase, DPI_ORACLE_TYPE_CLOB, "CLOB",
            "In CLOB value (string)");
}


//-----------------------------------------------------------------------------
// dpiTest_4125_bindClobInOut()
//   Verify that binding CLOB IN/OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4125_bindClobInOut(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobInOut(testCase, DPI_ORACLE_TYPE_CLOB, "CLOB",
            "IN/OUT CLOB value (string)");
}



//-----------------------------------------------------------------------------
// dpiTest_4126_bindClobOut()
//   Verify that binding CLOB OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4126_bindClobOut(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobOut(testCase, DPI_ORACLE_TYPE_CLOB, "CLOB",
            "OUT CLOB");
}


//-----------------------------------------------------------------------------
// dpiTest_4127_bindNclobIn()
//   Verify that binding NCLOB IN works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4127_bindNclobIn(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobIn(testCase, DPI_ORACLE_TYPE_NCLOB, "NCLOB",
            "In NCLOB value (national string)");
}


//-----------------------------------------------------------------------------
// dpiTest_4128_bindNclobInOut()
//   Verify that binding NCLOB IN/OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4128_bindNclobInOut(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobInOut(testCase, DPI_ORACLE_TYPE_NCLOB, "NCLOB",
            "IN/OUT NCLOB value (national string)");
}



//-----------------------------------------------------------------------------
// dpiTest_4129_bindNclobOut()
//   Verify that binding NCLOB OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4129_bindNclobOut(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobOut(testCase, DPI_ORACLE_TYPE_NCLOB, "NCLOB",
            "OUT NCLOB");
}


//-----------------------------------------------------------------------------
// dpiTest_4130_bindBlobIn()
//   Verify that binding BLOB IN works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4130_bindBlobIn(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobIn(testCase, DPI_ORACLE_TYPE_BLOB, "BLOB",
            "In BLOB value (raw)");
}


//-----------------------------------------------------------------------------
// dpiTest_4131_bindBlobInOut()
//   Verify that binding BLOB IN/OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4131_bindBlobInOut(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobInOut(testCase, DPI_ORACLE_TYPE_BLOB, "BLOB",
            "IN/OUT BLOB value (raw)");
}



//-----------------------------------------------------------------------------
// dpiTest_4132_bindBlobOut()
//   Verify that binding BLOB OUT works as expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_4132_bindBlobOut(dpiTestCase *testCase, dpiTestParams *params)
{
    return dpiTest__bindLobOut(testCase, DPI_ORACLE_TYPE_BLOB, "BLOB",
            "OUT BLOB");
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(4100);
    dpiTestSuite_addCase(dpiTest_4100_bindUnicodeArrayIn,
            "test PL/SQL bind of unicode array (IN)");
    dpiTestSuite_addCase(dpiTest_4101_bindUnicodeArrayInOut,
            "test PL/SQL bind of unicode array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4102_bindUnicodeArrayOut,
            "test PL/SQL bind of unicode array (OUT)");
    dpiTestSuite_addCase(dpiTest_4103_bindNumberArrayIn,
            "test PL/SQL bind of number array (IN)");
    dpiTestSuite_addCase(dpiTest_4104_bindNumberArrayInOut,
            "test PL/SQL bind of number array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4105_bindNumberArrayOut,
            "test PL/SQL bind of number array (OUT)");
    dpiTestSuite_addCase(dpiTest_4106_bindDateArrayIn,
            "test PL/SQL bind of date array (IN)");
    dpiTestSuite_addCase(dpiTest_4107_bindDateArrayInOut,
            "test PL/SQL bind of date array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4108_bindDateArrayOut,
            "test PL/SQL bind of date array (OUT)");
    dpiTestSuite_addCase(dpiTest_4109_bindObjArrayIn,
            "test PL/SQL bind of object array (IN)");
    dpiTestSuite_addCase(dpiTest_4110_bindObjArrayInOut,
            "test PL/SQL bind of object array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4111_bindObjArrayOut,
            "test PL/SQL bind of object array (OUT)");
    dpiTestSuite_addCase(dpiTest_4112_bindNumberVarrayIn,
            "test PL/SQL bind of number varray (IN)");
    dpiTestSuite_addCase(dpiTest_4113_bindNumberVarrayInOut,
            "test PL/SQL bind of number varray (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4114_bindNumberVarrayOut,
            "test PL/SQL bind of number varray (OUT)");
    dpiTestSuite_addCase(dpiTest_4115_bindStringArrayIn,
            "test PL/SQL bind of string array (IN)");
    dpiTestSuite_addCase(dpiTest_4116_bindStringArrayInOut,
            "test PL/SQL bind of string array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4117_bindStringArrayOut,
            "test PL/SQL bind of string array (OUT)");
    dpiTestSuite_addCase(dpiTest_4118_bindRecordIn,
            "test PL/SQL bind of record (IN)");
    dpiTestSuite_addCase(dpiTest_4119_bindRecordInOut,
            "test PL/SQL bind of record (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4120_bindRecordOut,
            "test PL/SQL bind of record (OUT)");
    dpiTestSuite_addCase(dpiTest_4121_bindRecordArrayIn,
            "test PL/SQL bind of record array (IN)");
    dpiTestSuite_addCase(dpiTest_4122_bindRecordArrayInOut,
            "test PL/SQL bind of record array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4123_bindRecordArrayOut,
            "test PL/SQL bind of record array (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4124_bindClobIn,
            "test PL/SQL bind of CLOBs (IN)");
    dpiTestSuite_addCase(dpiTest_4125_bindClobInOut,
            "test PL/SQL bind of CLOBs (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4126_bindClobOut,
            "test PL/SQL bind of CLOBs (OUT)");
    dpiTestSuite_addCase(dpiTest_4127_bindNclobIn,
            "test PL/SQL bind of NCLOBs (IN)");
    dpiTestSuite_addCase(dpiTest_4128_bindNclobInOut,
            "test PL/SQL bind of NCLOBs (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4129_bindNclobOut,
            "test PL/SQL bind of NCLOBs (OUT)");
    dpiTestSuite_addCase(dpiTest_4130_bindBlobIn,
            "test PL/SQL bind of BLOBs (IN)");
    dpiTestSuite_addCase(dpiTest_4131_bindBlobInOut,
            "test PL/SQL bind of BLOBs (IN/OUT)");
    dpiTestSuite_addCase(dpiTest_4132_bindBlobOut,
            "test PL/SQL bind of BLOBs (OUT)");
    return dpiTestSuite_run();
}
