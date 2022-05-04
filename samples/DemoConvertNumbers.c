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
// DemoConvertNumbers.c
//   Demos conversion of numbers to strings and strings to numbers.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

#define SQL_TEXT                        "select :1 from dual"

static const char *numbersToConvert[] = {
    "0",
    "1",
    "-1",
    "10",
    "-10",
    "100",
    "-100",
    "0.1",
    "-0.1",
    "0.01",
    "-0.01",
    "0.001",
    "-0.001",
    ".100004",
    "-.100004",
    "1234567890123456789012345678901234567891",
    "-1234567890123456789012345678901234567891",
    "1.2345E20",
    "-1.2345E+20",
    "9e125",
    "-9e125",
    "9e-130",
    "-9e-130",
    "9.99999999999999999999999999999999999999E-130",
    "-9.99999999999999999999999999999999999999E-130",
    NULL
};

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex, ix;
    dpiData *inputValue, *outputValue;
    dpiVar *inputVar, *outputVar;
    const char *inputStringValue;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // connect to database
    conn = dpiSamples_getConn(1, NULL);

    // create variables for the input and output values
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES,
            1, 0, 0, 0, NULL, &inputVar, &inputValue) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_BYTES,
            1, 0, 0, 0, NULL, &outputVar, &outputValue) < 0)
        return dpiSamples_showError();

    // prepare and execute statement for each of the numbers to convert
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 1, inputVar) < 0)
        return dpiSamples_showError();

    // perform query for each string in the array
    ix = 0;
    while (1) {
        inputStringValue = numbersToConvert[ix++];
        if (!inputStringValue)
            break;
        printf(" INPUT: |%s|\n", inputStringValue);

        if (dpiVar_setFromBytes(inputVar, 0, inputStringValue,
                strlen(inputStringValue)) < 0)
            return dpiSamples_showError();
        if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
            return dpiSamples_showError();
        if (dpiStmt_define(stmt, 1, outputVar) < 0)
            return dpiSamples_showError();

        // fetch rows
        while (1) {
            if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
                return dpiSamples_showError();
            if (!found)
                break;
            printf("OUTPUT: |%.*s|\n", outputValue->value.asBytes.length,
                    outputValue->value.asBytes.ptr);
        }

    }

    // clean up
    dpiVar_release(inputVar);
    dpiVar_release(outputVar);
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}
