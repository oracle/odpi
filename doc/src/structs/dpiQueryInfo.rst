.. _dpiQueryInfo:

ODPI-C Structure dpiQueryInfo
-----------------------------

This structure is used for passing query metadata from ODPI-C. It is populated
by the function :func:`dpiStmt_getQueryInfo()`. All values remain valid as long
as a reference is held to the statement and the statement is not re-executed or
closed.

.. member:: const char \*dpiQueryInfo.name

    Specifies the name of the column which is being queried, as a byte string
    in the encoding used for CHAR data.

.. member:: uint32_t dpiQueryInfo.nameLength

    Specifies the length of the :member:`dpiQueryInfo.name` member, in bytes.

.. member:: dpiDataTypeInfo dpiQueryInfo.typeInfo

    Specifies the type of data of the column that is being queried. It is a
    structure of type :ref:`dpiDataTypeInfo<dpiDataTypeInfo>`.

.. member:: int dpiQueryInfo.nullOk

    Specifies if the data that is being queried may return null values (1) or
    not (0).

