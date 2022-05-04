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
// DemoInsertAsArray.c
//   Demos simple insert of numbers and strings using array bind.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define DELETE_TEXT         "delete from DemoTempTable"
#define INSERT_TEXT         "insert into DemoTempTable values (:1, :2)"
#define NUM_ROWS            5

static int gc_IntColValues[NUM_ROWS] = { 3, 5, 7, 9, 11 };
static const char *gc_StringColValues[NUM_ROWS] = {
    "DEMO 1", "DEMO ROW 2", "DEMO #3", "DEMO ROW 4", "DEMO ROW #5"
};


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData *intColValue, *stringColValue;
    dpiVar *intColVar, *stringColVar;
    uint32_t numQueryColumns, i;
    uint64_t rowCount;
    dpiStmt *stmt;
    dpiConn *conn;

    // connect to database and create statement
    conn = dpiSamples_getConn(0, NULL);
    if (dpiConn_prepareStmt(conn, 0, DELETE_TEXT, strlen(DELETE_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // perform delete
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
        return dpiSamples_showError();
    printf("%" PRIu64 " rows deleted.\n", rowCount);
    dpiStmt_release(stmt);

    // prepare insert statement for execution
    if (dpiConn_prepareStmt(conn, 0, INSERT_TEXT, strlen(INSERT_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            NUM_ROWS, 0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 1, intColVar) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            NUM_ROWS, 20, 0, 0, NULL, &stringColVar, &stringColValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 2, stringColVar) < 0)
        return dpiSamples_showError();

    // populate data
    for (i = 0; i < NUM_ROWS; i++) {
        intColValue[i].isNull = 0;
        intColValue[i].value.asInt64 = gc_IntColValues[i];
        if (dpiVar_setFromBytes(stringColVar, i, gc_StringColValues[i],
                strlen(gc_StringColValues[i])) < 0)
            return dpiSamples_showError();
    }
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, NUM_ROWS) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
        return dpiSamples_showError();
    printf("%" PRIu64 " rows inserted.\n", rowCount);

    // commit changes
    if (dpiConn_commit(conn) < 0)
        return dpiSamples_showError();

    // clean up
    dpiVar_release(intColVar);
    dpiVar_release(stringColVar);
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}
