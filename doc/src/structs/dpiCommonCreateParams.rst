.. _dpiCommonCreateParams:

ODPI-C Structure dpiCommonCreateParams
--------------------------------------

This structure is used for creating session pools and standalone connections to
the database.  All members are initialized to default values using the
:func:`dpiContext_initCommonCreateParams()` function.

.. member:: dpiCreateMode dpiCommonCreateParams.createMode

    Specifies the mode used for creating connections. It is expected to be
    one or more of the values from the enumeration
    :ref:`dpiCreateMode<dpiCreateMode>`, OR'ed together. The default value is
    DPI_MODE_CREATE_DEFAULT.

.. member:: const char \* dpiCommonCreateParams.encoding

    Specifies the encoding to use for CHAR data, as a null-terminated ASCII
    string. Either an IANA or Oracle specific character set name is expected.
    NULL is also acceptable which implies the use of the NLS_LANG environment
    variable (or ASCII, if the NLS_LANG environment variable is not set).
    The default value is NULL.

.. member:: const char \* dpiCommonCreateParams.nencoding

    Specifies the encoding to use for NCHAR data, as a null-terminated ASCII
    string. Either an IANA or Oracle specific character set name is expected.
    NULL is also acceptable which implies the use of the NLS_NCHAR environment
    variable (or the same value as the :member:`dpiCommonCreateParams.encoding`
    member if the NLS_NCHAR environment variable is not set). The default value
    is NULL.

.. member:: const char \* dpiCommonCreateParams.edition

    Specifies the edition to be used when creating a standalone connection. It
    is expected to be NULL (meaning that no edition is set) or a byte string in
    the encoding specified by the :member:`dpiCommonCreateParams.encoding`
    member. The default value is NULL.

.. member:: uint32_t dpiCommonCreateParams.editionLength

    Specifies the length of the :member:`dpiCommonCreateParams.edition` member,
    in bytes. The default value is 0.

.. member:: const char \* dpiCommonCreateParams.driverName

    Specifies the name of the driver that is being used. It is expected to be
    NULL or a byte string in the encoding specified by the
    :member:`dpiCommonCreateParams.encoding` member. The default value is NULL.

    The standard is to set :member:`dpiCommonCreateParams.driverName`
    to ``"<name> : <version>"``, where <name> is the name of the
    driver and <version> is its version.  There should be a single
    space character before and after the colon.

    This value is shown in database views that give information about
    connections.  For example, it is in the ``CLIENT_DRIVER`` column
    of ``V$SESSION_CONNECT_INFO``.  If
    :member:`dpiCommonCreateParams.driverName` is NULL, then the
    database views will show the value of ``DPI_DEFAULT_DRIVER_NAME``
    from `dpi.h
    <https://github.com/oracle/odpi/blob/master/include/dpi.h>`__.

.. member:: uint32_t dpiCommonCreateParams.driverNameLength

    Specifies the length of the :member:`dpiCommonCreateParams.driverName`
    member, in bytes. The default value is 0.

