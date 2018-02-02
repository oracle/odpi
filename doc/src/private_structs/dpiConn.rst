.. _dpiConn:

ODPI-C Private Structure dpiConn
--------------------------------

This private structure represents connections to the database and is available
by handle to a calling application or driver. The implementation for this type
is found in dpiConn.c. Connections are created by calling the function
:func:`dpiConn_create()` or acquired from a session pool by calling the
function :func:`dpiPool_acquireConnection()`. They can be closed by calling the
function :func:`dpiConn_close()` or releasing the last reference to the
connection by calling the function :func:`dpiConn_release()`. Connection
handles are used to create all handles other than session pools and context
handles. All of the attributes of the structure :ref:`dpiBaseType<dpiBaseType>`
are included in this structure in addition to the ones specific to this
structure described below.

.. member:: dpiPool \*dpiConn.pool

    Specifies a pointer to a :ref:`dpiPool<dpiPool>` structure if the
    connection was acquired from a pool; otherwise, the value is NULL.

.. member:: void \*dpiConn.handle

    Specifies the OCI service context handle for the connection.

.. member:: void \*dpiConn.serverHandle

    Specifies the OCI server handle. This value is retrieved if it is not used
    during the creation of the connection and used to avoid excessive calls to
    OCIAttrGet().

.. member:: void \*dpiConn.sessionHandle

    Specifies the OCI session handle. This value is retrieved if it is not used
    during the creation of the connection and used to avoid excessive calls to
    OCIAttrGet().

.. member:: const char \*dpiConn.releaseString

    Specifies a pointer to an array of bytes making up the release string, in
    the encoding used for CHAR data. The initial value is NULL and is cached
    upon the first call to the function :func:`dpiConn_getServerVersion()`.

.. member:: uint32_t dpiConn.releaseStringLength

    Specifies the length of the :member:`dpiConn.releaseString` member, in
    bytes. If the release string has not yet been retrieved, however, this
    value is 0.

.. member:: int dpiConn.versionNum

    Specifies the first part of the five part server version. The initial value
    is 0 and is cached upon the first call to the function
    :func:`dpiConn_getServerVersion()`.

.. member:: int dpiConn.releaseNum

    Specifies the second part of the five part server version. The initial
    value is 0 and is cached upon the first call to the function
    :func:`dpiConn_getServerVersion()`.

.. member:: int dpiConn.updateNum

    Specifies the third part of the five part server version. The initial value
    is 0 and is cached upon the first call to the function
    :func:`dpiConn_getServerVersion()`.

.. member:: int dpiConn.portReleaseNum

    Specifies the fourth part of the five part server version. The initial
    value is 0 and is cached upon the first call to the function
    :func:`dpiConn_getServerVersion()`.

.. member:: int dpiConn.portUpdateNum

    Specifies the last part of the five part server version. The initial value
    is 0 and is cached upon the first call to the function
    :func:`dpiConn_getServerVersion()`.

.. member:: uint32_t dpiConn.commitMode

    Specifies the mode to use when performing commits. This value is set to
    OCI_TRANS_TWOPHASE during two phase commits; otherwise, it is always set to
    the value OCI_DEFAULT.

.. member:: uint16_t dpiConn.charsetId

    Specifies the ID of the character set that the server is using. This is
    used to determine if conversion is required when transferring strings
    between the client and the server.

.. member:: dpiHandleList \*dpiConn.openStmts

    Specifies the list of statement handles created by the connection that are
    still open. These statements will be automatically closed when
    :func:`dpiConn_close()` is called. It is a pointer to a structure of type
    :ref:`dpiHandleList <dpiHandleList>`.

.. member:: dpiHandleList \*dpiConn.openLobs

    Specifies the list of LOB handles created by the connection that are still
    open. These LOBs will be automatically closed when :func:`dpiConn_close()`
    is called. It is a pointer to a structure of type
    :ref:`dpiHandleList <dpiHandleList>`.

.. member:: int dpiConn.externalHandle

    Specifies if the OCI service context handle found in the
    :member:`dpiConn.handle` member was acquired from an external source (1) or
    created by the library (0). If this value is 1, the connection will be left
    untouched when the structure is freed as it is assumed that the external
    source will manage the connection itself.

.. member:: int dpiConn.deadSession

    Specifies if the session with the database is considered dead (1) or not
    (0). A session is marked dead if an unrecoverable error is encountered
    which prevents further communication with the database. If the connection
    is acquired from a session pool and is marked dead it will be dropped from
    the pool automatically when the connection is closed.

.. member:: int dpiConn.standalone

    Specifies if the connection is a standalone connection (1) or not (0).

.. member:: int dpiConn.closing

    Specifies if the connection is being closed (1) or not (0). This is
    needed to prevent multiple concurrent attempts to close the connection or
    to perform any other action which requires the connection handle.

