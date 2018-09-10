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
// TestBFILE.c
//   Tests whether BFILEs are handled properly using ODPI-C.
//
// NOTE: the program assumes that you have write access to the
// directory path pointed to by the directory object, i.e. that the
// program is being run on the same machine as the database.
//
// DIR_NAME is specified in the Makefile
//
//-----------------------------------------------------------------------------

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "SampleLib.h"
#define SQL_TEXT_QUERY_DIR  "select directory_path " \
                            "from all_directories " \
                            "where directory_name = :1"
#define SQL_TEXT_DELETE     "delete from TestBFILEs"
#define SQL_TEXT_INSERT     "insert into TestBFILEs " \
                            "values (:IntValue, :BFILEValue)"
#define SQL_TEXT_QUERY      "select IntCol, BFILECol " \
                            "from TestBFILEs"
#define FILE_NAME           "test_contents.txt"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiData *intColValue, *bfileColValue, *pathValue, *bfileValue, intValue;
    uint32_t numQueryColumns, bufferRowIndex, i;
    dpiNativeTypeNum nativeTypeNum;
    dpiSampleParams *params;
    dpiQueryInfo queryInfo;
    uint64_t blobSize;
    dpiData bindValue;
    dpiVar *bfileVar;
    dpiStmt *stmt;
    dpiConn *conn;
    char *path;
    int found;
    FILE *fp;

    // connect to database
    params = dpiSamples_getParams();
    conn = dpiSamples_getConn(0, NULL);
    printf("Note: this test must be run on the same machine as the database\n");

    // find the directory path location by querying from the database
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_QUERY_DIR,
            strlen(SQL_TEXT_QUERY_DIR), NULL, 0, &stmt) < 0)
        return dpiSamples_showError();
    dpiData_setBytes(&bindValue, (char*) params->dirName,
            params->dirNameLength);
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_BYTES, &bindValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &pathValue) < 0)
        return dpiSamples_showError();
    path = malloc(pathValue->value.asBytes.length + 1);
    if (!path) {
        printf("ERROR: unable to duplicate path string!?\n");
        return -1;
    }
    memcpy(path, pathValue->value.asBytes.ptr,
            pathValue->value.asBytes.length);
    path[pathValue->value.asBytes.length] = '\0';
    dpiStmt_release(stmt);
    printf("%.*s path is '%s'\n", params->dirNameLength, params->dirName,
            path);

    // write a temporary file at that location
    if (chdir(path) < 0) {
        printf("ERROR: unable to change directory to %.*s location\n",
                params->dirNameLength, params->dirName);
        return -1;
    }
    free(path);
    printf("Writing file named '%s'\n", FILE_NAME);
    fp = fopen(FILE_NAME, "w");
    if (!fp) {
        printf("ERROR: unable to open test file for writing\n");
        return -1;
    }
    fprintf(fp, "These are some test comments.\nFile can be deleted.\n");
    fclose(fp);

    // delete existing rows in table
    printf("Delete existing rows in table...\n");
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_DELETE, strlen(SQL_TEXT_DELETE),
            NULL, 0, &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);

    // inserting row into table
    printf("Inserting row into table...\n");
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_BFILE, DPI_NATIVE_TYPE_LOB, 1, 0,
            0, 0, NULL, &bfileVar, &bfileValue) < 0)
        return dpiSamples_showError();
    bfileValue->isNull = 0;
    if (dpiLob_setDirectoryAndFileName(bfileValue->value.asLOB,
            params->dirName, params->dirNameLength, FILE_NAME,
            strlen(FILE_NAME)) < 0)
        return dpiSamples_showError();
    intValue.isNull = 0;
    intValue.value.asInt64 = 1;
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_INSERT, strlen(SQL_TEXT_INSERT),
            NULL, 0, &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_INT64, &intValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 2, bfileVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiConn_commit(conn) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);
    dpiVar_release(bfileVar);

    // querying row from table
    printf("Querying row from table...\n");
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_QUERY, strlen(SQL_TEXT_QUERY),
            NULL, 0, &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &intColValue) < 0 ||
                dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum,
                        &bfileColValue) < 0)
            return dpiSamples_showError();
        if (dpiLob_getSize(bfileColValue->value.asLOB, &blobSize) < 0)
            return dpiSamples_showError();
        printf("Row: IntCol = %g, BfileCol = BFILE(%" PRIu64 ")\n",
                intColValue->value.asDouble, blobSize);
    }

    // display description of each variable
    for (i = 0; i < numQueryColumns; i++) {
        if (dpiStmt_getQueryInfo(stmt, i + 1, &queryInfo) < 0)
            return dpiSamples_showError();
        printf("('%.*s', %d, %d, %d, %d, %d, %d)\n", queryInfo.nameLength,
                queryInfo.name, queryInfo.typeInfo.oracleTypeNum,
                queryInfo.typeInfo.sizeInChars,
                queryInfo.typeInfo.clientSizeInBytes,
                queryInfo.typeInfo.precision, queryInfo.typeInfo.scale,
                queryInfo.nullOk);
    }

    // clean up
    dpiStmt_release(stmt);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

