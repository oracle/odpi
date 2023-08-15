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
// TestLib.c
//   Common code used by all test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

// global test suite
static dpiTestSuite gTestSuite;

// global DPI context used for most test cases
static dpiContext *gContext = NULL;

// global common creation parameters used for creating connections and pools
static dpiCommonCreateParams gCommonCreateParams;

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
    if (testCase->adminConn) {
        dpiConn_release(testCase->adminConn);
        testCase->adminConn = NULL;
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
// dpiTestSuite__fatalDPIError() [INTERNAL]
//   Called when a fatal DPI error is encountered from which recovery is not
// possible. This simply prints the ODPI-C error information to stderr and
// exits the program with a non-zero exit code to indicate an error.
//-----------------------------------------------------------------------------
static void dpiTestSuite__fatalDPIError(dpiErrorInfo *errorInfo,
        const char *message)
{
    fprintf(stderr, "FN: %s\n", errorInfo->fnName);
    fprintf(stderr, "ACTION: %s\n", errorInfo->action);
    fprintf(stderr, "MSG: %.*s\n", errorInfo->messageLength,
                errorInfo->message);
    dpiTestSuite__fatalError(message);
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
    *value = NULL;
    *valueLength = strlen(source);
    if (*valueLength > 0) {
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
// dpiTestCase_expectAnyError() [PUBLIC]
//   Check to see that any of the error message prefixes provided matches the
// actual error returned.
//-----------------------------------------------------------------------------
int dpiTestCase_expectAnyError(dpiTestCase *testCase,
        const char **expectedErrors)
{
    dpiErrorInfo errorInfo;

    dpiTestSuite_getErrorInfo(&errorInfo);
    return dpiTestCase_expectAnyErrorInfo(testCase, &errorInfo,
            expectedErrors);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectAnyErrorInfo() [PUBLIC]
//   Check to see that any of the error message prefixes provided matches the
// actual error returned.
//-----------------------------------------------------------------------------
int dpiTestCase_expectAnyErrorInfo(dpiTestCase *testCase,
        const dpiErrorInfo *errorInfo, const char **expectedErrors)
{
    uint32_t expectedErrorLength, i;
    size_t messageLength;
    char message[512];

    if (errorInfo->messageLength == 0) {
        messageLength = sizeof(message);
        messageLength -= snprintf(message, messageLength,
                "Expected error starting with: '%s'", expectedErrors[0]);
        for (i = 1; ; i++) {
            if (expectedErrors[i] == NULL)
                break;
            messageLength -= snprintf(message + strlen(message), messageLength,
                    " or '%s'", expectedErrors[i]);
        }
        return dpiTestCase_setFailed(testCase, message);
    }

    for (i = 0; ; i++) {
        if (expectedErrors[i] == NULL)
            break;
        expectedErrorLength = strlen(expectedErrors[i]);
        if (strncmp(errorInfo->message, expectedErrors[i],
                expectedErrorLength) == 0)
            return DPI_SUCCESS;
    }

    messageLength = sizeof(message);
    messageLength -= snprintf(message, messageLength,
            "Expected error starting with '%s'", expectedErrors[0]);
    for (i = 1; ; i++) {
        if (expectedErrors[i] == NULL)
            break;
        messageLength -= snprintf(message + strlen(message), messageLength,
                " or '%s'", expectedErrors[i]);
    }
    snprintf(message + strlen(message), messageLength, " but got '%.*s'.\n",
            errorInfo->messageLength, errorInfo->message);
    return dpiTestCase_setFailed(testCase, message);
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
// dpiTestCase_expectRoundTripsEqual()
//   Check that the number of round trips matches the expected value.
//-----------------------------------------------------------------------------
int dpiTestCase_expectRoundTripsEqual(dpiTestCase *testCase, uint64_t expected)
{
    uint64_t currentRoundTrips;

    currentRoundTrips = testCase->roundTrips;
    if (dpiTestCase_updateRoundTrips(testCase) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase,
            testCase->roundTrips - currentRoundTrips, expected);
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
            "String '%.*s' (%u) does not match expected string '%.*s' (%u).\n",
            actualLength, actual, actualLength, expectedLength, expected,
            expectedLength);
    return dpiTestCase_setFailed(testCase, message);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectTimestampEqual() [INTERNAL]
//   Check to see that the timestamps are equal and if not, report a failure
// and set the test case as failed.
//-----------------------------------------------------------------------------
int dpiTestCase_expectTimestampEqual(dpiTestCase *testCase,
        const dpiTimestamp *timestamp, const dpiTimestamp *expectedTimestamp)
{
    char message[512];

    if (timestamp->year == expectedTimestamp->year &&
            timestamp->month == expectedTimestamp->month &&
            timestamp->day == expectedTimestamp->day &&
            timestamp->hour == expectedTimestamp->hour &&
            timestamp->minute == expectedTimestamp->minute &&
            timestamp->second == expectedTimestamp->second &&
            timestamp->fsecond == expectedTimestamp->fsecond)
        return DPI_SUCCESS;
    snprintf(message, sizeof(message),
            "Timestamp %.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.6d does not match "
            "expected timestamp %.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.6d\n",
            timestamp->year, timestamp->month, timestamp->day,
            timestamp->hour, timestamp->minute, timestamp->second,
            timestamp->fsecond, expectedTimestamp->year,
            expectedTimestamp->month, expectedTimestamp->day,
            expectedTimestamp->hour, expectedTimestamp->minute,
            expectedTimestamp->second, expectedTimestamp->fsecond);
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
//   Check to see that the error message prefix matches.
//-----------------------------------------------------------------------------
int dpiTestCase_expectError(dpiTestCase *testCase, const char *expectedError)
{
    const char *expectedErrors[2];

    expectedErrors[0] = expectedError;
    expectedErrors[1] = NULL;
    return dpiTestCase_expectAnyError(testCase, expectedErrors);
}


//-----------------------------------------------------------------------------
// dpiTestCase_expectErrorInfo() [PUBLIC]
//   Check to see that the error message prefix matches.
//-----------------------------------------------------------------------------
int dpiTestCase_expectErrorInfo(dpiTestCase *testCase,
        const dpiErrorInfo *errorInfo, const char *expectedError)
{
    const char *expectedErrors[2];

    expectedErrors[0] = expectedError;
    expectedErrors[1] = NULL;
    return dpiTestCase_expectAnyErrorInfo(testCase, errorInfo, expectedErrors);
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
            gTestSuite.params.connectStringLength, &gCommonCreateParams, NULL,
            conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    testCase->conn = *conn;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTestCase_getPool() [PUBLIC]
//   Create a new pool and return it. If this cannot be done the test case is
// marked as failed.
//-----------------------------------------------------------------------------
int dpiTestCase_getPool(dpiTestCase *testCase, dpiPool **pool)
{
    dpiPoolCreateParams createParams;

    if (dpiContext_initPoolCreateParams(gContext, &createParams) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    createParams.minSessions = DPI_TEST_POOL_MIN_SESSIONS;
    createParams.maxSessions = DPI_TEST_POOL_MAX_SESSIONS;
    createParams.sessionIncrement = DPI_TEST_POOL_SESSION_INCREMENT;
    if (dpiPool_create(gContext, gTestSuite.params.mainUserName,
            gTestSuite.params.mainUserNameLength,
            gTestSuite.params.mainPassword,
            gTestSuite.params.mainPasswordLength,
            gTestSuite.params.connectString,
            gTestSuite.params.connectStringLength, &gCommonCreateParams,
            &createParams, pool) < 0)
        return dpiTestCase_setFailedFromError(testCase);
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
                gTestSuite.params.connectStringLength, &gCommonCreateParams,
                NULL, &conn) < 0)
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
// dpiTestCase_setupRoundTripChecker() [PUBLIC]
//   If an administrative user and password have been specified, establish a
// connection to this user and determine the number of round trips that have
// been made with the connection associated with the test case. If no
// administrative user and password have been specified, the test is marked as
// skipped.
//-----------------------------------------------------------------------------
int dpiTestCase_setupRoundTripChecker(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select sys_context('userenv', 'sid') from dual";
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiContext *context;
    dpiData *data;
    dpiStmt *stmt;
    int found;

    // first, check to see if an administrative user and password have been
    // specified and mark the test as skipped if these are not present
    if (!params->adminUserName || !params->adminPassword)
        return dpiTestCase_setSkipped(testCase,
                "administrative credentials not provided");

    // determine SID for connection
    if (dpiConn_prepareStmt(testCase->conn, 0, sql, strlen(sql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_defineValue(stmt, 1, DPI_ORACLE_TYPE_NUMBER,
            DPI_NATIVE_TYPE_INT64, 0, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    testCase->sid = data->value.asInt64;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // establish connection with administrative user
    dpiTestSuite_getContext(&context);
    if (dpiConn_create(context, params->adminUserName,
            params->adminUserNameLength, params->adminPassword,
            params->adminPasswordLength, params->connectString,
            params->connectStringLength, NULL, NULL, &testCase->adminConn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get the current round trips
    return dpiTestCase_updateRoundTrips(testCase);
}


//-----------------------------------------------------------------------------
// dpiTestCase_updateRoundTrips() [PUBLIC]
//   Update the number of round trips associated with the connection used by
// the test case. This assumes that a call to the function
// dpiTestCase_setupRoundTripChecker() was performed earlier.
//-----------------------------------------------------------------------------
int dpiTestCase_updateRoundTrips(dpiTestCase *testCase)
{
    const char *sql =
            "select ss.value from v$sesstat ss, v$statname sn "
            "where ss.sid = :sid and ss.statistic# = sn.statistic# "
            "  and sn.name like '%roundtrip%client%'";
    dpiNativeTypeNum nativeTypeNum;
    dpiData inData, *outData;
    uint32_t bufferRowIndex;
    dpiStmt *stmt;
    int found;

    inData.isNull = 0;
    inData.value.asInt64 = testCase->sid;
    if (dpiConn_prepareStmt(testCase->adminConn, 0, sql, strlen(sql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64, &inData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_defineValue(stmt, 1, DPI_ORACLE_TYPE_NUMBER,
            DPI_NATIVE_TYPE_INT64, 0, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    testCase->roundTrips = outData->value.asInt64;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

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
    testCase->adminConn = NULL;
    testCase->sid = 0;
    testCase->roundTrips = 0;
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

    // acquire test suite parameters from the environment
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
    dpiTestSuite__getEnvValue("ODPIC_TEST_ADMIN_USER", "",
            &params->adminUserName, &params->adminUserNameLength, 1);
    dpiTestSuite__getEnvValue("ODPIC_TEST_ADMIN_PASSWORD", "",
            &params->adminPassword, &params->adminPasswordLength, 0);

    // set up ODPI-C context and common creation parameters to use the UTF-8
    // encoding
    if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION, NULL,
            &gContext, &errorInfo) < 0)
        dpiTestSuite__fatalDPIError(&errorInfo,
                "Unable to create initial DPI context.");
    if (dpiContext_getClientVersion(gContext, &gClientVersionInfo) < 0) {
        dpiContext_getError(gContext, &errorInfo);
        dpiTestSuite__fatalDPIError(&errorInfo,
                "Unable to create initial DPI context.");
    }
    if (dpiContext_initCommonCreateParams(gContext,
            &gCommonCreateParams) < 0) {
        dpiContext_getError(gContext, &errorInfo);
        dpiTestSuite__fatalDPIError(&errorInfo,
                "Unable to initialize common create parameters.");
    }
    gCommonCreateParams.encoding = "UTF-8";
    gCommonCreateParams.nencoding = "UTF-8";

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
                params->connectStringLength, &gCommonCreateParams, NULL,
                &conn) < 0) {
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
