.. _dpiDataBuffer:

ODPI-C Union dpiDataBuffer
--------------------------

This union is used for passing data to and from the database without requiring
casts.

.. member:: int dpiDataBuffer.asBoolean

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_BOOLEAN. The value should be either 1 (true) or 0 (false).

.. member:: uint8_t dpiDataBuffer.asUint8

   Only used for getting/setting OCI attributes of 1 byte in length.

.. member:: uint16_t dpiDataBuffer.asUint16

   Only used for getting/setting OCI attributes of 2 bytes in length.

.. member:: uint32_t dpiDataBuffer.asUint32

   Only used for getting/setting OCI attributes of 4 bytes in length.

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

.. member:: char* dpiDataBuffer.asString

   Only used for getting/setting OCI attributes of type string.

.. member:: void* dpiDataBuffer.asRaw

   Raw pointer used for getting/setting all OCI attributes.

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

.. member:: dpiJson* dpiDataBuffer.asJson

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_JSON. This is a reference to a JSON value which can be used
    for reading and writing the data that belongs to it.

.. member:: dpiJsonObject dpiDataBuffer.asJsonObject

    Value this is used when the native type that is being used is
    DPI_NATIVE_TYPE_JSON_OBJECT. This is a structure of type
    :ref:`dpiJsonObject<dpiJsonObject>`.

.. member:: dpiJsonArray dpiDataBuffer.asJsonArray

    Value this is used when the native type that is being used is
    DPI_NATIVE_TYPE_JSON_ARRAY. This is a structure of type
    :ref:`dpiJsonArray<dpiJsonArray>`.

.. member:: dpiLob* dpiDataBuffer.asLOB

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_LOB. This is a reference to a LOB (large object) which can
    be used for reading and writing the data that belongs to it.

.. member:: dpiObject* dpiDataBuffer.asObject

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_OBJECT. This is a reference to an object which can be used
    for reading and writing its attributes or element values.

.. member:: dpiStmt* dpiDataBuffer.asStmt

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_STMT. This is a reference to a statement which can be used
    to query data from the database.

.. member:: dpiRowid* dpiDataBuffer.asRowid

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_ROWID. This is a reference to a rowid which is used to
    uniquely identify a row in a table in the database.

.. member:: dpiVector* dpiDataBuffer.asVector

    Value that is used when the native type that is being used is
    DPI_NATIVE_TYPE_VECTOR. This is a reference to a vector value which can be
    used for reading and writing the data that belongs to it.
