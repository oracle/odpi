//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestVariables.c
//   Test suite for testing variable creation and binding.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define MAX_ARRAY_SIZE                  3

//-----------------------------------------------------------------------------
// dpiTest_1000_varWithMaxArrSize0()
//   Create a variable specifying the maxArraySize parameter as 0
// (error DPI-1031).
//-----------------------------------------------------------------------------
int dpiTest_1000_varWithMaxArrSize0(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 0, 0,
            0, 0, NULL, &var, &data);
    return dpiTestCase_expectError(testCase,
            "DPI-1031: array size cannot be zero");
}


//-----------------------------------------------------------------------------
// dpiTest_1001_invalidOracleTypeNum()
//   Create a variable specifying a value for the parameter oracleTypeNum
// which is not part of the enumeration dpiOracleTypeNum (error DPI-1021).
//-----------------------------------------------------------------------------
int dpiTest_1001_invalidOracleTypeNum(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, 1000, DPI_NATIVE_TYPE_INT64, MAX_ARRAY_SIZE, 0, 0, 0,
            NULL, &var, &data);
    return dpiTestCase_expectError(testCase,
            "DPI-1021: Oracle type 1000 is invalid");
}


//-----------------------------------------------------------------------------
// dpiTest_1002_incompatibleValsForParams()
//   Create a variable specifying values for the parameters oracleTypeNum and
// nativeTypeNum which are not compatible with each other (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_1002_incompatibleValsForParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data);
    return dpiTestCase_expectError(testCase,
            "DPI-1014: conversion between Oracle type 2012 and native type "
            "3000 is not implemented");
}


//-----------------------------------------------------------------------------
// dpiTest_1003_validValsForArrsButNotSupported()
//   Create a variable specifying isArray as 1 and valid values for the
// parameters oracleTypeNum and nativeTypeNum, but that are not supported in
// arrays (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1003_validValsForArrsButNotSupported(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_BOOLEAN, DPI_NATIVE_TYPE_BOOLEAN,
            MAX_ARRAY_SIZE, 0, 0, 1, NULL, &var, &data);
    return dpiTestCase_expectError(testCase, "DPI-1013: not supported");
}


//-----------------------------------------------------------------------------
// dpiTest_1004_maxArrSizeTooLarge()
//   Create a variable specifying values for maxArraySize and sizeInBytes that
// when multiplied together would result in an integer that exceeds INT_MAX
// (error DPI-1015).
//-----------------------------------------------------------------------------
int dpiTest_1004_maxArrSizeTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t maxArrSize = 4294967295, size = 2;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            maxArrSize, size, 0, 0, NULL, &var, &data);
    return dpiTestCase_expectError(testCase,
            "DPI-1015: array size of 4294967295 is too large");
}


//-----------------------------------------------------------------------------
// dpiTest_1005_setFromBytesNotSupported()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_BYTES and
// then call dpiVar_setFromBytes() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1005_setFromBytesNotSupported(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *strVal = "string1";
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromBytes(var, 0, strVal, strlen(strVal));
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1006_setFromBytesValueTooLarge()
//   Create a variable that does use native type DPI_NATIVE_TYPE_BYTES and then
// call dpiVar_setFromBytes() with a valueLength that exceeds the size
// specified when the variable was created (error DPI-1019).
//-----------------------------------------------------------------------------
int dpiTest_1006_setFromBytesValueTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *strVal = "string1";
    dpiData *data;
    dpiVar *var;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            MAX_ARRAY_SIZE, 2, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromBytes(var, 0, strVal, strlen(strVal));
    if (dpiTestCase_expectError(testCase,
            "DPI-1019: buffer size of 2 is too small") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1007_setFromBytesPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_BYTES; call
// dpiVar_setFromBytes() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1007_setFromBytesPositionTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *strVal = "string1";
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromBytes(var, 4, strVal, strlen(strVal));
    if (dpiTestCase_expectError(testCase,
            "DPI-1009: zero-based position 4 is not valid with max array "
            "size of 3") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1008_setFromLobUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_LOB and
// then call dpiVar_setFromLob() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1008_setFromLobUnsupportedType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *lobStr = "dpiTest";
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_setFromBytes(lob, lobStr, strlen(lobStr)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromLob(var, 0, lob);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiLob_release(lob);
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1009_setFromLobPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_LOB; call
// dpiVar_setFromLob() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1009_setFromLobPositionTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *lobStr = "dpiTest";
    dpiData *lobValue;
    dpiVar *lobVar;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_setFromBytes(lob, lobStr, strlen(lobStr)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_CLOB, DPI_NATIVE_TYPE_LOB,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &lobVar, &lobValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromLob(lobVar, 3, lob);
    if (dpiTestCase_expectError(testCase,
            "DPI-1009: zero-based position 3 is not valid with max array "
            "size of 3") < 0)
        return DPI_FAILURE;
    dpiVar_release(lobVar);
    dpiLob_release(lob);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1010_setFromObjectUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_OBJECT and
// then call dpiVar_setFromObject() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1010_setFromObjectUnsupportedType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "UDT_OBJECT";
    dpiObjectType *objType;
    dpiObject *obj;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromObject(var, 0, obj);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiObject_release(obj);
    dpiObjectType_release(objType);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1011_setFromObjectPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_OBJECT; call
// dpiVar_setFromObject() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1011_setFromObjectPositionTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *objStr = "UDT_OBJECT";
    uint32_t maxArrSize = 1;
    dpiObjectType *objType;
    dpiData *objectValue;
    dpiVar *objectVar;
    dpiObject *obj;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getObjectType(conn, objStr, strlen(objStr), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT,
            maxArrSize, 0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromObject(objectVar, 1, obj);
    if (dpiTestCase_expectError(testCase,
            "DPI-1009: zero-based position 1 is not valid with max array "
            "size of 1") < 0)
        return DPI_FAILURE;
    dpiVar_release(objectVar);
    dpiObject_release(obj);
    dpiObjectType_release(objType);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1012_setFromRowidUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_ROWID and
// then call dpiVar_setFromRowid() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1012_setFromRowidUnsupportedType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiRowid *rowid = NULL;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromRowid(var, 0, rowid);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1013_setFromRowidPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_ROWID; call
// dpiVar_setFromRowid() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1013_setFromRowidPositionTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiRowid *rowid = NULL;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_ROWID, DPI_NATIVE_TYPE_ROWID,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromRowid(var, 3, rowid);
    if (dpiTestCase_expectError(testCase,
            "DPI-1009: zero-based position 3 is not valid with max array "
            "size of 3") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1014_setFromStmtUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_STMT and
// then call dpiVar_setFromStmt() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1014_setFromStmtUnsupportedType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiStmt *stmt = NULL;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromStmt(var, 0, stmt);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1015_setFromStmtPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_STMT; call
// dpiVar_setFromStmt() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1015_setFromStmtPositionTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiStmt *stmt = NULL;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_STMT, DPI_NATIVE_TYPE_STMT,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromStmt(var, 3, stmt);
    if (dpiTestCase_expectError(testCase,
            "DPI-1009: zero-based position 3 is not valid with max array "
            "size of 3") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1016_objectVarWithNullType()
//   Create a variable that uses native type DPI_NATIVE_TYPE_OBJECT but the
// object type parameter is set to NULL (error DPI-1025).
//-----------------------------------------------------------------------------
int dpiTest_1016_objectVarWithNullType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiData *objectValue;
    dpiVar *objectVar;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &objectVar, &objectValue);
    if (dpiTestCase_expectError(testCase,
            "DPI-1025: no object type specified for object variable") < 0)
        return DPI_FAILURE;
    dpiVar_release(objectVar);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1017_stmtDefineInvalidPositions()
//   Prepare and execute a query; call dpiStmt_define() with position set to 0
// and with position set to a value that exceeds the number of columns that are
// available in the query (error DPI-1028).
//-----------------------------------------------------------------------------
int dpiTest_1017_stmtDefineInvalidPositions(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    uint32_t numQueryColumns;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_define(stmt, 0, var);
    if (dpiTestCase_expectError(testCase,
            "DPI-1028: query position 0 is invalid") < 0)
        return DPI_FAILURE;
    dpiStmt_define(stmt, 3, var);
    if (dpiTestCase_expectError(testCase,
            "DPI-1028: query position 3 is invalid") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1018_stmtDefineWithNullVar()
//   Prepare and execute a query; call dpistmt_define() with the variable set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1018_stmtDefineWithNullVar(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    uint32_t numQueryColumns;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_define(stmt, 1, NULL);
    if (dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiVar handle") < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1019_bindByPosWithPosition0()
//   Prepare and execute a statement with bind variables identified in the
// statement text; create a variable and call dpiStmt_bindByPos() with the
// position parameter set to 0 (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1019_bindByPosWithPosition0(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select :1 from dual";
    uint32_t maxArrSize = 1;
    dpiData *varData;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64,
            maxArrSize, 0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_bindByPos(stmt, 0, var);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1020_copyDataWithInvalidPosition()
//   Create two variables with the same native type; call dpiVar_copyData()
// with the position parameter set to a value that exceeds the maxArraySize of
// at least one of the variables (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1020_copyDataWithInvalidPosition(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiData *data1, *data2;
    dpiVar *var1, *var2;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var1, &data1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var2, &data2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_copyData(var1, 3, var2, 0);
    if (dpiTestCase_expectError(testCase,
            "DPI-1009: zero-based position 3 is not valid with max array "
            "size of 3") < 0)
        return DPI_FAILURE;
    dpiVar_release(var1);
    dpiVar_release(var2);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1021_copyDataWithDifferentVarTypes()
//   Create two variables with different native types; call dpiVar_copyData()
// with either variable as the source (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1021_copyDataWithDifferentVarTypes(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiData *intColValue, *longColValue;
    dpiVar *intColVar, *longColVar;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_LONG_VARCHAR,
            DPI_NATIVE_TYPE_BYTES, MAX_ARRAY_SIZE, 0, 0, 0, NULL, &longColVar,
            &longColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_copyData(longColVar, 0, intColVar, 0);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiVar_release(intColVar);
    dpiVar_release(longColVar);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1022_setNumElementsInArrayTooLarge()
//   Create an array variable of any type; call dpiVar_setNumElementsInArray()
// with a value for the numElements parameter that exceeds the maxArraySize
// value that was used to create the variable (DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_1022_setNumElementsInArrayTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t numElements = 4;
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setNumElementsInArray(var, numElements);
    if (dpiTestCase_expectError(testCase,
            "DPI-1018: array size of 3 is too small") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1023_bindByNameWithNameLen0()
//   Prepare and execute a statement with bind variables identified in the
// statement text; create a variable and call dpiStmt_bindByName() with the
// nameLength parameter set to 0 (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1023_bindByNameWithNameLen0(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select :test from dual";
    dpiData *varData;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64, 1,
            0, 0, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_bindByName(stmt, "test", 0, var);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1000);
    dpiTestSuite_addCase(dpiTest_1000_varWithMaxArrSize0,
            "dpiConn_newVar() with max array size as 0");
    dpiTestSuite_addCase(dpiTest_1001_invalidOracleTypeNum,
            "dpiConn_newVar() with an invalid value for Oracle type");
    dpiTestSuite_addCase(dpiTest_1002_incompatibleValsForParams,
            "dpiConn_newVar() with incompatible values for Oracle and native "
            "types");
    dpiTestSuite_addCase(dpiTest_1003_validValsForArrsButNotSupported,
            "dpiConn_newVar() with invalid array type for array");
    dpiTestSuite_addCase(dpiTest_1004_maxArrSizeTooLarge,
            "dpiConn_newVar() with max array size that is too large");
    dpiTestSuite_addCase(dpiTest_1005_setFromBytesNotSupported,
            "dpiVar_setFromBytes() with unsupported variable");
    dpiTestSuite_addCase(dpiTest_1006_setFromBytesValueTooLarge,
            "dpiVar_setFromBytes() with value too large");
    dpiTestSuite_addCase(dpiTest_1007_setFromBytesPositionTooLarge,
            "dpiVar_setFromBytes() with position too large");
    dpiTestSuite_addCase(dpiTest_1008_setFromLobUnsupportedType,
            "dpiVar_setFromLob() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1009_setFromLobPositionTooLarge,
            "dpiVar_setFromLob() with position too large");
    dpiTestSuite_addCase(dpiTest_1010_setFromObjectUnsupportedType,
            "dpiVar_setFromObject() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1011_setFromObjectPositionTooLarge,
            "dpiVar_setFromObject() with position too large");
    dpiTestSuite_addCase(dpiTest_1012_setFromRowidUnsupportedType,
            "dpiVar_setFromRowid() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1013_setFromRowidPositionTooLarge,
            "dpiVar_setFromRowid() with position too large");
    dpiTestSuite_addCase(dpiTest_1014_setFromStmtUnsupportedType,
            "dpiVar_setFromStmt() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1015_setFromStmtPositionTooLarge,
            "dpiVar_setFromStmt() with position too large");
    dpiTestSuite_addCase(dpiTest_1016_objectVarWithNullType,
            "dpiConn_newVar() with NULL object type for object variable");
    dpiTestSuite_addCase(dpiTest_1017_stmtDefineInvalidPositions,
            "dpiStmt_define() with invalid positions");
    dpiTestSuite_addCase(dpiTest_1018_stmtDefineWithNullVar,
            "dpiStmt_define() with NULL variable");
    dpiTestSuite_addCase(dpiTest_1019_bindByPosWithPosition0,
            "dpiStmt_bindByPos() with position 0");
    dpiTestSuite_addCase(dpiTest_1020_copyDataWithInvalidPosition,
            "dpiVar_copyData() with invalid position");
    dpiTestSuite_addCase(dpiTest_1021_copyDataWithDifferentVarTypes,
            "dpiVar_copyData() with different variable types");
    dpiTestSuite_addCase(dpiTest_1022_setNumElementsInArrayTooLarge,
            "dpiVar_setNumElementsInArray() with value too large");
    dpiTestSuite_addCase(dpiTest_1023_bindByNameWithNameLen0,
            "dpiStmt_bindByName() with name length parameter 0");
    return dpiTestSuite_run();
}

