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
// TestLib.c
//   Common code used by all test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

// global test suite
static dpiTestSuite gTestSuite;

// global DPI context used for most test cases
static dpiContext *gContext = NULL;

// global Oracle Client version information
static dpiVersionInfo gClientVersionInfo;

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
    strncpy((char*) *value, source, *valueLength);
    if (convertToUpper) {
        ptr = (char*) *value;
        for (i = 0; i < *valueLength; i++)
            ptr[i] = toupper(ptr[i]);
    }
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

    if (actualLength == expectedLength && actual &&
            strncmp(actual, expected, expectedLength) == 0)
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
//   Check to see that the error message matches or not
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
    dpiTestSuite__getEnvValue("ODPIC_TEST_CONNECT_STRING", "localhost/orclpdb",
            &params->connectString, &params->connectStringLength, 0);
    dpiTestSuite__getEnvValue("ODPIC_TEST_DIR_NAME", "odpic_dir",
            &params->dirName, &params->dirNameLength, 1);

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
    if (minTestCaseId == 0) {
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
    uint32_t i, numPassed;
    dpiTestCase *testCase;
    int result;

    numPassed = 0;
    for (i = 0; i < gTestSuite.numTestCases; i++) {
        testCase = &gTestSuite.testCases[i];
        fprintf(gTestSuite.logFile, "%d. %s", gTestSuite.minTestCaseId + i,
                testCase->description);
        fflush(gTestSuite.logFile);
        fflush(gTestSuite.logFile);
        result = (*testCase->func)(testCase, &gTestSuite.params);
        if (result == 0) {
            numPassed++;
            fprintf(gTestSuite.logFile, " [OK]\n");
            fflush(gTestSuite.logFile);
        }
        dpiTestCase__cleanUp(testCase);
    }
    dpiContext_destroy(gContext);
    fprintf(gTestSuite.logFile, "%d / %d tests passed\n", numPassed,
            gTestSuite.numTestCases);
    return gTestSuite.numTestCases - numPassed;
}

