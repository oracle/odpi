//-----------------------------------------------------------------------------
// Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestMisCases.c
//   Test suite of miscellenous test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_900_miscChangePwd() [INTERNAL]
//   Call dpiConn_changePassword() and create a new connection using the new
// password to verify that the password was indeed changed (no error).
//-----------------------------------------------------------------------------
int dpiTest_900_miscChangePwd(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_901_miscPing() [INTERNAL]
//   call dpiConn_ping() (no error)
//-----------------------------------------------------------------------------
int dpiTest_901_miscPing(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_ping(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_902_fetchColAsXMLType() [INTERNAL]
//   Prepare and execute a query which fetches an XMLType object and verify
// that it is returned as a string (no error).
//-----------------------------------------------------------------------------
int dpiTest_902_fetchColAsXMLType(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_903_insertDataIntoXMLColAndVerify() [INTERNAL]
//   Insert data into XMLCol. Fetch the data from XMLCol and verify(no error).
//-----------------------------------------------------------------------------
int dpiTest_903_insertDataIntoXMLColAndVerify(dpiTestCase *testCase,
        dpiTestParams *params)
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
// dpiTest_904_verifySetNullAndGetIsNull()
//   Verify dpiData_setNull() and dpiData_getIsNull() are working properly
// (no error).
//-----------------------------------------------------------------------------
int dpiTest_904_verifySetNullAndGetIsNull(dpiTestCase *testCase,
        dpiTestParams *params)
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
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(900);
    dpiTestSuite_addCase(dpiTest_900_miscChangePwd,
            "change password and verify (no error)");
    dpiTestSuite_addCase(dpiTest_901_miscPing,
            "dpiConn_ping() (no error)");
    dpiTestSuite_addCase(dpiTest_902_fetchColAsXMLType,
            "fetch an XMLType object as a string");
    dpiTestSuite_addCase(dpiTest_903_insertDataIntoXMLColAndVerify,
            "insert data into table containing XMLType and verify fetch");
    dpiTestSuite_addCase(dpiTest_904_verifySetNullAndGetIsNull,
            "verify dpiData_setNull() & dpiData_getIsNull()");
    dpiTestSuite_run();
    return 0;
}
