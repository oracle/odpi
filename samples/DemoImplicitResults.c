//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2022, Oracle and/or its affiliates.
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
// DemoImplicitResults.c
//   Demos fetch of implicit results.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT            "declare " \
                            "    c1 sys_refcursor; " \
                            "    c2 sys_refcursor; " \
                            "begin " \
                            " " \
                            "    open c1 for " \
                            "    select NumberCol " \
                            "    from DemoNumbers " \
                            "    where IntCol between 3 and 5; " \
                            " " \
                            "    dbms_sql.return_result(c1); " \
                            " " \
                            "    open c2 for " \
                            "    select NumberCol " \
                            "    from DemoNumbers " \
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
