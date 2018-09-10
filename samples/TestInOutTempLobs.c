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
// TestInOutTempLobs.c
//   Tests whether temporary LOBs passed to a PL/SQL procedure are processed
// correctly without leaks.
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT_1                      "begin pkg_TestLOBs." \
                                        "TestInOutTempClob(:1, :2); end;"
#define SQL_TEXT_2                      "select sid from v$session " \
                                        "where audsid = userenv('sessionid')"
#define SQL_TEXT_3                      "select cache_lobs, nocache_lobs, " \
                                        "abstract_lobs " \
                                        "from v$temporary_lobs " \
                                        "where sid = :sid"
#define LOB_TEXT                        "This is but a test!"


//-----------------------------------------------------------------------------
// GetNumTempLobs()
//   Calculate and print the number of temporary LOBs for the current session.
//-----------------------------------------------------------------------------
int GetNumTempLobs(dpiConn *conn, int64_t sid)
{
    dpiData *cacheLobsData, *nocacheLobsData, *abstractLobsData, *sidData;
    dpiVar *cacheLobsVar, *nocacheLobsVar, *abstractLobsVar, *sidVar;
    uint32_t numQueryColumns, bufferRowIndex;
    dpiStmt *stmt;
    int found;

    // prepare and execute statement
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_3, strlen(SQL_TEXT_3), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &sidVar, &sidData) < 0)
        return dpiSamples_showError();
    sidData->value.asInt64 = sid;
    sidData->isNull = 0;
    if (dpiStmt_bindByPos(stmt, 1, sidVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    dpiVar_release(sidVar);

    // fetch row from database
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &cacheLobsVar, &cacheLobsData) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &nocacheLobsVar, &nocacheLobsData) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &abstractLobsVar, &abstractLobsData) < 0)
        return dpiSamples_showError();
    if (dpiStmt_define(stmt, 1, cacheLobsVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_define(stmt, 2, nocacheLobsVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_define(stmt, 3, abstractLobsVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiSamples_showError();
    if (!found) {
        fprintf(stderr, "No row found for sid %" PRId64 "!\n", sid);
        return -1;
    }

    // display result and clean up
    printf("Temporary LOBS: cache: %" PRId64 ", nocache: %" PRId64
            ", abstract: %" PRId64 "\n", cacheLobsData->value.asInt64,
            nocacheLobsData->value.asInt64, abstractLobsData->value.asInt64);
    dpiVar_release(cacheLobsVar);
    dpiVar_release(nocacheLobsVar);
    dpiVar_release(abstractLobsVar);
    dpiStmt_release(stmt);
    return 0;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t numQueryColumns, bufferRowIndex;
    dpiData *sidValue, *lobValue, *intValue;
    dpiVar *sidVar, *lobVar, *intVar;
    dpiStmt *stmt;
    dpiConn *conn;
    uint64_t sid;
    dpiLob *lob;
    int found;

    // connect to database
    conn = dpiSamples_getConn(0, NULL);

    // fetch SID
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_2, strlen(SQL_TEXT_2), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &sidVar, &sidValue) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiSamples_showError();
    if (dpiStmt_define(stmt, 1, sidVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiSamples_showError();
    if (!found) {
        fprintf(stderr, "No row found for current session!?\n");
        return -1;
    }
    sid = sidValue->value.asInt64;
    dpiVar_release(sidVar);
    dpiStmt_release(stmt);
    printf("SID of current session is %" PRId64 "\n", sid);

    // display the number of temporary LOBs at this point (should be 0)
    if (GetNumTempLobs(conn, sid) < 0)
        return -1;

    // create new temporary LOB and populate it
    if (dpiConn_newTempLob(conn, DPI_ORACLE_TYPE_CLOB, &lob) < 0)
        return dpiSamples_showError();
    if (dpiLob_setFromBytes(lob, LOB_TEXT, strlen(LOB_TEXT)) < 0)
        return dpiSamples_showError();

    // display the number of temporary LOBs at this point (should be 1)
    if (GetNumTempLobs(conn, sid) < 0)
        return -1;

    // prepare bind variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &intVar, &intValue) < 0)
        return dpiSamples_showError();
    intValue->isNull = 0;
    intValue->value.asInt64 = 1;
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_CLOB, DPI_NATIVE_TYPE_LOB, 1, 0,
            0, 0, NULL, &lobVar, &lobValue) < 0)
        return dpiSamples_showError();
    if (dpiVar_setFromLob(lobVar, 0, lob) < 0)
        return dpiSamples_showError();

    // call stored procedure
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT_1, strlen(SQL_TEXT_1), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 1, intVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_bindByPos(stmt, 2, lobVar) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiConn_commit(conn) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);
    dpiVar_release(intVar);
    dpiVar_release(lobVar);
    dpiLob_release(lob);

    // display the number of temporary LOBs at this point (should be 0)
    if (GetNumTempLobs(conn, sid) < 0)
        return -1;

    // clean up
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}

