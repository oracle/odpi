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
// test_2100_data_types.c
//   Test suite for testing all the possible combinations of data types.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__setTimeZone()
//   Set the session time zone to UTC in order to avoid any discrepancies when
// run in different time zones.
//-----------------------------------------------------------------------------
int dpiTest__setTimeZone(dpiTestCase *testCase, dpiConn *conn)
{
    const char *sql = "alter session set time_zone = 'UTC'";
    dpiStmt *stmt;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyQueryInfo() [INTERNAL]
//   Verifies each field of dpiQueryInfo.
//-----------------------------------------------------------------------------
int dpiTest__verifyQueryInfo(dpiTestCase *testCase, dpiStmt *stmt,
        uint32_t pos, const char *expectedName,
        dpiOracleTypeNum expectedOracleTypeNum,
        dpiNativeTypeNum expectedDefaultNativeTypeNum,
        uint32_t expectedDbSizeInBytes, uint32_t expectedClientSizeInBytes,
        uint32_t expectedSizeInChars, int16_t expectedPrecision,
        int8_t expectedScale, uint8_t expectedFsPrecision, int expectedNullOk)
{
    dpiQueryInfo queryInfo;

    if (dpiStmt_getQueryInfo(stmt, pos, &queryInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, queryInfo.name,
            queryInfo.nameLength, expectedName, strlen(expectedName)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, queryInfo.typeInfo.oracleTypeNum,
            expectedOracleTypeNum) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            queryInfo.typeInfo.defaultNativeTypeNum,
            expectedDefaultNativeTypeNum) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            queryInfo.typeInfo.dbSizeInBytes, expectedDbSizeInBytes) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            queryInfo.typeInfo.clientSizeInBytes,
            expectedClientSizeInBytes) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, queryInfo.typeInfo.sizeInChars,
            expectedSizeInChars) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectIntEqual(testCase, queryInfo.typeInfo.precision,
            expectedPrecision) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectIntEqual(testCase, queryInfo.typeInfo.fsPrecision,
            expectedFsPrecision) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectIntEqual(testCase, queryInfo.typeInfo.scale,
            expectedScale) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, queryInfo.nullOk,
            expectedNullOk) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyLongColsBindByPos() [INTERNAL]
//   Bind long raw/long varchar datatype by position and verify.
//-----------------------------------------------------------------------------
int dpiTest__verifyLongColsBindByPos(dpiTestCase *testCase, dpiConn *conn,
        const char *tableName, const char *colName)
{
    char *dataToSet = "1234ABCD";
    char truncateSql[100];
    char insertSql[200];
    dpiStmt *stmt;
    dpiData data;

    sprintf(truncateSql, "truncate table %s", tableName);
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row into table
    sprintf(insertSql, "insert into %s (IntCol, %s) values (:1, :2)",
        tableName, colName);
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 1);
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, dataToSet, strlen(dataToSet));
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyLongColsBindByName() [INTERNAL]
//   Bind long raw/long varchar datatype by name and verify.
//-----------------------------------------------------------------------------
int dpiTest__verifyLongColsBindByName(dpiTestCase *testCase, dpiConn *conn,
        const char *tableName, const char *colName)
{
    char *dataToSet = "1234ABCD";
    char truncateSql[100];
    char insertSql[200];
    dpiStmt *stmt;
    dpiData data;

    sprintf(truncateSql, "truncate table %s", tableName);
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row into table
    sprintf(insertSql, "insert into %s(IntCol, %s) values (:intCol, :longCol)",
        tableName, colName);
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 1);
    if (dpiStmt_bindValueByName(stmt, "intCol",
            strlen("intCol"), DPI_NATIVE_TYPE_INT64, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, dataToSet, strlen(dataToSet));
    if (dpiStmt_bindValueByName(stmt, "longCol",
            strlen("longCol"), DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2100_verifyMetadata()
//   Prepare and execute a query that returns each of the possible combinations
// of data types and verify that the metadata returned by
// dpiStmt_getQueryInfo() matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_2100_verifyMetadata(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *longRawSql = "select LongRawCol from TestLongRaws";
    const char *longVarSql = "select LongCol from TestLongs";
    const char *urowidSql = "select rowid from TestOrgIndex";
    const char *rowidSql = "select rowid from TestNumbers";
    const char *baseSql = "select * from TestDataTypes";
    dpiConn *conn;
    dpiStmt *stmt;

    // connect to database
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // base sql with most data types
    if (dpiConn_prepareStmt(conn, 0, baseSql, strlen(baseSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "STRINGCOL",
            DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 100, 100, 100, 0,
            0, 0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 2, "UNICODECOL",
            DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES, 200, 400, 100, 0,
            0, 0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 3, "FIXEDCHARCOL",
            DPI_ORACLE_TYPE_CHAR, DPI_NATIVE_TYPE_BYTES, 100, 100, 100, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 4, "FIXEDUNICODECOL",
            DPI_ORACLE_TYPE_NCHAR, DPI_NATIVE_TYPE_BYTES, 200, 400, 100, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 5, "RAWCOL",
            DPI_ORACLE_TYPE_RAW, DPI_NATIVE_TYPE_BYTES, 30, 30, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 6, "FLOATCOL",
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 0, 0, 0, 126, -127,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 7, "DOUBLEPRECCOL",
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 0, 0, 0, 126, -127,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 8, "INTCOL",
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 0, 0, 0, 9, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 9, "NUMBERCOL",
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 0, 0, 0, 9, 2,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 10, "DATECOL",
            DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 11, "TIMESTAMPCOL",
            DPI_ORACLE_TYPE_TIMESTAMP, DPI_NATIVE_TYPE_TIMESTAMP, 0, 0, 0, 0,
            0, 6, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 12, "TIMESTAMPTZCOL",
            DPI_ORACLE_TYPE_TIMESTAMP_TZ, DPI_NATIVE_TYPE_TIMESTAMP, 0, 0, 0,
            0, 0, 6, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 13, "TIMESTAMPLTZCOL",
            DPI_ORACLE_TYPE_TIMESTAMP_LTZ, DPI_NATIVE_TYPE_TIMESTAMP, 0, 0, 0,
            0, 0, 6, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 14, "INTERVALDSCOL",
            DPI_ORACLE_TYPE_INTERVAL_DS, DPI_NATIVE_TYPE_INTERVAL_DS, 0, 0, 0,
            2, 0, 6, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 15, "INTERVALYMCOL",
            DPI_ORACLE_TYPE_INTERVAL_YM, DPI_NATIVE_TYPE_INTERVAL_YM, 0, 0, 0,
            2, 0, 0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 16, "BINARYFLTCOL",
            DPI_ORACLE_TYPE_NATIVE_FLOAT, DPI_NATIVE_TYPE_FLOAT, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 17, "BINARYDOUBLECOL",
            DPI_ORACLE_TYPE_NATIVE_DOUBLE, DPI_NATIVE_TYPE_DOUBLE, 0, 0, 0, 0,
            0, 0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 18, "CLOBCOL",
            DPI_ORACLE_TYPE_CLOB, DPI_NATIVE_TYPE_LOB, 0, 0, 0, 0, 0, 0,
            1) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 19, "NCLOBCOL",
            DPI_ORACLE_TYPE_NCLOB, DPI_NATIVE_TYPE_LOB, 0, 0, 0, 0, 0, 0,
            1) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 20, "BLOBCOL",
            DPI_ORACLE_TYPE_BLOB, DPI_NATIVE_TYPE_LOB, 0, 0, 0, 0, 0, 0,
            1) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 21, "BFILECOL",
            DPI_ORACLE_TYPE_BFILE, DPI_NATIVE_TYPE_LOB, 0, 0, 0, 0, 0, 0,
            1) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 22, "LONGCOL",
            DPI_ORACLE_TYPE_LONG_VARCHAR, DPI_NATIVE_TYPE_BYTES, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 23, "UNCONSTRAINEDCOL",
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 0, 0, 0, 0, -127,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 24, "SIGNEDINTCOL",
            DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 0, 0, 0, 38, 0,
            0, 1) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 25, "SUBOBJECTCOL",
            DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 0, 0, 0, 0, 0,
            0, 1) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // sql testing rowid column
    if (dpiConn_prepareStmt(conn, 0, rowidSql, strlen(rowidSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "ROWID",
            DPI_ORACLE_TYPE_ROWID, DPI_NATIVE_TYPE_ROWID, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // sql testing urowid column
    if (dpiConn_prepareStmt(conn, 0, urowidSql, strlen(urowidSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "ROWID",
            DPI_ORACLE_TYPE_ROWID, DPI_NATIVE_TYPE_ROWID, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // sql testing long raw column
    if (dpiConn_prepareStmt(conn, 0, longRawSql, strlen(longRawSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "LONGRAWCOL",
            DPI_ORACLE_TYPE_LONG_RAW, DPI_NATIVE_TYPE_BYTES, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // sql testing long varchar column
    if (dpiConn_prepareStmt(conn, 0, longVarSql, strlen(longVarSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "LONGCOL",
            DPI_ORACLE_TYPE_LONG_VARCHAR, DPI_NATIVE_TYPE_BYTES, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2101_verifyBindsByPos()
//   Prepare and execute a statement that binds each data type and verify that
// the value is bound correctly using dpiStmt_bindValueByPos() (no error).
//-----------------------------------------------------------------------------
int dpiTest_2101_verifyBindsByPos(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *truncateSql = "truncate table TestDataTypes";
    const char *insertSql = "insert into TestDataTypes (StringCol, "
            "UnicodeCol, FixedCharCol, FixedUnicodeCol, RawCol, FloatCol, "
            "DoublePrecCol, IntCol, NumberCol, DateCol, TimestampCol, "
            "TimestampTZCol, TimestampLTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, LongCol, UnconstrainedCol, "
            "SignedIntCol, SubObjectCol) values "
            "(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, "
            ":15, :16, :17, :18, :19, :20, :21)";
    const char *subObjName = "UDT_SUBOBJECT";
    dpiObjectType *subObjType;
    dpiObject *subObj;
    dpiStmt *stmt;
    dpiConn *conn;
    dpiData data;

    // connect to database
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row into table containing all data types
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "string", strlen("string"));
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "unistring", strlen("unistring"));
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "fixedchar", strlen("fixedchar"));
    if (dpiStmt_bindValueByPos(stmt, 3, DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "fixedunichar", strlen("fixedunichar"));
    if (dpiStmt_bindValueByPos(stmt, 4, DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "12AB", strlen("12AB"));
    if (dpiStmt_bindValueByPos(stmt, 5, DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.25);
    if (dpiStmt_bindValueByPos(stmt, 6, DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.44);
    if (dpiStmt_bindValueByPos(stmt, 7, DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 1);
    if (dpiStmt_bindValueByPos(stmt, 8, DPI_NATIVE_TYPE_INT64, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.35);
    if (dpiStmt_bindValueByPos(stmt, 9, DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&data, 2017, 6, 1, 2, 2, 1, 0, 0, 0);
    if (dpiStmt_bindValueByPos(stmt, 10, DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByPos(stmt, 11, DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByPos(stmt, 12, DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByPos(stmt, 13, DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setIntervalDS(&data, 3, 2, 1, 1, 0);
    if (dpiStmt_bindValueByPos(stmt, 14, DPI_NATIVE_TYPE_INTERVAL_DS,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setIntervalYM(&data, 1, 1);
    if (dpiStmt_bindValueByPos(stmt, 15, DPI_NATIVE_TYPE_INTERVAL_YM,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.34);
    if (dpiStmt_bindValueByPos(stmt, 16, DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.95);
    if (dpiStmt_bindValueByPos(stmt, 17, DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.45);
    if (dpiStmt_bindValueByPos(stmt, 18, DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 999);
    if (dpiStmt_bindValueByPos(stmt, 19, DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 123);
    if (dpiStmt_bindValueByPos(stmt, 20, DPI_NATIVE_TYPE_UINT64, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get sub object type and create object
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(subObjType, &subObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, subObj);
    if (dpiStmt_bindValueByPos(stmt, 21, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(subObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify long raw data type
    if (dpiTest__verifyLongColsBindByPos(testCase, conn, "TestLongRaws",
            "LongRawCol") < 0)
        return DPI_FAILURE;

    // verify long varchar data type
    if (dpiTest__verifyLongColsBindByPos(testCase, conn, "TestLongs",
            "LongCol") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2102_verifyBindsByName()
//   Prepare and execute a statement that binds each data type and verify that
// the value is bound correctly using dpiStmt_bindValueByName() (no error).
//-----------------------------------------------------------------------------
int dpiTest_2102_verifyBindsByName(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *truncateSql = "truncate table TestDataTypes";
    const char *insertSql = "insert into TestDataTypes (StringCol, "
            "UnicodeCol, FixedCharCol, FixedUnicodeCol, RawCol, FloatCol, "
            "DoublePrecCol, IntCol, NumberCol, DateCol, TimestampCol, "
            "TimestampTZCol, TimestampLTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, LongCol, UnconstrainedCol, "
            "SignedIntCol, SubObjectCol) values "
            "(:stringCol, :unicodeCol, :fixedCharCol, :fixedUnicodeCol, "
            ":rawCol, :floatCol, :doublePrecCol, :intCol, :numberCol, "
            ":dateCol, :timestampCol, :timestampTZCol, :timestampLTZCol, "
            ":intervalDSCol, :intervalYMCol, :binaryFltCol, :binaryDoubleCol, "
            ":longCol, :unconstrainedCol, :signedIntCol, :subObjectCol)";
    const char *subObjName = "UDT_SUBOBJECT";
    dpiObjectType *subObjType;
    dpiObject *subObj;
    dpiStmt *stmt;
    dpiConn *conn;
    dpiData data;

    // connect to database
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row into table containing all data types
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "string", strlen("string"));
    if (dpiStmt_bindValueByName(stmt, "stringCol", strlen("stringCol"),
            DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "unistring", strlen("unistring"));
    if (dpiStmt_bindValueByName(stmt, "unicodeCol", strlen("unicodeCol"),
            DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "fixedchar", strlen("fixedchar"));
    if (dpiStmt_bindValueByName(stmt, "fixedCharCol", strlen("fixedCharCol"),
            DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "fixedunichar", strlen("fixedunichar"));
    if (dpiStmt_bindValueByName(stmt, "fixedUnicodeCol",
            strlen("fixedUnicodeCol"), DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "12AB", strlen("12AB"));
    if (dpiStmt_bindValueByName(stmt, "rawCol", strlen("rawCol"),
            DPI_NATIVE_TYPE_BYTES, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.25);
    if (dpiStmt_bindValueByName(stmt, "floatCol", strlen("floatCol"),
            DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.44);
    if (dpiStmt_bindValueByName(stmt, "doublePrecCol", strlen("doublePrecCol"),
            DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 1);
    if (dpiStmt_bindValueByName(stmt, "intCol", strlen("intCol"),
            DPI_NATIVE_TYPE_INT64, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.35);
    if (dpiStmt_bindValueByName(stmt, "numberCol", strlen("numberCol"),
            DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&data, 2017, 6, 1, 2, 2, 1, 0, 0, 0);
    if (dpiStmt_bindValueByName(stmt, "dateCol", strlen("dateCol"),
            DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByName(stmt, "timestampCol", strlen("timestampCol"),
            DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByName(stmt, "timestampTZCol",
            strlen("timestampTZCol"), DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByName(stmt, "timestampLTZCol",
            strlen("timestampLTZCol"), DPI_NATIVE_TYPE_TIMESTAMP, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setIntervalDS(&data, 3, 2, 1, 1, 0);
    if (dpiStmt_bindValueByName(stmt, "intervalDSCol", strlen("intervalDSCol"),
            DPI_NATIVE_TYPE_INTERVAL_DS, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setIntervalYM(&data, 1, 1);
    if (dpiStmt_bindValueByName(stmt, "intervalYMCol", strlen("intervalYMCol"),
            DPI_NATIVE_TYPE_INTERVAL_YM, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.34);
    if (dpiStmt_bindValueByName(stmt, "binaryFltCol", strlen("binaryFltCol"),
            DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.95);
    if (dpiStmt_bindValueByName(stmt, "binaryDoubleCol",
            strlen("binaryDoubleCol"), DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.45);
    if (dpiStmt_bindValueByName(stmt, "longCol", strlen("longCol"),
            DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 999);
    if (dpiStmt_bindValueByName(stmt, "unconstrainedCol",
            strlen("unconstrainedCol"), DPI_NATIVE_TYPE_DOUBLE, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 123);
    if (dpiStmt_bindValueByName(stmt, "signedIntCol",
            strlen("signedIntCol"), DPI_NATIVE_TYPE_UINT64, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get sub object type and create object
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_createObject(subObjType, &subObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, subObj);
    if (dpiStmt_bindValueByName(stmt, "subObjectCol",
            strlen("subObjectCol"), DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(subObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify long raw data type
    if (dpiTest__verifyLongColsBindByName(testCase, conn, "TestLongRaws",
            "LongRawCol") < 0)
        return DPI_FAILURE;

    // verify long varchar data type
    if (dpiTest__verifyLongColsBindByName(testCase, conn, "TestLongs",
            "LongCol") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2103_verifyDMLReturningValues()
//   Prepare and execute a DML returning statement that returns each of the
// possible combinations noted above and verify that the value is returned
// correctly in each case (no error).
//-----------------------------------------------------------------------------
int dpiTest_2103_verifyDMLReturningValues(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *truncateSql = "delete from TestDataTypes";
    const char *insertSql = "insert into TestDataTypes (StringCol, "
            "UnicodeCol, FixedCharCol, FixedUnicodeCol, RawCol, FloatCol, "
            "DoublePrecCol, IntCol, NumberCol, DateCol, TimestampCol, "
            "TimestampTZCol, TimestampLTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, LongCol, UnconstrainedCol, "
            "SignedIntCol, SubObjectCol) "
            "values ('string', 'unistring', 'fixedchar', 'fixedunichar', "
            "'12AB', 1.25, 1.44, 6, 1.35, TO_DATE('2002/12/10 01:02:03', "
            "'yyyy/mm/dd hh24:mi:ss'), to_timestamp('20021210', 'YYYYMMDD'), "
            "to_timestamp_tz('20021210 01:02:03 00:00', "
            "'YYYYMMDD HH24:MI:SS TZH:TZM'), "
            "to_timestamp_tz('20021210 01:02:03 00:00', "
            "'YYYYMMDD HH24:MI:SS TZH:TZM'), INTERVAL '3' DAY, "
            " INTERVAL '1' YEAR, 1.75, 1.95, 1.454, 999, 567, "
            "udt_SubObject(1, 'element #1'))";
    const char *deleteSql = "delete from TestDataTypes returning StringCol, "
            "UnicodeCol, FloatCol, DoublePrecCol, IntCol, NumberCol, DateCol, "
            "TimestampCol, TimestampTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, UnconstrainedCol, SignedIntCol, "
            "SubObjectCol into "
            ":stringCol, :unicodeCol, :floatCol, :doublePrecCol, :intCol, "
            ":numberCol, :dateCol, :timestampCol, :timestampTZCol, "
            ":intervalDSCol, :intervalYMCol, :binaryFltCol, :binaryDoubleCol, "
            ":unconstrainedCol, :signedIntCol, :subObjectCol";
    const char *subObjName = "UDT_SUBOBJECT";
    uint32_t numCols = 16, i, numAttr = 2, numReturnedRows;
    dpiData *colData[16], tempData;
    dpiObjectType *subObjType;
    dpiObjectAttr *attrs[2];
    dpiVar *colVar[16];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate table
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            100, 0, 0, NULL, &colVar[0], &colData[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            1, 100, 0, 0, NULL, &colVar[1], &colData[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 1,
            0, 0, 0, NULL, &colVar[2], &colData[2]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 1,
            0, 0, 0, NULL, &colVar[3], &colData[3]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &colVar[4], &colData[4]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 1,
            0, 0, 0, NULL, &colVar[5], &colData[5]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP,
            1, 0, 0, 0, NULL, &colVar[6], &colData[6]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP,
            DPI_NATIVE_TYPE_TIMESTAMP, 1, 0, 0, 0, NULL, &colVar[7],
            &colData[7]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP_TZ,
            DPI_NATIVE_TYPE_TIMESTAMP, 1, 0, 0, 0, NULL, &colVar[8],
            &colData[8]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_INTERVAL_DS,
            DPI_NATIVE_TYPE_INTERVAL_DS, 1, 0, 0, 0, NULL, &colVar[9],
            &colData[9]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_INTERVAL_YM,
            DPI_NATIVE_TYPE_INTERVAL_YM, 1, 0, 0, 0, NULL, &colVar[10],
            &colData[10]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_FLOAT,
            DPI_NATIVE_TYPE_FLOAT, 1, 0, 0, 0, NULL, &colVar[11],
            &colData[11]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_DOUBLE,
            DPI_NATIVE_TYPE_DOUBLE, 1, 0, 0, 0, NULL, &colVar[12],
            &colData[12]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 1,
            0, 0, 0, NULL, &colVar[13], &colData[13]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64, 1,
            0, 0, 0, NULL, &colVar[14], &colData[14]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, subObjName, strlen(subObjName),
            &subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT,
            1, 0, 0, 0, subObjType, &colVar[15], &colData[15]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // delete statement with DML returning
    if (dpiConn_prepareStmt(conn, 0, deleteSql, strlen(deleteSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numCols; i++) {
        if (dpiStmt_bindByPos(stmt, i + 1, colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numCols; i++) {
        if (dpiVar_getReturnedData(colVar[i], 0, &numReturnedRows,
                &colData[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // compare data returned to expected data
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(colData[0])->ptr,
            dpiData_getBytes(colData[0])->length, "string",
            strlen("string")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(colData[1])->ptr,
            dpiData_getBytes(colData[1])->length, "unistring",
            strlen("unistring")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(colData[2]), 1.25) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(colData[3]), 1.44) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase,
            dpiData_getUint64(colData[4]), 6) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(colData[5]), 1.35) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData, 2002, 12, 10, 1, 2, 3, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(colData[6]),
            dpiData_getTimestamp(&tempData)) < 0)
        return DPI_FAILURE;
    dpiData_setTimestamp(&tempData, 2002, 12, 10, 0, 0, 0, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(colData[7]),
            dpiData_getTimestamp(&tempData)) < 0)
        return DPI_FAILURE;
    dpiData_setTimestamp(&tempData, 2002, 12, 10, 1, 2, 3, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(colData[8]),
            dpiData_getTimestamp(&tempData)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            dpiData_getIntervalDS(colData[9])->days, 3) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase,
            dpiData_getIntervalYM(colData[10])->years, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getFloat(colData[11]), 1.75) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(colData[12]), 1.95) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(colData[13]), 999) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase,
            dpiData_getInt64(colData[14]), 567) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get attribute values of the object and compare
    if (dpiObjectType_getAttributes(subObjType, numAttr, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getAttributeValue(dpiData_getObject(colData[15]), attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(&tempData), 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_getAttributeValue(dpiData_getObject(colData[15]), attrs[1],
            DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(&tempData)->ptr,
            dpiData_getBytes(&tempData)->length, "element #1",
            strlen("element #1")) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    for (i = 0; i < numCols; i++) {
        if (dpiVar_release(colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    for (i = 0; i < numAttr; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiObjectType_release(subObjType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2104_verifyInOutBindVariables()
//   Prepare and execute a PL/SQL statement that binds both in and out each
// of the possible combinations of data types and verify that the value is
// returned correctly in each case (no error).
//-----------------------------------------------------------------------------
int dpiTest_2104_verifyInOutBindVariables(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin proc_TestInOut(:1, :2, :3, :4, :5, :6, :7, :8, "
            ":9, :10, :11, :12, :13, :14); end;";
    char *strValue = "TestData", *modifiedStrValue = "TestData (Modified)";
    const char *boolSql = "begin proc_TestInOutBool(:1); end;";
    char *modifiedUnicodeValue = "Unicode (Modified)";
    const char *objName = "UDT_SUBOBJECT";
    char *unicodeValue = "Unicode";
    dpiData *inOutData[14], tempData;
    dpiVersionInfo *versionInfo;
    uint32_t numCols = 14, i;
    dpiObjectAttr *attrs[2];
    dpiObjectType *objType;
    dpiVar *inOutVar[14];
    dpiBytes *bytes;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variable for boolean
    dpiTestSuite_getClientVersionInfo(&versionInfo);
    if (versionInfo->versionNum >= 12) {
        if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_BOOLEAN,
                DPI_NATIVE_TYPE_BOOLEAN, 1, 0, 0, 0, NULL, &inOutVar[0],
                &inOutData[0]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setBool(inOutData[0], 1);

        // call stored procedure
        if (dpiConn_prepareStmt(conn, 0, boolSql, strlen(boolSql),
                NULL, 0, &stmt) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_bindByPos(stmt, 1, inOutVar[0]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_execute(stmt, 0, NULL) < 0)
            return dpiTestCase_setFailedFromError(testCase);

        // compare results
        if (dpiTestCase_expectUintEqual(testCase,
                dpiData_getBool(inOutData[0]), 1) < 0)
            return DPI_FAILURE;
        if (dpiVar_release(inOutVar[0]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiStmt_release(stmt) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            100, 1, 0, NULL, &inOutVar[0], &inOutData[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            1, 100, 1, 0, NULL, &inOutVar[1], &inOutData[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 1,
            0, 0, 0, NULL, &inOutVar[2], &inOutData[2]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE, 1,
            0, 0, 0, NULL, &inOutVar[3], &inOutData[3]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &inOutVar[4], &inOutData[4]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP,
            1, 0, 0, 0, NULL, &inOutVar[5], &inOutData[5]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP,
            DPI_NATIVE_TYPE_TIMESTAMP, 1, 0, 0, 0, NULL, &inOutVar[6],
            &inOutData[6]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP_TZ,
            DPI_NATIVE_TYPE_TIMESTAMP, 1, 0, 0, 0, NULL, &inOutVar[7],
            &inOutData[7]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_INTERVAL_DS,
            DPI_NATIVE_TYPE_INTERVAL_DS, 1, 0, 0, 0, NULL, &inOutVar[8],
            &inOutData[8]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_INTERVAL_YM,
            DPI_NATIVE_TYPE_INTERVAL_YM, 1, 0, 0, 0, NULL, &inOutVar[9],
            &inOutData[9]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_FLOAT,
            DPI_NATIVE_TYPE_FLOAT, 1, 0, 0, 0, NULL, &inOutVar[10],
            &inOutData[10]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_DOUBLE,
            DPI_NATIVE_TYPE_DOUBLE, 1, 0, 0, 0, NULL, &inOutVar[11],
            &inOutData[11]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER,
            DPI_NATIVE_TYPE_UINT64, 1, 0, 0, 0, NULL, &inOutVar[12],
            &inOutData[12]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getObjectType(conn, objName, strlen(objName), &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, 2, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &inOutVar[13], &inOutData[13]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // set input values
    if (dpiVar_setFromBytes(inOutVar[0], 0, strValue, strlen(strValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(inOutVar[1], 0, unicodeValue,
            strlen(unicodeValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(inOutData[2], 1.5);
    dpiData_setDouble(inOutData[3], 1.5);
    dpiData_setInt64(inOutData[4], 1);
    dpiData_setTimestamp(inOutData[5], 2017, 6, 1, 1, 2, 1, 1, 0, 0);
    dpiData_setTimestamp(inOutData[6], 2017, 6, 1, 1, 2, 1, 1, 0, 0);
    dpiData_setTimestamp(inOutData[7], 2017, 6, 1, 1, 2, 1, 1, 0, 0);
    dpiData_setIntervalDS(inOutData[8], 3, 1, 1, 1, 1);
    dpiData_setIntervalYM(inOutData[9], 1, 1);
    dpiData_setFloat(inOutData[10], 1.34);
    dpiData_setDouble(inOutData[11], 1.95);
    dpiData_setUint64(inOutData[12], 123);
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&tempData, 12.33);
    if (dpiObject_setAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_DOUBLE,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&tempData, strValue, strlen(strValue));
    if (dpiObject_setAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromObject(inOutVar[13], 0, obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // call stored procedure
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    for (i = 0; i < numCols; i++){
        if (dpiStmt_bindByPos(stmt, i + 1, inOutVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // compare results
    bytes = dpiData_getBytes(inOutData[0]);
    if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
            modifiedStrValue, strlen(modifiedStrValue)) < 0)
        return DPI_FAILURE;
    bytes = dpiData_getBytes(inOutData[1]);
    if (dpiTestCase_expectStringEqual(testCase, bytes->ptr, bytes->length,
            modifiedUnicodeValue, strlen(modifiedUnicodeValue)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(inOutData[2]), 3) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(inOutData[3]), 3) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectIntEqual(testCase,
            dpiData_getInt64(inOutData[4]), 2) < 0)
        return DPI_FAILURE;
    dpiData_setTimestamp(&tempData, 2018, 6, 1, 1, 2, 1, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(inOutData[5]),
            dpiData_getTimestamp(&tempData)) < 0)
        return DPI_FAILURE;
    dpiData_setTimestamp(&tempData, 2017, 6, 1, 1, 32, 1, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(inOutData[6]),
            dpiData_getTimestamp(&tempData)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(inOutData[7]),
            dpiData_getTimestamp(&tempData)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            dpiData_getIntervalDS(inOutData[8])->days, 6) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            dpiData_getIntervalYM(inOutData[9])->years, 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getFloat(inOutData[10]), (float) 2.68) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(inOutData[11]), 3.9) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            dpiData_getUint64(inOutData[12]), 246) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(inOutData[13]->value.asObject, attrs[0],
            DPI_NATIVE_TYPE_DOUBLE, &tempData) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectDoubleEqual(testCase, tempData.value.asDouble,
            36.99) < 0)
        return DPI_FAILURE;
    if (dpiObject_getAttributeValue(inOutData[13]->value.asObject, attrs[1],
            DPI_NATIVE_TYPE_BYTES, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, tempData.value.asBytes.ptr,
            tempData.value.asBytes.length, modifiedStrValue,
            strlen(modifiedStrValue)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < 2; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    for (i = 0; i < numCols; i++) {
        if (dpiVar_release(inOutVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2105_verifyObjectAttributes()
//   Get and set the attribute value of an object that uses each of the
// possible combinations noted above and verify that the value is both set and
// acquired correctly in each case (no error).
//-----------------------------------------------------------------------------
int dpiTest_2105_verifyObjectAttributes(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestObjectDataTypes values (:1)";
    const char *selectSql = "select ObjectCol from TestObjectDataTypes";
    const char *objectName = "UDT_OBJECTDATATYPES";
    dpiData data, *objColValue, attrValues[14];
    uint32_t i, bufferRowIndex, numAttrs = 14;
    dpiNativeTypeNum nativeTypeNum;
    dpiObjectAttrInfo attrInfo;
    dpiObjectAttr *attrs[14];
    dpiQueryInfo queryInfo;
    dpiObjectType *objType;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__setTimeZone(testCase, conn) < 0)
        return DPI_FAILURE;

    // get object type and attributes
    if (dpiConn_getObjectType(conn, objectName, strlen(objectName),
            &objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(objType, numAttrs, attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create object and populate attributes
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "StringData", strlen("StringData"));
    if (dpiObject_setAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_BYTES,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "UnicodeData", strlen("UnicodeData"));
    if (dpiObject_setAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "FixedCharData", strlen("FixedCharData"));
    if (dpiObject_setAttributeValue(obj, attrs[2], DPI_NATIVE_TYPE_BYTES,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "FixedUnicodeData",
            strlen("FixedUnicodeData"));
    if (dpiObject_setAttributeValue(obj, attrs[3], DPI_NATIVE_TYPE_BYTES,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&data, "RawData", strlen("RawData"));
    if (dpiObject_setAttributeValue(obj, attrs[4], DPI_NATIVE_TYPE_BYTES,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 5);
    if (dpiObject_setAttributeValue(obj, attrs[5], DPI_NATIVE_TYPE_INT64,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.25);
    if (dpiObject_setAttributeValue(obj, attrs[6], DPI_NATIVE_TYPE_DOUBLE,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&data, 2017, 6, 1, 2, 2, 1, 0, 0, 0);
    if (dpiObject_setAttributeValue(obj, attrs[7], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_setAttributeValue(obj, attrs[8], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_setAttributeValue(obj, attrs[9], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_setAttributeValue(obj, attrs[10], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setFloat(&data, 13.25);
    if (dpiObject_setAttributeValue(obj, attrs[11], DPI_NATIVE_TYPE_FLOAT,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 13.25);
    if (dpiObject_setAttributeValue(obj, attrs[12], DPI_NATIVE_TYPE_DOUBLE,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&data, 123);
    if (dpiObject_setAttributeValue(obj, attrs[13], DPI_NATIVE_TYPE_INT64,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert data
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj);
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(obj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_release(objType) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numAttrs; i++)
        dpiObjectAttr_release(attrs[i]);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // retrieve data
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryInfo(stmt, 1, &queryInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObjectType_getAttributes(queryInfo.typeInfo.objectType, numAttrs,
            attrs) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &objColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    obj = dpiData_getObject(objColValue);

    // retrieve all of the attributes
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_getInfo(attrs[i], &attrInfo) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiObject_getAttributeValue(obj, attrs[i],
                attrInfo.typeInfo.defaultNativeTypeNum, &attrValues[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // compare values
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(&attrValues[0])->ptr,
            dpiData_getBytes(&attrValues[0])->length, "StringData",
            strlen("StringData")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(&attrValues[1])->ptr,
            dpiData_getBytes(&attrValues[1])->length, "UnicodeData",
            strlen("UnicodeData")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(&attrValues[2])->ptr,
            dpiData_getBytes(&attrValues[2])->length,
            "FixedCharData                 ",
            strlen("FixedCharData                 ")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(&attrValues[3])->ptr,
            dpiData_getBytes(&attrValues[3])->length,
            "FixedUnicodeData              ",
            strlen("FixedUnicodeData              ")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            dpiData_getBytes(&attrValues[4])->ptr,
            dpiData_getBytes(&attrValues[4])->length, "RawData",
            strlen("RawData")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(&attrValues[5]), 5) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(&attrValues[6]), 1.25) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&data, 2017, 6, 1, 2, 2, 1, 0, 0, 0);
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&attrValues[7]),
            dpiData_getTimestamp(&data)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&attrValues[8]),
            dpiData_getTimestamp(&data)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&attrValues[9]),
            dpiData_getTimestamp(&data)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectTimestampEqual(testCase,
            dpiData_getTimestamp(&attrValues[10]),
            dpiData_getTimestamp(&data)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getFloat(&attrValues[11]), 13.25) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(&attrValues[12]), 13.25) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase,
            dpiData_getDouble(&attrValues[13]), 123) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_release(attrs[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2106_verifyNumDataTypeWithDiffValues()
//   For Oracle type DPI_ORACLE_TYPE_NUMBER and native type
// DPI_NATIVE_TYPE_BYTES, verify binding and fetching for various string
// values:
//
// 1. value 0.
// 2. integers (+/-) with 38 digits without leading and trailing zeros.
// 3. integers (+/-) that test the upper boundary (9e125)
// 4. fractions (+/-) with 38 digits without leading and trailing zeros.
// 5. fractions (+/-) that test the lower boundary (1e-130)
//-----------------------------------------------------------------------------
int dpiTest_2106_verifyNumDataTypeWithDiffValues(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *outValues[] = {
        "0",
        "92999999999999999999999999999999999999",
        "-92999999999999999999999999999999999999",
        "999999999999999999999999999999999999999",
        "-999999999999999999999999999999999999999",
        "9999999999999999999999999999999999999999",
        "-9999999999999999999999999999999999999999",
        "900000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000",
        "-90000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000",
        "3.0123456789012345678901234567890123456",
        "-3.0123456789012345678901234567890123456",
        "0.0000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000"
                "01",
        "-0.000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000"
                "001",
    };
    const char *inValues[] = {
        "0",
        "92999999999999999999999999999999999999",
        "-92999999999999999999999999999999999999",
        "999999999999999999999999999999999999999",
        "-999999999999999999999999999999999999999",
        "9999999999999999999999999999999999999999",
        "-9999999999999999999999999999999999999999",
        "9E+125",
        "-9E+125",
        "3.0123456789012345678901234567890123456",
        "-3.0123456789012345678901234567890123456",
        "1E-130",
        "-1E-130",
        NULL
    };
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

    // process each case
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
// dpiTest_2107_verifyInvalidValues()
//   For Oracle type DPI_ORACLE_TYPE_NUMBER and native type
// DPI_NATIVE_TYPE_BYTES, verify binding and fetching for various unexpected
// string values that return errors:
//
// 1. integers (+/-) greater than the upper boundary fail (error DPI-1044)
// 2. numbers that have more than 38 digits (error DPI-1044)
// 3. string that is not a valid number (ex: www.json.org, non-numeric
//    characters, multiple decimal points (error DPI-1043)
//-----------------------------------------------------------------------------
int dpiTest_2107_verifyInvalidValues(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *outValues[] = {
        "DPI-1044:", "DPI-1044:", "DPI-1044:", "DPI-1044:", "DPI-1044:",
        "DPI-1044:", "DPI-1043:", "DPI-1043:", "DPI-1043:", "DPI-1043:",
        "DPI-1043:", "DPI-1043:"
    };
    const char *inValues[] = {
        "1E+126",
        "-1E+126",
        "1E-131",
        "-1E-131",
        "99999999999999999999999999999999999999999",
        "-99999999999999999999999999999999999999999",
        "www.json.org",
        "1.2.3",
        "a",
        "inf",
        "-inf",
        "nan",
        NULL
    };
    const char *sql = "select :1 from dual";
    dpiData *inputVarData;
    dpiVar *inputVar;
    dpiConn *conn;
    dpiStmt *stmt;
    int i;

    // create variables and prepare statement for execution
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES, 1,
            0, 0, 0, NULL, &inputVar, &inputVarData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inputVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // process each case
    for (i = 0; inValues[i]; i++) {
        if (dpiVar_setFromBytes(inputVar, 0, inValues[i],
                strlen(inValues[i])) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL);
        if (dpiTestCase_expectError(testCase, outValues[i]) < 0)
            return DPI_FAILURE;
    }

    // cleanup
    if (dpiVar_release(inputVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2108_verifyDatesCollection()
//   Verify that collections containing date and timestamp attributes work as
// expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_2108_verifyDatesCollection(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestDatesVarray values(:1)";
    const char *selectSql = "select ObjectCol from TestDatesVarray";
    uint32_t i, numElements = 10, numAttrs = 5, bufferRowIndex;
    dpiObjectType *arrayObjType, *elementObjType;
    const char *arrayObjName = "UDT_DATESARRAY";
    const char *elementObjName = "UDT_DATES";
    dpiObject *arrayObj, *elementObj;
    dpiNativeTypeNum nativeTypeNum;
    dpiTimestamp timestamps[10][4];
    dpiData tempData, *arrayValue;
    dpiObjectAttr *attrs[5];
    dpiVar *arrayVar;
    dpiStmt *stmt;
    dpiConn *conn;
    int32_t size;
    int found;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__setTimeZone(testCase, conn) < 0)
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

    // create variable
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, arrayObjType, &arrayVar, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate timestamps with expected values
    memset(timestamps, 0, sizeof(timestamps));
    for (i = 0; i < numElements; i++) {

        // date
        timestamps[i][0].year = i + 1917;
        timestamps[i][0].month = i + 1;
        timestamps[i][0].day = i + 1;
        timestamps[i][0].hour = i;
        timestamps[i][0].minute = i;
        timestamps[i][0].second = i;

        // timestamp
        timestamps[i][1].year = i + 1986;
        timestamps[i][1].month = i + 2;
        timestamps[i][1].day = i + 5;
        timestamps[i][1].hour = i + 1;
        timestamps[i][1].minute = i * 2;
        timestamps[i][1].second = i * 3;
        timestamps[i][1].fsecond = 201000;

        // timestamp with time zone
        timestamps[i][2].year = i + 1989;
        timestamps[i][2].month = i + 1;
        timestamps[i][2].day = i + 8;
        timestamps[i][2].hour = i;
        timestamps[i][2].minute = i * 3;
        timestamps[i][2].second = i * 4;
        timestamps[i][2].fsecond = 45000;

        // timestamp with local time zone
        timestamps[i][3].year = i + 1999;
        timestamps[i][3].month = i + 2;
        timestamps[i][3].day = i + 15;
        timestamps[i][3].hour = i + 1;
        timestamps[i][3].minute = i * 2;
        timestamps[i][3].second = i * 3;
        timestamps[i][3].fsecond = 123456000;

    }

    // populate bind variables with values
    for (i = 0; i < numElements; i++) {
        dpiData_setInt64(&tempData, i + 1);
        if (dpiObject_setAttributeValue(elementObj, attrs[0],
                DPI_NATIVE_TYPE_INT64, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        tempData.value.asTimestamp = timestamps[i][0];
        if (dpiObject_setAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        tempData.value.asTimestamp = timestamps[i][1];
        if (dpiObject_setAttributeValue(elementObj, attrs[2],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        tempData.value.asTimestamp = timestamps[i][2];
        if (dpiObject_setAttributeValue(elementObj, attrs[3],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        tempData.value.asTimestamp = timestamps[i][3];
        if (dpiObject_setAttributeValue(elementObj, attrs[4],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setObject(&tempData, elementObj);
        if (dpiObject_appendElement(arrayObj, DPI_NATIVE_TYPE_OBJECT,
                &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiVar_setFromObject(arrayVar, 0, arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare insert statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // execute statement and verify return value matches expectations
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(arrayObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_release(elementObj) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(arrayVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // retrieve data
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &arrayValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    arrayObj = dpiData_getObject(arrayValue);

    // verify that returned object matches what was inserted
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
        if (dpiTestCase_expectIntEqual(testCase,
                dpiData_getInt64(&tempData), i + 1) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[1],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData), &timestamps[i][0]) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[2],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData), &timestamps[i][1]) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[3],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData), &timestamps[i][2]) < 0)
            return DPI_FAILURE;
        if (dpiObject_getAttributeValue(elementObj, attrs[4],
                DPI_NATIVE_TYPE_TIMESTAMP, &tempData) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectTimestampEqual(testCase,
                dpiData_getTimestamp(&tempData), &timestamps[i][3]) < 0)
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

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2100);
    dpiTestSuite_addCase(dpiTest_2100_verifyMetadata,
            "verify metadata returned by dpiStmt_getQueryInfo()");
    dpiTestSuite_addCase(dpiTest_2101_verifyBindsByPos,
            "bind all data types using position and verify");
    dpiTestSuite_addCase(dpiTest_2102_verifyBindsByName,
            "bind all data types using name and verify");
    dpiTestSuite_addCase(dpiTest_2103_verifyDMLReturningValues,
            "verify DML returning stmt returns values correctly");
    dpiTestSuite_addCase(dpiTest_2104_verifyInOutBindVariables,
            "verify in/out binds are handled correctly");
    dpiTestSuite_addCase(dpiTest_2105_verifyObjectAttributes,
            "verify get and set attributes of an object");
    dpiTestSuite_addCase(dpiTest_2106_verifyNumDataTypeWithDiffValues,
            "verify oracle type number with diff string values");
    dpiTestSuite_addCase(dpiTest_2107_verifyInvalidValues,
            "test conversion of string to number for invalid values");
    dpiTestSuite_addCase(dpiTest_2108_verifyDatesCollection,
            "verify collection containing dates works as expected");
    return dpiTestSuite_run();
}
