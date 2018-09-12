.. _dpiPoolFunctions:

ODPI-C Pool Functions
---------------------

Pool handles are used to represent session pools. They are created using the
function :func:`dpiPool_create()` and can be closed by calling the function
:func:`dpiPool_close()` or releasing the last reference to the pool by
calling the function :func:`dpiPool_release()`. Pools can be used to create
connections by calling the function :func:`dpiPool_acquireConnection()`.


.. function:: int dpiPool_acquireConnection(dpiPool \*pool, \
        const char \*userName, uint32_t userNameLength, \
        const char \*password, uint32_t passwordLength, \
        dpiConnCreateParams \*params, dpiConn \**conn)

    Acquires a connection from the pool and returns a reference to it. This
    reference should be released as soon as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- the pool from which a connection is to be acquired. If the
    reference is NULL or invalid an error is returned.

    **userName** [IN] -- the name of the user used for authenticating the user,
    as a byte string in the encoding used for CHAR data. NULL is also
    acceptable if external authentication is being requested or credentials
    were supplied when the pool was created.

    **userNameLength** [IN] -- the length of the userName parameter, in bytes,
    or 0 if the userName parameter is NULL.

    **password** [IN] -- the password to use for authenticating the user, as a
    byte string in the encoding used for CHAR data. NULL is also acceptable if
    external authentication is being requested or if credentials were supplied
    when the pool was created.

    **passwordLength** [IN] -- the length of the password parameter, in bytes,
    or 0 if the password parameter is NULL.

    **params** [IN] -- a pointer to a
    :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure which is used to
    specify parameters for connection creation. NULL is also acceptable in
    which case all default parameters will be used when creating the
    connection.

    **conn** [OUT] -- a reference to the connection that is acquired from the
    pool. This value is populated upon successful completion of this function.


.. function:: int dpiPool_addRef(dpiPool \*pool)

    Adds a reference to the pool. This is intended for situations where a
    reference to the pool needs to be maintained independently of the reference
    returned when the pool was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- the pool to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiPool_close(dpiPool \*pool, dpiPoolCloseMode closeMode)

    Closes the pool and makes it unusable for further activity.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool which is to be closed. If the
    reference is NULL or invalid an error is returned.

    **closeMode** [IN] -- one or more of the values from the enumeration
    :ref:`dpiPoolCloseMode<dpiPoolCloseMode>`, OR'ed together.


.. function:: int dpiPool_create(const dpiContext \*context, \
        const char \*userName, uint32_t userNameLength, \
        const char \*password, uint32_t passwordLength, \
        const char \*connectString, uint32_t connectStringLength, \
        dpiCommonCreateParams \*commonParams, \
        dpiPoolCreateParams \*createParams, dpiPool \**pool)

    Creates a session pool which creates and maintains a group of stateless
    sessions to the database. The main benefit of session pooling is
    performance since making a connection to the database is a time-consuming
    activity, especially when the database is remote.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.
    If a failure occurs, the errorInfo structure is filled in with error
    information.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **userName** [IN] -- the name of the user used for authenticating sessions,
    as a byte string in the encoding used for CHAR data. NULL is also
    acceptable if external authentication is being requested or if a
    heterogeneous pool is being created.

    **userNameLength** [IN] -- the length of the userName parameter, in bytes,
    or 0 if the userName parameter is NULL.

    **password** [IN] -- the password to use for authenticating sessions, as a
    byte string in the encoding used for CHAR data. NULL is also acceptable if
    external authentication is being requested or if a heterogeneous pool is
    being created.

    **passwordLength** [IN] -- the length of the password parameter, in bytes,
    or 0 if the password parameter is NULL.

    **connectString** [IN] -- the connect string identifying the database to
    which connections are to be established by the session pool, as a byte
    string in the encoding used for CHAR data. NULL is also acceptable for
    local connections (identified by the environment variable ORACLE_SID).

    **connectStringLength** [IN] -- the length of the connectString parameter,
    in bytes, or 0 if the connectString parameter is NULL.

    **commonParams** [IN] -- a pointer to a
    :ref:`dpiCommonCreateParams<dpiCommonCreateParams>` structure which is used
    to specify context parameters for pool creation. NULL is also acceptable in
    which case all default parameters will be used when creating the pool.

    **createParams** [IN] -- a pointer to a
    :ref:`dpiPoolCreateParams<dpiPoolCreateParams>` structure which is used to
    specify parameters for pool creation. NULL is also acceptable in which case
    all default parameters will be used for pool creation.

    **pool** [OUT] -- a pointer to a reference to the pool that is created.
    Call :func:`dpiPool_release()` when the reference is no longer needed.


.. function:: int dpiPool_getBusyCount(dpiPool \*pool, uint32_t \*value)

    Returns the number of sessions in the pool that are busy.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool from which the number of busy
    sessions is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value which will be populated upon
    successful completion of this function.


.. function:: int dpiPool_getEncodingInfo(dpiPool \*pool, \
        dpiEncodingInfo \*info)

    Returns the encoding information used by the pool. This will be equivalent
    to the values passed when the pool was created, or the values retrieved
    from the environment variables NLS_LANG and NLS_NCHAR.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool whose encoding information is to
    be retrieved. If the reference is NULL or invalid an error is returned.

    **info** [OUT] -- a pointer to a :ref:`dpiEncodingInfo<dpiEncodingInfo>`
    structure which will be populated with the encoding information used by the
    pool.


.. function:: int dpiPool_getGetMode(dpiPool \*pool, dpiPoolGetMode \*value)

    Returns the mode used for acquiring or getting connections from the pool.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool from which the mode used for
    acquiring connections is to be retrieved. If the reference is NULL or
    invalid an error is returned.

    **value** [OUT] -- a pointer to the value which will be populated upon
    successful completion of this function.


.. function:: int dpiPool_getMaxLifetimeSession(dpiPool \*pool, \
        uint32_t \*value)

    Returns the maximum lifetime a pooled session may exist, in seconds.
    Sessions in use will not be closed. They become candidates for termination
    only when they are released back to the pool and have existed for longer
    then the returned value. Note that termination only occurs when the pool is
    accessed. The value 0 means that there is no maximum length of time that a
    pooled session may exist.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool from which the maximum lifetime of
    sessions is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value which will be populated upon
    successful completion of this function.


.. function:: int dpiPool_getOpenCount(dpiPool \*pool, uint32_t \*value)

    Returns the number of sessions in the pool that are open.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool from which the number of open
    sessions is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value which will be populated upon
    successful completion of this function.


.. function:: int dpiPool_getStmtCacheSize(dpiPool \*pool, \
        uint32_t \*cacheSize)

    Returns the default size of the statement cache for sessions in the pool,
    in number of statements.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool from which the default size of the
    statement cache is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **cacheSize** [OUT] -- a pointer to the default size of the statement
    cache, which will be populated upon successful completion of this function.


.. function:: int dpiPool_getTimeout(dpiPool \*pool, uint32_t \*value)

    Returns the length of time (in seconds) after which idle sessions in the
    pool are terminated. Note that termination only occurs when the pool is
    accessed. A value of 0 means that no ide sessions are terminated.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool from which the timeout for idle
    sessions is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value which will be populated upon
    successful completion of this function.


.. function:: int dpiPool_getWaitTimeout(dpiPool \*pool, uint32_t \*value)

    Returns the amount of time (in milliseconds) that the caller will wait for
    a session to become available in the pool before returning an error.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool from which the wait timeout is to
    be retrieved. If the reference is NULL or invalid an error is returned.

    **value** [OUT] -- a pointer to the value which will be populated upon
    successful completion of this function.


.. function:: int dpiPool_release(dpiPool \*pool)

    Releases a reference to the pool. A count of the references to the pool is
    maintained and when this count reaches zero, the memory associated with the
    pool is freed and the session pool is closed if that has not already
    taken place using the function :func:`dpiPool_close()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- the pool from which a reference is to be released. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiPool_setGetMode(dpiPool \*pool, dpiPoolGetMode value)

    Sets the mode used for acquiring or getting connections from the pool.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool in which the mode used for
    acquiring connections is to be set. If the reference is NULL or invalid an
    error is returned.

    **value** [IN] -- the value to set.


.. function:: int dpiPool_setMaxLifetimeSession(dpiPool \*pool, uint32_t value)

    Sets the maximum lifetime a pooled session may exist, in seconds. Sessions
    in use will not be closed. They become candidates for termination only when
    they are released back to the pool and have existed for longer then the
    specified value. Note that termination only occurs when the pool is
    accessed. The value 0 means that there is no maximum length of time that a
    pooled session may exist.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool in which the maximum lifetime of
    sessions is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the value to set.


.. function:: int dpiPool_setStmtCacheSize(dpiPool \*pool, uint32_t cacheSize)

    Sets the default size of the statement cache for sessions in the pool.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool in which the default size of the
    statement cache is to be set. If the reference is NULL or invalid an error
    is returned.

    **cacheSize** [IN] -- the new size of the statement cache, in number of
    statements.


.. function:: int dpiPool_setTimeout(dpiPool \*pool, uint32_t value)

    Sets the amount of time (in seconds) after which idle sessions in the
    pool are terminated. Note that termination only occurs when the pool is
    accessed. A value of zero will result in no idle sessions being terminated.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool in which the timeout for idle
    sessions is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the value to set.


.. function:: int dpiPool_setWaitTimeout(dpiPool \*pool, uint32_t value)

    Sets the amount of time (in milliseconds) that the caller should wait for a
    session to be available in the pool before returning with an error.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **pool** [IN] -- a reference to the pool in which the wait timeout is to be
    set. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- the value to set.

