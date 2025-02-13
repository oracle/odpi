.. _round_trips:

***************************
ODPI-C Function Round-Trips
***************************

The following table identifies if round-trips to the database are required when
calling each of the functions that ODPI-C supports. A round-trip is defined as
the trip from the Oracle Client libraries (used by ODPI-C) to the database and
back.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 10 10 30
    :summary: The first column displays the function. The second column
     indicates if round-trips to the database are required. The third column
     displays the relevant notes.

    * - Function
      - Round-Trips?
      - Notes
    * - :func:`dpiConn_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiConn_breakExecution()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_changePassword()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_close()`
      - Maybe
      - If the connection was acquired from a pool, a round-trip is only
        required if the parameter *mode* is set to the value
        **DPI_MODE_CONN_CLOSE_DROP**. For standalone connections, a round-trip
        is always required. If there is an open transaction, a round-trip is
        also required to perform the implicit rollback that takes place.
    * - :func:`dpiConn_commit()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_create()`
      - Maybe
      - If a standalone connection is being created, one round-trip is
        required. If a connection is being acquired from a pool, no round-trips
        are required if an unused connection is available in the pool;
        otherwise, a round trip is required for each connection that is added
        to the pool (see :member:`dpiPoolCreateParams.sessionIncrement`).
    * - :func:`dpiConn_deqObject()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_enqObject()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_getCallTimeout()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getCurrentSchema()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getDbDomain()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getDbName()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getEdition()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getEncodingInfo()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getExternalName()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getHandle()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getInfo()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getInstanceName()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getInternalName()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getIsHealthy()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getLTXID()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getMaxOpenCursors()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getObjectType()`
      - Maybe
      - If the type information is already available in the Oracle Client
        object cache, only one round trip is required; otherwise, two
        round-trips are required unless the type contains embedded types or is
        a collection. In that case additional round-trips are required to fetch
        the additional type information.
    * - :func:`dpiConn_getOciAttr()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getServerVersion()`
      - Maybe
      - The first call when requesting the release string always requires a
        round-trip. If the release string is not requested and Oracle Client
        20.3 or higher is being used, no round-trip is required. Similarly for
        earlier versions, if the release string is not requested and a pooled
        connection is used that previously had a call to this function made, no
        round-trip is required; otherwise, a round-trip is required. Subsequent
        calls with any client version do not require a round-trip.
    * - :func:`dpiConn_getServiceName()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getSodaDb()`
      - Maybe
      - An internal call to check the server version is made. The notes on the
        function :func:`dpiConn_getServerVersion()` apply.
    * - :func:`dpiConn_getStmtCacheSize()`
      - No
      - No relevant notes
    * - :func:`dpiConn_getTransactionInProgress()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newDeqOptions()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newEnqOptions()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newJson()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newJsonQueue()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newMsgProps()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newQueue()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newTempLob()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_newVar()`
      - No
      - No relevant notes
    * - :func:`dpiConn_newVector()`
      - No
      - No relevant notes
    * - :func:`dpiConn_ping()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_prepareStmt()`
      - No
      - No relevant notes
    * - :func:`dpiConn_release()`
      - Maybe
      - If the number of references exceeds 1 or the connection has already
        been closed by a call to :func:`dpiConn_close()` then no round-trips
        are required; otherwise, the connection is closed at this point and the
        notes on the function :func:`dpiConn_close()` apply.
    * - :func:`dpiConn_rollback()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_setAction()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setCallTimeout()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setClientIdentifier()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setClientInfo()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setCurrentSchema()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setDbOp()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setEcontextId()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setExternalName()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setInternalName()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setModule()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setOciAttr()`
      - No
      - No relevant notes
    * - :func:`dpiConn_setStmtCacheSize()`
      - No
      - No relevant notes
    * - :func:`dpiConn_shutdownDatabase()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_startupDatabase()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_startupDatabaseWithPfile()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_subscribe()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_tpcBegin()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_tpcCommit()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_tpcEnd()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_tpcForget()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_tpcPrepare()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_tpcRollback()`
      - Yes
      - No relevant notes
    * - :func:`dpiConn_unsubscribe()`
      - Yes
      - No relevant notes
    * - :func:`dpiContext_createWithParams()`
      - No
      - No relevant notes
    * - :func:`dpiContext_destroy()`
      - No
      - No relevant notes
    * - :func:`dpiContext_freeStringList()`
      - No
      - No relevant notes
    * - :func:`dpiContext_getClientVersion()`
      - No
      - No relevant notes
    * - :func:`dpiContext_getError()`
      - No
      - No relevant notes
    * - :func:`dpiContext_initCommonCreateParams()`
      - No
      - No relevant notes
    * - :func:`dpiContext_initConnCreateParams()`
      - No
      - No relevant notes
    * - :func:`dpiContext_initPoolCreateParams()`
      - No
      - No relevant notes
    * - :func:`dpiContext_initSodaOperOptions()`
      - No
      - No relevant notes
    * - :func:`dpiContext_initSubscrCreateParams()`
      - No
      - No relevant notes
    * - :func:`dpiData_getBool()`
      - No
      - No relevant notes
    * - :func:`dpiData_getBytes()`
      - No
      - No relevant notes
    * - :func:`dpiData_getDouble()`
      - No
      - No relevant notes
    * - :func:`dpiData_getFloat()`
      - No
      - No relevant notes
    * - :func:`dpiData_getInt64()`
      - No
      - No relevant notes
    * - :func:`dpiData_getIntervalDS()`
      - No
      - No relevant notes
    * - :func:`dpiData_getIntervalYM()`
      - No
      - No relevant notes
    * - :func:`dpiData_getIsNull()`
      - No
      - No relevant notes
    * - :func:`dpiData_getJson()`
      - No
      - No relevant notes
    * - :func:`dpiData_getJsonArray()`
      - No
      - No relevant notes
    * - :func:`dpiData_getJsonObject()`
      - No
      - No relevant notes
    * - :func:`dpiData_getLOB()`
      - No
      - No relevant notes
    * - :func:`dpiData_getObject()`
      - No
      - No relevant notes
    * - :func:`dpiData_getStmt()`
      - No
      - No relevant notes
    * - :func:`dpiData_getTimestamp()`
      - No
      - No relevant notes
    * - :func:`dpiData_getUint64()`
      - No
      - No relevant notes
    * - :func:`dpiData_getVector()`
      - No
      - No relevant notes
    * - :func:`dpiData_setBool()`
      - No
      - No relevant notes
    * - :func:`dpiData_setBytes()`
      - No
      - No relevant notes
    * - :func:`dpiData_setDouble()`
      - No
      - No relevant notes
    * - :func:`dpiData_setFloat()`
      - No
      - No relevant notes
    * - :func:`dpiData_setInt64()`
      - No
      - No relevant notes
    * - :func:`dpiData_setIntervalDS()`
      - No
      - No relevant notes
    * - :func:`dpiData_setIntervalYM()`
      - No
      - No relevant notes
    * - :func:`dpiData_setLOB()`
      - No
      - No relevant notes
    * - :func:`dpiData_setNull()`
      - No
      - No relevant notes
    * - :func:`dpiData_setObject()`
      - No
      - No relevant notes
    * - :func:`dpiData_setStmt()`
      - No
      - No relevant notes
    * - :func:`dpiData_setTimestamp()`
      - No
      - No relevant notes
    * - :func:`dpiData_setUint64()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getCondition()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getConsumerName()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getCorrelation()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getMode()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getMsgId()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getNavigation()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getTransformation()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getVisibility()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_getWait()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiDeqOptions_setCondition()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setConsumerName()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setCorrelation()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setDeliveryMode()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setMode()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setMsgId()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setNavigation()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setTransformation()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setVisibility()`
      - No
      - No relevant notes
    * - :func:`dpiDeqOptions_setWait()`
      - No
      - No relevant notes
    * - :func:`dpiEnqOptions_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiEnqOptions_getTransformation()`
      - No
      - No relevant notes
    * - :func:`dpiEnqOptions_getVisibility()`
      - No
      - No relevant notes
    * - :func:`dpiEnqOptions_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiEnqOptions_setDeliveryMode()`
      - No
      - No relevant notes
    * - :func:`dpiEnqOptions_setTransformation()`
      - No
      - No relevant notes
    * - :func:`dpiEnqOptions_setVisibility()`
      - No
      - No relevant notes
    * - :func:`dpiJson_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiJson_getValue()`
      - Maybe
      - A round trip is needed if the JSON data has not yet been fetched from
        the server.
    * - :func:`dpiJson_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiJson_setFromText()`
      - No
      - No relevant notes
    * - :func:`dpiJson_setValue()`
      - No
      - No relevant notes
    * - :func:`dpiLob_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiLob_close()`
      - Maybe
      - If the LOB is a temporary LOB, one round-trip is required to free the
        temporary LOB; otherwise, no round-trips are required.
    * - :func:`dpiLob_closeResource()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_copy()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_getBufferSize()`
      - No
      - No relevant notes
    * - :func:`dpiLob_getChunkSize()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_getDirectoryAndFileName()`
      - No
      - No relevant notes
    * - :func:`dpiLob_getFileExists()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_getIsResourceOpen()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_getSize()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_getType()`
      - No
      - No relevant notes
    * - :func:`dpiLob_openResource()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_readBytes()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiLob_setDirectoryAndFileName()`
      - No
      - No relevant notes
    * - :func:`dpiLob_setFromBytes()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_trim()`
      - Yes
      - No relevant notes
    * - :func:`dpiLob_writeBytes()`
      - Yes
      - No relevant notes
    * - :func:`dpiMsgProps_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getCorrelation()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getDelay()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getDeliveryMode()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getEnqTime()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getExceptionQ()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getExpiration()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getMsgId()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getNumAttempts()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getOriginalMsgId()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getPayload()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getPayloadJson()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getPriority()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_getState()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiMsgProps_setCorrelation()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setDelay()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setExceptionQ()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setExpiration()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setOriginalMsgId()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setPayloadBytes()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setPayloadJson()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setPayloadObject()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setPriority()`
      - No
      - No relevant notes
    * - :func:`dpiMsgProps_setRecipients()`
      - No
      - No relevant notes
    * - :func:`dpiObject_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiObject_appendElement()`
      - No
      - No relevant notes
    * - :func:`dpiObject_copy()`
      - No
      - No relevant notes
    * - :func:`dpiObject_deleteElementByIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getAttributeValue()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getElementExistsByIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getElementValueByIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getFirstIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getLastIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getNextIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getPrevIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_getSize()`
      - No
      - No relevant notes
    * - :func:`dpiObject_release()`
      - Maybe
      - No round trips are required unless the last reference is being
        released, the internal reference to the object type is the last
        reference being released and the object type's internal reference to
        the connection is the last reference to that connection. In that case,
        the notes on the function :func:`dpiConn_release()` apply.
    * - :func:`dpiObject_setAttributeValue()`
      - No
      - No relevant notes
    * - :func:`dpiObject_setElementValueByIndex()`
      - No
      - No relevant notes
    * - :func:`dpiObject_trim()`
      - No
      - No relevant notes
    * - :func:`dpiObjectAttr_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiObjectAttr_getInfo()`
      - No
      - No relevant notes
    * - :func:`dpiObjectAttr_release()`
      - Maybe
      - No round trips are required unless the last reference is being
        released, the internal reference to the object type is the last
        reference being released and the object type's internal reference to
        the connection is the last reference to that connection. In that case,
        the notes on the function :func:`dpiConn_release()` apply.
    * - :func:`dpiObjectType_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiObjectType_createObject()`
      - No
      - No relevant notes
    * - :func:`dpiObjectType_getAttributes()`
      - No
      - No relevant notes
    * - :func:`dpiObjectType_getInfo()`
      - No
      - No relevant notes
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
      - No relevant notes
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
      - No relevant notes
    * - :func:`dpiPool_getEncodingInfo()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getGetMode()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getMaxLifetimeSession()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getMaxSessionsPerShard()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getOpenCount()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getPingInterval()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getSodaMetadataCache()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getStmtCacheSize()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getTimeout()`
      - No
      - No relevant notes
    * - :func:`dpiPool_getWaitTimeout()`
      - No
      - No relevant notes
    * - :func:`dpiPool_reconfigure()`
      - Maybe
      - If the minimum size of the pool is not being increased, no round-trips
        are required; otherwise, round-trips are required for each session in
        the pool that needs to be added to reach the new minimum.
    * - :func:`dpiPool_release()`
      - Maybe
      - If the number of references exceeds 1 or the pool has already been
        closed by a call to :func:`dpiPool_close()` then no round-trips are
        required; otherwise, the pool is closed at this point and the notes on
        the function :func:`dpiPool_close()` apply.
    * - :func:`dpiPool_setAccessToken()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setGetMode()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setMaxLifetimeSession()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setMaxSessionsPerShard()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setPingInterval()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setSodaMetadataCache()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setStmtCacheSize()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setTimeout()`
      - No
      - No relevant notes
    * - :func:`dpiPool_setWaitTimeout()`
      - No
      - No relevant notes
    * - :func:`dpiQueue_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiQueue_deqMany()`
      - Yes
      - No relevant notes
    * - :func:`dpiQueue_deqOne()`
      - Yes
      - No relevant notes
    * - :func:`dpiQueue_enqMany()`
      - Yes
      - No relevant notes
    * - :func:`dpiQueue_enqOne()`
      - Yes
      - No relevant notes
    * - :func:`dpiQueue_getDeqOptions()`
      - No
      - No relevant notes
    * - :func:`dpiQueue_getEnqOptions()`
      - No
      - No relevant notes
    * - :func:`dpiQueue_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiRowid_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiRowid_getStringValue()`
      - No
      - No relevant notes
    * - :func:`dpiRowid_release()`
      - No
      - No relevant notes
    * - :func:`dpiSodaColl_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiSodaColl_createIndex()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_drop()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_dropIndex()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_find()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_findOne()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_getDataGuide()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_getDocCount()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_getMetadata()`
      - No
      - No relevant notes
    * - :func:`dpiSodaColl_getName()`
      - No
      - No relevant notes
    * - :func:`dpiSodaColl_insertMany()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_insertManyWithOptions()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_insertOne()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_insertOneWithOptions()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_listIndexes()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaColl_remove()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_replaceOne()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_save()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_saveWithOptions()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaColl_truncate()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaCollCursor_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiSodaCollCursor_close()`
      - No
      - No relevant notes
    * - :func:`dpiSodaCollCursor_getNext()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaCollCursor_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDb_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDb_createCollection()`
      - Maybe
      - If the SODA metadata cache has been enabled and no metadata has been
        specified, a round trip is not required after the first time that a
        collection with a particular name is opened.
    * - :func:`dpiSodaDb_createDocument()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDb_createJsonDocument()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDb_getCollections()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaDb_getCollectionNames()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaDb_openCollection()`
      - Maybe
      - If the SODA metadata cache has been enabled, a round trip is not
        required after the first time that a collection with a particular name
        is opened.
    * - :func:`dpiSodaDb_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDoc_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getContent()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getCreatedOn()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getIsJson()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getJsonContent()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getKey()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getLastModified()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getMediaType()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_getVersion()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDoc_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiSodaDocCursor_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDocCursor_close()`
      - No
      - No relevant notes
    * - :func:`dpiSodaDocCursor_getNext()`
      - Yes
      - No relevant notes
    * - :func:`dpiSodaDocCursor_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiStmt_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_bindByName()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_bindByPos()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_bindValueByName()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_bindValueByPos()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_close()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_define()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_defineValue()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_deleteFromCache()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_execute()`
      - Yes
      - No relevant notes
    * - :func:`dpiStmt_executeMany()`
      - Yes
      - No relevant notes
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
      - No relevant notes
    * - :func:`dpiStmt_getBatchErrors()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getBindCount()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getBindNames()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getFetchArraySize()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getImplicitResult()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getInfo()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getLastRowid()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getNumQueryColumns()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getOciAttr()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getPrefetchRows()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getQueryInfo()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getQueryValue()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getRowCount()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getRowCounts()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_getSubscrQueryId()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiStmt_scroll()`
      - Yes
      - No relevant notes
    * - :func:`dpiStmt_setFetchArraySize()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_setOciAttr()`
      - No
      - No relevant notes
    * - :func:`dpiStmt_setPrefetchRows()`
      - No
      - No relevant notes
    * - :func:`dpiSubscr_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiSubscr_prepareStmt()`
      - No
      - No relevant notes
    * - :func:`dpiSubscr_release()`
      - Maybe
      - No round trips are required unless the last reference is being released
        and the internal reference to the connection is also the last reference
        to that connection. In that case, the notes on the function
        :func:`dpiConn_release()` apply.
    * - :func:`dpiVar_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiVar_copyData()`
      - No
      - No relevant notes
    * - :func:`dpiVar_getNumElementsInArray()`
      - No
      - No relevant notes
    * - :func:`dpiVar_getReturnedData()`
      - No
      - No relevant notes
    * - :func:`dpiVar_getSizeInBytes()`
      - No
      - No relevant notes
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
    * - :func:`dpiVar_setFromJson()`
      - No
      - No relevant notes
    * - :func:`dpiVar_setFromLob()`
      - No
      - No relevant notes
    * - :func:`dpiVar_setFromObject()`
      - No
      - No relevant notes
    * - :func:`dpiVar_setFromRowid()`
      - No
      - No relevant notes
    * - :func:`dpiVar_setFromStmt()`
      - No
      - No relevant notes
    * - :func:`dpiVar_setFromVector()`
      - No
      - No relevant notes
    * - :func:`dpiVar_setNumElementsInArray()`
      - No
      - No relevant notes
    * - :func:`dpiVector_addRef()`
      - No
      - No relevant notes
    * - :func:`dpiVector_getValue()`
      - No
      - No relevant notes
    * - :func:`dpiVector_release()`
      - No
      - No relevant notes
    * - :func:`dpiVector_setValue()`
      - No
      - No relevant notes
