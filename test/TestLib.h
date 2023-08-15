//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// TestLib.h
//   Header used for all test cases.
//----------------------------------------------------------------------------

#include <dpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef _MSC_VER
#if _MSC_VER < 1900
#define PRId64                          "I64d"
#define PRIu64                          "I64u"
#endif
#endif

#ifndef PRIu64
#include <inttypes.h>
#endif

// constants
#define DPI_TEST_POOL_MIN_SESSIONS      3
#define DPI_TEST_POOL_MAX_SESSIONS      9
#define DPI_TEST_POOL_SESSION_INCREMENT 2

// forward declarations
typedef struct dpiTestCase dpiTestCase;
typedef struct dpiTestParams dpiTestParams;
typedef struct dpiTestSuite dpiTestSuite;

// define function prototype for test cases
typedef int (*dpiTestCaseFunction)(dpiTestCase *testCase,
        dpiTestParams *params);

// define test parameters
struct dpiTestParams {
    const char *mainUserName;
    uint32_t mainUserNameLength;
    const char *mainPassword;
    uint32_t mainPasswordLength;
    const char *proxyUserName;
    uint32_t proxyUserNameLength;
    const char *proxyPassword;
    uint32_t proxyPasswordLength;
    const char *editionUserName;
    uint32_t editionUserNameLength;
    const char *editionPassword;
    uint32_t editionPasswordLength;
    const char *editionName;
    uint32_t editionNameLength;
    const char *connectString;
    uint32_t connectStringLength;
    const char *dirName;
    uint32_t dirNameLength;
    const char *adminUserName;
    uint32_t adminUserNameLength;
    const char *adminPassword;
    uint32_t adminPasswordLength;
};

// define test case structure
struct dpiTestCase {
    const char *description;
    dpiTestCaseFunction func;
    dpiConn *conn;
    dpiConn *adminConn;
    uint64_t sid;
    uint64_t roundTrips;
    int skipped;
};

// define test suite
struct dpiTestSuite {
    uint32_t numTestCases;
    uint32_t allocatedTestCases;
    uint32_t minTestCaseId;
    dpiTestCase *testCases;
    dpiTestParams params;
    FILE *logFile;
};

// drop SODA collection and release connection to it
int dpiTestCase_cleanupSodaColl(dpiTestCase *testCase, dpiSodaColl *coll);

// drop all SODA collections in the database
int dpiTestCase_dropAllSodaColls(dpiTestCase *testCase, dpiSodaDb *db);

// expect an error with any of the specified message prefixes
int dpiTestCase_expectAnyError(dpiTestCase *testCase,
        const char **expectedErrors);

// expect an error with any of the specified message prefixes (in the given
// error info)
int dpiTestCase_expectAnyErrorInfo(dpiTestCase *testCase,
        const dpiErrorInfo *errorInfo, const char **expectedErrors);

// expect double to be equal and sets test case as failed if not
int dpiTestCase_expectDoubleEqual(dpiTestCase *testCase, double actualValue,
        double expectedValue);

// expect an error with the specified message prefix
int dpiTestCase_expectError(dpiTestCase *testCase, const char *expectedError);

// expect an error with the specified message prefix in the given error info
int dpiTestCase_expectErrorInfo(dpiTestCase *testCase,
        const dpiErrorInfo *erorInfo, const char *expectedError);

// expect signed integers to be equal and sets test case as failed if not
int dpiTestCase_expectIntEqual(dpiTestCase *testCase, int64_t actualValue,
        int64_t expectedValue);

// expect that the number of round trips matches the expected value
int dpiTestCase_expectRoundTripsEqual(dpiTestCase *testCase, uint64_t expected);

// expect string to be equal and sets test case as failed if not
int dpiTestCase_expectStringEqual(dpiTestCase *testCase, const char *actual,
        uint32_t actualLength, const char *expected, uint32_t expectedLength);

// expect timestamp to be equal and sets test caswe as failed if not
int dpiTestCase_expectTimestampEqual(dpiTestCase *testCase,
        const dpiTimestamp *timestamp, const dpiTimestamp *expectedTimestamp);

// expect unsigned integers to be equal and sets test case as failed if not
int dpiTestCase_expectUintEqual(dpiTestCase *testCase, uint64_t actualValue,
        uint64_t expectedValue);

// get standalone connection
int dpiTestCase_getConnection(dpiTestCase *testCase, dpiConn **conn);

// get database version info
int dpiTestCase_getDatabaseVersionInfo(dpiTestCase *testCase,
        dpiVersionInfo **versionInfo);

// get pool
int dpiTestCase_getPool(dpiTestCase *testCase, dpiPool **pool);

// get SODA database
int dpiTestCase_getSodaDb(dpiTestCase *testCase, dpiSodaDb **db);

// set test case as failed
int dpiTestCase_setFailed(dpiTestCase *testCase, const char *message);

// set test case as failed from DPI error (fetched from context)
int dpiTestCase_setFailedFromError(dpiTestCase *testCase);

// set test case as failed from DPI error info
int dpiTestCase_setFailedFromErrorInfo(dpiTestCase *testCase,
        dpiErrorInfo *info);

// set test case as skipped
int dpiTestCase_setSkipped(dpiTestCase *testCase, const char *message);

// set test case as skipped if OCI client and/or database version is too old
int dpiTestCase_setSkippedIfVersionTooOld(dpiTestCase *testCase,
        int clientOnly, unsigned minVersionNum, unsigned minReleaseNum);

// setup round trip checker
int dpiTestCase_setupRoundTripChecker(dpiTestCase *testCase,
        dpiTestParams *params);

// update the number of round trips
int dpiTestCase_updateRoundTrips(dpiTestCase *testCase);

// add test case to test suite
void dpiTestSuite_addCase(dpiTestCaseFunction func, const char *description);

// get client version info
void dpiTestSuite_getClientVersionInfo(dpiVersionInfo **versionInfo);

// get global context
void dpiTestSuite_getContext(dpiContext **context);

// get error information from global context
void dpiTestSuite_getErrorInfo(dpiErrorInfo *errorInfo);

// initialize test suite
void dpiTestSuite_initialize(uint32_t minTestCaseId);

// run test suite
int dpiTestSuite_run();
