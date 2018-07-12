.. _data_types:

ODPI-C Data Type Support
------------------------

The following table identifies each Oracle type from the enumeration
:ref:`dpiOracleTypeNum<dpiOracleTypeNum>` and the native C types from the
enumeration :ref:`dpiNativeTypeNum<dpiNativeTypeNum>` that can be used with
them when creating variables using the method :func:`dpiConn_newVar()`.

.. list-table::
    :header-rows: 1

    * - Oracle Type
      - Supported Native C Types [*]_
      - Notes
    * - DPI_ORACLE_TYPE_BFILE
      - DPI_NATIVE_TYPE_LOB
      - Use method :func:`dpiVar_setFromLob()` to set values.
    * - DPI_ORACLE_TYPE_BOOLEAN
      - DPI_NATIVE_TYPE_BOOLEAN
      - This type is only usable in PL/SQL.
    * - DPI_ORACLE_TYPE_BLOB
      - DPI_NATIVE_TYPE_LOB
      - Use method :func:`dpiVar_setFromLob()` to set values.
    * - DPI_ORACLE_TYPE_CHAR
      - DPI_NATIVE_TYPE_BYTES
      - Use method :func:`dpiVar_setFromBytes()` to set values.
    * - DPI_ORACLE_TYPE_CLOB
      - DPI_NATIVE_TYPE_LOB
      - Use method :func:`dpiVar_setFromLob()` to set values.
    * - DPI_ORACLE_TYPE_DATE
      - DPI_NATIVE_TYPE_TIMESTAMP
      -
    * - DPI_ORACLE_TYPE_INTERVAL_DS
      - DPI_NATIVE_TYPE_INTERVAL_DS
      -
    * - DPI_ORACLE_TYPE_INTERVAL_YM
      - DPI_NATIVE_TYPE_INTERVAL_YM
      -
    * - DPI_ORACLE_TYPE_LONG_RAW
      - DPI_NATIVE_TYPE_BYTES
      - Use method :func:`dpiVar_setFromBytes()` to set values.
    * - DPI_ORACLE_TYPE_LONG_VARCHAR
      - DPI_NATIVE_TYPE_BYTES
      - Use method :func:`dpiVar_setFromBytes()` to set values.
    * - DPI_ORACLE_TYPE_NATIVE_DOUBLE
      - DPI_NATIVE_TYPE_DOUBLE
      -
    * - DPI_ORACLE_TYPE_NATIVE_INT
      - DPI_NATIVE_TYPE_INT64
      -
    * - DPI_ORACLE_TYPE_NATIVE_FLOAT
      - DPI_NATIVE_TYPE_FLOAT
      -
    * - DPI_ORACLE_TYPE_NATIVE_UINT
      - DPI_NATIVE_TYPE_UINT64
      -
    * - DPI_ORACLE_TYPE_NCHAR
      - DPI_NATIVE_TYPE_BYTES
      - Use method :func:`dpiVar_setFromBytes()` to set values.
    * - DPI_ORACLE_TYPE_NCLOB
      - DPI_NATIVE_TYPE_LOB
      - Use method :func:`dpiVar_setFromLob()` to set values.
    * - DPI_ORACLE_TYPE_NUMBER
      - DPI_NATIVE_TYPE_DOUBLE, DPI_NATIVE_TYPE_BYTES,
        DPI_NATIVE_TYPE_INT64, DPI_NATIVE_TYPE_UINT64
      - Use method :func:`dpiVar_setFromBytes()` to set values when using
        DPI_NATIVE_TYPE_BYTES. Note that Oracle Database number values use a
        decimal format and that accurately transferring the value from the
        database can only be guaranteed with DPI_NATIVE_TYPE_BYTES.
        DPI_NATIVE_TYPE_INT64 and DPI_NATIVE_TYPE_UINT64 can only represent
        integers and DPI_NATIVE_TYPE_DOUBLE can only represent numbers
        accurately if they contain at most between 15 and 17 decimal digits.
        [*]_
    * - DPI_ORACLE_TYPE_NVARCHAR
      - DPI_NATIVE_TYPE_BYTES
      - Use method :func:`dpiVar_setFromBytes()` to set values.
    * - DPI_ORACLE_TYPE_OBJECT
      - DPI_NATIVE_TYPE_OBJECT
      - Use method :func:`dpiVar_setFromObject()` to set values.
    * - DPI_ORACLE_TYPE_RAW
      - DPI_NATIVE_TYPE_BYTES
      - Use method :func:`dpiVar_setFromBytes()` to set values.
    * - DPI_ORACLE_TYPE_ROWID
      - DPI_NATIVE_TYPE_ROWID
      - Use method :func:`dpiVar_setFromRowid()` to set values.
    * - DPI_ORACLE_TYPE_STMT
      - DPI_NATIVE_TYPE_STMT
      - Use method :func:`dpiVar_setFromStmt()` to set values.
    * - DPI_ORACLE_TYPE_TIMESTAMP
      - DPI_NATIVE_TYPE_TIMESTAMP, DPI_NATIVE_TYPE_DOUBLE
      - When using DPI_NATIVE_TYPE_DOUBLE the value corresponds to the number
        of seconds since Jan 1, 1970.
    * - DPI_ORACLE_TYPE_TIMESTAMP_LTZ
      - DPI_NATIVE_TYPE_TIMESTAMP, DPI_NATIVE_TYPE_DOUBLE
      - When using DPI_NATIVE_TYPE_DOUBLE the value corresponds to the number
        of seconds since Jan 1, 1970.
    * - DPI_ORACLE_TYPE_TIMESTAMP_TZ
      - DPI_NATIVE_TYPE_TIMESTAMP, DPI_NATIVE_TYPE_DOUBLE
      - When using DPI_NATIVE_TYPE_DOUBLE the value corresponds to the number
        of seconds since Jan 1, 1970.
    * - DPI_ORACLE_TYPE_VARCHAR
      - DPI_NATIVE_TYPE_BYTES
      - Use method :func:`dpiVar_setFromBytes()` to set values.

.. [*] The first type listed is the default type.
.. [*] https://en.wikipedia.org/wiki/Double-precision_floating-point_format

