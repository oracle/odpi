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
// TestRefCursors.c
//   Tests simple fetch of REF cursors.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT            "begin " \
                            "  open :1 for select 'X' StrVal from dual; " \
                            "end;"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex, i;
    dpiData *refCursorValue, *strValue;
    dpiNativeTypeNum nativeTypeNum;
    dpiQueryInfo queryInfo;
    dpiVar *refCursorVar;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // connect to database
    conn = dpiSamples_getConn(1, NULL);

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_STMT, DPI_NATIVE_TYPE_STMT, 1, 0,
            0, 0, NULL, &refCursorVar, &refCursorValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 1, refCursorVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();

    // get ref cursor
    dpiStmt_release(stmt);
    stmt = refCursorValue->value.asStmt;

    // fetch data from ref cursor
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &strValue) < 0)
            return dpiSamples_showError();
        printf("Row: StrVal = '%.*s'\n", strValue->value.asBytes.length,
                strValue->value.asBytes.ptr);
    }

    // display description of each fetched column
    if (dpiStmt_getNumQueryColumns(stmt, &numQueryColumns) < 0)
        return dpiSamples_showError();
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
    dpiVar_release(refCursorVar);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

