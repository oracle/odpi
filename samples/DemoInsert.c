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
// DemoInsert.c
//   Demos simple insert of numbers and strings.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define DELETE_TEXT         "delete from DemoTempTable"
#define INSERT_TEXT         "insert into DemoTempTable values (:1, :2)"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData intColValue, stringColValue;
    uint32_t rowidStrLength;
    const char *rowidStr;
    uint64_t rowCount;
    dpiRowid *rowid;
    dpiStmt *stmt;
    dpiConn *conn;

    // connect to database and create statement
    conn = dpiSamples_getConn(0, NULL);
    if (dpiConn_prepareStmt(conn, 0, DELETE_TEXT, strlen(DELETE_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();

    // perform delete
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
        return dpiSamples_showError();
    printf("%" PRIu64 " rows deleted.\n", rowCount);
    dpiStmt_release(stmt);

    // prepare insert statement for execution
    if (dpiConn_prepareStmt(conn, 0, INSERT_TEXT, strlen(INSERT_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    intColValue.isNull = 0;
    stringColValue.isNull = 0;

    // create first row
    intColValue.value.asInt64 = 1;
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64,
            &intColValue) < 0)
        return dpiSamples_showError();
    stringColValue.value.asBytes.ptr = "DEMO 1";
    stringColValue.value.asBytes.length = strlen("DEMO 1");
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES,
            &stringColValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiSamples_showError();
    if (dpiRowid_getStringValue(rowid, &rowidStr, &rowidStrLength))
        return dpiSamples_showError();
    printf("%" PRIu64 " row inserted (rowid %.*s).\n", rowCount,
            rowidStrLength, rowidStr);

    // create second row
    intColValue.value.asInt64 = 2;
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64,
            &intColValue) < 0)
        return dpiSamples_showError();
    stringColValue.value.asBytes.ptr = "DEMO #2";
    stringColValue.value.asBytes.length = strlen("DEMO #2");
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES,
            &stringColValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getRowCount(stmt, &rowCount) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getLastRowid(stmt, &rowid) < 0)
        return dpiSamples_showError();
    if (dpiRowid_getStringValue(rowid, &rowidStr, &rowidStrLength))
        return dpiSamples_showError();
    printf("%" PRIu64 " row inserted (rowid %.*s).\n", rowCount,
            rowidStrLength, rowidStr);

    // commit changes
    if (dpiConn_commit(conn) < 0)
        return dpiSamples_showError();

    // clean up
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}
