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
// TestLOBs.c
//   Test suite for testing all the LOB related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

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
    return dpiTestSuite_run();
}

