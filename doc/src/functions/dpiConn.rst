.. _dpiConnFunctions:

ODPI-C Connection Functions
---------------------------

Connection handles are used to represent connections to the database. These can
be standalone connections created by calling the function
:func:`dpiConn_create()` or acquired from a session pool by calling the
function :func:`dpiPool_acquireConnection()`. They can be closed by calling the
function :func:`dpiConn_close()` or releasing the last reference to the
connection by calling the function :func:`dpiConn_release()`. Connection
handles are used to create all handles other than session pools and context
handles.

.. function:: int dpiConn_addRef(dpiConn \*conn)

    Adds a reference to the connection. This is intended for situations where a
    reference to the connection needs to be maintained independently of the
    reference returned when the connection was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- the connection to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiConn_beginDistribTrans(dpiConn \*conn, long formatId, \
        const char \*transactionId, uint32_t transactionIdLength, \
        const char \*branchId, uint32_t branchIdLength)

    Begins a distributed transaction using the specified transaction id (XID)
    made up of the formatId, transactionId and branchId.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection which is to be a part of the
    distributed transaction. If the reference is NULL or invalid an error is
    returned.

    **formatId** [IN] -- the identifier of the format of the XID. A value of -1
    indicates that the entire XID is null.

    **transactionId** [IN] -- the global transaction id of the XID as a byte
    string. The maximum length permitted is 64 bytes.

    **transactionIdLength** [IN] -- the length of the global transaction id, in
    bytes.

    **branchId** [IN] -- the branch id of the XID as a byte string. The maximum
    length permitted is 64 bytes.

    **branchIdLength** [IN] -- the length of the branch id, in bytes.


.. function:: int dpiConn_breakExecution(dpiConn \*conn)

    Performs an immediate (asynchronous) termination of any currently executing
    function on the server associated with the connection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection on which the break is to
    take place. If the reference is NULL or invalid an error is returned.


.. function:: int dpiConn_changePassword(dpiConn \*conn, \
        const char \*userName, uint32_t userNameLength, \
        const char \*oldPassword, uint32_t oldPasswordLength, \
        const char \*newPassword, uint32_t newPasswordLength)

    Changes the password of the specified user.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection on which the password is to
    be changed. If the reference is NULL or invalid an error is returned.

    **userName** [IN] -- the name of the user whose password is to be changed,
    as a byte string in the encoding used for CHAR data.

    **userNameLength** [IN] -- the length of the user name parameter, in bytes.

    **oldPassword** [IN] -- the old password of the user whose password is to
    be changed, as a byte string in the encoding used for CHAR data.

    **oldPasswordLength** [IN] -- the length of the old password parameter, in
    bytes.

    **newPassword** [IN] -- the new password of the user whose password is to
    be changed, as a byte string in the encoding used for CHAR data.

    **newPasswordLength** [IN] -- the length of the new password parameter, in
    bytes.


.. function:: int dpiConn_close(dpiConn \*conn, dpiConnCloseMode mode, \
        const char \*tag, uint32_t tagLength)

    Closes the connection and makes it unusable for further activity. Any open
    statements and LOBs associated with the connection will also be closed and
    made unusable for further activity.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection which is to be closed. If
    the reference is NULL or invalid an error is returned.

    **mode** [IN] -- one or more of the values from the enumeration
    :ref:`dpiConnCloseMode<dpiConnCloseMode>`, OR'ed together.

    **tag** [IN] -- a byte string in the encoding used for CHAR data,
    indicating what tag should be set on the connection when it is released
    back to the pool. NULL is also acceptable when indicating that the tag
    should be cleared. This value is ignored unless the close mode includes the
    value DPI_MODE_CONN_CLOSE_RETAG.

    **tagLength** [IN] -- the length of the tag parameter, in bytes, or
    0 if the tag parameter is NULL.


.. function:: int dpiConn_commit(dpiConn \*conn)

    Commits the current active transaction.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection which holds the transaction
    which is to be committed. If the reference is NULL or invalid an error is
    returned.


.. function:: int dpiConn_create(const dpiContext \*context, \
        const char \*userName, uint32_t userNameLength, \
        const char \*password, uint32_t passwordLength, \
        const char \*connectString, uint32_t connectStringLength, \
        dpiCommonCreateParams \*commonParams, \
        dpiConnCreateParams \*createParams, dpiConn \**conn)

    Creates a standalone connection to a database or acquires a connection
    from a session pool and returns a reference to the connection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.
    If a failure occurs, the errorInfo structure is filled in with error
    information.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **userName** [IN] -- the name of the user used for authenticating the user,
    as a byte string in the encoding used for CHAR data. NULL is also
    acceptable if external authentication is being requested or if credentials
    were specified when the pool was created.

    **userNameLength** [IN] -- the length of the userName parameter, in bytes,
    or 0 if the userName parameter is NULL.

    **password** [IN] -- the password to use for authenticating the user, as a
    byte string in the encoding used for CHAR data. NULL is also acceptable if
    external authentication is being requested or if credentials were specified
    when the pool was created.

    **passwordLength** [IN] -- the length of the password parameter, in bytes,
    or 0 if the password parameter is NULL.

    **connectString** [IN] -- the connect string identifying the database to
    which a connection is to be established, as a byte string in the encoding
    used for CHAR data. NULL is also acceptable for local connections
    (identified by the environment variable $ORACLE_SID) or when a connection
    is being acquired from a session pool. This value is ignored when a
    connection is being acquired from a session pool.

    **connectStringLength** [IN] -- the length of the connectString parameter,
    in bytes, or 0 if the connectString parameter is NULL.

    **commonParams** [IN] -- a pointer to a
    :ref:`dpiCommonCreateParams<dpiCommonCreateParams>` structure which is used
    to specify context parameters for connection creation. NULL is also
    acceptable in which case all default parameters will be used when creating
    the connection. This value is ignored when acquiring a connection from a
    session pool.

    **createParams** [IN] -- a pointer to a
    :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure which is used to
    specify parameters for connection creation. NULL is also acceptable in
    which case all default parameters will be used when creating the
    connection.

    **conn** [OUT] -- a pointer to a reference to the connection that is
    created. Call :func:`dpiConn_release()` when the reference is no longer
    needed.


.. function:: int dpiConn_deqObject(dpiConn \*conn, const char \*queueName, \
        uint32_t queueNameLength, dpiDeqOptions \*options, \
        dpiMsgProps \*props, dpiObject \*payload, const char \**msgId, \
        uint32_t \*msgIdLength)

    Dequeues a message from a queue.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the message is to
    be dequeued. If the reference is NULL or invalid an error is returned.

    **queueName** [IN] -- the name of the queue from which the message is to be
    dequeued, as a byte string in the encoding used for CHAR data.

    **queueNameLength** [IN] -- the length of the queueName parameter, in
    bytes.

    **options** [IN] -- a reference to the dequeue options that should be used
    when dequeuing the message from the queue.

    **props** [IN] -- a reference to the message properties that will be
    populated with information from the message that is dequeued.

    **payload** [IN] -- a reference to the object which will be populated with
    the message that is dequeued.

    **msgId** -- [OUT] a pointer to a byte string which will be populated with
    the id of the message that is dequeued, or NULL if no message is available.
    If there is a message id, the pointer will remain valid until the next call
    to :func:`dpiConn_enqObject()` or :func:`dpiConn_deqObject()`.

    **msgIdLength** [OUT] -- a pointer to the length of the msgId parameter, or
    0 if the msgId parameter is NULL.


.. function:: int dpiConn_enqObject(dpiConn \*conn, const char \*queueName, \
        uint32_t queueNameLength, dpiEnqOptions \*options, \
        dpiMsgProps \*props, dpiObject \*payload, const char \**msgId, \
        uint32_t \*msgIdLength)

    Enqueues a message to a queue.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection to which the message is to
    be enqueued. If the reference is NULL or invalid an error is returned.

    **queueName** [IN] -- the name of the queue to which the message is to be
    enqueued, as a byte string in the encoding used for CHAR data.

    **queueNameLength** [IN] -- the length of the queueName parameter, in
    bytes.

    **options** [IN] -- a reference to the enqueue options that should be used
    when enqueuing the message to the queue.

    **props** [IN] -- a reference to the message properties that will affect
    the message that is enqueued.

    **payload** [IN] -- a reference to the object which will be enqueued.

    **msgId** [OUT] -- a pointer to a byte string which will be populated with
    the id of the message that is enqueued upon successful completion of this
    function. The pointer will remain valid until the next call to
    :func:`dpiConn_enqObject()` or :func:`dpiConn_deqObject()`.

    **msgIdLength** [OUT] -- a pointer to the length of the msgId parameter
    which will be populated upon successful completion of this function.


.. function:: int dpiConn_getCallTimeout(dpiConn \*conn, uint32_t \*value)

    Returns the current call timeout (in milliseconds) used for round-trips to
    the database made with this connection. A value of 0 means that no timeouts
    will take place. This value can be set using the function
    :func:`dpiConn_setCallTimeout()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the current
    call timeout is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the call timeout value, which will be
    populated upon successful completion of this function.


.. function:: int dpiConn_getCurrentSchema(dpiConn \*conn, \
        const char \**value, uint32_t \*valueLength)

    Returns the current schema that is being used by the connection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the current
    schema is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **value** [OUT] -- a pointer to the current schema, as a byte string in the
    encoding used for CHAR data, which will be populated upon successful
    completion of this function. The string returned will remain valid as long
    as a reference to the connection is held and the current schema is not
    changed by some means.

    **valueLength** [OUT] -- a pointer to the length of the current schema, in
    bytes, which will be populated upon successful completion of this function.


.. function:: int dpiConn_getEdition(dpiConn \*conn, const char \**value, \
        uint32_t \*valueLength)

    Returns the edition that is being used by the connection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the edition is to
    be retrieved. If the reference is NULL or invalid an error is returned.

    **value** [OUT] -- a pointer to the edition, as a byte string in the
    encoding used for CHAR data, which will be populated upon successful
    completion of this function. The string returned will remain valid as long
    as a reference to the connection is held and the edition is not changed by
    some means.

    **valueLength** [OUT] -- a pointer to the length of the edition, in bytes,
    which will be populated upon successful completion of this function.


.. function:: int dpiConn_getEncodingInfo(dpiConn \*conn, \
        dpiEncodingInfo \*info)

    Returns the encoding information used by the connection. This will be
    equivalent to the values passed when the standalone connection or session
    pool was created, or the values retrieved from the environment variables
    NLS_LANG and NLS_NCHAR.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection whose encoding information
    is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **info** [OUT] -- a pointer to a :ref:`dpiEncodingInfo<dpiEncodingInfo>`
    structure which will be populated with the encoding information used by the
    connection.


.. function:: int dpiConn_getExternalName(dpiConn \*conn, \
        const char \**value, uint32_t \*valueLength)

    Returns the external name that is being used by the connection. This value
    is used when logging distributed transactions.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the external name
    is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **value** [OUT] -- a pointer to the external name, as a byte string in the
    encoding used for CHAR data, which will be populated upon successful
    completion of this function. The string returned will remain valid as long
    as a reference to the connection is held and the external name is not
    changed by some means.

    **valueLength** [OUT] -- a pointer to the length of the external name, in
    bytes, which will be populated upon successful completion of this function.


.. function:: int dpiConn_getHandle(dpiConn \*conn, void \**handle)

    Returns the OCI service context handle in use by the connection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection whose service context handle
    is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **handle** [OUT] -- a pointer which will be populated with the service
    context handle of the connection upon successful completion of the
    function. This handle can be used within OCI calls independently of the
    library, but care must be taken not to cause problems due to shared use.


.. function:: int dpiConn_getInternalName(dpiConn \*conn, \
        const char \**value, uint32_t \*valueLength)

    Returns the internal name that is being used by the connection. This value
    is used when logging distributed transactions.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the internal name
    is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **value** [OUT] -- a pointer to the internal name, as a byte string in the
    encoding used for CHAR data, which will be populated upon successful
    completion of this function. The string returned will remain valid as long
    as a reference to the connection is held and the internal name is not
    changed by some means.

    **valueLength** [OUT] -- a pointer to the length of the internal name, in
    bytes, which will be populated upon successful completion of this function.


.. function:: int dpiConn_getLTXID(dpiConn \*conn, const char \**value, \
        uint32_t \*valueLength)

    Returns the logical transaction id for the connection. This value is used
    in Transaction Guard to determine if the last failed call was completed and
    if the transaction was committed using the procedure call
    dbms_app_cont.get_ltxid_outcome().

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the logical
    transaction id is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the logical transaction id, as a byte
    string, which will be populated upon successful completion of this
    function. The bytes returned will remain valid as long as a reference to
    the connection is held and the logical transaction id is not changed by
    some means.

    **valueLength** [OUT] -- a pointer to the length of the logical transaction
    id, in bytes, which will be populated upon successful completion of this
    function.


.. function:: int dpiConn_getObjectType(dpiConn \*conn, const char \*name, \
        uint32_t nameLength, dpiObjectType \**objType)

    Looks up an object type by name in the database and returns a reference to
    it. The reference should be released as soon as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection which contains the object
    type to look up. If the reference is NULL or invalid an error is returned.

    **name** [IN] -- the name of the object type to lookup, as a byte string in
    the encoding used for CHAR data.

    **nameLength** [IN] -- the length of the name parameter, in bytes.

    **objType** [OUT] -- a pointer to a reference to the object type, which
    will be populated upon successfully locating the object type.


.. function:: int dpiConn_getServerVersion(dpiConn \*conn, \
        const char \**releaseString, uint32_t \*releaseStringLength, \
        dpiVersionInfo \*versionInfo)

    Returns the version information of the Oracle Database to which the
    connection has been made.

    NOTE: if you connect to Oracle Database 18 or higher with client libraries
    12.2 or lower that you will only receive the base version (such as
    18.0.0.0.0) instead of the full version (such as 18.3.0.0.0).

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the server
    version information is to be retrieved. If the reference is NULL or invalid
    an error is returned.

    **releaseString** [OUT] -- a pointer to the release string which will be
    populated when this function returns successfully. The string remains
    valid as long as a reference is held to the connection.

    **releaseStringLength** [OUT] -- a pointer to the length of the release
    string which will be populated when this function returns successfully.

    **versionInfo** [OUT] -- a pointer to a
    :ref:`dpiVersionInfo<dpiVersionInfo>` structure which will be populated
    with the version information of the Oracle Database to which the connection
    has been made.


.. function:: int dpiConn_getSodaDb(dpiConn \*conn, dpiSodaDb \**db)

    Return a reference to a SODA database which can be used to create, open
    and drop collections. The connection that is passed should remain open
    while SODA operations are being performed. If the connection is closed an
    error will take place when the next SODA operation is attempted.

    SODA support in ODPI-C is in Preview status and should not be used in
    production. It will be supported with a future version of Oracle Client
    libraries.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection to use for accessing the
    SODA database.

    **db** [OUT] -- a pointer to a reference to a newly allocated SODA
    database if the function completes successfully. The function
    :func:`dpiSodaDb_release()` should be used when the database is no longer
    required.


.. function:: int dpiConn_getStmtCacheSize(dpiConn \*conn, \
        uint32_t \*cacheSize)

    Returns the size of the statement cache, in number of statements.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection from which the size of the
    statement cache is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **cacheSize** [OUT] -- a pointer to the size of the statement cache, which
    will be populated upon successful completion of this function.


.. function:: int dpiConn_newDeqOptions(dpiConn \*conn, \
        dpiDeqOptions \**options)

    Returns a reference to a new set of dequeue options, used in dequeuing
    objects from a queue. The reference should be released as soon as it is no
    longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the dequeue is
    going to take place. If the reference is NULL or invalid an error is
    returned.

    **options** [OUT] -- a pointer to a reference to the dequeue options that
    is created by this function.


.. function:: int dpiConn_newEnqOptions(dpiConn \*conn, \
        dpiEnqOptions \**options)

    Returns a reference to a new set of enqueue options, used in enqueuing
    objects into a queue. The reference should be released as soon as it is no
    longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the enqueue is
    going to take place. If the reference is NULL or invalid an error is
    returned.

    **options** [OUT] -- a pointer to a reference to the enqueue options that
    is created by this function.


.. function:: int dpiConn_newMsgProps(dpiConn \*conn, dpiMsgProps \**props)

    Returns a reference to a new set of message properties, used in enqueuing
    and dequeuing objects in a queue. The reference should be released as soon
    as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the enqueue or
    dequeue is going to take place. If the reference is NULL or invalid an
    error is returned.

    **props** [OUT] -- a pointer to a reference to the message properties that
    is created by this function.


.. function:: int dpiConn_newTempLob(dpiConn \*conn, \
        dpiOracleTypeNum lobType, dpiLob \**lob)

    Returns a reference to a new temporary LOB which may subsequently be
    written and bound to a statement. The reference should be released as soon
    as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the temporary LOB
    is to be created. If the reference is NULL or invalid an error is returned.

    **lobType** [IN] -- the type of LOB which should be created. It should be
    one of these values from the enumeration
    :ref:`dpiOracleTypeNum<dpiOracleTypeNum>`: DPI_ORACLE_TYPE_CLOB,
    DPI_ORACLE_TYPE_NCLOB or DPI_ORACLE_TYPE_BLOB.

    **lob** [OUT] -- a pointer to a reference to the temporary LOB that is
    created by this function, which will be populated upon successful
    completion of this function.


.. function:: int dpiConn_newVar(dpiConn \*conn, \
        dpiOracleTypeNum oracleTypeNum, dpiNativeTypeNum nativeTypeNum, \
        uint32_t maxArraySize, uint32_t size, int sizeIsBytes, int isArray, \
        dpiObjectType \*objType, dpiVar \**var, dpiData \**data)

    Returns a reference to a new variable which can be used for binding data to
    a statement or providing a buffer for querying data from the database.
    The reference should be released as soon as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection which this variable will be
    used for binding or querying. If the reference is NULL or invalid an error
    is returned.

    **oracleTypeNum** [IN] -- the type of Oracle data that is to be used. It
    should be one of the values from the enumeration
    :ref:`dpiOracleTypeNum<dpiOracleTypeNum>`.

    **nativeTypeNum** [IN] -- the type of native C data that is to be used. It
    should be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **maxArraySize** [IN] -- the maximum number of rows that can be fetched or
    bound at one time from the database, or the maximum number of elements that
    can be stored in a PL/SQL array.

    **size** [IN] -- the maximum size of the buffer used for transferring data
    to/from Oracle. This value is only used for variables transferred as byte
    strings. Size is either in characters or bytes depending on the value of
    the sizeIsBytes parameter. If the value is in characters, internally the
    value will be multipled by the maximum number of bytes for each character
    and that value used instead when determining the necessary buffer size.

    **sizeIsBytes** [IN] -- boolean value indicating if the size parameter
    refers to characters or bytes. This flag is only used if the variable
    refers to character data.

    **isArray** [IN] -- boolean value indicating if the variable refers to a
    PL/SQL array or simply to buffers used for binding or fetching data.

    **objType** [IN] -- a reference to the object type of the object that is
    being bound or fetched. This value is only used if the Oracle type is
    DPI_ORACLE_TYPE_OBJECT.

    **var** [OUT] -- a pointer to a reference to the variable that is created
    by this function.

    **data** [OUT] -- a pointer to an array of :ref:`dpiData<dpiData>`
    structures that are used to transfer data to/from the variable. These are
    allocated when the variable is created and the number of structures
    corresponds to the maxArraySize.


.. function:: int dpiConn_ping(dpiConn \*conn)

    Pings the database to verify that the connection is still alive.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection which will be pinged. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiConn_prepareDistribTrans(dpiConn \*conn, \
        int \*commitNeeded)

    Prepares a distributed transaction for commit. This function should only
    be called after :func:`dpiConn_beginDistribTrans()` is called and before
    :func:`dpiConn_commit()` is called.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection on which the distributed
    transaction is to be prepared for commit. If the reference is NULL or
    invalid an error is returned.

    **commitNeeded** [OUT] -- a pointer to a boolean value indicating if a
    commit is needed or not. If no commit is needed, attempting to commit
    anyway will result in an ORA-24756 error (transaction does not exist).


.. function:: int dpiConn_prepareStmt(dpiConn \*conn, int scrollable, \
        const char \*sql, uint32_t sqlLength, const char \*tag, \
        uint32_t tagLength, dpiStmt \**stmt)

    Returns a reference to a statement prepared for execution. The reference
    should be released as soon as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection on which the statement is to
    be prepared. If the reference is NULL or invalid an error is returned.

    **scrollable** [IN] -- a boolean indicating if the statement is scrollable
    or not. If it is scrollable, :func:`dpiStmt_scroll()` can be used to
    reposition the cursor; otherwise, rows are retrieved in order from the
    statement until the rows are exhausted. This value is ignored for
    statements that do not refer to a query.

    **sql** [IN] -- the SQL that is to be prepared for execution, as a byte
    string in the encoding used for CHAR data. The value can also be NULL if
    the tag parameter is specified.

    **sqlLength** [IN] -- the length of the SQL that is to be prepared for
    execution, in bytes, or 0 if the sql parameter is NULL.

    **tag** [IN] -- the key to be used for searching for the statement in the
    statement cache, as a byte string in the encoding used for CHAR data. The
    value can also be NULL if the sql parameter is specified.

    **tagLength** [IN] -- the length of the key to be used for searching for
    the statement in the statement cache, in bytes, or 0 if the tag parameter
    is NULL.

    **stmt** [OUT] -- a pointer to a reference to the statement that was just
    prepared, which will be populated upon successful completion of the
    function.


.. function:: int dpiConn_release(dpiConn \*conn)

    Releases a reference to the connection. A count of the references to the
    connection is maintained and when this count reaches zero, the memory
    associated with the connection is freed and the connection is closed or
    released back to the session pool if that has not already taken place using
    the function :func:`dpiConn_close()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- the connection from which a reference is to be released.
    If the reference is NULL or invalid an error is returned.


.. function:: int dpiConn_rollback(dpiConn \*conn)

    Rolls back the current active transaction.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection which holds the transaction
    which is to be rolled back. If the reference is NULL or invalid an error is
    returned.


.. function:: int dpiConn_setAction(dpiConn \*conn, const char \*value, \
        uint32_t valueLength)

    Sets the action attribute on the connection. This is one of the end-to-end
    tracing attributes that can be tracked in database views, shown in audit
    trails and seen in tools such as Enterprise Manager.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the action
    attribute is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the action attribute.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setCallTimeout(dpiConn \*conn, uint32_t value)

    Sets the call timeout (in milliseconds) to be used for round-trips to the
    database made with this connection. A value of 0 means that no timeouts
    will take place.  The current value can be acquired using the function
    :func:`dpiConn_getCallTimeout()`.

    The call timeout value applies to each database round-trip
    individually, not to the sum of all round-trips. Time spent
    processing in ODPI-C before or after the completion of each
    round-trip is not counted.

        - If the time from the start of any one round-trip to the
          completion of that same round-trip exceeds call timeout
          milliseconds, then the operation is halted and an exception
          occurs.

        - In the case where an ODPI-C operation requires more than one
          round-trip and each round-trip takes less than call timeout
          milliseconds, then no timeout will occur, even if the sum of
          all round-trip calls exceeds call timeout.

        - If no round-trip is required, the operation will never be
          interrupted.

    After a timeout is triggered, ODPI-C attempts to clean up the
    internal connection state. The cleanup is allowed to take another
    ``value`` milliseconds.

    If the cleanup was successful, an exception DPI-1067 will be
    raised but the application can continue to use the connection.

    For small values of call timeout, the connection cleanup may not
    complete successfully within the additional call timeout
    period. In this case an exception ORA-3114 is raised and the
    connection will no longer be usable. It should be closed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection on which the current call
    timeout is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the value to use (in milliseconds) for round-trips to the
    database made with this connection. A value of 0 means that no timeouts
    will take place.


.. function:: int dpiConn_setClientIdentifier(dpiConn \*conn, \
        const char \*value, uint32_t valueLength)

    Sets the client identifier attribute on the connection. This is one of the
    end-to-end tracing attributes that can be tracked in database views, shown
    in audit trails and seen in tools such as Enterprise Manager.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the client
    identifier attribute is to be set. If the reference is NULL or invalid an
    error is returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the client identifier attribute.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setClientInfo(dpiConn \*conn, const char \*value, \
        uint32_t valueLength)

    Sets the client info attribute on the connection. This is one of the
    end-to-end tracing attributes that can be tracked in database views, shown
    in audit trails and seen in tools such as Enterprise Manager.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the client info
    attribute is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the client info attribute.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setCurrentSchema(dpiConn \*conn, \
        const char \*value, uint32_t valueLength)

    Sets the current schema to be used on the connection. This has the same
    effect as the SQL statement ALTER SESSION SET CURRENT_SCHEMA. The value
    be changed when the next call requiring a round trip to the server is
    performed. If the new schema name does not exist, the same error is
    returned as when the alter session statement is executed. The new schema
    name is placed before database objects in statement that you execute that
    do not already have a schema.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the current schema
    is to be set. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the current schema.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setDbOp(dpiConn \*conn, const char \*value, \
        uint32_t valueLength)

    Sets the database operation attribute on the connection. This is one of the
    end-to-end tracing attributes that can be tracked in database views, shown
    in audit trails and seen in tools such as Enterprise Manager.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the database
    operation attribute is to be set. If the reference is NULL or invalid an
    error is returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the database operation attribute.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setExternalName(dpiConn \*conn, const char \*value, \
        uint32_t valueLength)

    Sets the external name that is being used by the connection. This value is
    used when logging distributed transactions.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the external name
    is to be set. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the external name.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setInternalName(dpiConn \*conn, const char \*value, \
        uint32_t valueLength)

    Sets the internal name that is being used by the connection. This value is
    used when logging distributed transactions.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the internal name
    is to be set. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the internal name.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setModule(dpiConn \*conn, const char \*value, \
        uint32_t valueLength)

    Sets the module attribute on the connection. This is one of the end-to-end
    tracing attributes that can be tracked in database views, shown in audit
    trails and seen in tools such as Enterprise Manager.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the module
    attribute is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- a pointer to a byte string in the encoding used for CHAR
    data which will be used to set the module attribute.

    **valueLength** [IN] -- the length of the value that is to be set, in
    bytes.


.. function:: int dpiConn_setStmtCacheSize(dpiConn \*conn, uint32_t cacheSize)

    Sets the size of the statement cache.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the size of the
    statement cache is to be set. If the reference is NULL or invalid an error
    is returned.

    **cacheSize** [IN] -- the new size of the statement cache, in number of
    statements.


.. function:: int dpiConn_shutdownDatabase(dpiConn \*conn, \
        dpiShutdownMode mode)

    Shuts down the database. This function must be called twice for the
    database to be shut down successfully. After calling this function the
    first time, the SQL statements "alter database close normal" and
    "alter database dismount" must be executed. Once that is complete this
    function should be called again with the mode DPI_MODE_SHUTDOWN_FINAL
    in order to complete the orderly shutdown of the database.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection to the database which is to
    be shut down. If the reference is NULL or invalid an error is returned. The
    connection needs to have been established at least with authorization mode
    set to DPI_MODE_AUTH_SYSDBA or DPI_MODE_AUTH_SYSOPER.

    **mode** [IN] -- one of the values from the enumeration
    :ref:`dpiShutdownMode<dpiShutdownMode>`.


.. function:: int dpiConn_startupDatabase(dpiConn \*conn, dpiStartupMode mode)

    Starts up a database.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection to the database which is to
    be started up. If the reference is NULL or invalid an error is returned.
    A connection like this can only be created with the authorization mode set
    to DPI_MODE_AUTH_PRELIM along with one of DPI_MODE_AUTH_SYSDBA or
    DPI_MODE_AUTH_SYSOPER.

    **mode** [IN] -- one of the values from the enumeration
    :ref:`dpiStartupMode<dpiStartupMode>`.


.. function:: int dpiConn_subscribe(dpiConn \*conn, \
        dpiSubscrCreateParams \*params, dpiSubscr \**subscr)

    Returns a reference to a subscription which is used for requesting
    notifications of events that take place in the database. Events that are
    supported are changes on tables or queries (continuous query notification)
    and the availability of messages to dequeue (advanced queuing). The
    reference should be released as soon as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the subscription is
    to be created. If the reference is NULL or invalid an error is returned.

    **params** [IN] -- a pointer to a
    :ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure which is used
    to specify parameters for the subscription. These parameters determine what
    events will result in notifications.

    **subscr** [OUT] -- a pointer to a reference to the subscription that is
    created by this function.


.. function:: int dpiConn_unsubscribe(dpiConn \*conn, dpiSubscr \*subscr)

    Unubscribes from the events that were earlier subscribed to via the
    function :func:`dpiConn_subscribe()`. Once this function completes
    successfully no further notifications will be sent for this subscription.
    Note that this method does not generate a notification either.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **conn** [IN] -- a reference to the connection in which the subscription is
    to be destroyed. If the reference is NULL or invalid an error is returned.
    The connection used to unsubscribe should be the same connection used to
    subscribe or should access the same database and be connected as the same
    user name.

    **subscr** [OUT] -- a pointer to a reference to the subscription that is to
    be destroyed. A reference will be released and the subscription will no
    longer be usable once this function completes successfully.

