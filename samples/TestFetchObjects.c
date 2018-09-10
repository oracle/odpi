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
// TestFetchObjects.c
//   Tests simple fetch of objects.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT            "select ObjectCol " \
                            "from TestObjects " \
                            "order by IntCol"
#define NUM_ATTRS           7
#define NUM_DISPLAY_ATTRS   5

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex, i;
    dpiObjectAttr *attrs[NUM_ATTRS];
    dpiData *objColValue, attrValue;
    dpiNativeTypeNum nativeTypeNum;
    dpiObjectTypeInfo typeInfo;
    dpiObjectAttrInfo attrInfo;
    dpiTimestamp *timestamp;
    dpiQueryInfo queryInfo;
    dpiStmt *stmt;
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

    // get object type and attributes
    if (dpiStmt_getQueryInfo(stmt, 1, &queryInfo) < 0)
        return dpiSamples_showError();
    if (dpiObjectType_getAttributes(queryInfo.typeInfo.objectType, NUM_ATTRS,
            attrs) < 0)
        return dpiSamples_showError();
    if (dpiObjectType_getInfo(queryInfo.typeInfo.objectType, &typeInfo) < 0)
        return dpiSamples_showError();
    printf("Fetching objects of type %.*s.%.*s\n", typeInfo.schemaLength,
            typeInfo.schema, typeInfo.nameLength, typeInfo.name);

    // fetch rows
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &objColValue) < 0)
            return dpiSamples_showError();
        if (objColValue->isNull)
            printf("Row: ObjCol = null\n");
        else {
            printf("Row: objCol =\n");
            for (i = 0; i < NUM_DISPLAY_ATTRS; i++) {
                if (dpiObjectAttr_getInfo(attrs[i], &attrInfo) < 0)
                    return dpiSamples_showError();
                printf("    %.*s => ", attrInfo.nameLength, attrInfo.name);
                if (dpiObject_getAttributeValue(objColValue->value.asObject,
                        attrs[i], attrInfo.typeInfo.defaultNativeTypeNum,
                        &attrValue) < 0)
                    return dpiSamples_showError();
                if (attrValue.isNull)
                    printf("null\n");
                else {
                    switch (attrInfo.typeInfo.defaultNativeTypeNum) {
                        case DPI_NATIVE_TYPE_BYTES:
                            printf("'%.*s'\n", attrValue.value.asBytes.length,
                                    attrValue.value.asBytes.ptr);
                            break;
                        case DPI_NATIVE_TYPE_DOUBLE:
                            printf("%.g\n", attrValue.value.asDouble);
                            break;
                        case DPI_NATIVE_TYPE_TIMESTAMP:
                            timestamp = &attrValue.value.asTimestamp;
                            printf("%.2d/%.2d/%.2d %.2d:%.2d:%.2d\n",
                                    timestamp->year, timestamp->month,
                                    timestamp->day, timestamp->hour,
                                    timestamp->minute, timestamp->second);
                            break;
                        default:
                            printf("???\n");
                            break;
                    }
                }
            }
        }
    }

    // clean up
    for (i = 0; i < NUM_ATTRS; i++)
        dpiObjectAttr_release(attrs[i]);
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

