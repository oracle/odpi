.. _dpiQueryInfo:

ODPI-C Public Structure dpiQueryInfo
------------------------------------

This structure is used for passing query metadata from ODPI-C. It is populated by
the function :func:`dpiStmt_getQueryInfo()`. All values remain valid as long as
a reference is held to the statement and the statement is not re-executed or
closed.

.. member:: const char \*dpiQueryInfo.name

    Specifies the name of the column which is being queried, as a byte string
    in the encoding used for CHAR data.

.. member:: uint32_t dpiQueryInfo.nameLength

    Specifies the length of the :member:`dpiQueryInfo.name` member, in bytes.

.. member:: dpiOracleTypeNum dpiQueryInfo.oracleTypeNum

    Specifies the type of the column that is being queried. It will be one of
    the values from the enumeration :ref:`dpiOracleTypeNum<dpiOracleTypeNum>`.

.. member:: dpiNativeTypeNum dpiQueryInfo.defaultNativeTypeNum

    Specifies the default native type for the column that is being queried.
    It will be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

.. member:: uint32_t dpiQueryInfo.dbSizeInBytes

    Specifies the size in bytes (from the database's perspective) of the column
    that is being queried. This value is only populated for strings and binary
    columns. For all other columns the value is zero.

.. member:: uint32_t dpiQueryInfo.clientSizeInBytes

    Specifies the size in bytes (from the client's perspective) of the column
    that is being queried. This value is only populated for strings and binary
    columns. For all other columns the value is zero.

.. member:: uint32_t dpiQueryInfo.sizeInChars

    Specifies the size in characters of the column that is being queried. This
    value is only populated for string columns. For all other columns the value
    is zero.

.. member:: int16_t dpiQueryInfo.precision

    Specifies the precision of the column that is being queried. This value is
    only populated for numeric and timestamp columns. For all other columns the
    value is zero.

.. member:: int8_t dpiQueryInfo.scale

    Specifies the scale of the column that is being queried. This value is
    only populated for numeric columns. For all other columns the value is
    zero.

.. member:: int dpiQueryInfo.nullOk

    Specifies if the column that is being queried may return null values (1)
    or not (0).

.. member:: dpiObjectType \*dpiQueryInfo.objectType

    Specifies a reference to the type of the object that is being queried. This
    value is only populated for named type columns. For all other columns the
    value is NULL. The reference that is returned must be released when it is
    no longer needed.

