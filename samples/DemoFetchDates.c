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
// DemoFetchDates.c
//   Demos simple fetch of dates.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT            "select * from DemoTimestamps"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex, i;
    dpiData *intColValue, *timestampColValue;
    dpiNativeTypeNum nativeTypeNum;
    dpiTimestamp *timestamp;
    dpiQueryInfo queryInfo;
    dpiStmt *stmt;
    dpiConn *conn;
    char sign;
    int found;

    // connect to database
    conn = dpiSamples_getConn(1, NULL);

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();

    // fetch rows
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &intColValue) < 0)
            return dpiSamples_showError();
        printf("Row: IntCol = %" PRId64 "\n", intColValue->value.asInt64);
        for (i = 1; i < numQueryColumns; i++) {
            if (dpiStmt_getQueryValue(stmt, i + 1, &nativeTypeNum,
                    &timestampColValue) < 0)
                return dpiSamples_showError();
            if (dpiStmt_getQueryInfo(stmt, i + 1, &queryInfo) < 0)
                return dpiSamples_showError();
            printf("     %-18.*s = ", queryInfo.nameLength, queryInfo.name);
            if (timestampColValue->isNull)
                printf("null\n");
            else {
                timestamp = &timestampColValue->value.asTimestamp;
                printf("%4d-%.2d-%.2d %.2d:%.2d:%.2d.%.6d",
                        timestamp->year, timestamp->month, timestamp->day,
                        timestamp->hour, timestamp->minute, timestamp->second,
                        timestamp->fsecond);
                if (queryInfo.typeInfo.oracleTypeNum ==
                        DPI_ORACLE_TYPE_TIMESTAMP_TZ) {
                    sign = (timestamp->tzHourOffset < 0 ||
                            timestamp->tzMinuteOffset < 0) ? '-' : '+';
                    printf(" %c%.2d:%.2d", sign, abs(timestamp->tzHourOffset),
                            abs(timestamp->tzMinuteOffset));
                }
                printf("\n");
            }
        }
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
