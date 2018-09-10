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
// TestLib.c
//   Common code used by all test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

// global test suite
static dpiTestSuite gTestSuite;

// global DPI context used for most test cases
static dpiContext *gContext = NULL;

// global Oracle version information
static dpiVersionInfo gClientVersionInfo;
static dpiVersionInfo gDatabaseVersionInfo;
static int gDatabaseVersionAcquired = 0;

//-----------------------------------------------------------------------------
// dpiTestCase__cleanUp() [PUBLIC]
//   Frees the memory used by connections and pools established by the test
// case.
//-----------------------------------------------------------------------------
static void dpiTestCase__cleanUp(dpiTestCase *testCase)
{
    if (testCase->conn) {
        dpiConn_release(testCase->conn);
        testCase->conn = NULL;
    }
}


//-----------------------------------------------------------------------------
// dpiTestSuite__fatalError() [INTERNAL]
//   Called when a fatal error is encountered from which recovery is not
// possible. This simply prints a message to stderr and exits the program with
// a non-zero exit code to indicate an error.
//-----------------------------------------------------------------------------
static void dpiTestSuite__fatalError(const char *message)
{
    fprintf(stderr, "FATAL: %s\n", message);
    exit(1);
}


//-----------------------------------------------------------------------------
// dpiTestSuite__getEnvValue() [PUBLIC]
//   Get value from environment or use supplied default value if the value is
// not set in the environment. Memory is allocated to accommodate the value and
// optionally converted to upper case.
//-----------------------------------------------------------------------------
static void dpiTestSuite__getEnvValue(const char *envName,
        const char *defaultValue, const char **value, uint32_t *valueLength,
        int convertToUpper)
{
    const char *source;
    uint32_t i;
    char *ptr;

    source = getenv(envName);
    if (!source)
        source = defaultValue;
    *valueLength = strlen(source);
    *value = malloc(*valueLength);
    if (!*value)
        dpiTestSuite__fatalError("Out of memory!");
    memcpy((void*) *value, source, *valueLength);
    if (convertToUpper) {
        ptr = (char*) *value;
        for (i = 0; i < *valueLength; i++)
            ptr[i] = toupper(ptr[i]);
    }
}


//-----------------------------------------------------------------------------
// dpiTestCase_cleanupSodaColl()
//   Drops the collection (and verifies that it was actually dropped), then
// releases the collection.
//-----------------------------------------------------------------------------
int dpiTestCase_cleanupSodaColl(dpiTestCase *testCase, dpiSodaColl *coll)
{
    int isDropped;

    if (dpiSodaColl_drop(coll, DPI_SODA_FLAGS_DEFAULT, &isDropped) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, isDropped, 1) < 0)
        return DPI_FAILURE;
    if (dpiSodaColl_release(coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTestCase_dropAllSodaColls()
//   Drops all collections in the database.
//-----------------------------------------------------------------------------
int dpiTestCase_dropAllSodaColls(dpiTestCase *testCase, dpiSodaDb *db)
{
    dpiSodaCollCursor *cursor;
    dpiSodaColl *coll;
    int isDropped;

    // create cursor
    if (dpiSodaDb_getCollections(db, NULL, 0, DPI_SODA_FLAGS_DEFAULT,
            &cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // iterate over all collections and drop them
    while (1) {
        if (dpiSodaCollCursor_getNext(cursor, DPI_SODA_FLAGS_DEFAULT,
                &coll) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!coll)
            break;
        if (dpiSodaColl_drop(coll, DPI_SODA_FLAGS_DEFAULT, &isDropped) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiSodaColl_release(coll) < 0)
             return dpiTestCase_setFailedFromError(testCase);
    }

    // cleanup
    if (dpiSodaCollCursor_release(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectDoubleEqual() [PUBLIC]
//   Check to see that the double values are equal and if not, report a failure
// and set the test case as failed.
//-----------------------------------------------------------------------------
int dpiTestCase_expectDoubleEqual(dpiTestCase *testCase, double actualValue,
        double expectedValue)
{
    char message[512];

    if (actualValue == expectedValue)
        return DPI_SUCCESS;
    snprintf(message, sizeof(message),
            "Value %g does not match expected value %g.\n", actualValue,
            expectedValue);
    return dpiTestCase_setFailed(testCase, message);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectIntEqual() [PUBLIC]
//   Check to see that the signed integers are equal and if not, report a
// failure and set the test case as failed.
//-----------------------------------------------------------------------------
int dpiTestCase_expectIntEqual(dpiTestCase *testCase, int64_t actualValue,
        int64_t expectedValue)
{
    char message[512];

    if (actualValue == expectedValue)
        return DPI_SUCCESS;
    snprintf(message, sizeof(message),
            "Value %" PRId64 " does not match expected value %" PRId64 ".\n",
            actualValue, expectedValue);
    return dpiTestCase_setFailed(testCase, message);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectStringEqual() [PUBLIC]
//   Check to see that the strings are equal and if not, report a failure and
// set the test case as failed.
//-----------------------------------------------------------------------------
int dpiTestCase_expectStringEqual(dpiTestCase *testCase, const char *actual,
        uint32_t actualLength, const char *expected, uint32_t expectedLength)
{
    char message[512];

    if (actualLength == expectedLength && (actualLength == 0 ||
            (actual && strncmp(actual, expected, expectedLength) == 0)))
        return DPI_SUCCESS;
    snprintf(message, sizeof(message),
            "String '%.*s' does not match expected string '%.*s'.\n",
            actualLength, actual, expectedLength, expected);
    return dpiTestCase_setFailed(testCase, message);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectUintEqual() [PUBLIC]
//   Check to see that the unsigned integers are equal and if not, report a
// failure and set the test case as failed.
//-----------------------------------------------------------------------------
int dpiTestCase_expectUintEqual(dpiTestCase *testCase, uint64_t actualValue,
        uint64_t expectedValue)
{
    char message[512];

    if (actualValue == expectedValue)
        return DPI_SUCCESS;
    snprintf(message, sizeof(message),
            "Value %" PRIu64 " does not match expected value %" PRIu64 ".\n",
            actualValue, expectedValue);
    return dpiTestCase_setFailed(testCase, message);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectError() [PUBLIC]
//   Check to see that the error message matches.
//-----------------------------------------------------------------------------
int dpiTestCase_expectError(dpiTestCase *testCase, const char *expectedError)
{
    uint32_t expectedErrorLength;
    dpiErrorInfo errorInfo;
    char message[512];

    dpiTestSuite_getErrorInfo(&errorInfo);
    if (errorInfo.messageLength == 0) {
        snprintf(message, sizeof(message), "Expected error: '%s'",
                expectedError);
        return dpiTestCase_setFailed(testCase, message);
    }

    expectedErrorLength = strlen(expectedError);
    if (errorInfo.messageLength == expectedErrorLength &&
            strncmp(errorInfo.message, expectedError,
                    expectedErrorLength) == 0)
        return DPI_SUCCESS;

    snprintf(message, sizeof(message), "Expected error '%s' but got '%.*s'.\n",
            expectedError, errorInfo.messageLength, errorInfo.message);
    return dpiTestCase_setFailed(testCase, message);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectErrorCode() [PUBLIC]
//   Check to see that the Oracle error code matches.
//-----------------------------------------------------------------------------
int dpiTestCase_expectErrorCode(dpiTestCase *testCase, int32_t expectedCode)
{
    dpiErrorInfo errorInfo;
    char message[256];

    dpiTestSuite_getErrorInfo(&errorInfo);
    if (errorInfo.messageLength == 0) {
        snprintf(message, sizeof(message), "Expected error code: %d",
                expectedCode);
        return dpiTestCase_setFailed(testCase, message);
    }

    if (errorInfo.code == expectedCode)
        return DPI_SUCCESS;
    snprintf(message, sizeof(message), "Expected error %d but got %d.\n",
            expectedCode, errorInfo.code);
    return dpiTestCase_setFailed(testCase, message);
}


//-----------------------------------------------------------------------------
// dpiTestCase_getConnection() [PUBLIC]
//   Create a new standalone connection and return it. If this cannot be done
// the test case is marked as failed.
//-----------------------------------------------------------------------------
int dpiTestCase_getConnection(dpiTestCase *testCase, dpiConn **conn)
{
    if (dpiConn_create(gContext, gTestSuite.params.mainUserName,
            gTestSuite.params.mainUserNameLength,
            gTestSuite.params.mainPassword,
            gTestSuite.params.mainPasswordLength,
            gTestSuite.params.connectString,
            gTestSuite.params.connectStringLength, NULL, NULL, conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    testCase->conn = *conn;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTestCase_getSodaDb() [PUBLIC]
//   Create a new standalone connection and then get the SODA database object
// associated with it. If this cannot be done the test case is marked as
// failed.
//-----------------------------------------------------------------------------
int dpiTestCase_getSodaDb(dpiTestCase *testCase, dpiSodaDb **db)
{
    dpiConn *conn;

    // verify client is a minimum of 18.3 and the server a minimum of 18.0
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 1, 18, 3) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 18, 0) < 0)
        return DPI_FAILURE;

    // get connection and SODA database object
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_getSodaDb(conn, db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTestCase_getDatabaseVersionInfo() [PUBLIC]
//   Return database version information.
//-----------------------------------------------------------------------------
int dpiTestCase_getDatabaseVersionInfo(dpiTestCase *testCase,
        dpiVersionInfo **versionInfo)
{
    uint32_t releaseStringLength;
    const char *releaseString;
    dpiConn *conn;

    if (!gDatabaseVersionAcquired) {
        if (dpiConn_create(gContext, gTestSuite.params.mainUserName,
                gTestSuite.params.mainUserNameLength,
                gTestSuite.params.mainPassword,
                gTestSuite.params.mainPasswordLength,
                gTestSuite.params.connectString,
                gTestSuite.params.connectStringLength, NULL, NULL, &conn) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiConn_getServerVersion(conn, &releaseString,
                &releaseStringLength, &gDatabaseVersionInfo) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        gDatabaseVersionAcquired = 1;
        dpiConn_release(conn);
    }
    *versionInfo = &gDatabaseVersionInfo;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTestCase_setFailed() [PUBLIC]
//   Set the test case as failed. Print the message to the log file and return
// DPI_FAILURE as a convenience to the caller.
//-----------------------------------------------------------------------------
int dpiTestCase_setFailed(dpiTestCase *testCase, const char *message)
{
    fprintf(gTestSuite.logFile, " [FAILED]\n");
    fprintf(gTestSuite.logFile, "    %s\n", message);
    fflush(gTestSuite.logFile);
    return DPI_FAILURE;
}


//-----------------------------------------------------------------------------
// dpiTestCase_setFailedFromError() [PUBLIC]
//   Set the test case as failed from a DPI error. The error is first fetched
// from the global DPI context.
//-----------------------------------------------------------------------------
int dpiTestCase_setFailedFromError(dpiTestCase *testCase)
{
    dpiErrorInfo errorInfo;

    dpiContext_getError(gContext, &errorInfo);
    return dpiTestCase_setFailedFromErrorInfo(testCase, &errorInfo);
}


//-----------------------------------------------------------------------------
// dpiTestCase_setFailedFromErrorInfo() [PUBLIC]
//   Set the test case as failed from a DPI error info structure.
//-----------------------------------------------------------------------------
int dpiTestCase_setFailedFromErrorInfo(dpiTestCase *testCase,
        dpiErrorInfo *info)
{
    fprintf(gTestSuite.logFile, " [FAILED]\n");
    fprintf(gTestSuite.logFile, "    FN: %s\n", info->fnName);
    fprintf(gTestSuite.logFile, "    ACTION: %s\n", info->action);
    fprintf(gTestSuite.logFile, "    MSG: %.*s\n", info->messageLength,
            info->message);
    fflush(gTestSuite.logFile);
    return DPI_FAILURE;
}


//-----------------------------------------------------------------------------
// dpiTestCase_setSkipped() [PUBLIC]
//   Set the test case as skipped. Print the message to the log file and return
// DPI_FAILURE as a convenience to the caller.
//-----------------------------------------------------------------------------
int dpiTestCase_setSkipped(dpiTestCase *testCase, const char *message)
{
    testCase->skipped = 1;
    fprintf(gTestSuite.logFile, " [SKIPPED]\n");
    fprintf(gTestSuite.logFile, "    %s\n", message);
    fflush(gTestSuite.logFile);
    return DPI_FAILURE;
}


//-----------------------------------------------------------------------------
// dpiTestCase_setSkippedIfVersionTooOld() [PUBLIC]
//   Set the test case as skipped if the client and/or database versions are
// too old.
//-----------------------------------------------------------------------------
int dpiTestCase_setSkippedIfVersionTooOld(dpiTestCase *testCase,
        int clientOnly, unsigned minVersionNum, unsigned minReleaseNum)
{
    dpiVersionInfo *versionInfo;
    char message[128];

    // check OCI client version
    dpiTestSuite_getClientVersionInfo(&versionInfo);
    if (versionInfo->versionNum < minVersionNum ||
            (versionInfo->versionNum == minVersionNum &&
             versionInfo->releaseNum < minReleaseNum)) {
        sprintf(message, "OCI client version must be %u.%u or higher",
                minVersionNum, minReleaseNum);
        return dpiTestCase_setSkipped(testCase, message);
    }
    if (clientOnly)
        return DPI_SUCCESS;

    // check database version
    if (dpiTestCase_getDatabaseVersionInfo(testCase, &versionInfo) < 0)
        return DPI_FAILURE;
    if (versionInfo->versionNum < minVersionNum ||
            (versionInfo->versionNum == minVersionNum &&
             versionInfo->releaseNum < minReleaseNum)) {
        sprintf(message, "Database version must be %u.%u or higher",
                minVersionNum, minReleaseNum);
        return dpiTestCase_setSkipped(testCase, message);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTestSuite_addCase() [PUBLIC]
// Adds a test case to the test suite. Memory for the test cases is allocated
// in groups in order to avoid constant memory allocation. Failure to allocate
// memory is a fatal error which will terminate the test program.
//-----------------------------------------------------------------------------
void dpiTestSuite_addCase(dpiTestCaseFunction func, const char *description)
{
    dpiTestCase *testCases, *testCase;

    // allocate space for more test cases if needed
    if (gTestSuite.numTestCases == gTestSuite.allocatedTestCases) {
        gTestSuite.allocatedTestCases += 16;
        testCases = malloc(gTestSuite.allocatedTestCases *
                sizeof(dpiTestCase));
        if (!testCases)
            dpiTestSuite__fatalError("Out of memory!");
        if (gTestSuite.testCases) {
            memcpy(testCases, gTestSuite.testCases,
                    gTestSuite.numTestCases * sizeof(dpiTestCase));
            free(gTestSuite.testCases);
        }
        gTestSuite.testCases = testCases;
    }

    // add new case
    testCase = &gTestSuite.testCases[gTestSuite.numTestCases++];
    testCase->description = description;
    testCase->func = func;
    testCase->conn = NULL;
    testCase->skipped = 0;
}


//-----------------------------------------------------------------------------
// dpiTestSuite_getClientVersionInfo() [PUBLIC]
//   Return Oracle Client version information.
//-----------------------------------------------------------------------------
void dpiTestSuite_getClientVersionInfo(dpiVersionInfo **versionInfo)
{
    *versionInfo = &gClientVersionInfo;
}


//-----------------------------------------------------------------------------
// dpiTestSuite_getContext() [PUBLIC]
//   Return global context used for most test cases.
//-----------------------------------------------------------------------------
void dpiTestSuite_getContext(dpiContext **context)
{
    *context = gContext;
}


//-----------------------------------------------------------------------------
// dpiTestSuite_getErrorInfo() [PUBLIC]
//   Return error information from the global context.
//-----------------------------------------------------------------------------
void dpiTestSuite_getErrorInfo(dpiErrorInfo *errorInfo)
{
    dpiContext_getError(gContext, errorInfo);
}


//-----------------------------------------------------------------------------
// dpiTestSuite_initialize() [PUBLIC]
//   Initializes the global test suite and test parameters structure.
//-----------------------------------------------------------------------------
void dpiTestSuite_initialize(uint32_t minTestCaseId)
{
    uint32_t releaseStringLength;
    const char *releaseString;
    dpiErrorInfo errorInfo;
    dpiTestParams *params;
    dpiConn *conn;

    gTestSuite.numTestCases = 0;
    gTestSuite.allocatedTestCases = 0;
    gTestSuite.testCases = NULL;
    gTestSuite.logFile = stderr;
    gTestSuite.minTestCaseId = minTestCaseId;
    params = &gTestSuite.params;

    dpiTestSuite__getEnvValue("ODPIC_TEST_MAIN_USER", "odpic",
            &params->mainUserName, &params->mainUserNameLength, 1);
    dpiTestSuite__getEnvValue("ODPIC_TEST_MAIN_PASSWORD", "welcome",
            &params->mainPassword, &params->mainPasswordLength, 0);
    dpiTestSuite__getEnvValue("ODPIC_TEST_PROXY_USER", "odpic_proxy",
            &params->proxyUserName, &params->proxyUserNameLength, 1);
    dpiTestSuite__getEnvValue("ODPIC_TEST_PROXY_PASSWORD", "welcome",
            &params->proxyPassword, &params->proxyPasswordLength, 0);
    dpiTestSuite__getEnvValue("ODPIC_TEST_EDITION_USER", "odpic_edition",
            &params->editionUserName, &params->editionUserNameLength, 1);
    dpiTestSuite__getEnvValue("ODPIC_TEST_EDITION_PASSWORD", "welcome",
            &params->editionPassword, &params->editionPasswordLength, 0);
    dpiTestSuite__getEnvValue("ODPIC_TEST_CONNECT_STRING", "localhost/orclpdb",
            &params->connectString, &params->connectStringLength, 0);
    dpiTestSuite__getEnvValue("ODPIC_TEST_DIR_NAME", "odpic_dir",
            &params->dirName, &params->dirNameLength, 1);
    dpiTestSuite__getEnvValue("ODPIC_TEST_EDITION_NAME", "odpic_e1",
            &params->editionName, &params->editionNameLength, 1);

    if (dpiContext_create(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, &gContext,
            &errorInfo) < 0) {
        fprintf(stderr, "FN: %s\n", errorInfo.fnName);
        fprintf(stderr, "ACTION: %s\n", errorInfo.action);
        fprintf(stderr, "MSG: %.*s\n", errorInfo.messageLength,
                errorInfo.message);
        dpiTestSuite__fatalError("Unable to create initial DPI context.");
    }
    if (dpiContext_getClientVersion(gContext, &gClientVersionInfo) < 0) {
        dpiContext_getError(gContext, &errorInfo);
        fprintf(stderr, "FN: %s\n", errorInfo.fnName);
        fprintf(stderr, "ACTION: %s\n", errorInfo.action);
        fprintf(stderr, "MSG: %.*s\n", errorInfo.messageLength,
                errorInfo.message);
        dpiTestSuite__fatalError("Unable to get client version.");
    }

    // if minTestCaseId is 0 a simple connection test is performed
    // and version information is displayed
    if (minTestCaseId == 0) {
        fprintf(stderr, "ODPI-C version: %s\n", DPI_VERSION_STRING);
        fprintf(stderr, "OCI Client version: %d.%d.%d.%d.%d\n",
                gClientVersionInfo.versionNum, gClientVersionInfo.releaseNum,
                gClientVersionInfo.updateNum,
                gClientVersionInfo.portReleaseNum,
                gClientVersionInfo.portUpdateNum);
        if (dpiConn_create(gContext, params->mainUserName,
                params->mainUserNameLength, params->mainPassword,
                params->mainPasswordLength, params->connectString,
                params->connectStringLength, NULL, NULL, &conn) < 0) {
            dpiContext_getError(gContext, &errorInfo);
            fprintf(stderr, "FN: %s\n", errorInfo.fnName);
            fprintf(stderr, "ACTION: %s\n", errorInfo.action);
            fprintf(stderr, "MSG: %.*s\n", errorInfo.messageLength,
                    errorInfo.message);
            fprintf(stderr, "CREDENTIALS: %.*s/%.*s@%.*s\n",
                    params->mainUserNameLength, params->mainUserName,
                    params->mainPasswordLength, params->mainPassword,
                    params->connectStringLength, params->connectString);
            dpiTestSuite__fatalError("Cannot connect to database.");
        }
        if (dpiConn_getServerVersion(conn, &releaseString,
                &releaseStringLength, &gDatabaseVersionInfo) < 0) {
            dpiContext_getError(gContext, &errorInfo);
            fprintf(stderr, "FN: %s\n", errorInfo.fnName);
            fprintf(stderr, "ACTION: %s\n", errorInfo.action);
            fprintf(stderr, "MSG: %.*s\n", errorInfo.messageLength,
                    errorInfo.message);
            dpiTestSuite__fatalError("Cannot get database version.");
        }
        gDatabaseVersionAcquired = 1;
        fprintf(stderr, "OCI Database version: %d.%d.%d.%d.%d\n\n",
                gDatabaseVersionInfo.versionNum,
                gDatabaseVersionInfo.releaseNum,
                gDatabaseVersionInfo.updateNum,
                gDatabaseVersionInfo.portReleaseNum,
                gDatabaseVersionInfo.portUpdateNum);
        fflush(stderr);
    }
}


//-----------------------------------------------------------------------------
// dpiTestSuite_run() [PUBLIC]
//   Runs the test cases in the test suite and reports to stderr the success
// and failure of each of those test cases. When all test cases have completed
// a summary is written to stderr.
//-----------------------------------------------------------------------------
int dpiTestSuite_run()
{
    uint32_t i, numPassed, numSkipped;
    dpiTestCase *testCase;
    int result;

    numPassed = numSkipped = 0;
    for (i = 0; i < gTestSuite.numTestCases; i++) {
        testCase = &gTestSuite.testCases[i];
        fprintf(gTestSuite.logFile, "%d. %s", gTestSuite.minTestCaseId + i,
                testCase->description);
        fflush(gTestSuite.logFile);
        fflush(gTestSuite.logFile);
        result = (*testCase->func)(testCase, &gTestSuite.params);
        if (testCase->skipped)
            numSkipped++;
        else if (result == 0) {
            numPassed++;
            fprintf(gTestSuite.logFile, " [OK]\n");
            fflush(gTestSuite.logFile);
        }
        dpiTestCase__cleanUp(testCase);
    }
    dpiContext_destroy(gContext);
    if (numSkipped > 0)
        fprintf(gTestSuite.logFile, "%d / %d tests passed (%d skipped)\n",
                numPassed, gTestSuite.numTestCases - numSkipped, numSkipped);
    else fprintf(gTestSuite.logFile, "%d / %d tests passed\n", numPassed,
            gTestSuite.numTestCases);
    return gTestSuite.numTestCases - numPassed - numSkipped;
}

