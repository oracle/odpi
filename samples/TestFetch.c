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
// TestFetch.c
//   Tests simple fetch of numbers and strings.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT_1          "select IntCol, StringCol, RawCol, rowid " \
                            "from TestStrings " \
                            "where IntCol > :intCol"
#define SQL_TEXT_2          "select IntCol " \
                            "from TestStrings " \
                            "where rowid = :1"
#define BIND_NAME           "intCol"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData *intColValue, *stringColValue, *rawColValue, *rowidValue;
    uint32_t numQueryColumns, bufferRowIndex, i, rowidAsStringLength;
    dpiData bindValue, *bindRowidValue;
    dpiNativeTypeNum nativeTypeNum;
    const char *rowidAsString;
    dpiQueryInfo queryInfo;
    dpiVar *rowidVar;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // connect to database
    conn = dpiSamples_getConn(1, NULL);

    // create variable for storing the rowid of one of the rows
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_ROWID, DPI_NATIVE_TYPE_ROWID, 1,
            0, 0, 0, NULL, &rowidVar, &bindRowidValue) < 0)
        return dpiSamples_showError();

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_1, strlen(SQL_TEXT_1), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    bindValue.value.asInt64 = 7;
    bindValue.isNull = 0;
    if (dpiStmt_bindValueByName(stmt, BIND_NAME, strlen(BIND_NAME),
            DPI_NATIVE_TYPE_INT64, &bindValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiStmt_defineValue(stmt, 1, DPI_ORACLE_TYPE_NUMBER,
            DPI_NATIVE_TYPE_BYTES, 0, 0, NULL) < 0)
        return dpiSamples_showError();

    // fetch rows
    printf("Fetch rows with IntCol > %" PRId64 "\n", bindValue.value.asInt64);
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &intColValue) < 0 ||
                dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum,
                        &stringColValue) < 0 ||
                dpiStmt_getQueryValue(stmt, 3, &nativeTypeNum,
                        &rawColValue) < 0 ||
                dpiStmt_getQueryValue(stmt, 4, &nativeTypeNum,
                        &rowidValue) < 0)
            return dpiSamples_showError();
        if (dpiRowid_getStringValue(rowidValue->value.asRowid,
                &rowidAsString, &rowidAsStringLength) < 0)
            return dpiSamples_showError();
        printf("Row: Int = %.*s, String = '%.*s', Raw = '%.*s', "
                "Rowid = '%.*s'\n", intColValue->value.asBytes.length,
                intColValue->value.asBytes.ptr,
                stringColValue->value.asBytes.length,
                stringColValue->value.asBytes.ptr,
                rawColValue->value.asBytes.length,
                rawColValue->value.asBytes.ptr, rowidAsStringLength,
                rowidAsString);
        if (dpiVar_setFromRowid(rowidVar, 0, rowidValue->value.asRowid) < 0)
            return dpiSamples_showError();
    }
    printf("\n");

    // display description of each variable
    printf("Display column metadata\n");
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
    printf("\n");
    printf("Fetch rows with rowid = %.*s\n", rowidAsStringLength,
            rowidAsString);
    dpiStmt_release(stmt);

    // prepare and execute statement to fetch by rowid
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_2, strlen(SQL_TEXT_2), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 1, rowidVar) < 0)
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
        printf("Row: Int = %" PRId64 "\n", intColValue->value.asInt64);
    }

    // clean up
    dpiVar_release(rowidVar);
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

