.. _dpiConnCreateParams:

ODPI-C Structure dpiConnCreateParams
------------------------------------

This structure is used for creating connections to the database, whether
standalone or acquired from a session pool. All members are initialized to
default values using the :func:`dpiContext_initConnCreateParams()` function.
Care should be taken to ensure a copy of this structure exists only as long as
needed to create the connection since it can contain a clear text copy of
credentials used for connecting to the database.

.. member:: dpiAuthMode dpiConnCreateParams.authMode

    Specifies the mode used for authorizing connections. It is expected to be
    one or more of the values from the enumeration
    :ref:`dpiAuthMode<dpiAuthMode>`, OR'ed together. The default value is
    DPI_MODE_AUTH_DEFAULT.

.. member:: const char \* dpiConnCreateParams.connectionClass

    Specifies the connection class to use when connecting to the database. This
    is used with DRCP (database resident connection pooling) or to further
    subdivide a session pool. It is expected to be NULL (meaning that no
    connection class will be set) or a byte string in the encoding used for
    CHAR data.  The default value is NULL.

.. member:: uint32_t dpiConnCreateParams.connectionClassLength

    Specifies the length of the
    :member:`dpiConnCreateParams.connectionClass` member, in bytes. The
    default value is 0.

.. member:: dpiPurity dpiConnCreateParams.purity

    Specifies the level of purity required when creating a connection using a
    connection class.  It is expected to be one of the values from the
    enumeration :ref:`dpiPurity<dpiPurity>`. The default value is
    DPI_PURITY_DEFAULT.

.. member:: const char \* dpiConnCreateParams.newPassword

    Specifies the new password to set when creating a connection. This value
    is only used when creating a standalone connection. It is expected to be
    NULL or a byte string in the encoding used for CHAR data. The default value
    of this member is NULL. If specified, the password for the user is changed
    when the connection is created (useful when the password has expired and a
    session cannot be established without changing the password).

.. member:: uint32_t dpiConnCreateParams.newPasswordLength

    Specifies the length of the :member:`dpiConnCreateParams.newPassword`
    member, in bytes. The default value is 0.

.. member:: dpiAppContext \*dpiConnCreateParams.appContext

    Specifies the application context that will be set when the connection is
    created. This value is only used when creating standalone connections. It
    is expected to be NULL or an array of :ref:`dpiAppContext<dpiAppContext>`
    structures. The context specified here can be used in logon triggers, for
    example. The default value is NULL.

.. member:: uint32_t dpiConnCreateParams.numAppContext

    Specifies the number of elements found in the
    :member:`dpiConnCreateParams.appContext` member. The default value is 0.

.. member:: int dpiConnCreateParams.externalAuth

    Specifies whether external authentication should be used to create the
    connection. If this value is 0, the user name and password values must be
    specified in the call to :func:`dpiConn_create()`; otherwise, the user
    name and password values must be zero length or NULL. The default value is
    0.

.. member:: void \* dpiConnCreateParams.externalHandle

    Specifies an OCI service context handle created externally that will be
    used instead of creating a connection. The default value is NULL.

.. member:: dpiPool \* dpiConnCreateParams.pool

    Specifies the session pool from which to acquire a connection or NULL if
    a standalone connection should be created. The default value is NULL.

.. member:: const char \* dpiConnCreateParams.tag

    Specifies the tag to use when acquiring a connection from a session pool.
    This member is ignored when creating a standalone connection. If specified,
    the tag restricts the type of session that can be returned to those with
    that tag or a NULL tag. If the member
    :member:`dpiConnCreateParams.matchAnyTag` is set, however, any session can
    be returned if no matching sessions are found.

    The value is expected to be NULL (any session can be returned) or a byte
    string in the encoding used for CHAR data. The default value is NULL.

.. member:: uint32_t dpiConnCreateParams.tagLength

    Specifies the length of the :member:`dpiConnCreateParams.tag` member, in
    bytes. The default value is 0.

.. member:: int dpiConnCreateParams.matchAnyTag

    Specifies whether any tagged session should be accepted when acquiring a
    connection from a session pool, if no connection using the tag specified
    in the :member:`dpiConnCreateParams.tag` is available. This value is only
    used when acquiring a connection from a session pool. The default value is
    0.

.. member:: const char \* dpiConnCreateParams.outTag

    Specifies the tag of the connection that was acquired from a session pool,
    or NULL if the session was not tagged. This member is left untouched when
    creating a standalone connection and is filled in only if the connection
    acquired from the session pool was tagged. If filled in, it is a byte
    string in the encoding used for CHAR data.

.. member:: uint32_t dpiConnCreateParams.outTagLength

    Specifies the length of the :member:`dpiConnCreateParams.outTag`
    member, in bytes.

.. member:: int dpiConnCreateParams.outTagFound

    Specifies if the connection created used the tag specified by the
    :member:`dpiConnCreateParams.tag` member. It is only filled in if the
    connection was acquired from a session pool and a tag was initially
    specified.

.. member:: dpiShardingKeyColumn \*dpiConnCreateParams.shardingKeyColumns

    Specifies an array of :ref:`dpiShardingKeyColumn<dpiShardingKeyColumn>`
    structures which are used to identify which database shard should be
    connected to. The number of elements in the array is assumed to contain at
    least :member:`dpiConnCreateParams.numShardingKeyColumns` elements.

.. member:: uint8_t dpiConnCreateParams.numShardingKeyColumns

    Specifies the number of elements in the array of sharding key columns found
    in the member :member:`dpiConnCreateParams.shardingKeyColumns`.

.. member:: dpiShardingKeyColumn \*dpiConnCreateParams.superShardingKeyColumns

    Specifies an array of :ref:`dpiShardingKeyColumn<dpiShardingKeyColumn>`
    structures which are used to identify which database shard should be
    connected to. The number of elements in the array is assumed to contain at
    least :member:`dpiConnCreateParams.numSuperShardingKeyColumns` elements.

.. member:: uint8_t dpiConnCreateParams.numSuperShardingKeyColumns

    Specifies the number of elements in the array of sharding key columns found
    in the member :member:`dpiConnCreateParams.superShardingKeyColumns`.

