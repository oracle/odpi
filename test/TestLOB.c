//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestLOB.c
//   Tests whether LOBs are handled properly using ODPI-C.
//-----------------------------------------------------------------------------

#include <inttypes.h>
#include "Test.h"
#define SQL_TEXT                        "select IntCol, BlobCol from TestBLOBs"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex, i;
    dpiData *intColValue, *blobColValue;
    dpiNativeTypeNum nativeTypeNum;
    dpiQueryInfo queryInfo;
    uint64_t blobSize;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // connect to database
    conn = GetConnection(0, NULL);
    if (!conn)
        return -1;

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return ShowError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return ShowError();

    // fetch rows
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return ShowError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &intColValue) < 0 ||
                dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum,
                        &blobColValue) < 0)
            return ShowError();
        if (dpiLob_getSize(blobColValue->value.asLOB, &blobSize) < 0)
            return ShowError();
        printf("Row: IntCol = %g, BlobCol = BLOB(%" PRIu64 ")\n",
                intColValue->value.asDouble, blobSize);
    }

    // display description of each variable
    for (i = 0; i < numQueryColumns; i++) {
        if (dpiStmt_getQueryInfo(stmt, i + 1, &queryInfo) < 0)
            return ShowError();
        printf("('%*s', %d, %d, %d, %d, %d, %d)\n", queryInfo.nameLength,
                queryInfo.name, queryInfo.oracleTypeNum, queryInfo.sizeInChars,
                queryInfo.clientSizeInBytes, queryInfo.precision,
                queryInfo.scale, queryInfo.nullOk);
    }

    // clean up
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

