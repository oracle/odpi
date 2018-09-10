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
// TestBindArrays.c
//   Tests calling stored procedures binding PL/SQL arrays in various ways.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_IN    "begin :1 := pkg_TestStringArrays.TestInArrays(:2, :3); end;"
#define SQL_INOUT "begin pkg_TestStringArrays.TestInOutArrays(:1, :2); end;"
#define SQL_OUT   "begin pkg_TestStringArrays.TestOutArrays(:1, :2); end;"
#define SQL_ASSOC "begin pkg_TestStringArrays.TestIndexBy(:1); end;"
#define TYPE_NAME "PKG_TESTSTRINGARRAYS.UDT_STRINGLIST"

static const char *gc_Strings[5] = {
    "Test String 1 (I)",
    "Test String 2 (II)",
    "Test String 3 (III)",
    "Test String 4 (IV)",
    "Test String 5 (V)"
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
    // prepare statement for testing in arrays
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
    // prepare statement for testing in/out arrays
    if (dpiConn_prepareStmt(conn, 0, SQL_INOUT, strlen(SQL_INOUT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // bind in numeric value
    numberValue->value.asInt64 = 5;
    if (dpiStmt_bindByPos(stmt, 1, numberVar) < 0)
        return dpiSamples_showError();

    // bind in array value (use same values as test for in arrays)
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
    // prepare statement for testing out arrays
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

    // prepare statement for testing associative arrays
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

