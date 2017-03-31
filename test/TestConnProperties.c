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
// TestConnProperties.c
//   Test suite for testing connection properties.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// Test_VerifyInvalidSetSchema() [PRIVATE]
//   Call dpiConn_setCurrentSchema() with an invalid schema name; perform any
// query (error ORA-01435).
//-----------------------------------------------------------------------------
int Test_VerifyInvalidSetSchema(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql_query = "SELECT count(*) FROM TestNumbers";
    uint32_t numQueryColumns;
    dpiStmt *stmt;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_setCurrentSchema(conn, "x", strlen("x")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql_query, strlen(sql_query), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_execute(stmt, 0, &numQueryColumns);
    return dpiTestCase_expectError(testCase, "ORA-01435: user does not exist");
}


//-----------------------------------------------------------------------------
// Test_GetEdition() [PRIVATE]
//   Call dpiConn_getEdition() and verify that the value returned is an empty
// string.
//-----------------------------------------------------------------------------
int Test_GetEdition(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t valueLength;
    const char *value;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getEdition(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectStringEqual(testCase, "", 0, value, valueLength);
}


//-----------------------------------------------------------------------------
// Test_VerifyEncodingInfo() [PRIVATE]
//   Call dpiConn_create() specifying a value for the encoding and nencoding
// attributes of the dpiCommonCreateParams structure and then call
// dpiConn_getEncodingInfo() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int Test_VerifyEncodingInfo(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *charSet = "ISO-8859-13";
    dpiCommonCreateParams commonParams;
    dpiEncodingInfo info;
    dpiContext *context;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    commonParams.encoding = charSet;
    commonParams.nencoding = charSet;
    if (dpiConn_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(conn, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.encoding, 
            strlen(info.encoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, info.nencoding, 
            strlen(info.nencoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    dpiConn_release(conn);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// Test_InvalidEncodingInfo() [PRIVATE]
//   Call dpiConn_create() specifying an invalid value for the encoding or
// nencoding attributes of the dpiCommonCreateParams structure
// (error DPI-1026).
//-----------------------------------------------------------------------------
int Test_InvalidEncodingInfo(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiCommonCreateParams commonParams;
    const char *charSet = "xx";
    dpiContext *context;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    commonParams.encoding = charSet;
    commonParams.nencoding = charSet;
    dpiConn_create(context, params->userName, params->userNameLength,
            params->password, params->passwordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &conn);
    return dpiTestCase_expectError(testCase,
            "DPI-1026: invalid character set xx");
}


//-----------------------------------------------------------------------------
// Test_VerifyExternalName() [PRIVATE]
//   Call dpiConn_getExternalName() and verify that the value returned is an
// empty string; call dpiConn_setExternalName() with any value and then call
// dpiConn_getExternalName() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int Test_VerifyExternalName(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *value = "";
    uint32_t valueLength;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getExternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, "", strlen(""), value,
            valueLength) < 0)
        return DPI_FAILURE;
    if (dpiConn_setExternalName(conn, "xx", strlen("xx")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getExternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectStringEqual(testCase, "xx", strlen("xx"), value,
            valueLength);
}


//-----------------------------------------------------------------------------
// Test_VerifyInternalName() [PRIVATE]
//   Call dpiConn_getInternalName() and verify that the value returned is an
// empty string; call dpiConn_setInternalName() with any value and then call
// dpiConn_getInternalName() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int Test_VerifyInternalName(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t valueLength;
    const char *value;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getInternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, "", strlen(""), value,
            valueLength != DPI_SUCCESS))
        return DPI_FAILURE;
    if (dpiConn_setInternalName(conn, "xx", strlen("xx")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getInternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectStringEqual(testCase, "xx", strlen("xx"), value, 
            valueLength);
}


//-----------------------------------------------------------------------------
// Test_VerifyStmtCacheSize() [PRIVATE]
//   Call dpiConn_getStmtCacheSize() and verify that the value returned matches
// the default value (20) (no error), verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int Test_VerifyStmtCacheSize(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t cacheSize, defCacheSize = 20, setCacheSize = 22;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getStmtCacheSize(conn, &cacheSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, cacheSize, defCacheSize) < 0)
        return DPI_FAILURE;
    if (dpiConn_setStmtCacheSize(conn, setCacheSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getStmtCacheSize(conn, &cacheSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, cacheSize, setCacheSize);
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(400);
    dpiTestSuite_addCase(Test_VerifyInvalidSetSchema,
            "dpiConn_setCurrentSchema() with an invalid schema name");
    dpiTestSuite_addCase(Test_GetEdition,
            "dpiConn_getEdition() with no edition set");
    dpiTestSuite_addCase(Test_VerifyEncodingInfo,
            "dpiConn_create() specifying a value for the encoding");
    dpiTestSuite_addCase(Test_InvalidEncodingInfo,
            "dpiConn_create() specifying an invalid value for the encoding");
    dpiTestSuite_addCase(Test_VerifyExternalName,
            "check get / set external name");
    dpiTestSuite_addCase(Test_VerifyInternalName,
            "check get / set internal name");
    dpiTestSuite_addCase(Test_VerifyStmtCacheSize,
            "check get / set statement cache size");
    return dpiTestSuite_run();
}

