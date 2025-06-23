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
// test_1800_misc.c
//   Test suite of miscellenous test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__verifySqlId() [INTERNAL]
//   This function retrieves the SQL_ID of the provided SQL query from v$sql
// and compares it with the SQL_ID fetched from dpiStmt_getInfo().
//-----------------------------------------------------------------------------
int dpiTest__verifySqlId(dpiTestCase *testCase, dpiConn *conn,
        dpiStmt *stmt, const char *sql)
{
    const char *querySql = "select dbms_sql_translator.sql_id(:1) from dual";
    dpiNativeTypeNum nativeTypeNum;
    dpiData *queryData, bindData;
    uint32_t bufferRowIndex;
    dpiStmt *queryStmt;
    dpiStmtInfo info;
    dpiBytes *bytes;
    int found;

    // acquire SQL_ID from the statement that was executed
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // acquire SQL_ID from v$sql
    if (dpiConn_prepareStmt(conn, 0, querySql, strlen(querySql), NULL, 0,
            &queryStmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&bindData, (char*) sql, strlen(sql));
    if (dpiStmt_bindValueByPos(queryStmt, 1, DPI_NATIVE_TYPE_BYTES,
            &bindData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(queryStmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(queryStmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(queryStmt, 1, &nativeTypeNum, &queryData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    bytes = dpiData_getBytes(queryData);
    if (dpiTestCase_expectStringEqual(testCase, info.sqlId, info.sqlIdLength,
            bytes->ptr, bytes->length) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(queryStmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1800()
//   Call dpiConn_changePassword() and create a new connection using the new
// password to verify that the password was indeed changed (no error).
//-----------------------------------------------------------------------------
int dpiTest_1800(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *newpwd = "newpwd";
    dpiContext *context;
    dpiConn *conn;

    // get first connection and change password
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_changePassword(conn, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, newpwd, strlen(newpwd)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get second connection and change password back
    dpiTestSuite_getContext(&context);
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, newpwd, strlen(newpwd),
            params->connectString, params->connectStringLength, NULL, NULL,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_changePassword(conn, params->mainUserName,
            params->mainUserNameLength, newpwd, strlen(newpwd),
            params->mainPassword, params->mainPasswordLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_release(conn);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1801() [INTERNAL]
//   call dpiConn_ping() (no error)
//-----------------------------------------------------------------------------
int dpiTest_1801(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_ping(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1802() [INTERNAL]
//   Prepare and execute a query which fetches an XMLType object and verify
// that it is returned as a string (no error).
//-----------------------------------------------------------------------------
int dpiTest_1802(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "select XMLElement(\"String\", StringCol) from "
            "TestStrings where IntCol = 1";
    const char *expectedValue = "<String>String 1</String>";
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiData *strColValue;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // prepare select statement and execute it
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &strColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, strColValue->value.asBytes.ptr,
            strColValue->value.asBytes.length, expectedValue,
            strlen(expectedValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1803() [INTERNAL]
//   Insert data into XMLCol. Fetch the data from XMLCol and verify(no error).
//-----------------------------------------------------------------------------
int dpiTest_1803(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select XMLCol from TestXML where IntCol = 1";
    const char *insertSql =
            "insert into TestXML (IntCol, XMLCol) values (1, :1)";
    const char *truncateSql = "truncate table TestXML";
    dpiNativeTypeNum nativeTypeNum;
    dpiData xmlData, *fetchedData;
    uint32_t bufferRowIndex, i;
    char xmlVal[1038], *ptr;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // generate random XML string
    strcpy(xmlVal, "<data>");
    ptr = xmlVal + strlen(xmlVal);
    for (i = 0; i < 1024; i++)
        *ptr++ = 'A' + (rand() % 26);
    strcpy(ptr, "</data>");

    // perform insert
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&xmlData, xmlVal, strlen(xmlVal));
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_BYTES, &xmlData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // fetch the data and compare
    if (dpiConn_prepareStmt(conn, 0, selectSql, strlen(selectSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &fetchedData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, fetchedData->value.asBytes.ptr,
            fetchedData->value.asBytes.length - 1, xmlVal, strlen(xmlVal)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1804()
//   Verify dpiData_setNull() and dpiData_getIsNull() are working properly
// (no error).
//-----------------------------------------------------------------------------
int dpiTest_1804(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiData data;

    dpiData_setNull(&data);
    if (dpiTestCase_expectUintEqual(testCase, dpiData_getIsNull(&data), 1) < 0)
        return DPI_FAILURE;
    dpiData_setInt64(&data, 5);
    if (dpiTestCase_expectUintEqual(testCase, dpiData_getIsNull(&data), 0) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1805()
//   Verify SQL_ID is NULL after dpiConn_prepareStmt() and that the value
// returned after dpiStmt_execute() matches the value found in v$sql. This
// requires 12.2 or higher.
//-----------------------------------------------------------------------------
int dpiTest_1805(dpiTestCase *testCase, dpiTestParams *params)
{
    char *sql = "select 100 from dual";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (info.sqlId && info.sqlIdLength > 0)
        return dpiTestCase_setFailed(testCase, "SQL_ID is not NULL");
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifySqlId(testCase, conn, stmt, sql) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1806()
//   Verify SQL_ID is NULL when dpiStmt_execute() fails. This requires 12.2 or
// higher.
//-----------------------------------------------------------------------------
int dpiTest_1806(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "select * from TestDummy";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (info.sqlId && info.sqlIdLength > 0)
        return dpiTestCase_setFailed(testCase, "SQL_ID is not NULL");
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1807()
//   Verify SQL_ID value when using bind variables. This requires 12.2 or
// higher.
//-----------------------------------------------------------------------------
int dpiTest_1807(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "insert into TestTempTable values (:1, 'test1')";
    const char *truncateSql = "truncate table TestTempTable";
    uint32_t iter, numOfRows = 3, numQueryColumns;
    dpiData *intColValue;
    dpiVar *intColVar;
    dpiConn *conn;
    dpiStmt *stmt;

    // truncate table
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform insert with binds and verify SQL_ID
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            4, 0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (iter = 1; iter <= numOfRows; iter++) {
        if (dpiStmt_bindByPos(stmt, 1, intColVar) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        dpiData_setInt64(intColValue, iter);
        if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTest__verifySqlId(testCase, conn, stmt, sql) < 0)
            return DPI_FAILURE;
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(intColVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1800);
    dpiTestSuite_addCase(dpiTest_1800,
            "change password and verify (no error)");
    dpiTestSuite_addCase(dpiTest_1801,
            "dpiConn_ping() (no error)");
    dpiTestSuite_addCase(dpiTest_1802,
            "fetch an XMLType object as a string");
    dpiTestSuite_addCase(dpiTest_1803,
            "insert data into table containing XMLType and verify fetch");
    dpiTestSuite_addCase(dpiTest_1804,
            "verify dpiData_setNull() & dpiData_getIsNull()");
    dpiTestSuite_addCase(dpiTest_1805,
            "verify SQL_ID with simple query");
    dpiTestSuite_addCase(dpiTest_1806,
            "verify SQL_ID is NULL when exec stmt fails");
    dpiTestSuite_addCase(dpiTest_1807,
            "verify SQL_ID values matches with binds");
    dpiTestSuite_run();
    return 0;
}
