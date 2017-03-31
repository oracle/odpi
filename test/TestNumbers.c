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
// TestNumbers.c
//   Test suite for testing the handling of numbers.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_200_bindLargeUintAsOracleNumber()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) is transferred to Oracle and returned from Oracle
// successfully as an Oracle number.
//-----------------------------------------------------------------------------
int dpiTest_200_bindLargeUintAsOracleNumber(dpiTestCase *testCase,
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
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_201_bindLargeUintAsNativeUint()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) is transferred to Oracle and returned from Oracle
// successfully as a native integer.
//-----------------------------------------------------------------------------
int dpiTest_201_bindLargeUintAsNativeUint(dpiTestCase *testCase,
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
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_202_fetchLargeUintAsOracleNumber()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) can be fetched from Oracle successfully as an Oracle
// number.
//-----------------------------------------------------------------------------
int dpiTest_202_fetchLargeUintAsOracleNumber(dpiTestCase *testCase,
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
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_203_fetchLargeUintAsNativeUint()
//   Verify that a large unsigned integer (larger than can be represented by
// a signed integer) can be fetched from Oracle successfully as a native
// unsigned integer.
//-----------------------------------------------------------------------------
int dpiTest_203_fetchLargeUintAsNativeUint(dpiTestCase *testCase,
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
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(200);
    dpiTestSuite_addCase(dpiTest_200_bindLargeUintAsOracleNumber,
            "bind large unsigned integer as Oracle number");
    dpiTestSuite_addCase(dpiTest_201_bindLargeUintAsNativeUint,
            "bind large unsigned integer as native unsigned integer");
    dpiTestSuite_addCase(dpiTest_202_fetchLargeUintAsOracleNumber,
            "fetch large unsigned integer as Oracle number");
    dpiTestSuite_addCase(dpiTest_203_fetchLargeUintAsNativeUint,
            "fetch large unsigned integer as native unsigned integer");

    return dpiTestSuite_run();
}

