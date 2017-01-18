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
// dpiSubscr.c
//   Implementation of subscriptions (CQN).
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

// forward declarations of internal functions only used in this file
static void dpiSubscr__freeMessage(dpiSubscr *subscr,
        dpiSubscrMessage *message);
static int dpiSubscr__populateMessage(dpiSubscr *subscr,
        dpiSubscrMessage *message, dvoid *descriptor, dpiError *error);
static int dpiSubscr__populateMessageTable(dpiSubscr *subscr,
        dpiSubscrMessageTable *table, dvoid *descriptor, dpiError *error);
static int dpiSubscr__populateQueryChangeMessage(dpiSubscr *subscr,
        dpiSubscrMessage *message, dvoid *descriptor, dpiError *error);


//-----------------------------------------------------------------------------
// dpiSubscr__callback() [INTERNAL]
//   Callback that is used to execute the callback registered when the
// subscription was created.
//-----------------------------------------------------------------------------
static void dpiSubscr__callback(dpiSubscr *subscr, OCISubscription *handle,
        dvoid *payload, ub4 payloadLength, dvoid *descriptor, ub4 mode)
{
    dpiSubscrMessage message;
    dpiErrorInfo errorInfo;
    dpiError error;

    // ensure that the subscription handle is still valid
    if (dpiGen__startPublicFn(subscr, DPI_HTYPE_SUBSCR, __func__, &error) < 0)
        return;

    // populate message
    memset(&message, 0, sizeof(message));
    if (dpiSubscr__populateMessage(subscr, &message, descriptor, &error) < 0) {
        dpiError__getInfo(&error, &errorInfo);
        message.errorInfo = &errorInfo;
    }

    // invoke user callback
    (*subscr->callback)(subscr->callbackContext, &message);

    // clean up message
    dpiSubscr__freeMessage(subscr, &message);
}


//-----------------------------------------------------------------------------
// dpiSubscr__create() [INTERNAL]
//   Create a new subscription structure and return it. In case of error NULL
// is returned.
//-----------------------------------------------------------------------------
int dpiSubscr__create(dpiSubscr *subscr, dpiConn *conn,
        dpiSubscrCreateParams *params, uint32_t *subscrId, dpiError *error)
{
    boolean rowids;
    sword status;
    ub4 qosFlags;

    // retain a reference to the connection
    if (dpiGen__setRefCount(conn, error, 1) < 0)
        return DPI_FAILURE;
    subscr->conn = conn;
    subscr->callback = params->callback;
    subscr->callbackContext = params->callbackContext;
    subscr->qos = params->qos;

    // create the subscription handle
    status = OCIHandleAlloc(conn->env->handle, (dvoid**) &subscr->handle,
            OCI_HTYPE_SUBSCRIPTION, 0, 0);
    if (dpiError__check(error, status, conn, "create handle") < 0)
        return DPI_FAILURE;

    // set the namespace
    status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
            (dvoid*) &params->subscrNamespace, sizeof(ub4),
            OCI_ATTR_SUBSCR_NAMESPACE, error->handle);
    if (dpiError__check(error, status, conn, "set namespace") < 0)
        return DPI_FAILURE;

    // set the protocol
    status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
            (dvoid*) &params->protocol, sizeof(ub4),
            OCI_ATTR_SUBSCR_RECPTPROTO, error->handle);
    if (dpiError__check(error, status, conn, "set protocol") < 0)
        return DPI_FAILURE;

    // set the timeout
    status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
            (dvoid*) &params->timeout, sizeof(ub4), OCI_ATTR_SUBSCR_TIMEOUT,
            error->handle);
    if (dpiError__check(error, status, conn, "set timeout") < 0)
        return DPI_FAILURE;

    // set the port number used on the client to listen for events
    if (params->portNumber > 0) {
        status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
                (dvoid*) &params->portNumber, 0, OCI_ATTR_SUBSCR_PORTNO,
                error->handle);
        if (dpiError__check(error, status, conn, "set port number") < 0)
            return DPI_FAILURE;
    }

    // set the context for the callback
    status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
            (dvoid*) subscr, 0, OCI_ATTR_SUBSCR_CTX, error->handle);
    if (dpiError__check(error, status, conn, "set callback context") < 0)
        return DPI_FAILURE;

    // set the callback, if applicable
    if (params->callback) {
        status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
                (dvoid*) dpiSubscr__callback, 0, OCI_ATTR_SUBSCR_CALLBACK,
                error->handle);
        if (dpiError__check(error, status, conn, "set callback") < 0)
            return DPI_FAILURE;
    }

    // set the subscription name, if applicable
    if (params->name && params->nameLength > 0) {
        status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
                (dvoid*) params->name, params->nameLength,
                OCI_ATTR_SUBSCR_NAME, error->handle);
        if (dpiError__check(error, status, conn, "set name") < 0)
            return DPI_FAILURE;
    }

    // set QOS flags
    qosFlags = 0;
    if (params->qos & DPI_SUBSCR_QOS_RELIABLE)
        qosFlags |= OCI_SUBSCR_QOS_RELIABLE;
    if (params->qos & DPI_SUBSCR_QOS_DEREG_NFY)
        qosFlags |= OCI_SUBSCR_QOS_PURGE_ON_NTFN;
    if (qosFlags) {
        status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
                (dvoid*) &qosFlags, sizeof(ub4), OCI_ATTR_SUBSCR_QOSFLAGS,
                error->handle);
        if (dpiError__check(error, status, conn, "set QOS") < 0)
            return DPI_FAILURE;
    }

    // set CQ specific QOS flags
    qosFlags = 0;
    if (params->qos & DPI_SUBSCR_QOS_QUERY)
        qosFlags |= OCI_SUBSCR_CQ_QOS_QUERY;
    if (params->qos & DPI_SUBSCR_QOS_BEST_EFFORT)
        qosFlags |= OCI_SUBSCR_CQ_QOS_BEST_EFFORT;
    if (qosFlags) {
        status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
                (dvoid*) &qosFlags, sizeof(ub4), OCI_ATTR_SUBSCR_CQ_QOSFLAGS,
                error->handle);
        if (dpiError__check(error, status, conn, "set CQ QOS") < 0)
            return DPI_FAILURE;
    }

    // set rowids flag, if applicable
    if (params->qos & DPI_SUBSCR_QOS_ROWIDS) {
        rowids = 1;
        status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
                (dvoid*) &rowids, 0, OCI_ATTR_CHNF_ROWIDS, error->handle);
        if (dpiError__check(error, status, conn, "set rowids flag") < 0)
            return DPI_FAILURE;
    }

    // set which operations are desired, if applicable
    if (params->operations) {
        status = OCIAttrSet(subscr->handle, OCI_HTYPE_SUBSCRIPTION,
                (dvoid*) &params->operations, 0, OCI_ATTR_CHNF_OPERATIONS,
                error->handle);
        if (dpiError__check(error, status, conn, "set operations") < 0)
            return DPI_FAILURE;
    }

    // register the subscription
    status = OCISubscriptionRegister(conn->handle, &subscr->handle, 1,
            error->handle, OCI_DEFAULT);
    if (dpiError__check(error, status, conn, "register") < 0)
        return DPI_FAILURE;

    // get the registration id
    status = OCIAttrGet(subscr->handle, OCI_HTYPE_SUBSCRIPTION, subscrId,
              NULL, OCI_ATTR_SUBSCR_CQ_REGID, error->handle);
    return dpiError__check(error, status, conn, "get registration id");
}


//-----------------------------------------------------------------------------
// dpiSubscr__free() [INTERNAL]
//   Free the memory and any resources associated with the subscription.
//-----------------------------------------------------------------------------
void dpiSubscr__free(dpiSubscr *subscr, dpiError *error)
{
    if (subscr->handle) {
        OCISubscriptionUnRegister(subscr->conn->handle, subscr->handle,
                error->handle, OCI_DEFAULT);
        subscr->handle = NULL;
    }
    if (subscr->conn) {
        dpiGen__setRefCount(subscr->conn, error, -1);
        subscr->conn = NULL;
    }
    free(subscr);
}


//-----------------------------------------------------------------------------
// dpiSubscr__freeMessage() [INTERNAL]
//   Free memory associated with the message.
//-----------------------------------------------------------------------------
static void dpiSubscr__freeMessage(dpiSubscr *subscr,
        dpiSubscrMessage *message)
{
    dpiSubscrMessageQuery *query;
    uint32_t i, j;

    // free the tables for the message
    if (message->numTables > 0) {
        for (i = 0; i < message->numTables; i++) {
            if (message->tables[i].numRows > 0)
                free(message->tables[i].rows);
        }
        free(message->tables);
    }

    // free the queries for the message
    if (message->numQueries > 0) {
        for (i = 0; i < message->numQueries; i++) {
            query = &message->queries[i];
            if (query->numTables > 0) {
                for (j = 0; j < query->numTables; j++) {
                    if (query->tables[i].numRows > 0)
                        free(query->tables[i].rows);
                }
                free(query->tables);
            }
        }
        free(message->queries);
    }
}


//-----------------------------------------------------------------------------
// dpiSubscr__populateObjectChangeMessage() [INTERNAL]
//   Populate object change message with details.
//-----------------------------------------------------------------------------
static int dpiSubscr__populateObjectChangeMessage(dpiSubscr *subscr,
        dpiSubscrMessage *message, dvoid *descriptor, dpiError *error)
{
    dvoid **tableDescriptor, *indicator;
    OCIColl *tables;
    boolean exists;
    sb4 numTables;
    sword status;
    uint32_t i;

    // determine table collection
    status = OCIAttrGet(descriptor, OCI_DTYPE_CHDES, &tables, 0,
            OCI_ATTR_CHDES_TABLE_CHANGES, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get tables") < 0)
        return DPI_FAILURE;
    if (!tables)
        return DPI_SUCCESS;

    // determine number of tables
    status = OCICollSize(subscr->env->handle, error->handle, tables,
            &numTables);
    if (dpiError__check(error, status, subscr->conn, "get num tables") < 0)
        return DPI_FAILURE;

    // allocate memory for table entries
    message->tables = calloc(numTables, sizeof(dpiSubscrMessageTable));
    if (!message->tables)
        return dpiError__set(error, "allocate msg tables", DPI_ERR_NO_MEMORY);
    message->numTables = numTables;

    // populate message table entries
    for (i = 0; i < message->numTables; i++) {
        status = OCICollGetElem(subscr->env->handle, error->handle, tables, i,
                &exists, (dvoid*) &tableDescriptor, &indicator);
        if (dpiError__check(error, status, subscr->conn, "get tab info") < 0)
            return DPI_FAILURE;
        if (dpiSubscr__populateMessageTable(subscr, &message->tables[i],
                *tableDescriptor, error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr__populateMessage() [INTERNAL]
//   Populate message with details.
//-----------------------------------------------------------------------------
static int dpiSubscr__populateMessage(dpiSubscr *subscr,
        dpiSubscrMessage *message, dvoid *descriptor, dpiError *error)
{
    sword status;

    // determine the type of event that was spawned
    status = OCIAttrGet(descriptor, OCI_DTYPE_CHDES, &message->eventType, NULL,
            OCI_ATTR_CHDES_NFYTYPE, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get event type") < 0)
        return DPI_FAILURE;

    // determine the name of the database which spawned the event
    status = OCIAttrGet(descriptor, OCI_DTYPE_CHDES, (void*) &message->dbName,
            &message->dbNameLength, OCI_ATTR_CHDES_DBNAME, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get DB name") < 0)
        return DPI_FAILURE;

    // populate event specific attributes
    switch (message->eventType) {
        case DPI_EVENT_OBJCHANGE:
            return dpiSubscr__populateObjectChangeMessage(subscr, message,
                    descriptor, error);
        case DPI_EVENT_QUERYCHANGE:
            return dpiSubscr__populateQueryChangeMessage(subscr, message,
                    descriptor, error);
        default:
            return dpiError__set(error, "event type", DPI_ERR_NOT_SUPPORTED);
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr__populateMessageQuery() [INTERNAL]
//   Populate a message query structure from the OCI descriptor.
//-----------------------------------------------------------------------------
static int dpiSubscr__populateMessageQuery(dpiSubscr *subscr,
        dpiSubscrMessageQuery *query, dvoid *descriptor, dpiError *error)
{
    dvoid **tableDescriptor, *indicator;
    OCIColl *tables;
    boolean exists;
    sb4 numTables;
    sword status;
    uint32_t i;

    // determine query id
    status = OCIAttrGet(descriptor, OCI_DTYPE_CQDES, &query->id, 0,
            OCI_ATTR_CQDES_QUERYID, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get id") < 0)
        return DPI_FAILURE;

    // determine operation
    status = OCIAttrGet(descriptor, OCI_DTYPE_CQDES, &query->operation, 0,
            OCI_ATTR_CQDES_OPERATION, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get operation") < 0)
        return DPI_FAILURE;

    // determine table collection
    status = OCIAttrGet(descriptor, OCI_DTYPE_CQDES, &tables, 0,
            OCI_ATTR_CQDES_TABLE_CHANGES, error->handle);
    if (dpiError__check(error, status, subscr->conn,
            "get table descriptor") < 0)
        return DPI_FAILURE;
    if (!tables)
        return DPI_SUCCESS;

    // determine number of tables
    status = OCICollSize(subscr->env->handle, error->handle, tables,
            &numTables);
    if (dpiError__check(error, status, subscr->conn, "get num tables") < 0)
        return DPI_FAILURE;

    // allocate memory for table entries
    query->tables = calloc(numTables, sizeof(dpiSubscrMessageTable));
    if (!query->tables)
        return dpiError__set(error, "allocate query tables",
                DPI_ERR_NO_MEMORY);
    query->numTables = numTables;

    // populate message table entries
    for (i = 0; i < query->numTables; i++) {
        status = OCICollGetElem(subscr->env->handle, error->handle, tables, i,
                &exists, (dvoid*) &tableDescriptor, &indicator);
        if (dpiError__check(error, status, subscr->conn, "get table info") < 0)
            return DPI_FAILURE;
        if (dpiSubscr__populateMessageTable(subscr, &query->tables[i],
                *tableDescriptor, error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr__populateMessageRow() [INTERNAL]
//   Populate a message row structure from the OCI descriptor.
//-----------------------------------------------------------------------------
static int dpiSubscr__populateMessageRow(dpiSubscr *subscr,
        dpiSubscrMessageRow *row, dvoid *descriptor, dpiError *error)
{
    sword status;

    // determine operation
    status = OCIAttrGet(descriptor, OCI_DTYPE_ROW_CHDES, &row->operation,
            0, OCI_ATTR_CHDES_ROW_OPFLAGS, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get operation") < 0)
        return DPI_FAILURE;

    // determine rowid
    status = OCIAttrGet(descriptor, OCI_DTYPE_ROW_CHDES, (void*) &row->rowid,
            &row->rowidLength, OCI_ATTR_CHDES_ROW_ROWID, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get rowid") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr__populateMessageTable() [INTERNAL]
//   Populate a message table structure from the OCI descriptor.
//-----------------------------------------------------------------------------
static int dpiSubscr__populateMessageTable(dpiSubscr *subscr,
        dpiSubscrMessageTable *table, dvoid *descriptor, dpiError *error)
{
    dvoid **rowDescriptor, *indicator;
    boolean exists;
    OCIColl *rows;
    sword status;
    sb4 numRows;
    uint32_t i;

    // determine operation
    status = OCIAttrGet(descriptor, OCI_DTYPE_TABLE_CHDES, &table->operation,
            0, OCI_ATTR_CHDES_TABLE_OPFLAGS, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get operation") < 0)
        return DPI_FAILURE;

    // determine table name
    status = OCIAttrGet(descriptor, OCI_DTYPE_TABLE_CHDES,
            (void*) &table->name, &table->nameLength,
            OCI_ATTR_CHDES_TABLE_NAME, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get table name") < 0)
        return DPI_FAILURE;

    // if change invalidated all rows, nothing to do
    if (table->operation & OCI_OPCODE_ALLROWS)
        return DPI_SUCCESS;

    // determine rows collection
    status = OCIAttrGet(descriptor, OCI_DTYPE_TABLE_CHDES, &rows, 0,
            OCI_ATTR_CHDES_TABLE_ROW_CHANGES, error->handle);
    if (dpiError__check(error, status, subscr->conn,
            "get rows descriptor") < 0)
        return DPI_FAILURE;

    // determine number of rows in collection
    status = OCICollSize(subscr->env->handle, error->handle, rows, &numRows);
    if (dpiError__check(error, status, subscr->conn, "get number of rows") < 0)
        return DPI_FAILURE;

    // allocate memory for row entries
    table->rows = calloc(numRows, sizeof(dpiSubscrMessageRow));
    if (!table->rows)
        return dpiError__set(error, "allocate rows", DPI_ERR_NO_MEMORY);
    table->numRows = numRows;

    // populate the rows attribute
    for (i = 0; i < table->numRows; i++) {
        status = OCICollGetElem(subscr->env->handle, error->handle, rows, i,
                &exists, (dvoid*) &rowDescriptor, &indicator);
        if (dpiError__check(error, status, subscr->conn, "get row info") < 0)
            return DPI_FAILURE;
        if (dpiSubscr__populateMessageRow(subscr, &table->rows[i],
                *rowDescriptor, error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr__populateQueryChangeMessage() [INTERNAL]
//   Populate query change message with details.
//-----------------------------------------------------------------------------
static int dpiSubscr__populateQueryChangeMessage(dpiSubscr *subscr,
        dpiSubscrMessage *message, dvoid *descriptor, dpiError *error)
{
    dvoid **queryDescriptor, *indicator;
    OCIColl *queries;
    boolean exists;
    sb4 numQueries;
    sword status;
    uint32_t i;

    // determine query collection
    status = OCIAttrGet(descriptor, OCI_DTYPE_CHDES, &queries, 0,
            OCI_ATTR_CHDES_QUERIES, error->handle);
    if (dpiError__check(error, status, subscr->conn, "get queries") < 0)
        return DPI_FAILURE;
    if (!queries)
        return DPI_SUCCESS;

    // determine number of queries
    status = OCICollSize(subscr->env->handle, error->handle, queries,
            &numQueries);
    if (dpiError__check(error, status, subscr->conn, "get num queries") < 0)
        return DPI_FAILURE;

    // allocate memory for query entries
    message->queries = calloc(numQueries, sizeof(dpiSubscrMessageQuery));
    if (!message->queries)
        return dpiError__set(error, "allocate queries", DPI_ERR_NO_MEMORY);
    message->numQueries = numQueries;

    // populate each entry with a message query instance
    for (i = 0; i < message->numQueries; i++) {
        status = OCICollGetElem(subscr->env->handle, error->handle, queries, i,
                &exists, (dvoid*) &queryDescriptor, &indicator);
        if (dpiError__check(error, status, subscr->conn, "get query info") < 0)
            return DPI_FAILURE;
        if (dpiSubscr__populateMessageQuery(subscr, &message->queries[i],
                *queryDescriptor, error) < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr__prepareStmt() [INTERNAL]
//   Internal method for preparing statement against a subscription. This
// allows for normal error processing without having to worry about freeing the
// statement for every error that might take place.
//-----------------------------------------------------------------------------
int dpiSubscr__prepareStmt(dpiSubscr *subscr, dpiStmt *stmt,
        const char *sql, uint32_t sqlLength, dpiError *error)
{
    sword status;

    // prepare statement for execution
    if (dpiStmt__prepare(stmt, sql, sqlLength, NULL, 0, error) < 0)
        return DPI_FAILURE;

    // fetch array size is set to 1 in order to avoid over allocation since
    // the query is not really going to be used for fetching rows, just for
    // registration
    stmt->fetchArraySize = 1;

    // set subscription handle
    status = OCIAttrSet(stmt->handle, OCI_HTYPE_STMT, subscr->handle, 0,
            OCI_ATTR_CHNF_REGHANDLE, error->handle);
    return dpiError__check(error, status, subscr->conn,
            "set subscription handle");
}


//-----------------------------------------------------------------------------
// dpiSubscr_addRef() [PUBLIC]
//   Add a reference to the subscription.
//-----------------------------------------------------------------------------
int dpiSubscr_addRef(dpiSubscr *subscr)
{
    return dpiGen__addRef(subscr, DPI_HTYPE_SUBSCR, __func__);
}


//-----------------------------------------------------------------------------
// dpiSubscr_close() [PUBLIC]
//   Close the subscription now, not when the last reference is released. This
// deregisters the subscription so that notifications are no longer sent.
//-----------------------------------------------------------------------------
int dpiSubscr_close(dpiSubscr *subscr)
{
    dpiError error;
    sword status;

    if (dpiGen__startPublicFn(subscr, DPI_HTYPE_SUBSCR, __func__, &error) < 0)
        return DPI_FAILURE;
    if (subscr->handle) {
        status = OCISubscriptionUnRegister(subscr->conn->handle,
                subscr->handle, error.handle, OCI_DEFAULT);
        if (dpiError__check(&error, status, subscr->conn, "unregister") < 0)
            return DPI_FAILURE;
        subscr->handle = NULL;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr_prepareStmt() [PUBLIC]
//   Prepare statement for registration with subscription.
//-----------------------------------------------------------------------------
int dpiSubscr_prepareStmt(dpiSubscr *subscr, const char *sql,
        uint32_t sqlLength, dpiStmt **stmt)
{
    dpiStmt *tempStmt;
    dpiError error;

    *stmt = NULL;
    if (dpiGen__startPublicFn(subscr, DPI_HTYPE_SUBSCR, __func__, &error) < 0)
        return DPI_FAILURE;
    if (dpiStmt__allocate(subscr->conn, 0, &tempStmt, &error) < 0)
        return DPI_FAILURE;
    if (dpiSubscr__prepareStmt(subscr, tempStmt, sql, sqlLength,
            &error) < 0) {
        dpiStmt__free(tempStmt, &error);
        return DPI_FAILURE;
    }

    *stmt = tempStmt;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiSubscr_release() [PUBLIC]
//   Release a reference to the subscription.
//-----------------------------------------------------------------------------
int dpiSubscr_release(dpiSubscr *subscr)
{
    return dpiGen__release(subscr, DPI_HTYPE_SUBSCR, __func__);
}

