.. _dpiPoolCreateParams:

ODPI-C Structure dpiPoolCreateParams
------------------------------------

This structure is used for creating session pools, which can in turn be used to
create connections that are acquired from that session pool. All members are
initialized to default values using the
:func:`dpiContext_initPoolCreateParams()` function.

.. member:: uint32_t dpiPoolCreateParams.minSessions

    Specifies the minimum number of sessions to be created by the session pool.
    This value is ignored if the :member:`dpiPoolCreateParams.homogeneous`
    member has a value of 0. The default value is 1.

.. member:: uint32_t dpiPoolCreateParams.maxSessions

    Specifies the maximum number of sessions that can be created by the session
    pool. Values of 1 and higher are acceptable. The default value is 1.

.. member:: uint32_t dpiPoolCreateParams.sessionIncrement

    Specifies the number of sessions that will be created by the session pool
    when more sessions are required and the number of sessions is less than
    the maximum allowed. This value is ignored if the
    :member:`dpiPoolCreateParams.homogeneous` member has a value of 0. This
    value added to the :member:`dpiPoolCreateParams.minSessions` member value
    must not exceed the :member:`dpiPoolCreateParams.maxSessions` member value.
    The default value is 0.

.. member:: int dpiPoolCreateParams.pingInterval

    Specifies the number of seconds since a connection has last been used
    before a ping will be performed to verify that the connection is still
    valid. A negative value disables this check. The default value is 60.

.. member:: int dpiPoolCreateParams.pingTimeout

    Specifies the number of milliseconds to wait when performing a ping to
    verify the connection is still valid before the connection is considered
    invalid and is dropped. The default value is 5000 (5 seconds).

.. member:: int dpiPoolCreateParams.homogeneous

    Specifies whether the pool is homogeneous or not. In a homogeneous pool all
    connections use the same credentials whereas in a heterogeneous pool other
    credentials are permitted. The default value is 1.

.. member:: int dpiPoolCreateParams.externalAuth

    Specifies whether external authentication should be used to create the
    sessions in the pool. If this value is 0, the user name and password values
    must be specified in the call to :func:`dpiPool_create()`; otherwise, the
    user name and password values must be zero length or NULL. The default
    value is 0. External authentication cannot be used with homogeneous pools.

.. member:: dpiPoolGetMode dpiPoolCreateParams.getMode

    Specifies the mode to use when sessions are acquired from the pool. It is
    expected to be one of the values from the enumeration
    :ref:`dpiPoolGetMode<dpiPoolGetMode>`. The default value is
    DPI_MODE_POOL_GET_NOWAIT. This value can be set after the pool has been
    created using the function :func:`dpiPool_setGetMode()` and acquired using
    the function :func:`dpiPool_getGetMode()`.

.. member:: const char* dpiPoolCreateParams.outPoolName

    This member is populated upon successful creation of a pool using the
    function :func:`dpiPool_create()`. It is a byte string in the encoding
    used for CHAR data. Any value specified prior to creating the session pool
    is ignored.

.. member:: uint32_t dpiPoolCreateParams.outPoolNameLength

    This member is populated upon successful creation of a pool using the
    function :func:`dpiPool_create()`. It is the length of the
    :member:`dpiPoolCreateParams.outPoolName` member, in bytes. Any value
    specified prior to creating the session pool is ignored.

.. member:: uint32_t dpiPoolCreateParams.timeout

    Specifies the length of time (in seconds) after which idle sessions in the
    pool are terminated. Note that termination only occurs when the pool is
    accessed. The default value is 0 which means that no idle sessions are
    terminated. This value can be set after the pool has been created using the
    function :func:`dpiPool_setTimeout()` and acquired using the function
    :func:`dpiPool_getTimeout()`.

.. member:: uint32_t dpiPoolCreateParams.waitTimeout

    Specifies the length of time (in milliseconds) that the caller should wait
    for a session to become available in the pool before returning with an
    error. This value is only used if the
    :member:`dpiPoolCreateParams.getMode` member is set to the value
    DPI_MODE_POOL_GET_TIMEDWAIT. The default value is 0. This value can be set
    after the pool has been created using the function
    :func:`dpiPool_setWaitTimeout()` and acquired using the function
    :func:`dpiPool_getWaitTimeout()`.

.. member:: uint32_t dpiPoolCreateParams.maxLifetimeSession

    Specifies the maximum length of time (in seconds) a pooled session may
    exist. Sessions in use will not be closed. They become candidates for
    termination only when they are released back to the pool and have existed
    for longer than maxLifetimeSession seconds. Session termination only occurs
    when the pool is accessed. The default value is 0 which means that there is
    no maximum length of time that a pooled session may exist. This value can
    be set after the pool has been created using the function
    :func:`dpiPool_setMaxLifetimeSession()` and acquired using the function
    :func:`dpiPool_getMaxLifetimeSession()`.

.. member:: const char* dpiPoolCreateParams.plsqlFixupCallback

    Specifies the name of a PL/SQL procedure in the format
    *schema.package.callback_proc* which will be called when a session is
    checked out from the pool and the requested tag doesn't match the actual
    tag assigned to the session. The procedure accepts the desired
    and actual tags as parameters and it is the responsibility of the procedure
    to ensure that the session matches the desired state upon completion. See
    the `OCI documentation <https://www.oracle.com/pls/topic/lookup?
    ctx=dblatest&id=GUID-B853A020-752F-494A-8D88-D0396EF57177>`__ for more
    information. The value is expected to be a byte string in the encoding used
    for CHAR data. This functionality is only available when Oracle Client
    is at version 12.2 and higher.

.. member:: uint32_t dpiPoolCreateParams.plsqlFixupCallbackLength

    Specifies the length of the PL/SQL fixup callback, in bytes.

.. member:: uint32_t dpiPoolCreateParams.maxSessionsPerShard

    Specifies the maximum number of sessions that can be created by the session
    pool for each shard in a sharded database. Set this attribute to a value
    other than zero to ensure that the pool is balanced towards each shard. A
    value of zero will not set any maximum number of sessions for each shard.
    If the Oracle client library version is less than 18.3, this value is
    ignored.

.. member:: dpiAccessTokenCallback dpiPoolCreateParams.accessTokenCallback

    The callback function which will get called by the connection pool if the
    pool needs to expand and create new connections but the current
    authentication token has expired.

    The function should get a new authentication token and then set
    :member:`dpiAccessToken.token` and :member:`dpiAccessToken.privateKey`
    in the accessToken argument. The authentication token can be generated by
    invoking the Oracle Cloud Infrastructure (OCI) Command Line Interface
    (CLI).

    The function should return DPI_SUCCESS for success and DPI_FAILURE
    for failure. Returning DPI_FAILURE will cause the connection to fail
    and return ORA-01017 to the application.

    The callback accepts the following arguments:

        **context** -- the value of the
        :member:`dpiPoolCreateParams.accessTokenCallbackContext` member.

        **accessToken** -- a pointer to the structure
        :ref:`dpiAccessToken<dpiAccessToken>`.

.. member:: void* dpiPoolCreateParams.accessTokenCallbackContext

    Specifies the value that will be used as the first argument to the callback
    specified in the :member:`dpiPoolCreateParams.accessTokenCallback`
    member. The default value is NULL.
