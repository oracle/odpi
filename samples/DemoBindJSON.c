//-----------------------------------------------------------------------------
// Copyright (c) 2020, 2022, Oracle and/or its affiliates.
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
// DemoBindJSON.c
//   Demos binding JSON values into the database. This requires Oracle Client
// and Oracle Database to both be 20 or higher.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

#define SQL     "select :1 from dual"

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
        case DPI_NATIVE_TYPE_INT64:
            printf("%" PRId64 "d", node->value->asInt64);
            break;
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
    uint32_t bufferRowIndex, fieldNameLengths[2];
    dpiJsonNode inNodes[12], *topNode;
    dpiNativeTypeNum nativeTypeNum;
    dpiData *inVarData, *outValue;
    dpiDataBuffer inNodeData[12];
    char *fieldNames[2];
    dpiVar *inVar;
    dpiConn *conn;
    dpiStmt *stmt;
    int found, i;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // build up JSON node tree; this can be done by allocating nodes or by
    // using fixed size arrays as demonstrated here; start with lowest levels
    // in tree; the data here is the same data used in the DemoFetchJSON
    // sample

    // zero all memory so only non-zero values need to be set
    memset(inNodes, 0, sizeof(inNodes));
    memset(inNodeData, 0, sizeof(inNodeData));

    // set value for each of the nodes
    for (i = 0; i < 12; i++)
        inNodes[i].value = &inNodeData[i];

    // scalar values stored in the object
    inNodes[0].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    inNodes[0].nativeTypeNum = DPI_NATIVE_TYPE_INT64;
    inNodeData[0].asInt64 = 5;

    inNodes[1].oracleTypeNum = DPI_ORACLE_TYPE_VARCHAR;
    inNodes[1].nativeTypeNum = DPI_NATIVE_TYPE_BYTES;
    inNodeData[1].asBytes.ptr = "A String";
    inNodeData[1].asBytes.length = strlen(inNodeData[1].asBytes.ptr);

    // JSON object
    fieldNames[0] = "Fred";
    fieldNameLengths[0] = strlen(fieldNames[0]);
    fieldNames[1] = "George";
    fieldNameLengths[1] = strlen(fieldNames[1]);
    inNodes[9].oracleTypeNum = DPI_ORACLE_TYPE_JSON_OBJECT;
    inNodes[9].nativeTypeNum = DPI_NATIVE_TYPE_JSON_OBJECT;
    inNodeData[9].asJsonObject.numFields = 2;
    inNodeData[9].asJsonObject.fieldNames = fieldNames;
    inNodeData[9].asJsonObject.fieldNameLengths = fieldNameLengths;
    inNodeData[9].asJsonObject.fields = &inNodes[0];
    inNodeData[9].asJsonObject.fieldValues = &inNodeData[0];

    // scalar values stored in the lowest level array
    inNodes[2].oracleTypeNum = DPI_ORACLE_TYPE_RAW;
    inNodes[2].nativeTypeNum = DPI_NATIVE_TYPE_BYTES;
    inNodeData[2].asBytes.ptr = "A Raw";
    inNodeData[2].asBytes.length = strlen(inNodeData[2].asBytes.ptr);

    inNodes[3].oracleTypeNum = DPI_ORACLE_TYPE_DATE;
    inNodes[3].nativeTypeNum = DPI_NATIVE_TYPE_TIMESTAMP;
    inNodeData[3].asTimestamp.year = 2020;
    inNodeData[3].asTimestamp.month = 10;
    inNodeData[3].asTimestamp.day = 15;
    inNodeData[3].asTimestamp.hour = 13;
    inNodeData[3].asTimestamp.minute = 54;
    inNodeData[3].asTimestamp.second = 12;

    inNodes[4].oracleTypeNum = DPI_ORACLE_TYPE_TIMESTAMP;
    inNodes[4].nativeTypeNum = DPI_NATIVE_TYPE_TIMESTAMP;
    inNodeData[4].asTimestamp.year = 2020;
    inNodeData[4].asTimestamp.month = 10;
    inNodeData[4].asTimestamp.day = 15;
    inNodeData[4].asTimestamp.hour = 13;
    inNodeData[4].asTimestamp.minute = 58;
    inNodeData[4].asTimestamp.second = 59;

    inNodes[5].oracleTypeNum = DPI_ORACLE_TYPE_NATIVE_DOUBLE;
    inNodes[5].nativeTypeNum = DPI_NATIVE_TYPE_DOUBLE;
    inNodeData[5].asDouble = 25.25;

    inNodes[6].oracleTypeNum = DPI_ORACLE_TYPE_NATIVE_FLOAT;
    inNodes[6].nativeTypeNum = DPI_NATIVE_TYPE_FLOAT;
    inNodeData[6].asFloat = 50.125;

    // lowest level JSON array
    inNodes[10].oracleTypeNum = DPI_ORACLE_TYPE_JSON_ARRAY;
    inNodes[10].nativeTypeNum = DPI_NATIVE_TYPE_JSON_ARRAY;
    inNodeData[10].asJsonArray.numElements = 5;
    inNodeData[10].asJsonArray.elements = &inNodes[2];
    inNodeData[10].asJsonArray.elementValues = &inNodeData[2];

    // values stored in the top level array
    inNodes[7].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    inNodes[7].nativeTypeNum = DPI_NATIVE_TYPE_INT64;
    inNodeData[7].asInt64 = 1;

    inNodes[8].oracleTypeNum = DPI_ORACLE_TYPE_VARCHAR;
    inNodes[8].nativeTypeNum = DPI_NATIVE_TYPE_BYTES;
    inNodeData[8].asBytes.ptr = "String Value";
    inNodeData[8].asBytes.length = strlen(inNodeData[8].asBytes.ptr);

    // top level JSON array
    inNodes[11].oracleTypeNum = DPI_ORACLE_TYPE_JSON_ARRAY;
    inNodes[11].nativeTypeNum = DPI_NATIVE_TYPE_JSON_ARRAY;
    inNodeData[11].asJsonArray.numElements = 4;
    inNodeData[11].asJsonArray.elements = &inNodes[7];
    inNodeData[11].asJsonArray.elementValues = &inNodeData[7];

    // create a JSON bind variable and populate it
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON, 1,
            0, 0, 0, NULL, &inVar, &inVarData) < 0)
        return dpiSamples_showError();
    inVarData->isNull = 0;
    if (dpiJson_setValue(inVarData->value.asJson, &inNodes[11]) < 0)
        return dpiSamples_showError();

    // perform query
    if (dpiConn_prepareStmt(conn, 0, SQL, strlen(SQL), NULL, 0, &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiSamples_showError();

    // fetch row
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outValue) < 0)
        return dpiSamples_showError();
    if (dpiJson_getValue(outValue->value.asJson, DPI_JSON_OPT_DEFAULT,
            &topNode) < 0)
        return dpiSamples_showError();

    // display results
    printf("Bound JSON value:\n");
    displayJson(topNode, 0, 0);
    printf("\n");

    return 0;
}
