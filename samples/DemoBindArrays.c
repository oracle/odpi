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
// DemoBindArrays.c
//   Demos calling stored procedures binding PL/SQL arrays in various ways.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_IN    "begin :1 := pkg_DemoStringArrays.DemoInArrays(:2, :3); end;"
#define SQL_INOUT "begin pkg_DemoStringArrays.DemoInOutArrays(:1, :2); end;"
#define SQL_OUT   "begin pkg_DemoStringArrays.DemoOutArrays(:1, :2); end;"
#define SQL_ASSOC "begin pkg_DemoStringArrays.DemoIndexBy(:1); end;"
#define TYPE_NAME "PKG_DEMOSTRINGARRAYS.UDT_STRINGLIST"

static const char *gc_Strings[5] = {
    "Demo String 1 (I)",
    "Demo String 2 (II)",
    "Demo String 3 (III)",
    "Demo String 4 (IV)",
    "Demo String 5 (V)"
};

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData *returnValue, *numberValue, *arrayValue, *objectValue;
    dpiVar *returnVar, *numberVar, *arrayVar, *objectVar;
    uint32_t numQueryColumns, i, numElementsInArray;
    int32_t elementIndex, nextElementIndex;
    dpiObjectType *objType;
    dpiData elementValue;
    dpiStmt *stmt;
    dpiConn *conn;
    int exists;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // create variable for return value
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &returnVar, &returnValue) < 0)
        return dpiSamples_showError();

    // create variable for numeric value passed to procedures
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &numberVar, &numberValue) < 0)
        return dpiSamples_showError();

    // create variable for string array passed to procedures
    // a maximum of 8 elements, each of 60 characters is permitted
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 8,
            60, 0, 1, NULL, &arrayVar, &arrayValue) < 0)
        return dpiSamples_showError();

    // ************** IN ARRAYS *****************
    // prepare statement for demoing in arrays
    if (dpiConn_prepareStmt(conn, 0, SQL_IN, strlen(SQL_IN), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // bind return value
    if (dpiStmt_bindByPos(stmt, 1, returnVar) < 0)
        return dpiSamples_showError();

    // bind in numeric value
    numberValue->isNull = 0;
    numberValue->value.asInt64 = 12;
    if (dpiStmt_bindByPos(stmt, 2, numberVar) < 0)
        return dpiSamples_showError();

    // bind in string array
    for (i = 0; i < 5; i++) {
        if (dpiVar_setFromBytes(arrayVar, i, gc_Strings[i],
                strlen(gc_Strings[i])) < 0)
            return dpiSamples_showError();
    }
    if (dpiVar_setNumElementsInArray(arrayVar, 5) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 3, arrayVar) < 0)
        return dpiSamples_showError();

    // perform execution (in arrays with 5 elements)
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    printf("IN array (5 elements): return value is %" PRId64 "\n\n",
            returnValue->value.asInt64);

    // perform execution (in arrays with 0 elements)
    if (dpiVar_setNumElementsInArray(arrayVar, 0) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);
    printf("IN array (0 elements): return value is %" PRId64 "\n\n",
            returnValue->value.asInt64);

    // ************** IN/OUT ARRAYS *****************
    // prepare statement for demoing in/out arrays
    if (dpiConn_prepareStmt(conn, 0, SQL_INOUT, strlen(SQL_INOUT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // bind in numeric value
    numberValue->value.asInt64 = 5;
    if (dpiStmt_bindByPos(stmt, 1, numberVar) < 0)
        return dpiSamples_showError();

    // bind in array value (use same values as demo for in arrays)
    if (dpiVar_setNumElementsInArray(arrayVar, 5) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiSamples_showError();

    // perform execution (in/out arrays)
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);

    // display value of array after procedure call
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsInArray) < 0)
        return dpiSamples_showError();
    printf("IN/OUT array contents:\n");
    for (i = 0; i < numElementsInArray; i++)
        printf("    [%d] %.*s\n", i + 1, arrayValue[i].value.asBytes.length,
                arrayValue[i].value.asBytes.ptr);
    printf("\n");

    // ************** OUT ARRAYS *****************
    // prepare statement for demoing out arrays
    if (dpiConn_prepareStmt(conn, 0, SQL_OUT, strlen(SQL_OUT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // bind in numeric value
    numberValue->value.asInt64 = 7;
    if (dpiStmt_bindByPos(stmt, 1, numberVar) < 0)
        return dpiSamples_showError();

    // bind in array value (value will be overwritten)
    if (dpiStmt_bindByPos(stmt, 2, arrayVar) < 0)
        return dpiSamples_showError();

    // perform execution (out arrays)
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);

    // display value of array after procedure call
    if (dpiVar_getNumElementsInArray(arrayVar, &numElementsInArray) < 0)
        return dpiSamples_showError();
    printf("OUT array contents:\n");
    for (i = 0; i < numElementsInArray; i++)
        printf("    [%d] %.*s\n", i + 1, arrayValue[i].value.asBytes.length,
                arrayValue[i].value.asBytes.ptr);
    printf("\n");

    // ************** INDEX-BY ASSOCIATIVE ARRAYS *****************
    // look up object type by name
    if (dpiConn_getObjectType(conn, TYPE_NAME, strlen(TYPE_NAME),
            &objType) < 0)
        return dpiSamples_showError();

    // create new object variable
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_OBJECT, DPI_NATIVE_TYPE_OBJECT, 1,
            0, 0, 0, objType, &objectVar, &objectValue) < 0)
        return dpiSamples_showError();

    // prepare statement for demoing associative arrays
    if (dpiConn_prepareStmt(conn, 0, SQL_ASSOC, strlen(SQL_ASSOC), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // bind array
    if (dpiStmt_bindByPos(stmt, 1, objectVar) < 0)
        return dpiSamples_showError();

    // perform execution (associative arrays)
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);

    // display contents of array after procedure call
    if (dpiObject_getFirstIndex(objectValue->value.asObject, &elementIndex,
            &exists) < 0)
        return dpiSamples_showError();
    printf("ASSOCIATIVE array contents:\n");
    while (1) {
        if (dpiObject_getElementValueByIndex(objectValue->value.asObject,
                elementIndex, DPI_NATIVE_TYPE_BYTES, &elementValue) < 0)
            return dpiSamples_showError();
        printf("    [%d] %.*s\n", elementIndex,
                elementValue.value.asBytes.length,
                elementValue.value.asBytes.ptr);
        if (dpiObject_getNextIndex(objectValue->value.asObject, elementIndex,
                &nextElementIndex, &exists) < 0)
            return dpiSamples_showError();
        if (!exists)
            break;
        elementIndex = nextElementIndex;
    }
    printf("\n");

    // clean up
    dpiVar_release(returnVar);
    dpiVar_release(numberVar);
    dpiVar_release(arrayVar);
    dpiVar_release(objectVar);
    dpiObjectType_release(objType);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}
