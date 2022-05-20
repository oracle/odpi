.. _dpiCommonCreateParams:

ODPI-C Structure dpiCommonCreateParams
--------------------------------------

This structure is used for creating session pools and standalone connections to
the database. All members are initialized to default values using the
:func:`dpiContext_initCommonCreateParams()` function.

.. member:: dpiCreateMode dpiCommonCreateParams.createMode

    Specifies the mode used for creating connections. It is expected to be
    one or more of the values from the enumeration
    :ref:`dpiCreateMode<dpiCreateMode>`, OR'ed together. The default value is
    `DPI_MODE_CREATE_DEFAULT`.

    Note that when this structure is used during creation of a pool, the mode
    `DPI_MODE_CREATE_THREADED` is automatically enabled regardless of what
    value is set in this member.

.. member:: const char* dpiCommonCreateParams.encoding

    Specifies the encoding to use for CHAR data, as a null-terminated ASCII
    string. Either an IANA or Oracle specific character set name is expected.
    NULL is also acceptable which implies the use of the NLS_LANG environment
    variable (or ASCII, if the NLS_LANG environment variable is not set).
    The default value is the value of the member
    :member:`dpiContextCreateParams.defaultEncoding` specified when the call to
    :func:`dpiContext_createWithParams()` was made.

.. member:: const char* dpiCommonCreateParams.nencoding

    Specifies the encoding to use for NCHAR data, as a null-terminated ASCII
    string. Either an IANA or Oracle specific character set name is expected.
    NULL is also acceptable which implies the use of the NLS_NCHAR environment
    variable (or the same value as the :member:`dpiCommonCreateParams.encoding`
    member if the NLS_NCHAR environment variable is not set). The default
    value is the value of the member
    :member:`dpiContextCreateParams.defaultEncoding` specified when the call to
    :func:`dpiContext_createWithParams()` was made.

.. member:: const char* dpiCommonCreateParams.edition

    Specifies the edition to be used when creating a standalone connection. It
    is expected to be NULL (meaning that no edition is set) or a byte string in
    the encoding specified by the :member:`dpiCommonCreateParams.encoding`
    member. The default value is NULL.

.. member:: uint32_t dpiCommonCreateParams.editionLength

    Specifies the length of the :member:`dpiCommonCreateParams.edition` member,
    in bytes. The default value is 0.

.. member:: const char* dpiCommonCreateParams.driverName

    Specifies the name of the driver that is being used. It is expected to be
    NULL or a byte string in the encoding specified by the
    :member:`dpiCommonCreateParams.encoding` member. The default value is the
    value of the member :member:`dpiContextCreateParams.defaultDriverName`
    specified when the call to
    :func:`dpiContext_createWithParams()` was made.

.. member:: uint32_t dpiCommonCreateParams.driverNameLength

    Specifies the length of the :member:`dpiCommonCreateParams.driverName`
    member, in bytes. The default value is the length of the member
    :member:`dpiContextCreateParams.defaultDriverName` specified when the call
    to :func:`dpiContext_createWithParams()` was made.

.. member:: int dpiCommonCreateParams.sodaMetadataCache

    Specifies whether to enable the SODA metadata cache or not, which can
    significantly improve the performance of repeated calls to methods
    :func:`dpiSodaDb_createCollection()` (when not specifying a value for the
    metadata parameter) and :func:`dpiSodaDb_openCollection()`. This requires
    Oracle Client 21.3 or higher (also available in Oracle Client 19 from
    19.11).

.. member:: uint32_t dpiCommonCreateParams.stmtCacheSize

    Specifies the number of statements to retain in the statement cache. Use a
    value of 0 to disable the statement cache completely.

.. member:: dpiAccessToken* dpiCommonCreateParams.accessToken

   Specifies a pointer to a :ref:`dpiAccessToken<dpiAccessToken>` structure.
   Each member of structure is populated with authentication parameters used for
   token based authentication. This requires Oracle Client 19.4 (or later),
   or 21.5 (or later).
