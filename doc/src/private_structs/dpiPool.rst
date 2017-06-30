.. _dpiPool:

ODPI-C Private Structure dpiPool
--------------------------------

This private structure represents session pools and is available by handle to a
calling application or driver. The implementation for this type is found in
dpiPool.c.  It is created by calling the function :func:`dpiPool_create()` and
its resources can be freed by calling the function :func:`dpiPool_close()` or
when the last reference is released using the function
:func:`dpiPool_release()`. All of the attributes of the structure
:ref:`dpiBaseType<dpiBaseType>` are included in this structure in addition to
the ones specific to this structure described below.

.. member:: OCISPool \*dpiPool.handle

    Specifies the OCI session pool handle.

.. member:: const char \*dpiPool.name

    Specifies the name of the pool as a byte string in the encoding for CHAR
    data. This name is used by OCI when acquiring connections from the session
    pool.

.. member:: uint32_t dpiPool.nameLength

    Specifies the length of the :member:`dpiPool.name` member, in bytes.

.. member:: int dpiPool.pingInterval

    Specifies the number of seconds since a connection has last been used
    before a ping will be performed to verify that the connection is still
    valid. A negative value disables this check. The value is ignored in
    clients 12.2 and later since a much faster internal check is done by the
    Oracle client.

.. member:: int dpiPool.pingTimeout

    Specifies the number of milliseconds to wait when performing a ping to
    verify the connection is still valid before the connection is considered
    invalid and is dropped. This value is ignored in clients 12.2 and later
    since a much faster internal check is done by the Oracle client.

.. member:: int dpiPool.homogeneous

    Specifies whether the pool is homogeneous (1) or not (0). In a homogeneous
    pool all connections use the same credentials whereas in a heterogeneous
    pool other credentials are permitted.

.. member:: int dpiPool.externalAuth

    Specifies whether external authentication should be used to create the
    sessions in the pool (1) or not (0). If this value is 0, the user name and
    password values must be specified in the call to :func:`dpiPool_create()`;
    otherwise, the user name and password values must be zero length or NULL.

