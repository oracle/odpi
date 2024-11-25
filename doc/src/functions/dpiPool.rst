.. _dpiPoolFunctions:

ODPI-C Pool Functions
---------------------

Pool handles are used to represent session pools. They are created using the
function :func:`dpiPool_create()` and can be closed by calling the function
:func:`dpiPool_close()` or releasing the last reference to the pool by
calling the function :func:`dpiPool_release()`. Pools can be used to create
connections by calling the function :func:`dpiPool_acquireConnection()`.


.. function:: int dpiPool_acquireConnection(dpiPool* pool, \
        const char* userName, uint32_t userNameLength, \
        const char* password, uint32_t passwordLength, \
        dpiConnCreateParams* params, dpiConn** conn)

    Acquires a connection from the pool and returns a reference to it. This
    reference should be released by calling :func:`dpiConn_release()` as soon
    as it is no longer needed, which will also return the connection back to
    the pool for subsequent calls to this function. The connection can be
    returned back to the pool earlier by calling :func:`dpiConn_close()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - The pool from which a connection is to be acquired. If the
            reference is NULL or invalid, an error is returned.
        * - ``userName``
          - IN
          - The name of the user used for authenticating the user, as a byte
            string in the encoding used for CHAR data. NULL is also acceptable
            if external authentication is being requested or credentials were
            supplied when the pool was created.
        * - ``userNameLength``
          - IN
          - The length of the userName parameter, in bytes, or 0 if the
            userName parameter is NULL.
        * - ``password``
          - IN
          - The password to use for authenticating the user, as a byte string
            in the encoding used for CHAR data. NULL is also acceptable if
            external authentication is being requested or if credentials were
            supplied when the pool was created.
        * - ``passwordLength``
          - IN
          - The length of the password parameter, in bytes, or 0 if the
            password parameter is NULL.
        * - ``params``
          - IN
          - A pointer to a :ref:`dpiConnCreateParams<dpiConnCreateParams>`
            structure which is used to specify parameters for connection
            creation. NULL is also acceptable in which case all default
            parameters will be used when creating the connection.
        * - ``conn``
          - OUT
          - A reference to the connection that is acquired from the pool.
            This value is populated upon successful completion of this
            function.

.. function:: int dpiPool_addRef(dpiPool* pool)

    Adds a reference to the pool. This is intended for situations where a
    reference to the pool needs to be maintained independently of the reference
    returned when the pool was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - The pool to which a reference is to be added. If the reference
            is NULL or invalid, an error is returned.

.. function:: int dpiPool_close(dpiPool* pool, dpiPoolCloseMode closeMode)

    Closes the pool and makes it unusable for further activity.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool which is to be closed. If the reference is
            NULL or invalid, an error is returned.
        * - ``closeMode``
          - IN
          - One or more of the values from the enumeration
            :ref:`dpiPoolCloseMode<dpiPoolCloseMode>`, OR'ed together.

.. function:: int dpiPool_create(const dpiContext* context, \
        const char* userName, uint32_t userNameLength, \
        const char* password, uint32_t passwordLength, \
        const char* connectString, uint32_t connectStringLength, \
        dpiCommonCreateParams* commonParams, \
        dpiPoolCreateParams* createParams, dpiPool** pool)

    Creates a session pool which creates and maintains a group of stateless
    sessions to the database. The main benefit of session pooling is
    performance since making a connection to the database is a time-consuming
    activity, especially when the database is remote.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.
    If a failure occurs, the errorInfo structure is filled in with error
    information.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, an error is returned.
        * - ``userName``
          - IN
          - The name of the user used for authenticating sessions, as a byte
            string in the encoding used for CHAR data. NULL is also acceptable
            if external authentication is being requested or if a heterogeneous
            pool is being created.
        * - ``userNameLength``
          - IN
          - The length of the userName parameter, in bytes, or 0 if the
            userName parameter is NULL.
        * - ``password``
          - IN
          - The password to use for authenticating sessions, as a byte string
            in the encoding used for CHAR data. NULL is also acceptable if
            external authentication is being requested or if a heterogeneous
            pool is being created.
        * - ``passwordLength``
          - IN
          - The length of the password parameter, in bytes, or 0 if the
            password parameter is NULL.
        * - ``connectString``
          - IN
          - The connect string identifying the database to which connections
            are to be established by the session pool, as a byte string in
            the encoding used for CHAR data. NULL is also acceptable for
            local connections (identified by the environment variable
            ORACLE_SID).
        * - ``connectStringLength``
          - IN
          - The length of the connectString parameter, in bytes, or 0 if the
            connectString parameter is NULL.
        * - ``commonParams``
          - IN
          - A pointer to a :ref:`dpiCommonCreateParams<dpiCommonCreateParams>`
            structure which is used to specify context parameters for pool
            creation. NULL is also acceptable in which case all default
            parameters will be used when creating the pool.
        * - ``createParams``
          - IN
          - A pointer to a :ref:`dpiPoolCreateParams<dpiPoolCreateParams>`
            structure which is used to specify parameters for pool creation.
            NULL is also acceptable in which case all default parameters will
            be used for pool creation.
        * - ``pool``
          - OUT
          - A pointer to a reference to the pool that is created. Call
            :func:`dpiPool_release()` when the reference is no longer needed.

.. function:: int dpiPool_getBusyCount(dpiPool* pool, uint32_t* value)

    Returns the number of sessions in the pool that are busy.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the number of busy sessions is
            to be retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getEncodingInfo(dpiPool* pool, \
        dpiEncodingInfo* info)

    Returns the encoding information used by the pool. This will be equivalent
    to the values passed when the pool was created, or the values retrieved
    from the environment variables NLS_LANG and NLS_NCHAR.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool whose encoding information is to be
            retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``info``
          - OUT
          - A pointer to a :ref:`dpiEncodingInfo<dpiEncodingInfo>` structure
            which will be populated with the encoding information used by the
            pool.

.. function:: int dpiPool_getGetMode(dpiPool* pool, dpiPoolGetMode* value)

    Returns the mode used for acquiring or getting connections from the pool.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the mode used for acquiring
            connections is to be retrieved. If the reference is NULL or
            invalid an error is returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getMaxLifetimeSession(dpiPool* pool, \
        uint32_t* value)

    Returns the maximum lifetime a pooled session may exist, in seconds.
    Sessions in use will not be closed. They become candidates for termination
    only when they are released back to the pool and have existed for longer
    then the returned value. Note that termination only occurs when the pool is
    accessed. The value 0 means that there is no maximum length of time that a
    pooled session may exist.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the maximum lifetime of
            sessions is to be retrieved. If the reference is NULL or invalid,
            an error is returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getMaxSessionsPerShard(dpiPool* pool, \
        uint32_t* value)

    Returns the maximum sessions per shard. This parameter is used for
    balancing shards.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the maximum sessoins per shard
            is to be retrieved. If the refernece is NULL or invalid, an error
            is returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getOpenCount(dpiPool* pool, uint32_t* value)

    Returns the number of sessions in the pool that are open.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the number of open sessions is
            to be retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getPingInterval(dpiPool* pool, int* value)

    Returns the ping interval duration (in seconds), which is used to check the
    healthiness of idle connections before getting checked out. A negative
    value indicates this check is disabled.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the ping interval is to be
            retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getSodaMetadataCache(dpiPool* pool, int* enabled)

    Returns whether or not the SODA metadata cache is enabled or not.

    The SODA metadata cache requires Oracle Client 21.3, or later. It is also
    available in Oracle Client 19 from 19.11.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the status of the SODA metadata
            cache is to be retrieved. If the reference is NULL or invalid, an
            error is returned.
        * - ``enabled``
          - OUT
          - A pointer to an integer defining whether the SODA metadata cache
            is enabled (1) or not (0), which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getStmtCacheSize(dpiPool* pool, \
        uint32_t* cacheSize)

    Returns the default size of the statement cache for sessions in the pool,
    in number of statements.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the default size of the
            statement cache is to be retrieved. If the reference is NULL
            or invalid, an error is returned.
        * - ``cacheSize``
          - OUT
          - A pointer to the default size of the statement cache, which will
            be populated upon successful completion of this function.

.. function:: int dpiPool_getTimeout(dpiPool* pool, uint32_t* value)

    Returns the length of time (in seconds) after which idle sessions in the
    pool are terminated. Note that termination only occurs when the pool is
    accessed. A value of 0 means that no ide sessions are terminated.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the timeout for idle sessions
            is to be retrieved. If the reference is NULL or invalid, an error
            is returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_getWaitTimeout(dpiPool* pool, uint32_t* value)

    Returns the amount of time (in milliseconds) that the caller will wait for
    a session to become available in the pool before returning an error.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool from which the wait timeout is to be
            retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the value which will be populated upon successful
            completion of this function.

.. function:: int dpiPool_reconfigure(dpiPool* pool, uint32_t minSessions, \
        uint32_t maxSessions, uint32 sessionIncrement)

    Changes pool configuration corresponding to members
    :member:`dpiPoolCreateParams.minSessions`,
    :member:`dpiPoolCreateParams.maxSessions` and
    :member:`dpiPoolCreateParams.sessionIncrement` to the specified values.
    Connections will be created as needed if the value of `minSessions` is
    increased. Connections will be dropped from the pool as they are released
    back to the pool if `minSessions` is decreased.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool which needs to be reconfigured. If the
            reference is NULL or invalid, an error is returned.
        * - ``minSessions``
          - IN
          - New value for the minimum number of sessions that should be
            maintained.
        * - ``maxSessions``
          - IN
          - New value for the maximum number of sessions that may be retained
            in the pool.
        * - ``sessionIncrement``
          - IN
          - New value for the number of sessions that will be created each
            time the pool needs to be expanded.

.. function:: int dpiPool_release(dpiPool* pool)

    Releases a reference to the pool. A count of the references to the pool is
    maintained and when this count reaches zero, the memory associated with the
    pool is freed and the session pool is closed if that has not already
    taken place using the function :func:`dpiPool_close()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - The pool from which a reference is to be released. If the
            reference is NULL or invalid, an error is returned.

.. function:: int dpiPool_setAccessToken(dpiPool* pool, \
        dpiAccessToken* accessToken)

    The function is used to manually set the token and private key for a
    session pool. After pool creation it can be used to periodically reset
    the token and private key to avoid the need for the pool token callback
    function
    :member:`accessTokenCallback<dpiPoolCreateParams.accessTokenCallback>` to
    be called during pool growth.

    This function may also be useful for testing. By setting an expired token
    and key the subsequent pool callback function behavior can be seen without
    waiting for normal token expiry.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool where authentication parameters will be
            set. If the pool parameter is NULL or invalid, an error is
            returned.
        * - ``accessToken``
          - IN
          - A pointer to a :ref:`dpiAccessToken<dpiAccessToken>` structure
            which is used to specify parameters for token based authentication.
            If the pointer is NULL, an error is returned.

.. function:: int dpiPool_setGetMode(dpiPool* pool, dpiPoolGetMode value)

    Sets the mode used for acquiring or getting connections from the pool.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the mode used for acquiring
            connections is to be set. If the reference is NULL or invalid, an
            error is returned.
        * - ``value``
          - IN
          - The value to set.

.. function:: int dpiPool_setMaxLifetimeSession(dpiPool* pool, uint32_t value)

    Sets the maximum lifetime a pooled session may exist, in seconds. Sessions
    in use will not be closed. They become candidates for termination only when
    they are released back to the pool and have existed for longer then the
    specified value. Note that termination only occurs when the pool is
    accessed. The value 0 means that there is no maximum length of time that a
    pooled session may exist.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the maximum lifetime of sessions
            is to be set. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - IN
          - The value to set.

.. function:: int dpiPool_setMaxSessionsPerShard(dpiPool* pool, uint32_t value)

    Sets the maximum number of sessions per shard.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the maximum sessions per shard is
            to be set. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - IN
          - The value to set.

.. function:: int dpiPool_setPingInterval(dpiPool* pool, int value)

    Sets the ping interval duration (in seconds) which is used to to check for
    healthiness of sessions. If this time has passed since the last time the
    session was checked out a ping will be performed. A negative value will
    disable this check.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the ping interval is to be set.
            If the reference is NULL or invalid, an error is returned.
        * - ``value``
          - IN
          - The value to set.

.. function:: int dpiPool_setSodaMetadataCache(dpiPool* pool, int enabled)

    Sets whether the SODA metadata cache is enabled or not. Enabling the SODA
    metadata cache can significantly improve the performance of repeated calls
    to methods :func:`dpiSodaDb_createCollection()` (when not specifying a
    value for the metadata parameter) and :func:`dpiSodaDb_openCollection()`.
    Note that the cache can become out of date if changes to the metadata of
    cached collections are made externally.

    The SODA metadata cache requires Oracle Client 21.3, or later. It is also
    available in Oracle Client 19 from 19.11.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the SODA metadata cache is to be
            enabled or disabled. If the reference is NULL or invalid, an error
            is returned.
        * - ``enabled``
          - IN
          - Identifies if the cache should be enabled (1) or not (0).

.. function:: int dpiPool_setStmtCacheSize(dpiPool* pool, uint32_t cacheSize)

    Sets the default size of the statement cache for sessions in the pool.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the default size of the statement
            cache is to be set. If the reference is NULL or invalid, an error
            is returned.
        * - ``cacheSize``
          - IN
          - The new size of the statement cache, in number of statements.

.. function:: int dpiPool_setTimeout(dpiPool* pool, uint32_t value)

    Sets the amount of time (in seconds) after which idle sessions in the
    pool are terminated. Note that termination only occurs when the pool is
    accessed. A value of zero will result in no idle sessions being terminated.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the timeout for idle sessions is
            to be set. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - IN
          - The value to set.

.. function:: int dpiPool_setWaitTimeout(dpiPool* pool, uint32_t value)

    Sets the amount of time (in milliseconds) that the caller should wait for a
    session to be available in the pool before returning with an error.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``pool``
          - IN
          - A reference to the pool in which the wait timeout is to be set. If
            the reference is NULL or invalid, an error is returned.
        * - ``value``
          - IN
          - The value to set.
