//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2023, Oracle and/or its affiliates.
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
// test_1300_conn_properties.c
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
// dpiTest__checkServerVersionInfo()
//   Check the server version information, taking into account the changes made
// to how the server version is reported in different versions of the database.
//-----------------------------------------------------------------------------
static int dpiTest__checkServerVersionInfo(dpiTestCase *testCase,
        const char **releaseString, uint32_t *releaseStringLength)
{
    const char *sql = "select version_full from product_component_version "
            "where product like 'Oracle Database%'";
    const char *sqlPre18 = "select version from product_component_version "
            "where product like 'Oracle Database%'";
    const char *versionFormat = "%d.%d.%d.%d.%02d";
    const char *versionFormatPre23 = "%d.%d.%d.%d.%d";
    dpiVersionInfo versionInfo, *clientVersionInfo;
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    char tempVersion[40];
    dpiData *getValue;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    dpiTestSuite_getClientVersionInfo(&clientVersionInfo);
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getServerVersion(conn, releaseString, releaseStringLength,
            &versionInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (versionInfo.versionNum < 23)
        versionFormat = versionFormatPre23;
    snprintf(tempVersion, sizeof(tempVersion), versionFormat,
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
// dpiTest_1300_setCurrentSchema()
//   Call dpiConn_setCurrentSchema() with an invalid schema name; perform any
// query (error ORA-01435/ORA-28726).
//-----------------------------------------------------------------------------
int dpiTest_1300_setCurrentSchema(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedErrors[] = { "ORA-01435:", "ORA-28726:", NULL };
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
    if (dpiTestCase_expectAnyError(testCase, expectedErrors) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1301_getEdition()
//   Call dpiConn_getEdition() and verify that the value returned is an empty
// string.
//-----------------------------------------------------------------------------
int dpiTest_1301_getEdition(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1302_withValidEncoding()
//   Call dpiConn_create() specifying a value for the encoding and nencoding
// attributes of the dpiCommonCreateParams structure and then call
// dpiConn_getEncodingInfo() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_1302_withValidEncoding(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1303_withInvalidEncoding()
//   Call dpiConn_create() specifying an invalid value for the encoding or
// nencoding attributes of the dpiCommonCreateParams structure
// (error DPI-1026).
//-----------------------------------------------------------------------------
int dpiTest_1303_withInvalidEncoding(dpiTestCase *testCase,
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
    return dpiTestCase_expectError(testCase, "DPI-1026:");
}


//-----------------------------------------------------------------------------
// dpiTest_1304_checkExternalName()
//   Call dpiConn_getExternalName() and verify that the value returned is an
// empty string; call dpiConn_setExternalName() with any value and then call
// dpiConn_getExternalName() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_1304_checkExternalName(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1305_checkInternalName()
//   Call dpiConn_getInternalName() and verify that the value returned is an
// empty string; call dpiConn_setInternalName() with any value and then call
// dpiConn_getInternalName() to verify that the values match (no error).
//-----------------------------------------------------------------------------
int dpiTest_1305_checkInternalName(dpiTestCase *testCase, dpiTestParams *params)
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
// dpiTest_1306_checkStmtCacheSize()
//   Call dpiConn_getStmtCacheSize() and verify that the value returned matches
// the default value (20) (no error), verify that the value returned matches
// the value that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1306_checkStmtCacheSize(dpiTestCase *testCase,
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
// dpiTest_1307_withValidEncoding()
//   Call dpiConn_create() specifying a value for the encoding and null for
// nencoding of the dpiCommonCreateParams structure and then call
// dpiConn_getEncodingInfo() to verify that the values are as expected.
//-----------------------------------------------------------------------------
int dpiTest_1307_withValidEncoding(dpiTestCase *testCase, dpiTestParams *params)
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
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    commonParams.encoding = charSet;
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
    if (dpiContext_initCommonCreateParams(context, &commonParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
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
// dpiTest_1308_verifyClientIdentifierIsSetAsExp()
//   Call dpiConn_setClientIdentifier() and check that the value has been set
// correctly by querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_1308_verifyClientIdentifierIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_1309_verifyClientInfoIsSetAsExp()
//   Call dpiConn_setClientInfo() and check that the value has been set
// correctly by querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_1309_verifyClientInfoIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_1310_verifyActionIsSetAsExp()
//   Call dpiConn_setAction() and check that the value has been set correctly
// by querying the value from the database (no error)
//-----------------------------------------------------------------------------
int dpiTest_1310_verifyActionIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_1311_verifyModuleIsSetAsExp()
//   Call dpiConn_setModule() and check that the value has been set correctly
// by querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_1311_verifyModuleIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_1312_verifySetCurSchemaWorksAsExp()
//   Call dpiConn_setCurrentSchema() with a valid schema name and check that
// the value has been set correctly by querying the value from the database
// (no error).
//-----------------------------------------------------------------------------
int dpiTest_1312_verifySetCurSchemaWorksAsExp(dpiTestCase *testCase,
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
// dpiTest_1313_verifyGetCurSchemaWorksAsExp()
//   Call dpiConn_getCurrentSchema() and verify that the value returned is an
// empty string; call dpiConn_setCurrentSchema() followed by
// dpiConn_getCurrentSchema() and verify that the value is identical to
// what was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1313_verifyGetCurSchemaWorksAsExp(dpiTestCase *testCase,
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
// dpiTest_1314_verifyGetEditionWorksAsExp()
//   Call dpiConn_create() specifying a value for the edition attribute of the
// dpiCommonCreateParams structure and then call dpiConn_getEdition() and
// verify that the value returned matches the value passed to the connection
// creation routine (no error).
//-----------------------------------------------------------------------------
int dpiTest_1314_verifyGetEditionWorksAsExp(dpiTestCase *testCase,
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
// dpiTest_1315_verifyDBOPIsSetAsExp()
//   Call dpiConn_setDbOp() and check that the value has been set correctly by
// querying the value from the database (no error).
//-----------------------------------------------------------------------------
int dpiTest_1315_verifyDBOPIsSetAsExp(dpiTestCase *testCase,
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
// dpiTest_1316_verifygetLTXIDWorksAsExp()
//   Call dpiConn_getLTXID() and verify that the value returned is an empty
// string (no error).
//-----------------------------------------------------------------------------
int dpiTest_1316_verifygetLTXIDWorksAsExp(dpiTestCase *testCase,
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
// dpiTest_1317_verifyGetServerVersionWorksAsExp()
//   Call dpiConn_getServerVersion() and verify that the values returned
// matches the database version that is being used (no error).
//-----------------------------------------------------------------------------
int dpiTest_1317_verifyGetServerVersionWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t releaseStringLength;
    const char *releaseString;

    return dpiTest__checkServerVersionInfo(testCase, &releaseString,
            &releaseStringLength);
}


//-----------------------------------------------------------------------------
// dpiTest_1318_verifyGetServerVersionWithNull()
//   Call dpiConn_getServerVersion() with release string as NULL and verify
// that the values returned matches the database version that is being used (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_1318_verifyGetServerVersionWihtNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    return dpiTest__checkServerVersionInfo(testCase, NULL, 0);
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1300);
    dpiTestSuite_addCase(dpiTest_1300_setCurrentSchema,
            "dpiConn_setCurrentSchema() with an invalid schema name");
    dpiTestSuite_addCase(dpiTest_1301_getEdition,
            "dpiConn_getEdition() with no edition set");
    dpiTestSuite_addCase(dpiTest_1302_withValidEncoding,
            "dpiConn_create() specifying a value for the encoding");
    dpiTestSuite_addCase(dpiTest_1303_withInvalidEncoding,
            "dpiConn_create() specifying an invalid value for the encoding");
    dpiTestSuite_addCase(dpiTest_1304_checkExternalName,
            "check get / set external name");
    dpiTestSuite_addCase(dpiTest_1305_checkInternalName,
            "check get / set internal name");
    dpiTestSuite_addCase(dpiTest_1306_checkStmtCacheSize,
            "check get / set statement cache size");
    dpiTestSuite_addCase(dpiTest_1307_withValidEncoding,
            "specifying a value for the encoding and null for nencoding");
    dpiTestSuite_addCase(dpiTest_1308_verifyClientIdentifierIsSetAsExp,
            "verify client identifier attr is set as expected");
    dpiTestSuite_addCase(dpiTest_1309_verifyClientInfoIsSetAsExp,
            "verify client info attr is set as expected");
    dpiTestSuite_addCase(dpiTest_1310_verifyActionIsSetAsExp,
            "verify action attr is set as expected");
    dpiTestSuite_addCase(dpiTest_1311_verifyModuleIsSetAsExp,
            "verify module attr is set as expected");
    dpiTestSuite_addCase(dpiTest_1312_verifySetCurSchemaWorksAsExp,
            "verify current schema is set as expected");
    dpiTestSuite_addCase(dpiTest_1313_verifyGetCurSchemaWorksAsExp,
            "verify dpiConn_getCurrentSchema() works as expected");
    dpiTestSuite_addCase(dpiTest_1314_verifyGetEditionWorksAsExp,
            "verify dpiConn_getEdition() works as expected");
    dpiTestSuite_addCase(dpiTest_1315_verifyDBOPIsSetAsExp,
            "verify DBOP attr is set as expected");
    dpiTestSuite_addCase(dpiTest_1316_verifygetLTXIDWorksAsExp,
            "verify dpiConn_getLTXID() works as expected");
    dpiTestSuite_addCase(dpiTest_1317_verifyGetServerVersionWorksAsExp,
            "verify dpiConn_getServerVersion() with release string");
    dpiTestSuite_addCase(dpiTest_1318_verifyGetServerVersionWihtNull,
            "verify dpiConn_getServerVersion() with NULL release string");
    return dpiTestSuite_run();
}
