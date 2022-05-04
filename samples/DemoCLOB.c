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
// DemoCLOB.c
//   Populates a table containing CLOBs and then fetches them using LOB
// locators. For smaller sized LOBs (up to a few megabytes in size, depending
// on platform and configuration) this can be substantially slower because
// there are more round trips to the database that are required.
//
//   See DemoCLOBsAsStrings.c for a similar example that fetches the CLOBs as
// strings instead.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT_1                      "truncate table DemoCLOBs"
#define SQL_TEXT_2                      "insert into DemoCLOBs values (:1, :2)"
#define SQL_TEXT_3                      "select IntCol, ClobCol from DemoCLOBs"
#define NUM_ROWS                        10
#define LOB_SIZE_INCREMENT              25000
#define MAX_LOB_SIZE                    NUM_ROWS * LOB_SIZE_INCREMENT

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex, i;
    dpiData *intColValue, *clobColValue;
    dpiVar *intColVar, *clobColVar;
    dpiNativeTypeNum nativeTypeNum;
    char buffer[MAX_LOB_SIZE];
    dpiQueryInfo queryInfo;
    uint64_t clobSize;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_1, strlen(SQL_TEXT_1), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiStmt_release(stmt) < 0)
        return dpiSamples_showError();

    // populate with a number of rows
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_2, strlen(SQL_TEXT_2), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &intColVar, &intColValue) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_LONG_VARCHAR,
            DPI_NATIVE_TYPE_BYTES, 1, 0, 0, 0, NULL, &clobColVar,
            &clobColValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 1, intColVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 2, clobColVar) < 0)
        return dpiSamples_showError();
    for (i = 0; i < NUM_ROWS; i++) {
        dpiData_setInt64(intColValue, i + 1);
        intColValue->value.asInt64 = i + 1;
        memset(buffer, i + 'A', LOB_SIZE_INCREMENT * (i + 1));
        if (dpiVar_setFromBytes(clobColVar, 0, buffer,
                LOB_SIZE_INCREMENT * (i + 1)) < 0)
            return dpiSamples_showError();
        if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
            return dpiSamples_showError();
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiSamples_showError();
    if (dpiVar_release(intColVar) < 0)
        return dpiSamples_showError();
    if (dpiVar_release(clobColVar) < 0)
        return dpiSamples_showError();

    // fetch rows
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_3, strlen(SQL_TEXT_3), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &intColValue) < 0 ||
                dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum,
                        &clobColValue) < 0)
            return dpiSamples_showError();
        if (dpiLob_getSize(clobColValue->value.asLOB, &clobSize) < 0)
            return dpiSamples_showError();
        printf("Row: IntCol = %" PRId64 ", ClobCol = CLOB(%" PRIu64 ")\n",
                intColValue->value.asInt64, clobSize);
    }

    // display description of each variable
    for (i = 0; i < numQueryColumns; i++) {
        if (dpiStmt_getQueryInfo(stmt, i + 1, &queryInfo) < 0)
            return dpiSamples_showError();
        printf("('%*s', %d, %d, %d, %d, %d, %d)\n", queryInfo.nameLength,
                queryInfo.name, queryInfo.typeInfo.oracleTypeNum,
                queryInfo.typeInfo.sizeInChars,
                queryInfo.typeInfo.clientSizeInBytes,
                queryInfo.typeInfo.precision, queryInfo.typeInfo.scale,
                queryInfo.nullOk);
    }

    // clean up
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}
