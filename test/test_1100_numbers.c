//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2022, Oracle and/or its affiliates.
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
// test_1100_numbers.c
//   Test suite for testing the handling of numbers.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_1100_bindLargeUintAsOracleNumber()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) is transferred to Oracle and returned from Oracle
// successfully as an Oracle number.
//-----------------------------------------------------------------------------
int dpiTest_1100_bindLargeUintAsOracleNumber(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t numQueryColumns, bufferRowIndex;
    const char *sql = "select :1 from dual";
    dpiData *varData;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *var;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64,
            1, 0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    varData->isNull = 0;
    varData->value.asUint64 = 18446744073709551615UL;
    if (dpiStmt_bindByPos(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES,
            1, 0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_define(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, varData->value.asBytes.ptr,
            varData->value.asBytes.length, "18446744073709551615", 20) < 0)
        return DPI_FAILURE;
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1101_bindLargeUintAsNativeUint()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) is transferred to Oracle and returned from Oracle
// successfully as a native integer.
//-----------------------------------------------------------------------------
int dpiTest_1101_bindLargeUintAsNativeUint(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t numQueryColumns, bufferRowIndex;
    const char *sql = "select :1 from dual";
    dpiData *varData;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *var;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_UINT,
            DPI_NATIVE_TYPE_UINT64, 1, 0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    varData->isNull = 0;
    varData->value.asUint64 = 18446744073709551614UL;
    if (dpiStmt_bindByPos(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES,
            1, 0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_define(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, varData->value.asBytes.ptr,
            varData->value.asBytes.length, "18446744073709551614", 20) < 0)
        return DPI_FAILURE;
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1102_fetchLargeUintAsOracleNumber()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) can be fetched from Oracle successfully as an Oracle
// number.
//-----------------------------------------------------------------------------
int dpiTest_1102_fetchLargeUintAsOracleNumber(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select 18446744073709551612 from dual";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiData *varData;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *var;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64,
            1, 0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_define(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, varData->value.asUint64,
            18446744073709551612UL) < 0)
        return DPI_FAILURE;
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1103_fetchLargeUintAsNativeUint()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) can be fetched from Oracle successfully as a native
// unsigned integer.
//-----------------------------------------------------------------------------
int dpiTest_1103_fetchLargeUintAsNativeUint(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select 18446744073709551613 from dual";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiData *varData;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *var;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_UINT,
            DPI_NATIVE_TYPE_UINT64, 1, 0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_define(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, varData->value.asUint64,
            18446744073709551613UL) < 0)
        return DPI_FAILURE;
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1104_bindZeroFromString()
//   Verify that the value zero is returned properly when converted from a
// string representing the number zero. Test with varying numbers of trailing
// zeroes, with and without a decimal point.
//-----------------------------------------------------------------------------
int dpiTest_1104_bindZeroFromString(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *values[] = { "0", "0.0", "0.00", "0.000" };
    const char *sql = "select :1 from dual";
    uint32_t numQueryColumns, bufferRowIndex;
    dpiData *inputVarData, *resultVarData;
    dpiVar *inputVar, *resultVar;
    dpiConn *conn;
    dpiStmt *stmt;
    int found, i;

    // create variables and prepare statement for execution
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES, 1,
            0, 0, 0, NULL, &inputVar, &inputVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 1,
            0, 0, 0, NULL, &resultVar, &resultVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inputVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // test each of 4 different values for zero
    for (i = 0; i < 4; i++) {
        if (dpiVar_setFromBytes(inputVar, 0, values[i], strlen(values[i])) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_define(stmt, 1, resultVar) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectDoubleEqual(testCase,
                resultVarData->value.asDouble, 0.0) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiVar_release(inputVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(resultVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1105_bindNumberAsString()
//   Verify that values are bound as string and returned from the database with
// the same value.
//-----------------------------------------------------------------------------
int dpiTest_1105_bindNumberAsString(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *outValues[] = { "400000000", "1521000000000000",
            "5478000000000000000", "100000000000",
            "-1234567890123456789012345678901234567.8", "0", "0", "0", "0",
            "0.00000001", "0.000000001", NULL };
    const char *inValues[] = { "4E+8", "1.521E+15", "5.478E+18", "1E+11",
            "-1234567890123456789012345678901234567.8", "0", "-0", "0.0",
            "-0.0", "1e-08", "1e-09", NULL };
    const char *sql = "select :1 from dual";
    dpiData *inputVarData, *resultVarData;
    dpiVar *inputVar, *resultVar;
    uint32_t bufferRowIndex;
    dpiConn *conn;
    dpiStmt *stmt;
    int found, i;

    // create variables and prepare statement for execution
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES, 1,
            0, 0, 0, NULL, &inputVar, &inputVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES, 1,
            0, 0, 0, NULL, &resultVar, &resultVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inputVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // test each of the different values
    for (i = 0; inValues[i]; i++) {
        if (dpiVar_setFromBytes(inputVar, 0, inValues[i],
                strlen(inValues[i])) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_define(stmt, 1, resultVar) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectStringEqual(testCase,
                resultVarData->value.asBytes.ptr,
                resultVarData->value.asBytes.length,
                outValues[i], strlen(outValues[i])) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiVar_release(inputVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(resultVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1100);
    dpiTestSuite_addCase(dpiTest_1100_bindLargeUintAsOracleNumber,
            "bind large unsigned integer as Oracle number");
    dpiTestSuite_addCase(dpiTest_1101_bindLargeUintAsNativeUint,
            "bind large unsigned integer as native unsigned integer");
    dpiTestSuite_addCase(dpiTest_1102_fetchLargeUintAsOracleNumber,
            "fetch large unsigned integer as Oracle number");
    dpiTestSuite_addCase(dpiTest_1103_fetchLargeUintAsNativeUint,
            "fetch large unsigned integer as native unsigned integer");
    dpiTestSuite_addCase(dpiTest_1104_bindZeroFromString,
            "bind zero as a string value with trailing zeroes");
    dpiTestSuite_addCase(dpiTest_1105_bindNumberAsString,
            "bind numbers as strings and verify round trip");

    return dpiTestSuite_run();
}
