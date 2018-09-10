//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2017, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestCallProc.c
//   Tests simple call of stored procedure with in, in/out and out variables.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT        "begin proc_Test(:1, :2, :3); end;"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData *inOutValue, *outValue, inValue;
    dpiVar *inOutVar, *outVar;
    uint32_t numQueryColumns;
    dpiStmt *stmt;
    dpiConn *conn;

    // connect to database and create statement
    conn = dpiSamples_getConn(0, NULL);
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // bind IN value
    inValue.isNull = 0;
    inValue.value.asBytes.ptr = "In value for testing";
    inValue.value.asBytes.length = strlen("In value for testing");
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_BYTES, &inValue) < 0)
        return dpiSamples_showError();

    // bind IN/OUT variable
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &inOutVar, &inOutValue) < 0)
        return dpiSamples_showError();
    inOutValue->isNull = 0;
    inOutValue->value.asInt64 = 347;
    if (dpiStmt_bindByPos(stmt, 2, inOutVar) < 0)
        return dpiSamples_showError();

    // bind OUT variable
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &outVar, &outValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 3, outVar) < 0)
        return dpiSamples_showError();

    // perform execution
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();

    // display value of IN/OUT variable
    printf("IN/OUT value (after call) is %" PRId64 "\n",
            inOutValue->value.asInt64);
    dpiVar_release(inOutVar);

    // display value of OUT variable
    printf("OUT value (after call) is %" PRId64 "\n", outValue->value.asInt64);
    dpiVar_release(outVar);

    // clean up
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

