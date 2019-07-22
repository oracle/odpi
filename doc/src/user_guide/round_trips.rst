.. _round_trips:

ODPI-C Function Round-Trips
---------------------------

The following table identifies the number of round-trips to the database that
are performed for each function. A round-trip is defined as the trip from the
Oracle Client libraries (used by ODPI-C) to the database and back.

.. list-table::
    :header-rows: 1

    * - Function
      - Number of Round-Trips
      - Notes
    * - :func:`dpiConn_addRef()`
      - 0
      -
    * - :func:`dpiConn_beginDistribTrans()`
      - 1
      -
    * - :func:`dpiConn_breakExecution()`
      - 1
      -
    * - :func:`dpiConn_changePassword()`
      - 1
      -
    * - :func:`dpiConn_close()`
      - 0, 1 or 2
      - If the connection was acquired from a pool, a round-trip is only
        required if the parameter *mode* is set to the value
        **DPI_MODE_CONN_CLOSE_DROP**. For standalone connections, a round-trip
        is always required. If there is an open transaction, a round-trip is
        also required to perform the implicit rollback that takes place.
    * - :func:`dpiConn_commit()`
      - 1
      -
    * - :func:`dpiConn_create()`
      - See notes.
      - If a standalone connection is being created, one round-trip is
        required. If a connection is being acquired from a pool, no round-trips
        are required if an unused connection is available in the pool;
        otherwise, a round trip is required for each connection that is added
        to the pool (see :member:`dpiPoolCreateParams.sessionIncrement`).
    * - :func:`dpiConn_deqObject()`
      - 1
      -
    * - :func:`dpiConn_enqObject()`
      - 1
      -
    * - :func:`dpiConn_getCallTimeout()`
      - 0
      -
    * - :func:`dpiConn_getCurrentSchema()`
      - 0
      -
    * - :func:`dpiConn_getEdition()`
      - 0
      -
    * - :func:`dpiConn_getEncodingInfo()`
      - 0
      -
    * - :func:`dpiConn_getExternalName()`
      - 0
      -
    * - :func:`dpiConn_getHandle()`
      - 0
      -
    * - :func:`dpiConn_getInternalName()`
      - 0
      -
    * - :func:`dpiConn_getLTXID()`
      - 0
      -
    * - :func:`dpiConn_getObjectType()`
      - See notes.
      - If the type information is already available in the Oracle Client
        object cache, only one round trip is required; otherwise, two
        round-trips are required unless the type contains embedded types or is
        a collection. In that case additional round-trips are required to fetch
        the additional type information.
    * - :func:`dpiConn_getServerVersion()`
      - 0 or 1
      - The first call requires a round-trip. Subsequent calls do not require a
        round-trip.
    * - :func:`dpiConn_getSodaDb()`
      - 0
      -
    * - :func:`dpiConn_getStmtCacheSize()`
      - 0
      -
    * - :func:`dpiConn_newDeqOptions()`
      - 0
      -
    * - :func:`dpiConn_newEnqOptions()`
      - 0
      -
    * - :func:`dpiConn_newMsgProps()`
      - 0
      -
    * - :func:`dpiConn_newQueue()`
      - 0
      -
    * - :func:`dpiConn_newTempLob()`
      - 1
      -
    * - :func:`dpiConn_newVar()`
      - 0
      -
    * - :func:`dpiConn_ping()`
      - 1
      -
    * - :func:`dpiConn_prepareDistribTrans()`
      - 1
      -
    * - :func:`dpiConn_prepareStmt()`
      - 0
      -
    * - :func:`dpiConn_release()`
      - 0 or 1
      - If the number of references exceeds 1 or the connection has already
        been closed by a call to :func:`dpiConn_close()` then no round-trips
        are required; otherwise, the connection is closed at this point and the
        notes on the function :func:`dpiConn_close()` apply.
    * - :func:`dpiConn_rollback()`
      - 1
      -
    * - :func:`dpiConn_setAction()`
      - 0
      -
    * - :func:`dpiConn_setCallTimeout()`
      - 0
      -
    * - :func:`dpiConn_setClientIdentifier()`
      - 0
      -
    * - :func:`dpiConn_setClientInfo()`
      - 0
      -
    * - :func:`dpiConn_setCurrentSchema()`
      - 0
      -
    * - :func:`dpiConn_setDbOp()`
      - 0
      -
    * - :func:`dpiConn_setExternalName()`
      - 0
      -
    * - :func:`dpiConn_setInternalName()`
      - 0
      -
    * - :func:`dpiConn_setModule()`
      - 0
      -
    * - :func:`dpiConn_setStmtCacheSize()`
      - 0
      -
    * - :func:`dpiConn_shutdownDatabase()`
      - 1
      -
    * - :func:`dpiConn_startupDatabase()`
      - 1
      -
    * - :func:`dpiConn_subscribe()`
      - 1
      -
    * - :func:`dpiConn_unsubscribe()`
      - 1
      -
    * - :func:`dpiContext_create()`
      - 0
      -
    * - :func:`dpiContext_destroy()`
      - 0
      -
    * - :func:`dpiContext_getClientVersion()`
      - 0
      -
    * - :func:`dpiContext_getError()`
      - 0
      -
    * - :func:`dpiContext_initCommonCreateParams()`
      - 0
      -
    * - :func:`dpiContext_initConnCreateParams()`
      - 0
      -
    * - :func:`dpiContext_initPoolCreateParams()`
      - 0
      -
    * - :func:`dpiContext_initSodaOperOptions()`
      - 0
      -
    * - :func:`dpiContext_initSubscrCreateParams()`
      - 0
      -
    * - :func:`dpiData_getBool()`
      - 0
      -
    * - :func:`dpiData_getBytes()`
      - 0
      -
    * - :func:`dpiData_getDouble()`
      - 0
      -
    * - :func:`dpiData_getFloat()`
      - 0
      -
    * - :func:`dpiData_getInt64()`
      - 0
      -
    * - :func:`dpiData_getIntervalDS()`
      - 0
      -
    * - :func:`dpiData_getIntervalYM()`
      - 0
      -
    * - :func:`dpiData_getIsNull()`
      - 0
      -
    * - :func:`dpiData_getLOB()`
      - 0
      -
    * - :func:`dpiData_getObject()`
      - 0
      -
    * - :func:`dpiData_getStmt()`
      - 0
      -
    * - :func:`dpiData_getTimestamp()`
      - 0
      -
    * - :func:`dpiData_getUint64()`
      - 0
      -
    * - :func:`dpiData_setBool()`
      - 0
      -
    * - :func:`dpiData_setBytes()`
      - 0
      -
    * - :func:`dpiData_setDouble()`
      - 0
      -
    * - :func:`dpiData_setFloat()`
      - 0
      -
    * - :func:`dpiData_setInt64()`
      - 0
      -
    * - :func:`dpiData_setIntervalDS()`
      - 0
      -
    * - :func:`dpiData_setIntervalYM()`
      - 0
      -
    * - :func:`dpiData_setLOB()`
      - 0
      -
    * - :func:`dpiData_setNull()`
      - 0
      -
    * - :func:`dpiData_setObject()`
      - 0
      -
    * - :func:`dpiData_setStmt()`
      - 0
      -
    * - :func:`dpiData_setTimestamp()`
      - 0
      -
    * - :func:`dpiData_setUint64()`
      - 0
      -
    * - :func:`dpiDeqOptions_addRef()`
      - 0
      -
    * - :func:`dpiDeqOptions_getCondition()`
      - 0
      -
    * - :func:`dpiDeqOptions_getConsumerName()`
      - 0
      -
    * - :func:`dpiDeqOptions_getCorrelation()`
      - 0
      -
    * - :func:`dpiDeqOptions_getMode()`
      - 0
      -
    * - :func:`dpiDeqOptions_getMsgId()`
      - 0
      -
    * - :func:`dpiDeqOptions_getNavigation()`
      - 0
      -
    * - :func:`dpiDeqOptions_getTransformation()`
      - 0
      -
    * - :func:`dpiDeqOptions_getVisibility()`
      - 0
      -
    * - :func:`dpiDeqOptions_getWait()`
      - 0
      -
    * - :func:`dpiDeqOptions_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiDeqOptions_setCondition()`
      - 0
      -
    * - :func:`dpiDeqOptions_setConsumerName()`
      - 0
      -
    * - :func:`dpiDeqOptions_setCorrelation()`
      - 0
      -
    * - :func:`dpiDeqOptions_setDeliveryMode()`
      - 0
      -
    * - :func:`dpiDeqOptions_setMode()`
      - 0
      -
    * - :func:`dpiDeqOptions_setMsgId()`
      - 0
      -
    * - :func:`dpiDeqOptions_setNavigation()`
      - 0
      -
    * - :func:`dpiDeqOptions_setTransformation()`
      - 0
      -
    * - :func:`dpiDeqOptions_setVisibility()`
      - 0
      -
    * - :func:`dpiDeqOptions_setWait()`
      - 0
      -
    * - :func:`dpiEnqOptions_addRef()`
      - 0
      -
    * - :func:`dpiEnqOptions_getTransformation()`
      - 0
      -
    * - :func:`dpiEnqOptions_getVisibility()`
      - 0
      -
    * - :func:`dpiEnqOptions_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiEnqOptions_setDeliveryMode()`
      - 0
      -
    * - :func:`dpiEnqOptions_setTransformation()`
      - 0
      -
    * - :func:`dpiEnqOptions_getVisibility()`
      - 0
      -
    * - :func:`dpiLob_addRef()`
      - 0
      -
    * - :func:`dpiLob_close()`
      - 0 or 1
      - If the LOB is a temporary LOB, one round-trip is required to free the
        temporary LOB; otherwise, no round-trips are required.
    * - :func:`dpiLob_closeResource()`
      - 1
      -
    * - :func:`dpiLob_copy()`
      - 1
      -
    * - :func:`dpiLob_getBufferSize()`
      - 0
      -
    * - :func:`dpiLob_getChunkSize()`
      - 1
      -
    * - :func:`dpiLob_getDirectoryAndFileName()`
      - 0
      -
    * - :func:`dpiLob_getFileExists()`
      - 1
      -
    * - :func:`dpiLob_getIsResourceOpen()`
      - 1
      -
    * - :func:`dpiLob_getSize()`
      - 1
      -
    * - :func:`dpiLob_openResource()`
      - 1
      -
    * - :func:`dpiLob_readBytes()`
      - 1
      -
    * - :func:`dpiLob_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiLob_setDirectoryAndFileName()`
      - 0
      -
    * - :func:`dpiLob_setFromBytes()`
      - 1
      -
    * - :func:`dpiLob_trim()`
      - 1
      -
    * - :func:`dpiLob_writeBytes()`
      - 1
      -
    * - :func:`dpiMsgProps_addRef()`
      - 0
      -
    * - :func:`dpiMsgProps_getNumAttempts()`
      - 0
      -
    * - :func:`dpiMsgProps_getCorrelation()`
      - 0
      -
    * - :func:`dpiMsgProps_getDelay()`
      - 0
      -
    * - :func:`dpiMsgProps_getDeliveryMode()`
      - 0
      -
    * - :func:`dpiMsgProps_getEnqTime()`
      - 0
      -
    * - :func:`dpiMsgProps_getExceptionQ()`
      - 0
      -
    * - :func:`dpiMsgProps_getExpiration()`
      - 0
      -
    * - :func:`dpiMsgProps_getMsgId()`
      - 0
      -
    * - :func:`dpiMsgProps_getOriginalMsgId()`
      - 0
      -
    * - :func:`dpiMsgProps_getPayload()`
      - 0
      -
    * - :func:`dpiMsgProps_getPriority()`
      - 0
      -
    * - :func:`dpiMsgProps_getState()`
      - 0
      -
    * - :func:`dpiMsgProps_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiMsgProps_setCorrelation()`
      - 0
      -
    * - :func:`dpiMsgProps_setDelay()`
      - 0
      -
    * - :func:`dpiMsgProps_setExceptionQ()`
      - 0
      -
    * - :func:`dpiMsgProps_setExpiration()`
      - 0
      -
    * - :func:`dpiMsgProps_setOriginalMsgId()`
      - 0
      -
    * - :func:`dpiMsgProps_setPayloadBytes()`
      - 0
      -
    * - :func:`dpiMsgProps_setPayloadObject()`
      - 0
      -
    * - :func:`dpiMsgProps_setPriority()`
      - 0
      -
    * - :func:`dpiObject_addRef()`
      - 0
      -
    * - :func:`dpiObject_appendElement()`
      - 0
      -
    * - :func:`dpiObject_copy()`
      - 0
      -
    * - :func:`dpiObject_deleteElementByIndex()`
      - 0
      -
    * - :func:`dpiObject_getAttributeValue()`
      - 0
      -
    * - :func:`dpiObject_getElementExistsByIndex()`
      - 0
      -
    * - :func:`dpiObject_getElementValueByIndex()`
      - 0
      -
    * - :func:`dpiObject_getFirstIndex()`
      - 0
      -
    * - :func:`dpiObject_getLastIndex()`
      - 0
      -
    * - :func:`dpiObject_getNextIndex()`
      - 0
      -
    * - :func:`dpiObject_getPrevIndex()`
      - 0
      -
    * - :func:`dpiObject_getSize()`
      - 0
      -
    * - :func:`dpiObject_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being
        released, the internal reference to the object type is the last
        reference being released and the object type's internal reference to
        the connection is the last reference to that connection. In that case,
        the notes on the function :func:`dpiConn_release()` apply.
    * - :func:`dpiObject_setAttributeValue()`
      - 0
      -
    * - :func:`dpiObject_setElementValueByIndex()`
      - 0
      -
    * - :func:`dpiObject_trim()`
      - 0
      -
    * - :func:`dpiObjectAttr_addRef()`
      - 0
      -
    * - :func:`dpiObjectAttr_getInfo()`
      - 0
      -
    * - :func:`dpiObjectAttr_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being
        released, the internal reference to the object type is the last
        reference being released and the object type's internal reference to
        the connection is the last reference to that connection. In that case,
        the notes on the function :func:`dpiConn_release()` apply.
    * - :func:`dpiObjectType_addRef()`
      - 0
      -
    * - :func:`dpiObjectType_createObject()`
      - 0
      -
    * - :func:`dpiObjectType_getAttributes()`
      - 0
      -
    * - :func:`dpiObjectType_getInfo()`
      - 0
      -
    * - :func:`dpiObjectType_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiPool_acquireConnection()`
      - See notes.
      - No round-trips are required if an unused connection is available in the
        pool; otherwise, a round trip is required for each connection that is
        added to the pool (see :member:`dpiPoolCreateParams.sessionIncrement`).
    * - :func:`dpiPool_addRef()`
      - 0
      -
    * - :func:`dpiPool_close()`
      - See notes.
      - One round trip is required for each session in the pool that needs to
        be closed.
    * - :func:`dpiPool_create()`
      - See notes.
      - One round trip is required for each session that is initially added to
        the pool (see :member:`dpiPoolCreateParams.minSessions`).
    * - :func:`dpiPool_getBusyCount()`
      - 0
      -
    * - :func:`dpiPool_getEncodingInfo()`
      - 0
      -
    * - :func:`dpiPool_getGetMode()`
      - 0
      -
    * - :func:`dpiPool_getMaxLifetimeSession()`
      - 0
      -
    * - :func:`dpiPool_getOpenCount()`
      - 0
      -
    * - :func:`dpiPool_getStmtCacheSize()`
      - 0
      -
    * - :func:`dpiPool_getTimeout()`
      - 0
      -
    * - :func:`dpiPool_getWaitTimeout()`
      - 0
      -
    * - :func:`dpiPool_release()`
      - 0 or 1
      - If the number of references exceeds 1 or the pool has already been
        closed by a call to :func:`dpiPool_close()` then no round-trips are
        required; otherwise, the pool is closed at this point and the notes on
        the function :func:`dpiPool_close()` apply.
    * - :func:`dpiPool_setGetMode()`
      - 0
      -
    * - :func:`dpiPool_setMaxLifetimeSession()`
      - 0
      -
    * - :func:`dpiPool_setStmtCacheSize()`
      - 0
      -
    * - :func:`dpiPool_setTimeout()`
      - 0
      -
    * - :func:`dpiPool_setWaitTimeout()`
      - 0
      -
    * - :func:`dpiQueue_addRef()`
      - 0
      -
    * - :func:`dpiQueue_deqMany()`
      - 1
      -
    * - :func:`dpiQueue_deqOne()`
      - 1
      -
    * - :func:`dpiQueue_enqMany()`
      - 1
      -
    * - :func:`dpiQueue_enqOne()`
      - 1
      -
    * - :func:`dpiQueue_getDeqOptions()`
      - 0
      -
    * - :func:`dpiQueue_getEnqOptions()`
      - 0
      -
    * - :func:`dpiQueue_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiRowid_addRef()`
      - 0
      -
    * - :func:`dpiRowid_getStringValue()`
      - 0
      -
    * - :func:`dpiRowid_release()`
      - 0
      -
    * - :func:`dpiSodaColl_addRef()`
      - 0
      -
    * - :func:`dpiSodaColl_createIndex()`
      - 1
      -
    * - :func:`dpiSodaColl_drop()`
      - 1
      -
    * - :func:`dpiSodaColl_dropIndex()`
      - 1
      -
    * - :func:`dpiSodaColl_find()`
      - 1
      -
    * - :func:`dpiSodaColl_findOne()`
      - 1
      -
    * - :func:`dpiSodaColl_getDataGuide()`
      - 1
      -
    * - :func:`dpiSodaColl_getDocCount()`
      - 1
      -
    * - :func:`dpiSodaColl_getMetadata()`
      - 0
      -
    * - :func:`dpiSodaColl_getName()`
      - 0
      -
    * - :func:`dpiSodaColl_insertMany()`
      - 1
      -
    * - :func:`dpiSodaColl_insertOne()`
      - 1
      -
    * - :func:`dpiSodaColl_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaColl_remove()`
      - 1
      -
    * - :func:`dpiSodaColl_replaceOne()`
      - 1
      -
    * - :func:`dpiSodaCollCursor_addRef()`
      - 0
      -
    * - :func:`dpiSodaCollCursor_close()`
      - 0
      -
    * - :func:`dpiSodaCollCursor_getNext()`
      - 1
      -
    * - :func:`dpiSodaCollCursor_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDb_addRef()`
      - 0
      -
    * - :func:`dpiSodaDb_createCollection()`
      - 1
      -
    * - :func:`dpiSodaDb_createDocument()`
      - 0
      -
    * - :func:`dpiSodaDb_freeCollectionNames()`
      - 0
      -
    * - :func:`dpiSodaDb_getCollections()`
      - 1
      -
    * - :func:`dpiSodaDb_getCollectionNames()`
      - 1
      -
    * - :func:`dpiSodaDb_openCollection()`
      - 1
      -
    * - :func:`dpiSodaDb_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDoc_addRef()`
      - 0
      -
    * - :func:`dpiSodaDoc_getContent()`
      - 0
      -
    * - :func:`dpiSodaDoc_getCreatedOn()`
      - 0
      -
    * - :func:`dpiSodaDoc_getKey()`
      - 0
      -
    * - :func:`dpiSodaDoc_getLastModified()`
      - 0
      -
    * - :func:`dpiSodaDoc_getMediaType()`
      - 0
      -
    * - :func:`dpiSodaDoc_getVersion()`
      - 0
      -
    * - :func:`dpiSodaDoc_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDocCursor_addRef()`
      - 0
      -
    * - :func:`dpiSodaDocCursor_close()`
      - 0
      -
    * - :func:`dpiSodaDocCursor_getNext()`
      - 1
      -
    * - :func:`dpiSodaDocCursor_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiStmt_addRef()`
      - 0
      -
    * - :func:`dpiStmt_bindByName()`
      - 0
      -
    * - :func:`dpiStmt_bindByPos()`
      - 0
      -
    * - :func:`dpiStmt_bindValueByName()`
      - 0
      -
    * - :func:`dpiStmt_bindValueByPos()`
      - 0
      -
    * - :func:`dpiStmt_close()`
      - 0
      -
    * - :func:`dpiStmt_define()`
      - 0
      -
    * - :func:`dpiStmt_defineValue()`
      - 0
      -
    * - :func:`dpiStmt_execute()`
      - 1
      -
    * - :func:`dpiStmt_executeMany()`
      - 1
      -
    * - :func:`dpiStmt_fetch()`
      - 0 or 1
      - An internal array of rows corresponding to the value set by a call to
        :func:`dpiStmt_setFetchArraySize()` is maintained. A round trip is only
        required when this array has been exhausted and new rows are required
        to be fetched from the database.
    * - :func:`dpiStmt_fetchRows()`
      - 0 or 1
      - An internal array of rows corresponding to the value set by a call to
        :func:`dpiStmt_setFetchArraySize()` is maintained. If any rows exist in
        this array, no round trip is required; otherwise, a round trip is
        required.
    * - :func:`dpiStmt_getBatchErrorCount()`
      - 0
      -
    * - :func:`dpiStmt_getBatchErrors()`
      - 0
      -
    * - :func:`dpiStmt_getBindCount()`
      - 0
      -
    * - :func:`dpiStmt_getBindNames()`
      - 0
      -
    * - :func:`dpiStmt_getFetchArraySize()`
      - 0
      -
    * - :func:`dpiStmt_getImplicitResult()`
      - 0
      -
    * - :func:`dpiStmt_getInfo()`
      - 0
      -
    * - :func:`dpiStmt_getNumQueryColumns()`
      - 0
      -
    * - :func:`dpiStmt_getQueryInfo()`
      - 0
      -
    * - :func:`dpiStmt_getQueryValue()`
      - 0
      -
    * - :func:`dpiStmt_getRowCount()`
      - 0
      -
    * - :func:`dpiStmt_getRowCounts()`
      - 0
      -
    * - :func:`dpiStmt_getSubscrQueryId()`
      - 0
      -
    * - :func:`dpiStmt_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiStmt_scroll()`
      - 1
      -
    * - :func:`dpiStmt_setFetchArraySize()`
      - 0
      -
    * - :func:`dpiSubscr_addRef()`
      - 0
      -
    * - :func:`dpiSubscr_prepareStmt()`
      - 0
      -
    * - :func:`dpiSubscr_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiVar_addRef()`
      - 0
      -
    * - :func:`dpiVar_copyData()`
      - 0
      -
    * - :func:`dpiVar_getNumElementsInArray()`
      - 0
      -
    * - :func:`dpiVar_getReturnedData()`
      - 0
      -
    * - :func:`dpiVar_getSizeInBytes()`
      - 0
      -
    * - :func:`dpiVar_release()`
      - 0 or 1
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiVar_setFromBytes()`
      - 0 or 1
      - If the variable refers to a LOB, one round-trip is required; otherwise,
        no round trips are required.
    * - :func:`dpiVar_setFromLob()`
      - 0
      -
    * - :func:`dpiVar_setFromObject()`
      - 0
      -
    * - :func:`dpiVar_setFromRowid()`
      - 0
      -
    * - :func:`dpiVar_setFromStmt()`
      - 0
      -
    * - :func:`dpiVar_setNumElementsInArray()`
      - 0
      -
