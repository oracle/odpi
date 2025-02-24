//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2025, Oracle and/or its affiliates.
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

#define NUM_EXECUTABLES                 35

static const char *dpiTestNames[NUM_EXECUTABLES] = {
    "test_1000_context",
    "test_1100_numbers",
    "test_1200_conn",
    "test_1300_conn_properties",
    "test_1400_pool",
    "test_1500_pool_properties",
    "test_1600_queries",
    "test_1700_transactions",
    "test_1800_misc",
    "test_1900_variables",
    "test_2000_statements",
    "test_2100_data_types",
    "test_2200_object_types",
    "test_2300_objects",
    "test_2400_enq_options",
    "test_2500_deq_options",
    "test_2600_msg_props",
    "test_2700_aq",
    "test_2800_lobs",
    "test_2900_implicit_results",
    "test_3000_scroll_cursors",
    "test_3100_subscriptions",
    "test_3200_batch_errors",
    "test_3300_dml_returning",
    "test_3400_soda_db",
    "test_3500_soda_coll",
    "test_3600_soda_coll_cursor",
    "test_3700_soda_doc",
    "test_3800_soda_doc_cursor",
    "test_3900_sess_tags",
    "test_4000_queue",
    "test_4100_binds",
    "test_4200_rowids",
    "test_4300_json",
    "test_4400_vector"
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
