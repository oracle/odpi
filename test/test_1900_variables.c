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
// test_1900_variables.c
//   Test suite for testing variable creation and binding.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define MAX_ARRAY_SIZE                  3

//-----------------------------------------------------------------------------
// dpiTest_1900_varWithMaxArrSize0()
//   Create a variable specifying the maxArraySize parameter as 0
// (error DPI-1031).
//-----------------------------------------------------------------------------
int dpiTest_1900_varWithMaxArrSize0(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 0, 0,
            0, 0, NULL, &var, &data);
    return dpiTestCase_expectError(testCase, "DPI-1031:");
}


//-----------------------------------------------------------------------------
// dpiTest_1901_invalidOracleTypeNum()
//   Create a variable specifying a value for the parameter oracleTypeNum
// which is not part of the enumeration dpiOracleTypeNum (error DPI-1021).
//-----------------------------------------------------------------------------
int dpiTest_1901_invalidOracleTypeNum(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, 1000, DPI_NATIVE_TYPE_INT64, MAX_ARRAY_SIZE, 0, 0, 0,
            NULL, &var, &data);
    return dpiTestCase_expectError(testCase, "DPI-1021:");
}


//-----------------------------------------------------------------------------
// dpiTest_1902_incompatibleValsForParams()
//   Create a variable specifying values for the parameters oracleTypeNum and
// nativeTypeNum which are not compatible with each other (error DPI-1014).
//-----------------------------------------------------------------------------
int dpiTest_1902_incompatibleValsForParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &var, &data);
    return dpiTestCase_expectError(testCase, "DPI-1014:");
}


//-----------------------------------------------------------------------------
// dpiTest_1903_validValsForArrsButNotSupported()
//   Create a variable specifying isArray as 1 and valid values for the
// parameters oracleTypeNum and nativeTypeNum, but that are not supported in
// arrays (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1903_validValsForArrsButNotSupported(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_BOOLEAN, DPI_NATIVE_TYPE_BOOLEAN,
            MAX_ARRAY_SIZE, 0, 0, 1, NULL, &var, &data);
    return dpiTestCase_expectError(testCase, "DPI-1013:");
}


//-----------------------------------------------------------------------------
// dpiTest_1904_maxArrSizeTooLarge()
//   Create a variable specifying values for maxArraySize and sizeInBytes that
// when multiplied together would result in an integer that exceeds INT_MAX
// (error DPI-1015).
//-----------------------------------------------------------------------------
int dpiTest_1904_maxArrSizeTooLarge(dpiTestCase *testCase,
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
    return dpiTestCase_expectError(testCase, "DPI-1015:");
}


//-----------------------------------------------------------------------------
// dpiTest_1905_setFromBytesNotSupported()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_BYTES and
// then call dpiVar_setFromBytes() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1905_setFromBytesNotSupported(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1906_setFromBytesValueTooLarge()
//   Create a variable that does use native type DPI_NATIVE_TYPE_BYTES and then
// call dpiVar_setFromBytes() with a valueLength that exceeds the size
// specified when the variable was created (error DPI-1019).
//-----------------------------------------------------------------------------
int dpiTest_1906_setFromBytesValueTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *strVal = "string_that_is_too_long";
    dpiData *data;
    dpiVar *var;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            MAX_ARRAY_SIZE, 2, 0, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromBytes(var, 0, strVal, strlen(strVal));
    if (dpiTestCase_expectError(testCase, "DPI-1019:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1907_setFromBytesPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_BYTES; call
// dpiVar_setFromBytes() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1907_setFromBytesPositionTooLarge(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1009:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1908_setFromLobUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_LOB and
// then call dpiVar_setFromLob() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1908_setFromLobUnsupportedType(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiLob_release(lob);
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1909_setFromLobPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_LOB; call
// dpiVar_setFromLob() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1909_setFromLobPositionTooLarge(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1009:") < 0)
        return DPI_FAILURE;
    dpiVar_release(lobVar);
    dpiLob_release(lob);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1910_setFromObjectUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_OBJECT and
// then call dpiVar_setFromObject() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1910_setFromObjectUnsupportedType(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiObject_release(obj);
    dpiObjectType_release(objType);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1911_setFromObjectPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_OBJECT; call
// dpiVar_setFromObject() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1911_setFromObjectPositionTooLarge(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1009:") < 0)
        return DPI_FAILURE;
    dpiVar_release(objectVar);
    dpiObject_release(obj);
    dpiObjectType_release(objType);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1912_setFromRowidUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_ROWID and
// then call dpiVar_setFromRowid() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1912_setFromRowidUnsupportedType(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1913_setFromRowidPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_ROWID; call
// dpiVar_setFromRowid() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1913_setFromRowidPositionTooLarge(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1009:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1914_setFromStmtUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_STMT and
// then call dpiVar_setFromStmt() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1914_setFromStmtUnsupportedType(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1915_setFromStmtPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_STMT; call
// dpiVar_setFromStmt() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1915_setFromStmtPositionTooLarge(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1009:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1916_objectVarWithNullType()
//   Create a variable that uses native type DPI_NATIVE_TYPE_OBJECT but the
// object type parameter is set to NULL (error DPI-1025).
//-----------------------------------------------------------------------------
int dpiTest_1916_objectVarWithNullType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiData *objectValue;
    dpiVar *objectVar;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &objectVar, &objectValue);
    if (dpiTestCase_expectError(testCase, "DPI-1025:") < 0)
        return DPI_FAILURE;
    dpiVar_release(objectVar);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1917_stmtDefineInvalidPositions()
//   Prepare and execute a query; call dpiStmt_define() with position set to 0
// and with position set to a value that exceeds the number of columns that are
// available in the query (error DPI-1028).
//-----------------------------------------------------------------------------
int dpiTest_1917_stmtDefineInvalidPositions(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1028:") < 0)
        return DPI_FAILURE;
    dpiStmt_define(stmt, 3, var);
    if (dpiTestCase_expectError(testCase, "DPI-1028:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1918_stmtDefineWithNullVar()
//   Prepare and execute a query; call dpistmt_define() with the variable set
// to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1918_stmtDefineWithNullVar(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1919_bindByPosWithPosition0()
//   Prepare and execute a statement with bind variables identified in the
// statement text; create a variable and call dpiStmt_bindByPos() with the
// position parameter set to 0 (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1919_bindByPosWithPosition0(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1920_copyDataWithInvalidPosition()
//   Create two variables with the same native type; call dpiVar_copyData()
// with the position parameter set to a value that exceeds the maxArraySize of
// at least one of the variables (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1920_copyDataWithInvalidPosition(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1009:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var1);
    dpiVar_release(var2);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1921_copyDataWithDifferentVarTypes()
//   Create two variables with different native types; call dpiVar_copyData()
// with either variable as the source (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1921_copyDataWithDifferentVarTypes(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiVar_release(intColVar);
    dpiVar_release(longColVar);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1922_setNumElementsInArrayTooLarge()
//   Create an array variable of any type; call dpiVar_setNumElementsInArray()
// with a value for the numElements parameter that exceeds the maxArraySize
// value that was used to create the variable (DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_1922_setNumElementsInArrayTooLarge(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1018:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1923_bindByNameWithNameLen0()
//   Prepare and execute a statement with bind variables identified in the
// statement text; create a variable and call dpiStmt_bindByName() with the
// nameLength parameter set to 0 (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1923_bindByNameWithNameLen0(dpiTestCase *testCase,
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
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    dpiVar_release(var);
    dpiStmt_release(stmt);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1924_verifySetFromBytesWithValueLenAsZero()
//   Verify dpiVar_setFromBytes() allows the value parameter to be NULL when
// the valueLength parameter is zero.
//-----------------------------------------------------------------------------
int dpiTest_1924_verifySetFromBytesWithValueLenAsZero(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            MAX_ARRAY_SIZE, 100, 1, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(var, 0, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1925_verifySetFromBytesWithValueLenAsNonZero()
//   Verify dpiVar_setFromBytes() does not allow the value parameter to be NULL
// when the valueLength is set to a non-zero value.
//-----------------------------------------------------------------------------
int dpiTest_1925_verifySetFromBytesWithValueLenAsNonZero(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiData *data;
    dpiVar *var;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            MAX_ARRAY_SIZE, 100, 1, 0, NULL, &var, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromBytes(var, 0, NULL, 1);
    if (dpiTestCase_expectError(testCase, "DPI-1053:") < 0)
        return DPI_FAILURE;
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1926_setFromJsonUnsupportedType()
//   Create a variable that does not use native type DPI_NATIVE_TYPE_JSON and
// then call dpiVar_setFromJson() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1926_setFromJsonUnsupportedType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *strVal = "{\"key\":1026}";
    dpiData *intData, *jsonData;
    dpiVar *intVar, *jsonVar;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON,
            1, 0, 0, 0, NULL, &jsonVar, &jsonData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &intVar, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_setFromText(dpiData_getJson(jsonData), strVal,
            strlen(strVal), 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromJson(intVar, 0, dpiData_getJson(jsonData));
    if (dpiTestCase_expectError(testCase, "DPI-1013:") < 0)
        return DPI_FAILURE;
    if (dpiVar_release(jsonVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1927_setFromJsonPositionTooLarge()
//   Create a variable that uses native type DPI_NATIVE_TYPE_JSON; call
// dpiVar_setFromJson() with position >= the value for maxArraySize used when
// the variable was created (error DPI-1009).
//-----------------------------------------------------------------------------
int dpiTest_1927_setFromJsonPositionTooLarge(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *strVal = "{\"key\":1027}";
    dpiData *jsonData1, *jsonData2;
    dpiVar *jsonVar1, *jsonVar2;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON, 1, 0,
            0, 0, NULL, &jsonVar1, &jsonData1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &jsonVar2, &jsonData2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_setFromText(dpiData_getJson(jsonData1), strVal,
            strlen(strVal), 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiVar_setFromJson(jsonVar2, 3, dpiData_getJson(jsonData1));
    if (dpiTestCase_expectError(testCase, "DPI-1009:") < 0)
        return DPI_FAILURE;
    if (dpiVar_release(jsonVar1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(jsonVar2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1928_setFromJsonWithValidArguments()
//   Create a variable that uses native type DPI_NATIVE_TYPE_JSON; call
// dpiVar_setFromJson() with valid arguments.
//-----------------------------------------------------------------------------
int dpiTest_1928_setFromJsonWithValidArguments(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *strVal = "{\"key\":1028}";
    dpiData *jsonData1, *jsonData2;
    dpiVar *jsonVar1, *jsonVar2;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON,
            1, 0, 0, 0, NULL, &jsonVar1, &jsonData1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON,
            MAX_ARRAY_SIZE, 0, 0, 0, NULL, &jsonVar2, &jsonData2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiJson_setFromText(dpiData_getJson(jsonData1), strVal,
            strlen(strVal), 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromJson(jsonVar2, 0, dpiData_getJson(jsonData1)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(jsonVar1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(jsonVar2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1929_verifySetFromJsonWithNull()
//   Call each of the dpiJson public functions with the json parameter set to
// NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1929_verifySetFromJsonWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 21, 0) < 0)
        return DPI_FAILURE;
    dpiVar_setFromJson(NULL, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1900);
    dpiTestSuite_addCase(dpiTest_1900_varWithMaxArrSize0,
            "dpiConn_newVar() with max array size as 0");
    dpiTestSuite_addCase(dpiTest_1901_invalidOracleTypeNum,
            "dpiConn_newVar() with an invalid value for Oracle type");
    dpiTestSuite_addCase(dpiTest_1902_incompatibleValsForParams,
            "dpiConn_newVar() with incompatible values for Oracle and native "
            "types");
    dpiTestSuite_addCase(dpiTest_1903_validValsForArrsButNotSupported,
            "dpiConn_newVar() with invalid array type for array");
    dpiTestSuite_addCase(dpiTest_1904_maxArrSizeTooLarge,
            "dpiConn_newVar() with max array size that is too large");
    dpiTestSuite_addCase(dpiTest_1905_setFromBytesNotSupported,
            "dpiVar_setFromBytes() with unsupported variable");
    dpiTestSuite_addCase(dpiTest_1906_setFromBytesValueTooLarge,
            "dpiVar_setFromBytes() with value too large");
    dpiTestSuite_addCase(dpiTest_1907_setFromBytesPositionTooLarge,
            "dpiVar_setFromBytes() with position too large");
    dpiTestSuite_addCase(dpiTest_1908_setFromLobUnsupportedType,
            "dpiVar_setFromLob() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1909_setFromLobPositionTooLarge,
            "dpiVar_setFromLob() with position too large");
    dpiTestSuite_addCase(dpiTest_1910_setFromObjectUnsupportedType,
            "dpiVar_setFromObject() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1911_setFromObjectPositionTooLarge,
            "dpiVar_setFromObject() with position too large");
    dpiTestSuite_addCase(dpiTest_1912_setFromRowidUnsupportedType,
            "dpiVar_setFromRowid() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1913_setFromRowidPositionTooLarge,
            "dpiVar_setFromRowid() with position too large");
    dpiTestSuite_addCase(dpiTest_1914_setFromStmtUnsupportedType,
            "dpiVar_setFromStmt() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1915_setFromStmtPositionTooLarge,
            "dpiVar_setFromStmt() with position too large");
    dpiTestSuite_addCase(dpiTest_1916_objectVarWithNullType,
            "dpiConn_newVar() with NULL object type for object variable");
    dpiTestSuite_addCase(dpiTest_1917_stmtDefineInvalidPositions,
            "dpiStmt_define() with invalid positions");
    dpiTestSuite_addCase(dpiTest_1918_stmtDefineWithNullVar,
            "dpiStmt_define() with NULL variable");
    dpiTestSuite_addCase(dpiTest_1919_bindByPosWithPosition0,
            "dpiStmt_bindByPos() with position 0");
    dpiTestSuite_addCase(dpiTest_1920_copyDataWithInvalidPosition,
            "dpiVar_copyData() with invalid position");
    dpiTestSuite_addCase(dpiTest_1921_copyDataWithDifferentVarTypes,
            "dpiVar_copyData() with different variable types");
    dpiTestSuite_addCase(dpiTest_1922_setNumElementsInArrayTooLarge,
            "dpiVar_setNumElementsInArray() with value too large");
    dpiTestSuite_addCase(dpiTest_1923_bindByNameWithNameLen0,
            "dpiStmt_bindByName() with name length parameter 0");
    dpiTestSuite_addCase(dpiTest_1924_verifySetFromBytesWithValueLenAsZero,
            "dpiVar_setFromBytes() with value NULL and valueLength zero");
    dpiTestSuite_addCase(dpiTest_1925_verifySetFromBytesWithValueLenAsNonZero,
            "dpiVar_setFromBytes() with value not NULL and valueLength "
            "non zero");
    dpiTestSuite_addCase(dpiTest_1926_setFromJsonUnsupportedType,
            "dpiVar_setFromJson() with unsupported type");
    dpiTestSuite_addCase(dpiTest_1927_setFromJsonPositionTooLarge,
            "dpiVar_setFromJson() with position too large");
    dpiTestSuite_addCase(dpiTest_1928_setFromJsonWithValidArguments,
            "dpiVar_setFromJson() with valid arguments");
    dpiTestSuite_addCase(dpiTest_1929_verifySetFromJsonWithNull,
            "verify dpiVar_setFromJson() with NULL");
    return dpiTestSuite_run();
}
