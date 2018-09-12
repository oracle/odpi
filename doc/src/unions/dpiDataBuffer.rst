.. _dpiDataBuffer:

ODPI-C Union dpiDataBuffer
--------------------------

This union is used for passing data to and from the database without requiring
casts.

.. member:: int dpiDataBuffer.asBoolean

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_BOOLEAN. The value should be either 1 (true) or 0 (false).

.. member:: int64_t dpiDataBuffer.asInt64

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_INT64.

.. member:: uint64_t dpiDataBuffer.asUint64

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_UINT64.

.. member:: float dpiDataBuffer.asFloat

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_FLOAT.

.. member:: double dpiDataBuffer.asDouble

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_DOUBLE.

.. member:: dpiBytes dpiDataBuffer.asBytes

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_BYTES. This is a structure of type
    :ref:`dpiBytes<dpiBytes>`.

.. member:: dpiTimestamp dpiDataBuffer.asTimestamp

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_TIMESTAMP. This is a structure of type
    :ref:`dpiTimestamp<dpiTimestamp>`.

.. member:: dpiIntervalDS dpiDataBuffer.asIntervalDS

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_INTERVAL_DS. This is a structure of type
    :ref:`dpiIntervalDS<dpiIntervalDS>`.

.. member:: dpiIntervalYM dpiDataBuffer.asIntervalYM

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_INTERVAL_YM. This is a structure of type
    :ref:`dpiIntervalYM<dpiIntervalYM>`.

.. member:: dpiLob \*dpiDataBuffer.asLOB

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_LOB. This is a reference to a LOB (large object) which can
    be used for reading and writing the data that belongs to it.

.. member:: int dpiDataBuffer.asObject

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_OBJECT. This is a reference to an object which can be used
    for reading and writing its attributes or element values.

.. member:: int dpiDataBuffer.asStmt

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_STMT. This is a reference to a statement which can be used
    to query data from the database.

.. member:: int dpiDataBuffer.asRowid

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_ROWID. This is a reference to a rowid which is used to
    uniquely identify a row in a table in the database.

