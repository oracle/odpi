.. _round_trips:

ODPI-C Function Round-Trips
---------------------------

The following table identifies if round-trips to the database are required when
calling each of the functions that ODPI-C supports.  A round-trip is defined as
the trip from the Oracle Client libraries (used by ODPI-C) to the database and
back.

.. list-table::
    :header-rows: 1
    :widths: 3 2 12

    * - Function
      - Round-Trips?
      - Notes
    * - :func:`dpiConn_addRef()`
      - No
      -
    * - :func:`dpiConn_beginDistribTrans()`
      - Yes
      -
    * - :func:`dpiConn_breakExecution()`
      - Yes
      -
    * - :func:`dpiConn_changePassword()`
      - Yes
      -
    * - :func:`dpiConn_close()`
      - Maybe
      - If the connection was acquired from a pool, a round-trip is only
        required if the parameter *mode* is set to the value
        **DPI_MODE_CONN_CLOSE_DROP**. For standalone connections, a round-trip
        is always required. If there is an open transaction, a round-trip is
        also required to perform the implicit rollback that takes place.
    * - :func:`dpiConn_commit()`
      - Yes
      -
    * - :func:`dpiConn_create()`
      - Maybe
      - If a standalone connection is being created, one round-trip is
        required. If a connection is being acquired from a pool, no round-trips
        are required if an unused connection is available in the pool;
        otherwise, a round trip is required for each connection that is added
        to the pool (see :member:`dpiPoolCreateParams.sessionIncrement`).
    * - :func:`dpiConn_deqObject()`
      - Yes
      -
    * - :func:`dpiConn_enqObject()`
      - Yes
      -
    * - :func:`dpiConn_getCallTimeout()`
      - No
      -
    * - :func:`dpiConn_getCurrentSchema()`
      - No
      -
    * - :func:`dpiConn_getEdition()`
      - No
      -
    * - :func:`dpiConn_getEncodingInfo()`
      - No
      -
    * - :func:`dpiConn_getExternalName()`
      - No
      -
    * - :func:`dpiConn_getHandle()`
      - No
      -
    * - :func:`dpiConn_getInternalName()`
      - No
      -
    * - :func:`dpiConn_getLTXID()`
      - No
      -
    * - :func:`dpiConn_getObjectType()`
      - Maybe
      - If the type information is already available in the Oracle Client
        object cache, only one round trip is required; otherwise, two
        round-trips are required unless the type contains embedded types or is
        a collection. In that case additional round-trips are required to fetch
        the additional type information.
    * - :func:`dpiConn_getServerVersion()`
      - Maybe
      - The first call when requesting the release string always requires a
        round-trip. If the release string is not requested and Oracle Client
        20.3 or higher is being used, no round-trip is required; otherwise, a
        round-trip is required. Subsequent calls with any client version do not
        require a round-trip.
    * - :func:`dpiConn_getSodaDb()`
      - No
      -
    * - :func:`dpiConn_getStmtCacheSize()`
      - No
      -
    * - :func:`dpiConn_newDeqOptions()`
      - No
      -
    * - :func:`dpiConn_newEnqOptions()`
      - No
      -
    * - :func:`dpiConn_newMsgProps()`
      - No
      -
    * - :func:`dpiConn_newQueue()`
      - No
      -
    * - :func:`dpiConn_newTempLob()`
      - Yes
      -
    * - :func:`dpiConn_newVar()`
      - No
      -
    * - :func:`dpiConn_ping()`
      - Yes
      -
    * - :func:`dpiConn_prepareDistribTrans()`
      - Yes
      -
    * - :func:`dpiConn_prepareStmt()`
      - No
      -
    * - :func:`dpiConn_release()`
      - Maybe
      - If the number of references exceeds 1 or the connection has already
        been closed by a call to :func:`dpiConn_close()` then no round-trips
        are required; otherwise, the connection is closed at this point and the
        notes on the function :func:`dpiConn_close()` apply.
    * - :func:`dpiConn_rollback()`
      - Yes
      -
    * - :func:`dpiConn_setAction()`
      - No
      -
    * - :func:`dpiConn_setCallTimeout()`
      - No
      -
    * - :func:`dpiConn_setClientIdentifier()`
      - No
      -
    * - :func:`dpiConn_setClientInfo()`
      - No
      -
    * - :func:`dpiConn_setCurrentSchema()`
      - No
      -
    * - :func:`dpiConn_setDbOp()`
      - No
      -
    * - :func:`dpiConn_setExternalName()`
      - No
      -
    * - :func:`dpiConn_setInternalName()`
      - No
      -
    * - :func:`dpiConn_setModule()`
      - No
      -
    * - :func:`dpiConn_setStmtCacheSize()`
      - No
      -
    * - :func:`dpiConn_shutdownDatabase()`
      - Yes
      -
    * - :func:`dpiConn_startupDatabase()`
      - Yes
      -
    * - :func:`dpiConn_subscribe()`
      - Yes
      -
    * - :func:`dpiConn_unsubscribe()`
      - Yes
      -
    * - :func:`dpiContext_createWithParams()`
      - No
      -
    * - :func:`dpiContext_destroy()`
      - No
      -
    * - :func:`dpiContext_getClientVersion()`
      - No
      -
    * - :func:`dpiContext_getError()`
      - No
      -
    * - :func:`dpiContext_initCommonCreateParams()`
      - No
      -
    * - :func:`dpiContext_initConnCreateParams()`
      - No
      -
    * - :func:`dpiContext_initPoolCreateParams()`
      - No
      -
    * - :func:`dpiContext_initSodaOperOptions()`
      - No
      -
    * - :func:`dpiContext_initSubscrCreateParams()`
      - No
      -
    * - :func:`dpiData_getBool()`
      - No
      -
    * - :func:`dpiData_getBytes()`
      - No
      -
    * - :func:`dpiData_getDouble()`
      - No
      -
    * - :func:`dpiData_getFloat()`
      - No
      -
    * - :func:`dpiData_getInt64()`
      - No
      -
    * - :func:`dpiData_getIntervalDS()`
      - No
      -
    * - :func:`dpiData_getIntervalYM()`
      - No
      -
    * - :func:`dpiData_getIsNull()`
      - No
      -
    * - :func:`dpiData_getLOB()`
      - No
      -
    * - :func:`dpiData_getObject()`
      - No
      -
    * - :func:`dpiData_getStmt()`
      - No
      -
    * - :func:`dpiData_getTimestamp()`
      - No
      -
    * - :func:`dpiData_getUint64()`
      - No
      -
    * - :func:`dpiData_setBool()`
      - No
      -
    * - :func:`dpiData_setBytes()`
      - No
      -
    * - :func:`dpiData_setDouble()`
      - No
      -
    * - :func:`dpiData_setFloat()`
      - No
      -
    * - :func:`dpiData_setInt64()`
      - No
      -
    * - :func:`dpiData_setIntervalDS()`
      - No
      -
    * - :func:`dpiData_setIntervalYM()`
      - No
      -
    * - :func:`dpiData_setLOB()`
      - No
      -
    * - :func:`dpiData_setNull()`
      - No
      -
    * - :func:`dpiData_setObject()`
      - No
      -
    * - :func:`dpiData_setStmt()`
      - No
      -
    * - :func:`dpiData_setTimestamp()`
      - No
      -
    * - :func:`dpiData_setUint64()`
      - No
      -
    * - :func:`dpiDeqOptions_addRef()`
      - No
      -
    * - :func:`dpiDeqOptions_getCondition()`
      - No
      -
    * - :func:`dpiDeqOptions_getConsumerName()`
      - No
      -
    * - :func:`dpiDeqOptions_getCorrelation()`
      - No
      -
    * - :func:`dpiDeqOptions_getMode()`
      - No
      -
    * - :func:`dpiDeqOptions_getMsgId()`
      - No
      -
    * - :func:`dpiDeqOptions_getNavigation()`
      - No
      -
    * - :func:`dpiDeqOptions_getTransformation()`
      - No
      -
    * - :func:`dpiDeqOptions_getVisibility()`
      - No
      -
    * - :func:`dpiDeqOptions_getWait()`
      - No
      -
    * - :func:`dpiDeqOptions_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiDeqOptions_setCondition()`
      - No
      -
    * - :func:`dpiDeqOptions_setConsumerName()`
      - No
      -
    * - :func:`dpiDeqOptions_setCorrelation()`
      - No
      -
    * - :func:`dpiDeqOptions_setDeliveryMode()`
      - No
      -
    * - :func:`dpiDeqOptions_setMode()`
      - No
      -
    * - :func:`dpiDeqOptions_setMsgId()`
      - No
      -
    * - :func:`dpiDeqOptions_setNavigation()`
      - No
      -
    * - :func:`dpiDeqOptions_setTransformation()`
      - No
      -
    * - :func:`dpiDeqOptions_setVisibility()`
      - No
      -
    * - :func:`dpiDeqOptions_setWait()`
      - No
      -
    * - :func:`dpiEnqOptions_addRef()`
      - No
      -
    * - :func:`dpiEnqOptions_getTransformation()`
      - No
      -
    * - :func:`dpiEnqOptions_getVisibility()`
      - No
      -
    * - :func:`dpiEnqOptions_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiEnqOptions_setDeliveryMode()`
      - No
      -
    * - :func:`dpiEnqOptions_setTransformation()`
      - No
      -
    * - :func:`dpiEnqOptions_getVisibility()`
      - No
      -
    * - :func:`dpiLob_addRef()`
      - No
      -
    * - :func:`dpiLob_close()`
      - Maybe
      - If the LOB is a temporary LOB, one round-trip is required to free the
        temporary LOB; otherwise, no round-trips are required.
    * - :func:`dpiLob_closeResource()`
      - Yes
      -
    * - :func:`dpiLob_copy()`
      - Yes
      -
    * - :func:`dpiLob_getBufferSize()`
      - No
      -
    * - :func:`dpiLob_getChunkSize()`
      - Yes
      -
    * - :func:`dpiLob_getDirectoryAndFileName()`
      - No
      -
    * - :func:`dpiLob_getFileExists()`
      - Yes
      -
    * - :func:`dpiLob_getIsResourceOpen()`
      - Yes
      -
    * - :func:`dpiLob_getSize()`
      - Yes
      -
    * - :func:`dpiLob_openResource()`
      - Yes
      -
    * - :func:`dpiLob_readBytes()`
      - Yes
      -
    * - :func:`dpiLob_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiLob_setDirectoryAndFileName()`
      - No
      -
    * - :func:`dpiLob_setFromBytes()`
      - Yes
      -
    * - :func:`dpiLob_trim()`
      - Yes
      -
    * - :func:`dpiLob_writeBytes()`
      - Yes
      -
    * - :func:`dpiMsgProps_addRef()`
      - No
      -
    * - :func:`dpiMsgProps_getNumAttempts()`
      - No
      -
    * - :func:`dpiMsgProps_getCorrelation()`
      - No
      -
    * - :func:`dpiMsgProps_getDelay()`
      - No
      -
    * - :func:`dpiMsgProps_getDeliveryMode()`
      - No
      -
    * - :func:`dpiMsgProps_getEnqTime()`
      - No
      -
    * - :func:`dpiMsgProps_getExceptionQ()`
      - No
      -
    * - :func:`dpiMsgProps_getExpiration()`
      - No
      -
    * - :func:`dpiMsgProps_getMsgId()`
      - No
      -
    * - :func:`dpiMsgProps_getOriginalMsgId()`
      - No
      -
    * - :func:`dpiMsgProps_getPayload()`
      - No
      -
    * - :func:`dpiMsgProps_getPriority()`
      - No
      -
    * - :func:`dpiMsgProps_getState()`
      - No
      -
    * - :func:`dpiMsgProps_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiMsgProps_setCorrelation()`
      - No
      -
    * - :func:`dpiMsgProps_setDelay()`
      - No
      -
    * - :func:`dpiMsgProps_setExceptionQ()`
      - No
      -
    * - :func:`dpiMsgProps_setExpiration()`
      - No
      -
    * - :func:`dpiMsgProps_setOriginalMsgId()`
      - No
      -
    * - :func:`dpiMsgProps_setPayloadBytes()`
      - No
      -
    * - :func:`dpiMsgProps_setPayloadObject()`
      - No
      -
    * - :func:`dpiMsgProps_setPriority()`
      - No
      -
    * - :func:`dpiObject_addRef()`
      - No
      -
    * - :func:`dpiObject_appendElement()`
      - No
      -
    * - :func:`dpiObject_copy()`
      - No
      -
    * - :func:`dpiObject_deleteElementByIndex()`
      - No
      -
    * - :func:`dpiObject_getAttributeValue()`
      - No
      -
    * - :func:`dpiObject_getElementExistsByIndex()`
      - No
      -
    * - :func:`dpiObject_getElementValueByIndex()`
      - No
      -
    * - :func:`dpiObject_getFirstIndex()`
      - No
      -
    * - :func:`dpiObject_getLastIndex()`
      - No
      -
    * - :func:`dpiObject_getNextIndex()`
      - No
      -
    * - :func:`dpiObject_getPrevIndex()`
      - No
      -
    * - :func:`dpiObject_getSize()`
      - No
      -
    * - :func:`dpiObject_release()`
      - Maybe
      - No round trips are required unless the last reference is being
        released, the internal reference to the object type is the last
        reference being released and the object type's internal reference to
        the connection is the last reference to that connection. In that case,
        the notes on the function :func:`dpiConn_release()` apply.
    * - :func:`dpiObject_setAttributeValue()`
      - No
      -
    * - :func:`dpiObject_setElementValueByIndex()`
      - No
      -
    * - :func:`dpiObject_trim()`
      - No
      -
    * - :func:`dpiObjectAttr_addRef()`
      - No
      -
    * - :func:`dpiObjectAttr_getInfo()`
      - No
      -
    * - :func:`dpiObjectAttr_release()`
      - Maybe
      - No round trips are required unless the last reference is being
        released, the internal reference to the object type is the last
        reference being released and the object type's internal reference to
        the connection is the last reference to that connection. In that case,
        the notes on the function :func:`dpiConn_release()` apply.
    * - :func:`dpiObjectType_addRef()`
      - No
      -
    * - :func:`dpiObjectType_createObject()`
      - No
      -
    * - :func:`dpiObjectType_getAttributes()`
      - No
      -
    * - :func:`dpiObjectType_getInfo()`
      - No
      -
    * - :func:`dpiObjectType_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiPool_acquireConnection()`
      - Maybe
      - No round-trips are required if an unused connection is available in the
        pool; otherwise, a round trip is required for each connection that is
        added to the pool (see :member:`dpiPoolCreateParams.sessionIncrement`).
        A round-trip may also be required if a PL/SQL fixup callback is
        specified (see :member:`dpiPoolCreateParams.plsqlFixupCallback`).
    * - :func:`dpiPool_addRef()`
      - No
      -
    * - :func:`dpiPool_close()`
      - Maybe
      - One round trip is required for each session in the pool that needs to
        be closed.
    * - :func:`dpiPool_create()`
      - Maybe
      - One round trip is required for each session that is initially added to
        the pool (see :member:`dpiPoolCreateParams.minSessions`).
    * - :func:`dpiPool_getBusyCount()`
      - No
      -
    * - :func:`dpiPool_getEncodingInfo()`
      - No
      -
    * - :func:`dpiPool_getGetMode()`
      - No
      -
    * - :func:`dpiPool_getMaxLifetimeSession()`
      - No
      -
    * - :func:`dpiPool_getOpenCount()`
      - No
      -
    * - :func:`dpiPool_getStmtCacheSize()`
      - No
      -
    * - :func:`dpiPool_getTimeout()`
      - No
      -
    * - :func:`dpiPool_getWaitTimeout()`
      - No
      -
    * - :func:`dpiPool_release()`
      - Maybe
      - If the number of references exceeds 1 or the pool has already been
        closed by a call to :func:`dpiPool_close()` then no round-trips are
        required; otherwise, the pool is closed at this point and the notes on
        the function :func:`dpiPool_close()` apply.
    * - :func:`dpiPool_setGetMode()`
      - No
      -
    * - :func:`dpiPool_setMaxLifetimeSession()`
      - No
      -
    * - :func:`dpiPool_setStmtCacheSize()`
      - No
      -
    * - :func:`dpiPool_setTimeout()`
      - No
      -
    * - :func:`dpiPool_setWaitTimeout()`
      - No
      -
    * - :func:`dpiQueue_addRef()`
      - No
      -
    * - :func:`dpiQueue_deqMany()`
      - Yes
      -
    * - :func:`dpiQueue_deqOne()`
      - Yes
      -
    * - :func:`dpiQueue_enqMany()`
      - Yes
      -
    * - :func:`dpiQueue_enqOne()`
      - Yes
      -
    * - :func:`dpiQueue_getDeqOptions()`
      - No
      -
    * - :func:`dpiQueue_getEnqOptions()`
      - No
      -
    * - :func:`dpiQueue_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiRowid_addRef()`
      - No
      -
    * - :func:`dpiRowid_getStringValue()`
      - No
      -
    * - :func:`dpiRowid_release()`
      - No
      -
    * - :func:`dpiSodaColl_addRef()`
      - No
      -
    * - :func:`dpiSodaColl_createIndex()`
      - Yes
      -
    * - :func:`dpiSodaColl_drop()`
      - Yes
      -
    * - :func:`dpiSodaColl_dropIndex()`
      - Yes
      -
    * - :func:`dpiSodaColl_find()`
      - Yes
      -
    * - :func:`dpiSodaColl_findOne()`
      - Yes
      -
    * - :func:`dpiSodaColl_getDataGuide()`
      - Yes
      -
    * - :func:`dpiSodaColl_getDocCount()`
      - Yes
      -
    * - :func:`dpiSodaColl_getMetadata()`
      - No
      -
    * - :func:`dpiSodaColl_getName()`
      - No
      -
    * - :func:`dpiSodaColl_insertMany()`
      - Yes
      -
    * - :func:`dpiSodaColl_insertOne()`
      - Yes
      -
    * - :func:`dpiSodaColl_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaColl_remove()`
      - Yes
      -
    * - :func:`dpiSodaColl_replaceOne()`
      - Yes
      -
    * - :func:`dpiSodaCollCursor_addRef()`
      - No
      -
    * - :func:`dpiSodaCollCursor_close()`
      - No
      -
    * - :func:`dpiSodaCollCursor_getNext()`
      - Yes
      -
    * - :func:`dpiSodaCollCursor_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDb_addRef()`
      - No
      -
    * - :func:`dpiSodaDb_createCollection()`
      - Yes
      -
    * - :func:`dpiSodaDb_createDocument()`
      - No
      -
    * - :func:`dpiSodaDb_freeCollectionNames()`
      - No
      -
    * - :func:`dpiSodaDb_getCollections()`
      - Yes
      -
    * - :func:`dpiSodaDb_getCollectionNames()`
      - Yes
      -
    * - :func:`dpiSodaDb_openCollection()`
      - Yes
      -
    * - :func:`dpiSodaDb_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDoc_addRef()`
      - No
      -
    * - :func:`dpiSodaDoc_getContent()`
      - No
      -
    * - :func:`dpiSodaDoc_getCreatedOn()`
      - No
      -
    * - :func:`dpiSodaDoc_getKey()`
      - No
      -
    * - :func:`dpiSodaDoc_getLastModified()`
      - No
      -
    * - :func:`dpiSodaDoc_getMediaType()`
      - No
      -
    * - :func:`dpiSodaDoc_getVersion()`
      - No
      -
    * - :func:`dpiSodaDoc_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDocCursor_addRef()`
      - No
      -
    * - :func:`dpiSodaDocCursor_close()`
      - No
      -
    * - :func:`dpiSodaDocCursor_getNext()`
      - Yes
      -
    * - :func:`dpiSodaDocCursor_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiStmt_addRef()`
      - No
      -
    * - :func:`dpiStmt_bindByName()`
      - No
      -
    * - :func:`dpiStmt_bindByPos()`
      - No
      -
    * - :func:`dpiStmt_bindValueByName()`
      - No
      -
    * - :func:`dpiStmt_bindValueByPos()`
      - No
      -
    * - :func:`dpiStmt_close()`
      - No
      -
    * - :func:`dpiStmt_define()`
      - No
      -
    * - :func:`dpiStmt_defineValue()`
      - No
      -
    * - :func:`dpiStmt_execute()`
      - Yes
      -
    * - :func:`dpiStmt_executeMany()`
      - Yes
      -
    * - :func:`dpiStmt_fetch()`
      - Maybe
      - An internal array of rows corresponding to the value set by a call to
        :func:`dpiStmt_setFetchArraySize()` is maintained. A round trip is only
        required when this array has been exhausted and new rows are required
        to be fetched from the database.
    * - :func:`dpiStmt_fetchRows()`
      - Maybe
      - An internal array of rows corresponding to the value set by a call to
        :func:`dpiStmt_setFetchArraySize()` is maintained. If any rows exist in
        this array, no round trip is required; otherwise, a round trip is
        required.
    * - :func:`dpiStmt_getBatchErrorCount()`
      - No
      -
    * - :func:`dpiStmt_getBatchErrors()`
      - No
      -
    * - :func:`dpiStmt_getBindCount()`
      - No
      -
    * - :func:`dpiStmt_getBindNames()`
      - No
      -
    * - :func:`dpiStmt_getFetchArraySize()`
      - No
      -
    * - :func:`dpiStmt_getImplicitResult()`
      - No
      -
    * - :func:`dpiStmt_getInfo()`
      - No
      -
    * - :func:`dpiStmt_getNumQueryColumns()`
      - No
      -
    * - :func:`dpiStmt_getQueryInfo()`
      - No
      -
    * - :func:`dpiStmt_getQueryValue()`
      - No
      -
    * - :func:`dpiStmt_getRowCount()`
      - No
      -
    * - :func:`dpiStmt_getRowCounts()`
      - No
      -
    * - :func:`dpiStmt_getSubscrQueryId()`
      - No
      -
    * - :func:`dpiStmt_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiStmt_scroll()`
      - Yes
      -
    * - :func:`dpiStmt_setFetchArraySize()`
      - No
      -
    * - :func:`dpiSubscr_addRef()`
      - No
      -
    * - :func:`dpiSubscr_prepareStmt()`
      - No
      -
    * - :func:`dpiSubscr_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiVar_addRef()`
      - No
      -
    * - :func:`dpiVar_copyData()`
      - No
      -
    * - :func:`dpiVar_getNumElementsInArray()`
      - No
      -
    * - :func:`dpiVar_getReturnedData()`
      - No
      -
    * - :func:`dpiVar_getSizeInBytes()`
      - No
      -
    * - :func:`dpiVar_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiVar_setFromBytes()`
      - Maybe
      - If the variable refers to a LOB, one round-trip is required; otherwise,
        no round trips are required.
    * - :func:`dpiVar_setFromLob()`
      - No
      -
    * - :func:`dpiVar_setFromObject()`
      - No
      -
    * - :func:`dpiVar_setFromRowid()`
      - No
      -
    * - :func:`dpiVar_setFromStmt()`
      - No
      -
    * - :func:`dpiVar_setNumElementsInArray()`
      - No
      -
