.. _dpiContextCreateParams:

ODPI-C Structure dpiContextCreateParams
---------------------------------------

This structure is used for specifying parameters when creating an ODPI-C
context by calling :func:`dpiContext_createWithParams()`.

.. member:: const char* dpiContextCreateParams.defaultDriverName

    A null-terminated string defining the default driver name to use when
    creating pools or standalone connections.

    The standard is to set this value to ``"<name> : <version>"``, where <name>
    is the name of the driver and <version> is its version. There should be a
    single space character before and after the colon.

    This value is shown in database views that give information about
    connections. For example, it is in the ``CLIENT_DRIVER`` column
    of ``V$SESSION_CONNECT_INFO``.

    If this member is NULL, then the default value will be the value of
    ``DPI_DEFAULT_DRIVER_NAME`` from `dpi.h
    <https://github.com/oracle/odpi/blob/main/include/dpi.h>`__.

    This member is propagated to the :member:`dpiCommonCreateParams.driverName`
    and :member:`dpiCommonCreateParams.driverNameLength` members when the
    function :func:`dpiContext_initCommonCreateParams()` is called.

.. member:: const char* dpiContextCreateParams.defaultEncoding

    A null-terminated string defining the default encoding to use for all
    string data (including NCHAR, NVARCHAR2 and NCLOB data) passed into or
    returned from the database.

    If this member is NULL, then the default value will be "UTF-8". The Oracle
    equivalent of this is "AL32UTF8".

    This member is propagated to the :member:`dpiCommonCreateParams.encoding`
    and :member:`dpiCommonCreateParams.nencoding` members when the
    function :func:`dpiContext_initCommonCreateParams()` is called.

.. member:: const char* dpiContextCreateParams.loadErrorUrl

    A null-terminated UTF-8 encoded string defining the URL that should be
    provided in the error message returned when the Oracle Client library
    cannot be loaded. This URL should direct the user to the installation
    instructions for the application or driver using ODPI-C. If this value is
    NULL then the default ODPI-C URL is provided in the error message instead.

.. member:: const char* dpiContextCreateParams.oracleClientLibDir

    A null-terminated UTF-8 encoded string defining the location from which to
    load the Oracle Client library. If this value is not NULL it is the only
    location that is searched; otherwise, if this value is NULL the Oracle
    Client library is searched for in the usual fashion as noted in
    :ref:`Oracle Client Library Loading <clientlibloading>`. Also see that
    section for limitations on using this member.

.. member:: const char* dpiContextCreateParams.oracleClientConfigDir

    A null-terminated UTF-8 encoded string defining the location the Oracle
    client library will search for configuration files. This is equivalent to
    setting the environment variable ``TNS_ADMIN``. If this value is not NULL,
    it overrides any value set by the environment variable ``TNS_ADMIN``.

.. member:: int dpiContextCreateParams.sodaUseJsonDesc

    A boolean value indicating whether or not to use JSON descriptors with
    SODA. This requires Oracle Client 23.4 or higher and setting this value to
    1 in earlier versions will simply be ignored. Once a call to
    :func:`dpiContext_createWithParams()` is made with this structure, this
    member will be cleared if JSON descriptors cannot be used.

.. member:: int dpiContextCreateParams.useJsonId

    A boolean value indicating whether or not to treat JSON ID values
    distinctly from other binary data.
