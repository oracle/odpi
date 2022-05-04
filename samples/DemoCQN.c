//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2022, Oracle and/or its affiliates.
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
// DemoCQN.c
//   Demos continuous query notification.
//-----------------------------------------------------------------------------

#ifdef _WIN32
#include <windows.h>
#define sleep(seconds) Sleep(seconds * 1000)
#else
#include <unistd.h>
#endif

#include "SampleLib.h"
#define SQL_TEXT            "select * from DemoTempTable"

//-----------------------------------------------------------------------------
// DemoCallback()
//   Demo callback for continuous query notification.
//-----------------------------------------------------------------------------
void DemoCallback(void *context, dpiSubscrMessage *message)
{
    dpiSubscrMessageQuery *query;
    dpiSubscrMessageTable *table;
    dpiSubscrMessageRow *row;
    uint32_t i, j, k;

    // check for error
    if (message->errorInfo) {
        fprintf(stderr, "ERROR: %.*s (%s: %s)\n",
                message->errorInfo->messageLength, message->errorInfo->message,
                message->errorInfo->fnName, message->errorInfo->action);
        return;
    }

    // display contents of message
    printf("===========================================================\n");
    printf("NOTIFICATION RECEIVED from database %.*s (EVENT TYPE %d)\n",
            message->dbNameLength, message->dbName, message->eventType);
    printf("===========================================================\n");
    for (i = 0; i < message->numQueries; i++) {
        query = &message->queries[i];
        printf("--> Query ID: %" PRIu64 "\n", query->id);
        for (j = 0; j < query->numTables; j++) {
            table = &query->tables[j];
            printf("--> --> Table Name: %.*s\n", table->nameLength,
                    table->name);
            printf("--> --> Table Operation: %d\n", table->operation);
            if (table->numRows > 0) {
                printf("--> --> Table Rows:\n");
                for (k = 0; k < table->numRows; k++) {
                    row = &table->rows[k];
                    printf("--> --> --> ROWID: %.*s\n", row->rowidLength,
                            row->rowid);
                    printf("--> --> --> Operation: %d\n", row->operation);
                }
            }
        }
    }
}



//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiCommonCreateParams commonParams;
    dpiSubscrCreateParams createParams;
    uint32_t numQueryColumns, i;
    dpiSampleParams *params;
    dpiSubscr *subscr;
    uint64_t queryId;
    dpiStmt *stmt;
    dpiConn *conn;

    // connect to database
    // NOTE: events mode must be configured
    params = dpiSamples_getParams();
    if (dpiContext_initCommonCreateParams(params->context, &commonParams) < 0)
        return dpiSamples_showError();
    commonParams.createMode = DPI_MODE_CREATE_EVENTS;
    conn = dpiSamples_getConn(0, &commonParams);

    // create subscription
    if (dpiContext_initSubscrCreateParams(params->context, &createParams) < 0)
        return dpiSamples_showError();
    createParams.qos = DPI_SUBSCR_QOS_QUERY | DPI_SUBSCR_QOS_ROWIDS;
    createParams.callback = DemoCallback;
    if (dpiConn_subscribe(conn, &createParams, &subscr) < 0)
        return dpiSamples_showError();
    printf("Created subscription with registration id %" PRIu64 "\n",
            createParams.outRegId);

    // register query
    if (dpiSubscr_prepareStmt(subscr, SQL_TEXT, strlen(SQL_TEXT), &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiSamples_showError();
    if (dpiStmt_getSubscrQueryId(stmt, &queryId) < 0)
        return dpiSamples_showError();
    dpiStmt_release(stmt);
    printf("Registered query with id %" PRIu64 "\n\n", queryId);

    // wait for events to come through
    printf("In another session, modify the results of the query\n\n%s\n\n",
            SQL_TEXT);
    printf("Use Ctrl-C to terminate or wait for 100 seconds\n");
    for (i = 0; i < 20; i++) {
        printf("Waiting for notifications...\n");
        sleep(5);
    }

    // clean up
    dpiSubscr_release(subscr);
    dpiConn_release(conn);

    printf("Done.\n");
    return 0;
}
