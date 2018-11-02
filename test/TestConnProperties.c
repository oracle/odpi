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
// TestConnProperties.c
//   Test suite for testing connection properties.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__checkAttribute()
//   Using the provided SQL, check that the value returned matches the value
// that was set.
//-----------------------------------------------------------------------------
static int dpiTest__checkAttribute(dpiTestCase *testCase, dpiConn *conn,
        const char *sql, const char *setValue, uint32_t setValueLength)
{
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiData *getValue;
    dpiStmt *stmt;
    int found;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (setValueLength == 0)
        setValueLength = (uint32_t) strlen(setValue);
    if (dpiTestCase_expectStringEqual(testCase, getValue->value.asBytes.ptr,
            getValue->value.asBytes.length, setValue, setValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_400_setCurrentSchema()
//   Call dpiConn_setCurrentSchema() with an invalid schema name; perform any
// query (error ORA-01435).
//-----------------------------------------------------------------------------
int dpiTest_400_setCurrentSchema(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiTestCase_expectError(testCase,
            "ORA-01435: user does not exist") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_401_getEdition()
//   Call dpiConn_getEdition() and verify that the value returned is an empty
// string.
//-----------------------------------------------------------------------------
int dpiTest_401_getEdition(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t valueLength;
    const char *value;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getEdition(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectStringEqual(testCase, value, valueLength, "", 0);
}


//-----------------------------------------------------------------------------
// dpiTest_402_withValidEncoding()
//   Call dpiConn_create() specifying a value for the encoding and nencoding
// attributes of the dpiCommonCreateParams structure and then call
// dpiConn_getEncodingInfo() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_402_withValidEncoding(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
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
// dpiTest_403_withInvalidEncoding()
//   Call dpiConn_create() specifying an invalid value for the encoding or
// nencoding attributes of the dpiCommonCreateParams structure
// (error DPI-1026).
//-----------------------------------------------------------------------------
int dpiTest_403_withInvalidEncoding(dpiTestCase *testCase,
        dpiTestParams *params)
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
    dpiConn_create(context, params->mainUserName, params->mainUserNameLength,
            params->mainPassword, params->mainPasswordLength,
            params->connectString, params->connectStringLength, &commonParams,
            NULL, &conn);
    return dpiTestCase_expectError(testCase,
            "DPI-1026: invalid character set xx");
}


//-----------------------------------------------------------------------------
// dpiTest_404_checkExternalName()
//   Call dpiConn_getExternalName() and verify that the value returned is an
// empty string; call dpiConn_setExternalName() with any value and then call
// dpiConn_getExternalName() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_404_checkExternalName(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *value, *setValue = "xx";
    uint32_t valueLength;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getExternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, value, valueLength, "", 0) < 0)
        return DPI_FAILURE;
    if (dpiConn_setExternalName(conn, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getExternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectStringEqual(testCase, value, valueLength,
            setValue, strlen(setValue));
}


//-----------------------------------------------------------------------------
// dpiTest_405_checkInternalName()
//   Call dpiConn_getInternalName() and verify that the value returned is an
// empty string; call dpiConn_setInternalName() with any value and then call
// dpiConn_getInternalName() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_405_checkInternalName(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *value, *setValue = "xyz";
    uint32_t valueLength;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getInternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, value, valueLength, "", 0) < 0)
        return DPI_FAILURE;
    if (dpiConn_setInternalName(conn, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getInternalName(conn, &value, &valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectStringEqual(testCase, value, valueLength,
            setValue, strlen(setValue));
}


//-----------------------------------------------------------------------------
// dpiTest_406_checkStmtCacheSize()
//   Call dpiConn_getStmtCacheSize() and verify that the value returned matches
// the default value (20) (no error), verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_406_checkStmtCacheSize(dpiTestCase *testCase,
        dpiTestParams *params)
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
// dpiTest_407_withValidEncoding()
//   Call dpiConn_create() specifying a value for the encoding and null for
// nencoding of the dpiCommonCreateParams structure and then call
// dpiConn_getEncodingInfo() to verify that the values are as expected.
//-----------------------------------------------------------------------------
int dpiTest_407_withValidEncoding(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *charSet = "ISO-8859-13";
    dpiCommonCreateParams commonParams;
    dpiEncodingInfo info, defaultInfo;
    dpiConn *conn, *defaultConn;
    dpiContext *context;

    // get default encodings
    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL,
            &defaultConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(defaultConn, &defaultInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get connection with just the encoding specified
    commonParams.encoding = charSet;
    commonParams.nencoding = NULL;
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(conn, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.encoding,
            strlen(info.encoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, info.nencoding,
            strlen(info.nencoding), defaultInfo.nencoding,
            strlen(defaultInfo.nencoding)) < 0)
        return DPI_FAILURE;
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get connection with just the nencoding specified
    commonParams.encoding = NULL;
    commonParams.nencoding = charSet;
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(conn, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.encoding,
            strlen(info.encoding), defaultInfo.encoding,
            strlen(defaultInfo.encoding)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, info.nencoding,
            strlen(info.nencoding), charSet, strlen(charSet)) < 0)
        return DPI_FAILURE;
    if (dpiConn_release(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiConn_release(defaultConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_408_verifyClientIdentifierIsSetAsExp()
//   Call dpiConn_setClientIdentifier() and check that the value has been set
// correctly by querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_408_verifyClientIdentifierIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "select sys_context('userenv', 'client_identifier') from dual";
    const char *setValue = "ODPIC_TEST_CLIENT_IDENTIFIER";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_setClientIdentifier(conn, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__checkAttribute(testCase, conn, sql, setValue, 0);
}


//-----------------------------------------------------------------------------
// dpiTest_409_verifyClientInfoIsSetAsExp()
//   Call dpiConn_setClientInfo() and check that the value has been set
// correctly by querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_409_verifyClientInfoIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select sys_context('userenv', 'client_info') from dual";
    const char *setValue = "ODPIC_TEST_CLIENTINFO";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_setClientInfo(conn, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__checkAttribute(testCase, conn, sql, setValue, 0);
}


//-----------------------------------------------------------------------------
// dpiTest_410_verifyActionIsSetAsExp()
//   Call dpiConn_setAction() and check that the value has been set correctly
// by querying the value from the database (no error)
//-----------------------------------------------------------------------------
int dpiTest_410_verifyActionIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select sys_context('userenv', 'action') from dual";
    const char *setValue = "ODPIC_TEST_ACTION";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_setAction(conn, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__checkAttribute(testCase, conn, sql, setValue, 0);
}


//-----------------------------------------------------------------------------
// dpiTest_411_verifyModuleIsSetAsExp()
//   Call dpiConn_setModule() and check that the value has been set correctly
// by querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_411_verifyModuleIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select sys_context('userenv', 'module') from dual";
    const char *setValue = "ODPIC_TEST_MODULE";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_setModule(conn, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__checkAttribute(testCase, conn, sql, setValue, 0);
}


//-----------------------------------------------------------------------------
// dpiTest_412_verifySetCurSchemaWorksAsExp()
//   Call dpiConn_setCurrentSchema() with a valid schema name and check that
// the value has been set correctly by querying the value from the database
// (no error).
//-----------------------------------------------------------------------------
int dpiTest_412_verifySetCurSchemaWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql =
            "select sys_context('userenv', 'current_schema') from dual";
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_setCurrentSchema(conn, params->editionUserName,
            params->editionUserNameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__checkAttribute(testCase, conn, sql,
            params->editionUserName, params->editionUserNameLength);
}


//-----------------------------------------------------------------------------
// dpiTest_413_verifyGetCurSchemaWorksAsExp()
//   Call dpiConn_getCurrentSchema() and verify that the value returned is an
// empty string; call dpiConn_setCurrentSchema() followed by
// dpiConn_getCurrentSchema() and verify that the value is identical to
// what was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_413_verifyGetCurSchemaWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t getValueLength;
    const char *getValue;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getCurrentSchema(conn, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength, "",
            0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_setCurrentSchema(conn, params->editionUserName,
            params->editionUserNameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getCurrentSchema(conn, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            params->editionUserName, params->editionUserNameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_414_verifyGetEditionWorksAsExp()
//   Call dpiConn_create() specifying a value for the edition attribute of the
// dpiCommonCreateParams structure and then call dpiConn_getEdition() and
// verify that the value returned matches the value passed to the connection
// creation routine (no error).
//-----------------------------------------------------------------------------
int dpiTest_414_verifyGetEditionWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiCommonCreateParams commonParams;
    uint32_t getValueLength;
    const char *getValue;
    dpiContext *context;
    dpiConn *conn;

    dpiTestSuite_getContext(&context);
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    commonParams.edition = params->editionName;
    commonParams.editionLength = params->editionNameLength;
    if (dpiConn_create(context, params->editionUserName,
            params->editionUserNameLength, params->editionPassword,
            params->editionPasswordLength, params->connectString,
            params->connectStringLength, &commonParams, NULL, &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEdition(conn, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            params->editionName, params->editionNameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_415_verifyDBOPIsSetAsExp()
//   Call dpiConn_setDbOp() and check that the value has been set correctly by
// querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_415_verifyDBOPIsSetAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select dbop_name from v$sql_monitor "
            "where sid = sys_context('userenv', 'sid') "
            "and status = 'EXECUTING'";
    const char *setValue = "ODPIC_TEST_DBOP";
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_setDbOp(conn, setValue, strlen(setValue)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__checkAttribute(testCase, conn, sql, setValue, 0);
}


//-----------------------------------------------------------------------------
// dpiTest_416_verifygetLTXIDWorksAsExp()
//   Call dpiConn_getLTXID() and verify that the value returned is an empty
// string (no error).
//-----------------------------------------------------------------------------
int dpiTest_416_verifygetLTXIDWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t getValueLength;
    const char *getValue;
    dpiConn *conn;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getLTXID(conn, &getValue, &getValueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectStringEqual(testCase, getValue, getValueLength,
            "", 0);
}


//-----------------------------------------------------------------------------
// dpiTest_417_verifyGetServerVersionWorksAsExp()
//   Call dpiConn_getServerVersion() and verify that the values returned
// matches the database version that is being used (no error).
//-----------------------------------------------------------------------------
int dpiTest_417_verifyGetServerVersionWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select version_full from product_component_version "
            "where product like 'Oracle Database%'";
    const char *sqlPre18 = "select version from product_component_version "
            "where product like 'Oracle Database%'";
    dpiVersionInfo versionInfo, *clientVersionInfo;
    uint32_t bufferRowIndex, releaseStringLength;
    dpiNativeTypeNum nativeTypeNum;
    const char *releaseString;
    char tempVersion[40];
    dpiData *getValue;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    dpiTestSuite_getClientVersionInfo(&clientVersionInfo);
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getServerVersion(conn, &releaseString, &releaseStringLength,
            &versionInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    snprintf(tempVersion, sizeof(tempVersion), "%d.%d.%d.%d.%d",
            versionInfo.versionNum, versionInfo.releaseNum,
            versionInfo.updateNum, versionInfo.portReleaseNum,
            versionInfo.portUpdateNum);
    if (versionInfo.versionNum < 18 || clientVersionInfo->versionNum < 18)
        sql = sqlPre18;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &getValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, tempVersion,
            strlen(tempVersion), getValue->value.asBytes.ptr,
            getValue->value.asBytes.length) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(400);
    dpiTestSuite_addCase(dpiTest_400_setCurrentSchema,
            "dpiConn_setCurrentSchema() with an invalid schema name");
    dpiTestSuite_addCase(dpiTest_401_getEdition,
            "dpiConn_getEdition() with no edition set");
    dpiTestSuite_addCase(dpiTest_402_withValidEncoding,
            "dpiConn_create() specifying a value for the encoding");
    dpiTestSuite_addCase(dpiTest_403_withInvalidEncoding,
            "dpiConn_create() specifying an invalid value for the encoding");
    dpiTestSuite_addCase(dpiTest_404_checkExternalName,
            "check get / set external name");
    dpiTestSuite_addCase(dpiTest_405_checkInternalName,
            "check get / set internal name");
    dpiTestSuite_addCase(dpiTest_406_checkStmtCacheSize,
            "check get / set statement cache size");
    dpiTestSuite_addCase(dpiTest_407_withValidEncoding,
            "specifying a value for the encoding and null for nencoding");
    dpiTestSuite_addCase(dpiTest_408_verifyClientIdentifierIsSetAsExp,
            "verify client identifier attr is set as expected");
    dpiTestSuite_addCase(dpiTest_409_verifyClientInfoIsSetAsExp,
            "verify client info attr is set as expected");
    dpiTestSuite_addCase(dpiTest_410_verifyActionIsSetAsExp,
            "verify action attr is set as expected");
    dpiTestSuite_addCase(dpiTest_411_verifyModuleIsSetAsExp,
            "verify module attr is set as expected");
    dpiTestSuite_addCase(dpiTest_412_verifySetCurSchemaWorksAsExp,
            "verify current schema is set as expected");
    dpiTestSuite_addCase(dpiTest_413_verifyGetCurSchemaWorksAsExp,
            "verify dpiConn_getCurrentSchema() works as expected");
    dpiTestSuite_addCase(dpiTest_414_verifyGetEditionWorksAsExp,
            "verify dpiConn_getEdition() works as expected");
    dpiTestSuite_addCase(dpiTest_415_verifyDBOPIsSetAsExp,
            "verify DBOP attr is set as expected");
    dpiTestSuite_addCase(dpiTest_416_verifygetLTXIDWorksAsExp,
            "verify dpiConn_getLTXID() works as expected");
    dpiTestSuite_addCase(dpiTest_417_verifyGetServerVersionWorksAsExp,
            "verify dpiConn_getServerVersion() works as expected");
    return dpiTestSuite_run();
}

