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
// TestDataTypes.c
//   Test suite for testing all the possible combinations of data types.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__compareTimestamps() [INTERNAL]
//   Compares each field of dpiTimestamp.
//-----------------------------------------------------------------------------
int dpiTest__compareTimestamps(dpiTestCase *testCase,
        dpiTimestamp *timestamp, dpiTimestamp *expectedTimestamp)
{
    if (dpiTestCase_expectUintEqual(testCase, timestamp->year,
            expectedTimestamp->year) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, timestamp->month,
            expectedTimestamp->month) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, timestamp->day,
            expectedTimestamp->day) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, timestamp->hour,
            expectedTimestamp->hour) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, timestamp->minute,
            expectedTimestamp->minute) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, timestamp->second,
            expectedTimestamp->second) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, timestamp->fsecond,
            expectedTimestamp->fsecond) < 0)
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
// dpiTest_1200_verifyMetadata()
//   Prepare and execute a query that returns each of the possible combinations
// of data types and verify that the metadata returned by
// dpiStmt_getQueryInfo() matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_1200_verifyMetadata(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *longRawSql = "select LongRawCol from TestLongRaws";
    const char *rowidSql = "select rowid from TestNumbers";
    const char *baseSql = "select * from TestDataTypes";
    uint32_t numQueryColumns;
    dpiConn *conn;
    dpiStmt *stmt;

    // connect to database
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // base sql with most data types
    if (dpiConn_prepareStmt(conn, 0, baseSql, strlen(baseSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "STRINGCOL",
            DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 100, 100, 100, 0,
            0, 0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 2, "UNICODECOL",
            DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES, 200, 100, 100, 0,
            0, 0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 3, "FIXEDCHARCOL",
            DPI_ORACLE_TYPE_CHAR, DPI_NATIVE_TYPE_BYTES, 100, 100, 100, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyQueryInfo(testCase, stmt, 4, "FIXEDUNICODECOL",
            DPI_ORACLE_TYPE_NCHAR, DPI_NATIVE_TYPE_BYTES, 200, 100, 100, 0, 0,
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
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // sql testing rowid column
    if (dpiConn_prepareStmt(conn, 0, rowidSql, strlen(rowidSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "ROWID",
            DPI_ORACLE_TYPE_ROWID, DPI_NATIVE_TYPE_ROWID, 8, 8, 8, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // sql testing long raw column
    if (dpiConn_prepareStmt(conn, 0, longRawSql, strlen(longRawSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifyQueryInfo(testCase, stmt, 1, "LONGRAWCOL",
            DPI_ORACLE_TYPE_LONG_RAW, DPI_NATIVE_TYPE_BYTES, 0, 0, 0, 0, 0,
            0, 0) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1201_verifyBindsByPos()
//   Prepare and execute a statement that binds each data type and verify that
// the value is bound correctly using dpiStmt_bindValueByPos() (no error).
//-----------------------------------------------------------------------------
int dpiTest_1201_verifyBindsByPos(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *truncateSql = "truncate table TestDataTypes";
    const char *insertSql = "insert into TestDataTypes (StringCol, "
            "UnicodeCol, FixedCharCol, FixedUnicodeCol, RawCol, FloatCol, "
            "DoublePrecCol, IntCol, NumberCol, DateCol, TimestampCol, "
            "TimestampTZCol, TimestampLTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, LongCol, UnconstrainedCol) values "
            "(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, "
            ":15, :16, :17, :18, :19)";
    uint32_t numQueryColumns;
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
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
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
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1202_verifyBindsByName()
//   Prepare and execute a statement that binds each data type and verify that
// the value is bound correctly using dpiStmt_bindValueByName() (no error).
//-----------------------------------------------------------------------------
int dpiTest_1202_verifyBindsByName(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *truncateSql = "truncate table TestDataTypes";
    const char *insertSql = "insert into TestDataTypes (StringCol, "
            "UnicodeCol, FixedCharCol, FixedUnicodeCol, RawCol, FloatCol, "
            "DoublePrecCol, IntCol, NumberCol, DateCol, TimestampCol, "
            "TimestampTZCol, TimestampLTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, LongCol, UnconstrainedCol) values "
            "(:stringCol, :unicodeCol, :fixedCharCol, :fixedUnicodeCol, "
            ":rawCol, :floatCol, :doublePrecCol, :intCol, :numberCol, "
            ":dateCol, :timestampCol, :timestampTZCol, :timestampLTZCol, "
            ":intervalDSCol, :intervalYMCol, :binaryFltCol, :binaryDoubleCol, "
            ":longCol, :unconstrainedCol)";
    uint32_t numQueryColumns;
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
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
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
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1203_verifyDMLReturningValues()
//   Prepare and execute a DML returning statement that returns each of the
// possible combinations noted above and verify that the value is returned
// correctly in each case (no error).
//-----------------------------------------------------------------------------
int dpiTest_1203_verifyDMLReturningValues(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *truncateSql = "delete from TestDataTypes";
    const char *insertSql = "insert into TestDataTypes (StringCol, "
            "UnicodeCol, FixedCharCol, FixedUnicodeCol, RawCol, FloatCol, "
            "DoublePrecCol, IntCol, NumberCol, DateCol, TimestampCol, "
            "TimestampTZCol, TimestampLTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, LongCol, UnconstrainedCol) "
            "values ('string', 'unistring', 'fixedchar', 'fixedunichar', "
            "'12AB', 1.25, 1.44, 6, 1.35, TO_DATE('2002/12/10 01:02:03', "
            "'yyyy/mm/dd hh24:mi:ss'), to_timestamp('20021210', 'YYYYMMDD'), "
            "to_timestamp_tz('20021210 01:02:03 00:00', "
            "'YYYYMMDD HH24:MI:SS TZH:TZM'), "
            "to_timestamp_tz('20021210 01:02:03 00:00', "
            "'YYYYMMDD HH24:MI:SS TZH:TZM'), INTERVAL '3' DAY, "
            " INTERVAL '1' YEAR, 1.75, 1.95, 1.454, 999)";
    const char *deleteSql = "delete from TestDataTypes returning StringCol, "
            "UnicodeCol, FloatCol, DoublePrecCol, IntCol, NumberCol, DateCol, "
            "TimestampCol, TimestampTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, UnconstrainedCol into "
            ":stringCol, :unicodeCol, :floatCol, :doublePrecCol, :intCol, "
            ":numberCol, :dateCol, :timestampCol, :timestampTZCol, "
            ":intervalDSCol, :intervalYMCol, :binaryFltCol, :binaryDoubleCol, "
            ":unconstrainedCol";
    uint32_t numQueryColumns, numCols = 14, i;
    dpiData *colData[14], tempData;
    dpiVar *colVar[14];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate table
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
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

    // delete statement with DML returning
    if (dpiConn_prepareStmt(conn, 0, deleteSql, strlen(deleteSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numCols; i++) {
        if (dpiStmt_bindByPos(stmt, i + 1, colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // compare data returned to expected data
    if (dpiTestCase_expectStringEqual(testCase, "string", strlen("string"),
            colData[0]->value.asBytes.ptr,
            colData[0]->value.asBytes.length) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, "unistring",
            strlen("unistring"), colData[1]->value.asBytes.ptr,
            colData[1]->value.asBytes.length) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 1.25,
            colData[2]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 1.44,
            colData[3]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectIntEqual(testCase, 6,
            colData[4]->value.asUint64) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 1.35,
            colData[5]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData, 2002, 12, 10, 1, 2, 3, 0, 0, 0);
    if (dpiTest__compareTimestamps(testCase, &tempData.value.asTimestamp,
            &colData[6]->value.asTimestamp) < 0)
        return DPI_FAILURE;
    dpiData_setTimestamp(&tempData, 2002, 12, 10, 0, 0, 0, 0, 0, 0);
    if (dpiTest__compareTimestamps(testCase, &tempData.value.asTimestamp,
            &colData[7]->value.asTimestamp) < 0)
        return DPI_FAILURE;
    dpiData_setTimestamp(&tempData, 2002, 12, 10, 1, 2, 3, 0, 0, 0);
    if (dpiTest__compareTimestamps(testCase, &tempData.value.asTimestamp,
            &colData[8]->value.asTimestamp) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, 3,
            colData[9]->value.asIntervalDS.days) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, 1,
            colData[10]->value.asIntervalYM.years) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 1.75,
            colData[11]->value.asFloat) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 1.95,
            colData[12]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 999,
            colData[13]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    for (i = 0; i < numCols; i++) {
        if (dpiVar_release(colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1204_verifyInOutBindVariables()
//   Prepare and execute a PL/SQL statement that binds both in and out each
// of the possible combinations of data types and verify that the value is
// returned correctly in each case (no error).
//-----------------------------------------------------------------------------
int dpiTest_1204_verifyInOutBindVariables(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin proc_TestInOut(:1, :2, :3, :4, :5, :6, :7, :8, "
            ":9, :10, :11, :12); end;";
    uint32_t numQueryColumns, numCols = 12, i;
    dpiData *inOutData[12], tempData;
    dpiVar *inOutVar[12];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            100, 0, 0, NULL, &inOutVar[0], &inOutData[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            1, 100, 0, 0, NULL, &inOutVar[1], &inOutData[1]) < 0)
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

    // set input values
    dpiData_setBytes(inOutData[0], "String", strlen("String"));
    dpiData_setBytes(inOutData[1], "Unicode", strlen("Unicode"));
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

    // call stored procedure
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numCols; i++){
        if (dpiStmt_bindByPos(stmt, i + 1, inOutVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // compare results
    if (dpiTestCase_expectDoubleEqual(testCase, 3,
            inOutData[2]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 3,
            inOutData[3]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, 2,
            inOutData[4]->value.asInt64) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&tempData, 2018, 6, 1, 1, 2, 1, 0, 0, 0);
    if (dpiTest__compareTimestamps(testCase, &tempData.value.asTimestamp,
            &inOutData[5]->value.asTimestamp) < 0)
        return DPI_FAILURE;
    dpiData_setTimestamp(&tempData, 2017, 6, 1, 1, 32, 1, 0, 0, 0);
    if (dpiTest__compareTimestamps(testCase, &tempData.value.asTimestamp,
            &inOutData[6]->value.asTimestamp) < 0)
        return DPI_FAILURE;
    if (dpiTest__compareTimestamps(testCase, &tempData.value.asTimestamp,
            &inOutData[7]->value.asTimestamp) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, 6,
            inOutData[8]->value.asIntervalDS.days) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, 2,
            inOutData[9]->value.asIntervalYM.years) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, (float)2.68,
            inOutData[10]->value.asFloat) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 3.9,
            inOutData[11]->value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    for (i = 0; i < numCols; i++) {
        if (dpiVar_release(inOutVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1205_verifyObjectAttributes()
//   Get and set the attribute value of an object that uses each of the
// possible combinations noted above and verify that the value is both set and
// acquired correctly in each case (no error).
//-----------------------------------------------------------------------------
int dpiTest_1205_verifyObjectAttributes(dpiTestCase *testCase,
                                        dpiTestParams *params)
{
    const char *insertSql = "insert into TestObjectDataTypes values (:1)";
    const char *selectSql = "select ObjectCol from TestObjectDataTypes";
    uint32_t numQueryColumns, i, bufferRowIndex, numAttrs = 12;
    const char *objectName = "UDT_OBJECTDATATYPES";
    dpiData data, *objColValue, attrValues[12];
    dpiNativeTypeNum nativeTypeNum;
    dpiObjectAttrInfo attrInfo;
    dpiObjectAttr *attrs[12];
    dpiQueryInfo queryInfo;
    dpiObjectType *objType;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
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
    dpiData_setInt64(&data, 5);
    if (dpiObject_setAttributeValue(obj, attrs[4], DPI_NATIVE_TYPE_INT64,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 1.25);
    if (dpiObject_setAttributeValue(obj, attrs[5], DPI_NATIVE_TYPE_DOUBLE,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&data, 2017, 6, 1, 2, 2, 1, 0, 0, 0);
    if (dpiObject_setAttributeValue(obj, attrs[6], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_setAttributeValue(obj, attrs[7], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_setAttributeValue(obj, attrs[8], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiObject_setAttributeValue(obj, attrs[9], DPI_NATIVE_TYPE_TIMESTAMP,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setFloat(&data, 13.25);
    if (dpiObject_setAttributeValue(obj, attrs[10], DPI_NATIVE_TYPE_FLOAT,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setDouble(&data, 13.25);
    if (dpiObject_setAttributeValue(obj, attrs[11], DPI_NATIVE_TYPE_DOUBLE,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert data
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setObject(&data, obj);
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_OBJECT, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
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
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
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
    obj = objColValue->value.asObject;

    // retrieve all of the attributes
    for (i = 0; i < numAttrs; i++) {
        if (dpiObjectAttr_getInfo(attrs[i], &attrInfo) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiObject_getAttributeValue(obj, attrs[i],
                attrInfo.typeInfo.defaultNativeTypeNum, &attrValues[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // compare values
    if (dpiTestCase_expectStringEqual(testCase, "StringData",
            strlen("StringData"), attrValues[0].value.asBytes.ptr,
            attrValues[0].value.asBytes.length) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, "UnicodeData",
            strlen("UnicodeData"), attrValues[1].value.asBytes.ptr,
            attrValues[1].value.asBytes.length) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            "FixedCharData                 ",
            strlen("FixedCharData                 "),
            attrValues[2].value.asBytes.ptr,
            attrValues[2].value.asBytes.length) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase,
            "FixedUnicodeData              ",
            strlen("FixedUnicodeData              "),
            attrValues[3].value.asBytes.ptr,
            attrValues[3].value.asBytes.length) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 5,
            attrValues[4].value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 1.25,
            attrValues[5].value.asDouble) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setTimestamp(&data, 2017, 6, 1, 2, 2, 1, 0, 0, 0);
    if (dpiTest__compareTimestamps(testCase, &data.value.asTimestamp,
            &attrValues[6].value.asTimestamp) < 0)
        return DPI_FAILURE;
    if (dpiTest__compareTimestamps(testCase, &data.value.asTimestamp,
            &attrValues[7].value.asTimestamp) < 0)
        return DPI_FAILURE;
    if (dpiTest__compareTimestamps(testCase, &data.value.asTimestamp,
            &attrValues[8].value.asTimestamp) < 0)
        return DPI_FAILURE;
    if (dpiTest__compareTimestamps(testCase, &data.value.asTimestamp,
            &attrValues[9].value.asTimestamp) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectDoubleEqual(testCase, 13.25,
            attrValues[10].value.asFloat) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectDoubleEqual(testCase, 13.25,
            attrValues[11].value.asDouble) < 0)
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
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1200);
    dpiTestSuite_addCase(dpiTest_1200_verifyMetadata,
            "verify metadata returned by dpiStmt_getQueryInfo()");
    dpiTestSuite_addCase(dpiTest_1201_verifyBindsByPos,
            "bind all data types using position and verify");
    dpiTestSuite_addCase(dpiTest_1202_verifyBindsByName,
            "bind all data types using name and verify");
    dpiTestSuite_addCase(dpiTest_1203_verifyDMLReturningValues,
            "verify DML returning stmt returns values correctly");
    dpiTestSuite_addCase(dpiTest_1204_verifyInOutBindVariables,
            "verify in/out binds are handled correctly");
    dpiTestSuite_addCase(dpiTest_1205_verifyObjectAttributes,
            "verify get and set attributes of an object");
    return dpiTestSuite_run();
}

