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
// TestLOBs.c
//   Test suite for testing all the LOB related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#define DEFAULT_CHARS                   "abcdef"
#define MAX_CHARS                       200


//-----------------------------------------------------------------------------
// dpiTest__expectErrorInvalidOracleType() [INTERNAL]
//   Expect error "DPI-1021: Oracle Type <> is invalid".
//-----------------------------------------------------------------------------
static int dpiTest__expectErrorInvalidOracleType(dpiTestCase *testCase,
        uint32_t typeNum)
{
    char expectedError[512];

    snprintf(expectedError, sizeof(expectedError),
            "DPI-1021: Oracle type %d is invalid", typeNum);
    return dpiTestCase_expectError(testCase, expectedError);
}


//-----------------------------------------------------------------------------
// dpiTest__populateAndGetLobFromTable() [INTERNAL]
//   Function to insert LOB and select LOB from corresponding LOB tables.
//-----------------------------------------------------------------------------
int dpiTest__populateAndGetLobFromTable(dpiTestCase *testCase, dpiConn *conn,
        dpiOracleTypeNum oracleTypeNum, const char *value,
        uint32_t valueLength, dpiLob **lob)
{
    dpiOracleTypeNum varOracleTypeNum;
    dpiNativeTypeNum nativeTypeNum;
    dpiData *varData, *tempData;
    uint32_t bufferRowIndex;
    const char *lobType;
    char sql[100];
    dpiStmt *stmt;
    dpiVar *var;
    int found;

    // verify type of LOB
    switch (oracleTypeNum) {
        case DPI_ORACLE_TYPE_CLOB:
            lobType = "CLOB";
            varOracleTypeNum = DPI_ORACLE_TYPE_VARCHAR;
            break;
        case DPI_ORACLE_TYPE_NCLOB:
            varOracleTypeNum = DPI_ORACLE_TYPE_NVARCHAR;
            lobType = "NCLOB";
            break;
        case DPI_ORACLE_TYPE_BLOB:
            varOracleTypeNum = DPI_ORACLE_TYPE_RAW;
            lobType = "BLOB";
            break;
        default:
            return dpiTestCase_setFailed(testCase, "invalid LOB type");
    }

    // use default values if none supplied
    if (!value) {
        value = DEFAULT_CHARS;
        valueLength = strlen(value);
    }

    // truncate table
    sprintf(sql, "truncate table Test%ss", lobType);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // insert row into the database
    sprintf(sql, "insert into Test%ss values (1, :1)", lobType);
    if (dpiConn_newVar(conn, varOracleTypeNum, DPI_NATIVE_TYPE_BYTES, 1,
            valueLength, 1, 0, NULL, &var, &varData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(var, 0, value, valueLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(var) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform query
    sprintf(sql, "select * from Test%ss for update", lobType);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase, "no rows found");
    if (dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum, &tempData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    *lob = tempData->value.asLOB;
    if (dpiLob_addRef(*lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyLobWithGivenSize() [INTERNAL]
//   Function to fetch LOB and check their sizes.
//-----------------------------------------------------------------------------
int dpiTest__verifyLobWithGivenSize(dpiTestCase *testCase, dpiConn *conn,
        uint32_t lobSize, dpiOracleTypeNum oracleTypeNum)
{
    const char alphaNum[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char *readBuffer, *writeBuffer;
    uint64_t numBytes, i;
    dpiLob *lob;

    readBuffer = malloc(lobSize);
    writeBuffer = malloc(lobSize);
    if (!readBuffer || !writeBuffer)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    for (i = 0; i < lobSize; i++)
        writeBuffer[i] = alphaNum[rand() % (sizeof(alphaNum) - 1)];
    if (dpiTest__populateAndGetLobFromTable(testCase, conn, oracleTypeNum,
            writeBuffer, lobSize, &lob) < 0)
        return DPI_FAILURE;
    numBytes = lobSize;
    if (dpiLob_readBytes(lob, 1, lobSize, readBuffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, readBuffer, numBytes,
            writeBuffer, lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    free(readBuffer);
    free(writeBuffer);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1900_createAllTypesOfLobs()
//   Call dpiConn_newTempLob() for lobType values of DPI_ORACLE_TYPE_CLOB,
// DPI_ORACLE_TYPE_BLOB and DPI_ORACLE_TYPE_NCLOB (no error).
//-----------------------------------------------------------------------------
int dpiTest_1900_createAllTypesOfLobs(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_BLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_NCLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1901_createInvalidLobType()
//   Call dpiConn_newTempLob() for an invalid lob type value (error DPI-1021).
//-----------------------------------------------------------------------------
int dpiTest_1901_createInvalidLobType(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_VARCHAR, &lob);
    return dpiTest__expectErrorInvalidOracleType(testCase,
            DPI_ORACLE_TYPE_VARCHAR);
}


//-----------------------------------------------------------------------------
// dpiTest_1902_releaseLobTwice()
//   Call dpiConn_newTempLob() for a valid lob type; call dpiLob_release()
// twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1902_releaseLobTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiLob_release(lob);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiLob handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1903_closeLobAndVerifyPubFuncsOfLobs()
//   Call dpiConn_newTempLob() for a valid lob type; call dpiLob_close(); call
// each of the public LOB functions except dpilob_addRef() and dpiLob_release()
// (error DPI-1040).
//-----------------------------------------------------------------------------
int dpiTest_1903_closeLobAndVerifyPubFuncsOfLobs(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1040: LOB was already closed";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_close(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiLob_copy(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getBufferSize(lob, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getChunkSize(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, NULL, NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getIsResourceOpen(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_getSize(lob, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_openResource(lob);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_closeResource(lob);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_readBytes(lob, 0, 0, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, NULL, 0, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_setFromBytes(lob, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_trim(lob, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiLob_writeBytes(lob, 0, NULL, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1904_callCloseResOnUnopenedRes()
//   Call dpiConn_newTempLob() for a valid lob type; call
// dpiLob_closeResource() (error).
//-----------------------------------------------------------------------------
int dpiTest_1904_callCloseResOnUnopenedRes(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "ORA-22289: cannot perform %s operation "
            "on an unopened file or LOB";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiLob_closeResource(lob);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1905_verifyOpenResWorksAsExp()
//   Call dpiConn_newTempLob() for a valid lob type; call dpiLob_openResource()
// (no error).
//-----------------------------------------------------------------------------
int dpiTest_1905_verifyOpenResWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1906_verifyCloseResOnFetchedLob()
//   Prepare and execute a select for update statment that returns a LOB and
// fetch one of the LOBs; call dpiLob_closeResource() (error).
//-----------------------------------------------------------------------------
int dpiTest_1906_verifyCloseResOnFetchedLob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "ORA-22289: cannot perform  operation "
            "on an unopened file or LOB";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_closeResource(lob);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1907_callCommitOnUnclosedLob()
//   Prepare and execute a select for update statment that returns a LOB and
// fetch one of the LOBs; call dpiLob_openResource(); call dpiLob_writeBytes()
// with any value; call dpiConn_commit() (error).
//-----------------------------------------------------------------------------
int dpiTest_1907_callCommitOnUnclosedLob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "ORA-22297: warning: Open LOBs exist at "
            "transaction commit time";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 1, "test", strlen("test")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_commit(conn);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1908_callCommitOnClosedLob()
//   Prepare and execute a select for update statment that returns a LOB and
// fetch one of the LOBs; call dpiLob_openResource(); call dpiLob_writeBytes()
// with any value; call dpiLob_closeResource(); call dpiConn_commit() (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_1908_callCommitOnClosedLob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 1, "test", strlen("test")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_closeResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1909_verifyGetBuffSizeOnClob()
//   Fetch a CLOB from the database; call dpiLob_getBufferSize() and verify
// that the size in bytes returned is a multiple of the maxBytesPerCharacter
// value in the connection encoding information structure (no error).
//-----------------------------------------------------------------------------
int dpiTest_1909_verifyGetBuffSizeOnClob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint64_t lobSize, sizeInBytes;
    dpiEncodingInfo info;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getBufferSize(lob, lobSize, &sizeInBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(conn, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, sizeInBytes,
            lobSize * info.maxBytesPerCharacter) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1910_verifyGetBuffSizeOnNclob()
//   Fetch a NCLOB from the database; call dpiLob_getBufferSize() and verify
// that the size in bytes returned is a multiple of the nmaxBytesPerCharacter
// value in the connection encoding information structure (no error).
//-----------------------------------------------------------------------------
int dpiTest_1910_verifyGetBuffSizeOnNclob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint64_t lobSize, sizeInBytes;
    dpiEncodingInfo info;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getBufferSize(lob, lobSize, &sizeInBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getEncodingInfo(conn, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, sizeInBytes,
            lobSize * info.nmaxBytesPerCharacter) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1911_verifyGetBuffSizeOnBlob()
//   Fetch a BLOB from the database; call dpiLob_getBufferSize() and verify
// that the size in bytes returned is identical to the sizeInChars value that
// was passed (no error).
//-----------------------------------------------------------------------------
int dpiTest_1911_verifyGetBuffSizeOnBlob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint64_t lobSize, sizeInBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getBufferSize(lob, lobSize, &sizeInBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, sizeInBytes, lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1912_verifyChunkSizeIsAsExp()
//   Fetch a LOB from the database; call dpiLob_getChunkSize() and verify that
// the value returned matches the expected value (no error).
//-----------------------------------------------------------------------------
int dpiTest_1912_verifyChunkSizeIsAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    uint32_t size;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getChunkSize(lob, &size) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase,
            (16384 - size) & 0xfffffe00, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1913_verifyGetDirAndFnmOnLobs()
//   Fetch a CLOB, NCLOB or BLOB from the database; call the function
// dpiLob_getDirectoryAndFileName() (error).
//-----------------------------------------------------------------------------
int dpiTest_1913_verifyGetDirAndFnmOnLobs(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid OCI handle";
    uint32_t directoryAliasLength, fileNameLength;
    const char *directoryAlias, *fileName;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, &directoryAlias, &directoryAliasLength,
            &fileName, &fileNameLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, &directoryAlias, &directoryAliasLength,
            &fileName, &fileNameLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getDirectoryAndFileName(lob, &directoryAlias, &directoryAliasLength,
            &fileName, &fileNameLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1914_verifyGetFileExistsOnLobs()
//   Fetch a CLOB, NCLOB or BLOB from the database; call the function
// dpiLob_getFileExists() (error).
//-----------------------------------------------------------------------------
int dpiTest_1914_verifyGetFileExistsOnLobs(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid OCI handle";
    dpiConn *conn;
    dpiLob *lob;
    int exists;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_getFileExists(lob, &exists);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1915_verifyIsResOpenWorksAsExp()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_getIsResourceOpen() and verify that the
// value returned is 0; call dpiLob_openResource(); call
// dpiLob_getIsResourceOpen() a second time and verify that the value returned
// is now 1 (no error).
//-----------------------------------------------------------------------------
int dpiTest_1915_verifyIsResOpenWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;
    dpiLob *lob;
    int isOpen;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getIsResourceOpen(lob, &isOpen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isOpen, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_openResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getIsResourceOpen(lob, &isOpen) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isOpen, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_closeResource(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1916_verifyGetSizeWorksAsExp()
//   Fetch a LOB from the database; call dpiLob_getSize() and verify that the
// value returned matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_1916_verifyGetSizeWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint64_t lobSize;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, lobSize, 6) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1917_verifySetDirAndFnmOnLobs()
//   Fetch a CLOB, NCLOB or BLOB from the database; call
// dpiLob_setDirectoryAndFileName() (error).
//-----------------------------------------------------------------------------
int dpiTest_1917_verifySetDirAndFnmOnLobs(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "OCI-22275: invalid LOB locator specified";
    const char *dirName = "X", *fileName = "garbage.txt";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, dirName, strlen(dirName), fileName,
            strlen(fileName));
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, dirName, strlen(dirName), fileName,
            strlen(fileName));
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_BLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_setDirectoryAndFileName(lob, dirName, strlen(dirName), fileName,
            strlen(fileName));
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1918_setTrimValueLarger()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_trim() with a value greater than the
// current length of the LOB (error).
//-----------------------------------------------------------------------------
int dpiTest_1918_setTrimValueLarger(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "ORA-22926: specified trim length is "
            "greater than current LOB value's length";
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    dpiLob_trim(lob, 30);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1919_setTrimValueSmaller()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_trim() with a value smaller than the
// current length of the LOB and verify that the new size matches the size
// that was set (no error).
//-----------------------------------------------------------------------------
int dpiTest_1919_setTrimValueSmaller(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint64_t lobSize;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_trim(lob, 3) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_getSize(lob, &lobSize) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, lobSize, 3) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1920_verifyLobCopyWorksAsExp()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_writeBytes() with any value; call
// dpiLob_copy() and verify that the values in the LOBs are identical; call
// dpiLob_writeBytes() on one of the LOBs and verify that the values in the
// other LOB is unchanged (no error).
//-----------------------------------------------------------------------------
int dpiTest_1920_verifyLobCopyWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    dpiLob *lob, *copyLob;
    uint64_t numBytes;
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_writeBytes(lob, 1, "copying", strlen("copying")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_copy(lob, &copyLob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(copyLob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "copying",
            strlen("copying")) < 0)
        return DPI_FAILURE;
    if (dpiLob_writeBytes(copyLob, 1, "Testing", strlen("Testing")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "copying",
            strlen("copying")) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(copyLob) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1921_verifyReadBytesWorksAsExpOnClob()
//   Fetch a CLOB from the database; call dpiLob_readBytes() at a number of
// offsets (beginning, end, somewhere in the middle) and verify that the
// data returned matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_1921_verifyReadBytesWorksAsExpOnClob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "abcdef",
            strlen("abcdef")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 3, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "cdef",
            strlen("cdef")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 8, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "", 0) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1922_verifyReadBytesWorksAsExpOnNclob()
//   Fetch a NCLOB from the database; call dpiLob_readBytes() at a number of
// offsets (beginning, end, somewhere in the middle) and verify that the data
// returned matches expectations (no error).
//-----------------------------------------------------------------------------
int dpiTest_1922_verifyReadBytesWorksAsExpOnNclob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_NCLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "abcdef",
            strlen("abcdef")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 4, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "def",
            strlen("def")) < 0)
        return DPI_FAILURE;
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 8, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, "", 0) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1923_verifySetFromBytesWithSmallerSize()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_setFromBytes() with a value that is
// smaller than the original size of the LOB and verify that the new value
// of the LOB is correct (no error)
//-----------------------------------------------------------------------------
int dpiTest_1923_verifySetFromBytesWithSmallerSize(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *value = "Test";
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_setFromBytes(lob, value, strlen(value)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, value,
            strlen(value)) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1924_verifySetFromBytesWithLargerSize()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_setFromBytes() with a value that is
// larger than the original size of the LOB and verify that the new value of
// the LOB is correct (no error).
//-----------------------------------------------------------------------------
int dpiTest_1924_verifySetFromBytesWithLargerSize(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *value = "a much longer LOB value than previously";
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_setFromBytes(lob, value, strlen(value)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes, value,
            strlen(value)) < 0)
        return DPI_FAILURE;
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1925_verifyWriteBytesWorksAsExpOnClob()
//   Prepare and execute a select for update statement that returns a LOB and
// fetch one of the LOBs; call dpiLob_writeBytes() for a number of offsets
// (beginning, middle and end of the LOB) and after commiting the transaction
// perform a second fetch and verify that the LOB contents match what was
// written (no error).
//-----------------------------------------------------------------------------
int dpiTest_1925_verifyWriteBytesWorksAsExpOnClob(dpiTestCase *testCase,
        dpiTestParams *params)
{
    char buffer[MAX_CHARS];
    uint64_t numBytes;
    dpiConn *conn;
    dpiLob *lob;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__populateAndGetLobFromTable(testCase, conn,
            DPI_ORACLE_TYPE_CLOB, NULL, 0, &lob) < 0)
        return DPI_FAILURE;
    if (dpiLob_writeBytes(lob, 1, "lob", strlen("lob")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 4, "test", strlen("test")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_writeBytes(lob, 8, "ing", strlen("ing")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_commit(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    numBytes = MAX_CHARS;
    if (dpiLob_readBytes(lob, 1, MAX_CHARS, buffer, &numBytes) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, buffer, numBytes,
                "lobtesting", strlen("lobtesting")) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiLob_release(lob) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1926_verifyLobValuesWithDiffSizes()
//   Fetch LOBs (CLOB, NCLOB, BLOB) from the database using a variable with
// native type DPI_NATIVE_TYPE_BYTES; verify that the value returned matches
// the expected value for each of the below combinations (no error):
//     LOB of 64K - 1 bytes
//     LOB of 64K bytes
//     LOB of 64K + 1 bytes
//     LOB of 512K bytes
//     LOB of 512K + 1 bytes
//     LOB of 1024K bytes
//     LOB of 1024K + 1 bytes
//-----------------------------------------------------------------------------
int dpiTest_1926_verifyLobValuesWithDiffSizes(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const uint32_t sizes[7] = { 65535, 65536, 65537, 524288, 524289, 1048576,
            1048577 };
    const dpiOracleTypeNum oracleTypeNums[3] = { DPI_ORACLE_TYPE_CLOB,
            DPI_ORACLE_TYPE_NCLOB, DPI_ORACLE_TYPE_BLOB };
    dpiConn *conn;
    int i, j;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 7; j++) {
            if (dpiTest__verifyLobWithGivenSize(testCase, conn, sizes[j],
                    oracleTypeNums[i]) < 0)
                return DPI_FAILURE;
        }
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1900);
    dpiTestSuite_addCase(dpiTest_1900_createAllTypesOfLobs,
            "create all types of LOBs");
    dpiTestSuite_addCase(dpiTest_1901_createInvalidLobType,
            "create invalid LOB type");
    dpiTestSuite_addCase(dpiTest_1902_releaseLobTwice,
            "call dpiLob_release() twice");
    dpiTestSuite_addCase(dpiTest_1903_closeLobAndVerifyPubFuncsOfLobs,
            "call all LOB public functions after closing the lob");
    dpiTestSuite_addCase(dpiTest_1904_callCloseResOnUnopenedRes,
            "call dpiLob_closeResource on unopened lob");
    dpiTestSuite_addCase(dpiTest_1905_verifyOpenResWorksAsExp,
            "call dpiLob_openResource and verify");
    dpiTestSuite_addCase(dpiTest_1906_verifyCloseResOnFetchedLob,
            "fetch a lob and call dpiLob_closeResource");
    dpiTestSuite_addCase(dpiTest_1907_callCommitOnUnclosedLob,
            "verify commit on a unclosed lob returns error");
    dpiTestSuite_addCase(dpiTest_1908_callCommitOnClosedLob,
            "verify commit on a closed lob");
    dpiTestSuite_addCase(dpiTest_1909_verifyGetBuffSizeOnClob,
            "verify getBufferSize on Clob returns the expected value");
    dpiTestSuite_addCase(dpiTest_1910_verifyGetBuffSizeOnNclob,
            "verify getBufferSize on Nclob returns the expected value");
    dpiTestSuite_addCase(dpiTest_1911_verifyGetBuffSizeOnBlob,
            "verify getBufferSize on Blob returns the expected value");
    dpiTestSuite_addCase(dpiTest_1912_verifyChunkSizeIsAsExp,
            "verify getChunkSize returns the expected value");
    dpiTestSuite_addCase(dpiTest_1913_verifyGetDirAndFnmOnLobs,
            "verify getDirectoryAndFileName func with CLOB, NCLOB, BLOB");
    dpiTestSuite_addCase(dpiTest_1914_verifyGetFileExistsOnLobs,
            "verify getFileExists func with CLOB, NCLOB, BLOB");
    dpiTestSuite_addCase(dpiTest_1915_verifyIsResOpenWorksAsExp,
            "verify dpiLob_getIsResourceOpen works as expected");
    dpiTestSuite_addCase(dpiTest_1916_verifyGetSizeWorksAsExp,
            "verify dpiLob_getSize works as expected");
    dpiTestSuite_addCase(dpiTest_1917_verifySetDirAndFnmOnLobs,
            "verify setDirectoryAndFileName func with CLOB, NCLOB, BLOB");
    dpiTestSuite_addCase(dpiTest_1918_setTrimValueLarger,
            "verify trim function with bigger value than existing length");
    dpiTestSuite_addCase(dpiTest_1919_setTrimValueSmaller,
            "verify trim function with smaller value than existing length");
    dpiTestSuite_addCase(dpiTest_1920_verifyLobCopyWorksAsExp,
            "verify dpiLob_copy works as expected ");
    dpiTestSuite_addCase(dpiTest_1921_verifyReadBytesWorksAsExpOnClob,
            "verify readBytes on Clob with diff offsets works as expected");
    dpiTestSuite_addCase(dpiTest_1922_verifyReadBytesWorksAsExpOnNclob,
            "verify readBytes on Nclob with diff offsets works as expected");
    dpiTestSuite_addCase(dpiTest_1923_verifySetFromBytesWithSmallerSize,
            "verify setFromBytes with small value than existing and verify");
    dpiTestSuite_addCase(dpiTest_1924_verifySetFromBytesWithLargerSize,
            "verify setFromBytes with big value than existing and verify");
    dpiTestSuite_addCase(dpiTest_1925_verifyWriteBytesWorksAsExpOnClob,
            "verify writeBytes on Clob with diff offsets works as expected");
    dpiTestSuite_addCase(dpiTest_1926_verifyLobValuesWithDiffSizes,
            "verify CLOB, NCLOB, BLOB values with different buffer sizes");
    return dpiTestSuite_run();
}
