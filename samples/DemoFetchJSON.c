//-----------------------------------------------------------------------------
// Copyright (c) 2020, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DemoFetchJSON.c
//   Demos fetching JSON values from the database. This requires Oracle Client
// and Oracle Database to both be 20 or higher.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

#define SQL     "select" \
                "    json_array(" \
                "        json_scalar(1)," \
                "        json_scalar('String value')," \
                "        json_object(" \
                "            key 'Fred' value json_scalar(5)," \
                "            key 'George' value json_scalar('A string')" \
                "        returning json)," \
                "        json_array(" \
                "            json_scalar(utl_raw.cast_to_raw('A raw'))," \
                "            json_scalar(sysdate - 1)," \
                "            json_scalar(systimestamp + 1)" \
                "        returning json)" \
                "    returning json) " \
                "from dual"


//-----------------------------------------------------------------------------
// displayJson()
//-----------------------------------------------------------------------------
void displayJson(dpiJsonNode *node, uint32_t indentLevel, int printIndent)
{
    dpiTimestamp *timestamp;
    dpiJsonArray *array;
    dpiJsonObject *obj;
    uint32_t i, j;

    if (printIndent) {
        printf("\n");
        for (i = 0; i < indentLevel; i++)
            printf("    ");
    }

    switch (node->nativeTypeNum) {
        case DPI_NATIVE_TYPE_DOUBLE:
            printf("%g", node->value->asDouble);
            break;
        case DPI_NATIVE_TYPE_BYTES:
            printf("'%.*s'", node->value->asBytes.length,
                    node->value->asBytes.ptr);
            break;
        case DPI_NATIVE_TYPE_TIMESTAMP:
            timestamp = &node->value->asTimestamp;
            printf("%.4d-%.2d-%.2d %.2d:%.2d:%.2d", timestamp->year,
                    timestamp->month, timestamp->day, timestamp->hour,
                    timestamp->minute, timestamp->second);
            break;
        case DPI_NATIVE_TYPE_JSON_ARRAY:
            array = &node->value->asJsonArray;
            printf("[");
            for (i = 0; i < array->numElements; i++) {
                if (i > 0)
                    printf(",");
                displayJson(&array->elements[i], indentLevel + 1, 1);
            }
            printf("\n");
            for (i = 0; i < indentLevel; i++)
                printf("    ");
            printf("]");
            break;
        case DPI_NATIVE_TYPE_JSON_OBJECT:
            obj = &node->value->asJsonObject;
            printf("{");
            for (i = 0; i < obj->numFields; i++) {
                if (i > 0)
                    printf(",");
                printf("\n");
                for (j = 0; j < indentLevel + 1; j++)
                    printf("    ");
                printf("'%.*s': ", obj->fieldNameLengths[i],
                        obj->fieldNames[i]);
                displayJson(&obj->fields[i], indentLevel + 1, 0);
            }
            printf("\n");
            for (i = 0; i < indentLevel; i++)
                printf("    ");
            printf("}");
            break;
        default:
            printf("Unhandled native type %u", node->nativeTypeNum);
    }
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiData *jsonColValue;
    dpiJsonNode *topNode;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // perform query
    if (dpiConn_prepareStmt(conn, 0, SQL, strlen(SQL), NULL, 0, &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiSamples_showError();

    // fetch row
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &jsonColValue) < 0)
        return dpiSamples_showError();
    if (dpiJson_getValue(jsonColValue->value.asJson, DPI_JSON_OPT_DEFAULT,
            &topNode) < 0)
        return dpiSamples_showError();

    // display results
    printf("JSON value:\n");
    displayJson(topNode, 0, 0);
    printf("\n");

    return 0;
}
