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
// TestImplicitResults.c
//   Tests fetch of implicit results.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT            "declare " \
                            "    c1 sys_refcursor; " \
                            "    c2 sys_refcursor; " \
                            "begin " \
                            " " \
                            "    open c1 for " \
                            "    select NumberCol " \
                            "    from TestNumbers " \
                            "    where IntCol between 3 and 5; " \
                            " " \
                            "    dbms_sql.return_result(c1); " \
                            " " \
                            "    open c2 for " \
                            "    select NumberCol " \
                            "    from TestNumbers " \
                            "    where IntCol between 7 and 10; " \
                            " " \
                            "    dbms_sql.return_result(c2); " \
                            "end;"


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex;
    dpiNativeTypeNum nativeTypeNum;
    dpiStmt *stmt, *resultStmt;
    dpiData *doubleValue;
    dpiConn *conn;
    int found;

    // connect to database
    conn = dpiSamples_getConn(1, NULL);

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();

    // retrieve from implicit results
    while (1) {

        // get implicit result
        if (dpiStmt_getImplicitResult(stmt, &resultStmt) < 0)
            return dpiSamples_showError();
        if (!resultStmt)
            break;

        // fetch from cursor
        printf("----------------------------------------------------------\n");
        while (1) {
            if (dpiStmt_fetch(resultStmt, &found, &bufferRowIndex) < 0)
                return dpiSamples_showError();
            if (!found)
                break;
            if (dpiStmt_getQueryValue(resultStmt, 1, &nativeTypeNum,
                    &doubleValue) < 0)
                return dpiSamples_showError();
            printf("Row: NumberValue = %g\n", doubleValue->value.asDouble);
        }
        dpiStmt_release(resultStmt);

    }

    // clean up
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

