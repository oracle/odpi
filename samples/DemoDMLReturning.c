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
// DemoDMLReturning.c
//   Demos DML returning clause which returns multiple rows.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT_1          "truncate table DemoTempTable"
#define SQL_TEXT_2          "begin " \
                            "    for i in 1..10 loop " \
                            "        insert into DemoTempTable values (i, " \
                            "                'Demo String ' || i); " \
                            "    end loop; " \
                            "end;"
#define SQL_TEXT_3          "delete from DemoTempTable " \
                            "where IntCol <= :inIntCol " \
                            "returning IntCol, StringCol " \
                            "into :outIntCol, :outStrCol"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData *inIntColData, *outIntColData, *outStrColData;
    dpiVar *inIntColVar, *outIntColVar, *outStrColVar;
    uint32_t numQueryColumns, numRows, i;
    dpiStmt *stmt;
    dpiConn *conn;

    // connect to database
    conn = dpiSamples_getConn(1, NULL);

    // truncate table
    printf("Truncating table...\n");
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_1, strlen(SQL_TEXT_1), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);

    // populate table
    printf("Populating table...\n");
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_2, strlen(SQL_TEXT_2), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &inIntColVar, &inIntColData) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &outIntColVar, &outIntColData) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            100, 0, 0, NULL, &outStrColVar, &outStrColData) < 0)
        return dpiSamples_showError();

    // prepare and execute delete statement with DML returning clause
    printf("Deleting rows with DML returning...\n");
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_3, strlen(SQL_TEXT_3), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    inIntColData->isNull = 0;
    inIntColData->value.asInt64 = 4;
    if (dpiStmt_bindByPos(stmt, 1, inIntColVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 2, outIntColVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 3, outStrColVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();

    // get the data and number of rows returned
    if (dpiVar_getReturnedData(outIntColVar, 0, &numRows, &outIntColData) < 0)
        return dpiSamples_showError();
    if (dpiVar_getReturnedData(outStrColVar, 0, &numRows, &outStrColData) < 0)
        return dpiSamples_showError();
    printf("%d rows returned.\n", numRows);

    // display the output from the rows
    for (i = 0; i < numRows; i++)
        printf("IntCol = %" PRId64 ", StrCol = %.*s\n",
                outIntColData[i].value.asInt64,
                outStrColData[i].value.asBytes.length,
                outStrColData[i].value.asBytes.ptr);

    // clean up
    dpiVar_release(inIntColVar);
    dpiVar_release(outIntColVar);
    dpiVar_release(outStrColVar);
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}
