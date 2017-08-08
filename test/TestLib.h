//----------------------------------------------------------------------------
// Copyright (c) 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
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

#ifdef _MSC_VER
#if _MSC_VER < 1900
#define PRId64                  "I64d"
#define PRIu64                  "I64u"
#endif
#endif

#ifndef PRIu64
#include <inttypes.h>
#endif


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
    const char *connectString;
    uint32_t connectStringLength;
    const char *dirName;
    uint32_t dirNameLength;
};

// define test case structure
struct dpiTestCase {
    const char *description;
    dpiTestCaseFunction func;
    dpiConn *conn;
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

// expect double to be equal and sets test case as failed if not
int dpiTestCase_expectDoubleEqual(dpiTestCase *testCase, double actualValue,
        double expectedValue);

// expect an error with the specified message
int dpiTestCase_expectError(dpiTestCase *testCase, const char *expectedError);

// expect signed integers to be equal and sets test case as failed if not
int dpiTestCase_expectIntEqual(dpiTestCase *testCase, int64_t actualValue,
        int64_t expectedValue);

// expect string to be equal and sets test case as failed if not
int dpiTestCase_expectStringEqual(dpiTestCase *testCase, const char *actual,
        uint32_t actualLength, const char *expected, uint32_t expectedLength);

// expect unsigned integers to be equal and sets test case as failed if not
int dpiTestCase_expectUintEqual(dpiTestCase *testCase, uint64_t actualValue,
        uint64_t expectedValue);

// get standalone connection
int dpiTestCase_getConnection(dpiTestCase *testCase, dpiConn **conn);

// set test case as failed
int dpiTestCase_setFailed(dpiTestCase *testCase, const char *message);

// set test case as failed from DPI error (fetched from context)
int dpiTestCase_setFailedFromError(dpiTestCase *testCase);

// set test case as failed from DPI error info
int dpiTestCase_setFailedFromErrorInfo(dpiTestCase *testCase,
        dpiErrorInfo *info);

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

