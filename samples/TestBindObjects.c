//-----------------------------------------------------------------------------
// Copyright (c) 2016 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestBindObjects.c
//   Tests simple binding of objects.
//-----------------------------------------------------------------------------

#include "Test.h"
#define OBJECT_TYPE_NAME    "UDT_OBJECT"
#define SQL_TEXT            "begin :1 := " \
                            "pkg_TestBindObject.GetStringRep(:2); end;"
#define NUM_ATTRS           7

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData attrValue, objValue, *stringRepValue;
    dpiObjectAttr *attrs[NUM_ATTRS];
    uint32_t numQueryColumns, i;
    dpiObjectType *objType;
    dpiVar *stringRepVar;
    dpiObject *obj;
    dpiStmt *stmt;
    dpiConn *conn;

    // connect to database
    conn = GetConnection(0, NULL);
    if (!conn)
        return -1;

    // get object type and attributes
    if (dpiConn_getObjectType(conn, OBJECT_TYPE_NAME, strlen(OBJECT_TYPE_NAME),
            &objType) < 0)
        return ShowError();
    if (dpiObjectType_getAttributes(objType, NUM_ATTRS, attrs) < 0)
        return ShowError();

    // create object and populate attributes
    if (dpiObjectType_createObject(objType, &obj) < 0)
        return ShowError();
    attrValue.isNull = 0;
    attrValue.value.asDouble = 13;
    if (dpiObject_setAttributeValue(obj, attrs[0], DPI_NATIVE_TYPE_DOUBLE,
            &attrValue) < 0)
        return ShowError();
    attrValue.value.asBytes.ptr = "Test String";
    attrValue.value.asBytes.length = strlen(attrValue.value.asBytes.ptr);
    if (dpiObject_setAttributeValue(obj, attrs[1], DPI_NATIVE_TYPE_BYTES,
            &attrValue) < 0)
        return ShowError();

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return ShowError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES, 1,
            100, 0, 0, NULL, &stringRepVar, &stringRepValue) < 0)
        return ShowError();
    if (dpiStmt_bindByPos(stmt, 1, stringRepVar) < 0)
        return ShowError();
    objValue.isNull = 0;
    objValue.value.asObject = obj;
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_OBJECT, &objValue) < 0)
        return ShowError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return ShowError();
    dpiObject_release(obj);
    dpiObjectType_release(objType);
    for (i = 0; i < NUM_ATTRS; i++)
        dpiObjectAttr_release(attrs[i]);

    // display result
    printf("String rep: '%.*s'\n", stringRepValue->value.asBytes.length,
            stringRepValue->value.asBytes.ptr);
    dpiVar_release(stringRepVar);

    // clean up
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

