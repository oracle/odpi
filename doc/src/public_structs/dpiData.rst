.. _dpiData:

ODPI-C Public Structure dpiData
-------------------------------

This structure is used for passing data to and from the database for variables
and for manipulating object attributes and collection values.

.. member:: int dpiData.isNull

    Specifies if the value refers to a null value (1) or not (0).

.. member:: union dpiData.value

    Specifies the value that is being passed or received.

.. member:: int dpiData.value.asBoolean

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_BOOLEAN. The value should be either
    1 (true) or 0 (false).

.. member:: int dpiData.value.asInt64

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_INT64.

.. member:: int dpiData.value.asUint64

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_UINT64.

.. member:: int dpiData.value.asFloat

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_FLOAT.

.. member:: int dpiData.value.asDouble

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_DOUBLE.

.. member:: int dpiData.value.asBytes

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_BYTES. This is a structure of type
    :ref:`dpiBytes<dpiBytes>`.

.. member:: int dpiData.value.asTimestamp

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_TIMESTAMP. This is a structure of
    type :ref:`dpiTimestamp<dpiTimestamp>`.

.. member:: int dpiData.value.asIntervalDS

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_INTERVAL_DS. This is a structure of
    type :ref:`dpiIntervalDS<dpiIntervalDS>`.

.. member:: int dpiData.value.asIntervalYM

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_INTERVAL_YM. This is a structure of
    type :ref:`dpiIntervalYM<dpiIntervalYM>`.

.. member:: int dpiData.value.asLOB

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_LOB. This is a reference to a LOB
    (large object) which can be used for reading and writing the data that
    belongs to it.

.. member:: int dpiData.value.asObject

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_OBJECT. This is a reference to an
    object which can be used for reading and writing its attributes or element
    values.

.. member:: int dpiData.value.asStmt

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_STMT. This is a reference to a
    statement which can be used to query data from the database.

.. member:: int dpiData.value.asRowid

    Value that is used when :member:`dpiData.isNull` is 0 and the native type
    that is being used is DPI_NATIVE_TYPE_ROWID. This is a reference to a
    rowid which is used to uniquely identify a row in a table in the database.

