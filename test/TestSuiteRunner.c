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
// TestSuiteRunner.c
//   Runs each of the test executables.
//-----------------------------------------------------------------------------

#include "TestLib.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;
#endif

#define NUM_EXECUTABLES                 30

static const char *dpiTestNames[NUM_EXECUTABLES] = {
    "TestContext",
    "TestNumbers",
    "TestConn",
    "TestConnProperties",
    "TestPool",
    "TestPoolProperties",
    "TestQueries",
    "TestTransactions",
    "TestMiscCases",
    "TestVariables",
    "TestStatements",
    "TestDataTypes",
    "TestObjectTypes",
    "TestObjects",
    "TestEnqOptions",
    "TestDeqOptions",
    "TestMsgProps",
    "TestAQ",
    "TestLOBs",
    "TestImplicitResults",
    "TestRowIds",
    "TestScrollCursors",
    "TestSubscriptions",
    "TestBatchErrors",
    "TestDMLReturning",
    "TestSodaDb",
    "TestSodaColl",
    "TestSodaCollCursor",
    "TestSodaDoc",
    "TestSodaDocCursor"
};


//-----------------------------------------------------------------------------
// dpiTest_runExecutable()
//   Run executable with the specified name. If all tests pass, return 0;
// otherwise, return a negative value to indicate failure.
//-----------------------------------------------------------------------------
int dpiTest_runExecutable(const char *runnerName, const char *name)
{
#ifdef _WIN32
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;
    DWORD result;

    // initialization
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

    // create process
    if (!CreateProcess(NULL, (char*) name, NULL, NULL, FALSE, 0, NULL, NULL,
            &startupInfo, &processInfo)) {
        fprintf(stderr, "Unable to create process\n");
        return -1;
    }

    // wait for process to complete and return success only if all tests pass
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    GetExitCodeProcess(processInfo.hProcess, &result);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    if (result != 0)
        return -1;

#else
    char executableName[PATH_MAX + 1], *temp;
    char * const argv[2] = { executableName, NULL };
    int status;
    pid_t pid;

    // calculate name of executable to run
    strcpy(executableName, runnerName);
    temp = strrchr(executableName, '/');
    if (temp)
        strcpy(temp + 1, name);
    else strcpy(executableName, name);

    // run executable and return success only if all tests pass
    status = posix_spawn(&pid, executableName, NULL, NULL, argv, environ);
    if (status != 0) {
        perror("Failed to spawn executable");
        return -1;
    }
    if (waitpid(pid, &status, 0) < 0) {
        perror("Failed to wait for child process");
        return -1;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        return -1;

#endif

    return 0;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    int testResults[NUM_EXECUTABLES], i, someTestsFailed;

    // verify that connection parameters are correct
    dpiTestSuite_initialize(0);

    // run all tests
    someTestsFailed = 0;
    for (i = 0; i < NUM_EXECUTABLES; i++) {
        fprintf(stderr, "Running cases in %s\n", dpiTestNames[i]);
        fflush(stderr);
        testResults[i] = dpiTest_runExecutable(argv[0], dpiTestNames[i]);
        fprintf(stderr, "\n");
        fflush(stderr);
        if (testResults[i] < 0)
            someTestsFailed = 1;
    }

    // if some tests failed, report on which ones
    if (someTestsFailed) {
        fprintf(stderr, "Cases in the following tests failed:\n");
        for (i = 0; i < NUM_EXECUTABLES; i++) {
            if (testResults[i] < 0)
                fprintf(stderr, "    %s\n", dpiTestNames[i]);
        }
        return 1;
    }

    fprintf(stderr, "All tests passed!\n");
    fflush(stderr);
    return 0;
}

