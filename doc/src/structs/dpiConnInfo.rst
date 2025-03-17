.. _dpiConnInfo:

ODPI-C Structure dpiConnInfo
----------------------------

This structure is used for passing information about a connection from ODPI-C.
It is used by the function :func:`dpiConn_getInfo()`.

.. member:: const char* dpiConnInfo.dbDomain

    A pointer to the name of the Oracle Database Domain name associated with
    the connection, encoded in the encoding used for CHAR data. This is the
    same value returned by the SQL expression
    ``SELECT VALUE FROM V$PARAMETER WHERE NAME = 'db_domain'``.

.. member:: uint32_t dpiConnInfo.dbDomainLength

    Contains the length of the :member:`dpiConnInfo.dbDomain` member, in bytes.

.. member:: const char* dpiConnInfo.dbName

    A pointer to the Oracle Database name associated with the connection,
    encoded in the encoding used for CHAR data. This is the same value returned
    by the SQL expression ``SELECT NAME FROM V$DATABASE``. Note the values may
    have different cases.

.. member:: uint32_t dpiConnInfo.dbNameLength

    Contains the length of the :member:`dpiConnInfo.dbName` member, in bytes.

.. member:: const char* dpiConnInfo.instanceName

    A pointer to the Oracle Database instance name associated with the
    connection, encoded in the encoding used for CHAR data. This is the same
    value returned by the SQL expression
    ``SELECT SYS_CONTEXT('USERENV', 'INSTANCE_NAME') FROM DUAL``. Note the
    values may have different cases.

.. member:: uint32_t dpiConnInfo.instanceNameLength

    Contains the length of the :member:`dpiConnInfo.instanceName` member, in
    bytes.

.. member:: const char* dpiConnInfo.serviceName

    A pointer to the Oracle Database service name associated with the
    connection, encoded in the encoding used for CHAR data. This is the same
    value returned by the SQL expression
    ``SELECT SYS_CONTEXT('USERENV', 'SERVICE_NAME') FROM DUAL``.

.. member:: uint32_t dpiConnInfo.serviceNameLength

    Contains the length of the :member:`dpiConnInfo.serviceName` member, in
    bytes.

.. member:: uint32_t dpiConnInfo.maxIdentifierLength

    Specifies the maximum length of identifiers (in bytes) supported by the
    database to which the connection has been established. See `Database Object
    Naming Rules <https://www.oracle.com/pls/topic/lookup
    ?ctx=dblatest&id=GUID-75337742-67FD-4EC0-985F-741C93D918DA>`__. The value
    may be 0, 30, or 128. The value 0 indicates the size cannot be reliably
    determined by ODPI-C, which occurs when using Oracle Client libraries 12.1
    (or older) and also connected to Oracle Database 12.2, or later.

.. member:: uint32_t dpiConnInfo.maxOpenCursors

    Specifies the maximum number of cursors that can be opened. This is the
    same value returned by the SQL expression
    ``SELECT VALUE FROM V$PARAMETER WHERE NAME = 'open_cursors'``. The value
    will be 0 when using Oracle Client libraries 11.2.

.. member:: uint8_t dpiConnInfo.serverType

    Specifies the type of server process used by the connection. This value
    will be one of the values in the enumeration
    :ref:`dpiServerType <dpiServerType>`.
