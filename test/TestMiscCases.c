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
// TestMisCases.c
//   Test suite of miscellenous test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_900_miscChangePwd() [INTERNAL]
//   Call dpiConn_changePassword() and create a new connection using the new
// password to verify that the password was indeed changed (no error).
//-----------------------------------------------------------------------------
int dpiTest_900_miscChangePwd(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *newpwd = "newpwd";
    dpiContext *context;
    dpiConn *conn;

    // get first connection and change password
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_changePassword(conn, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, newpwd, strlen(newpwd)) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // get second connection and change password back
    dpiTestSuite_getContext(&context);
    if (dpiConn_create(context, params->mainUserName,
            params->mainUserNameLength, newpwd, strlen(newpwd),
            params->connectString, params->connectStringLength, NULL, NULL,
            &conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_changePassword(conn, params->mainUserName,
            params->mainUserNameLength, newpwd, strlen(newpwd),
            params->mainPassword, params->mainPasswordLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiConn_release(conn);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_901_miscPing() [INTERNAL]
//   call dpiConn_ping() (no error)
//-----------------------------------------------------------------------------
int dpiTest_901_miscPing(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_ping(conn) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(900);
    dpiTestSuite_addCase(dpiTest_900_miscChangePwd,
            "change password and verify (no error)");
    dpiTestSuite_addCase(dpiTest_901_miscPing,
            "dpiConn_ping() (no error)");
    dpiTestSuite_run();
    return 0;
}

