.. _dpiOracleDataBuffer:

ODPI-C Private Union dpiOracleDataBuffer
----------------------------------------

This private union is used to avoid casts. Unlike the data in the union
:ref:`dpiOracleData<dpiOracleData>`, which is intended to be placed on a memory
location that is an array of buffers, this union itself contains the memory
needed to supply the buffer that is being transferred to or from the Oracle
database.

.. member:: int64_t dpiOracleDataBuffer.asInt64

    Specifies a 64-bit integer buffer.

.. member:: uint64_t dpiOracleDataBuffer.asUint64

    Specifies a 64-bit unsigned integer buffer.

.. member:: float dpiOracleDataBuffer.asFloat

    Specifies a floating point number buffer.

.. member:: double dpiOracleDataBuffer.asDouble

    Specifies a double floating point number buffer.

.. member:: dpiOciNumber dpiOracleDataBuffer.asNumber

    Specifies an OCINumber structure buffer.

.. member:: dpiOciDate dpiOracleDataBuffer.asDate

    Specifies an OCIDate structure buffer.

.. member:: boolean dpiOracleDataBuffer.asBoolean

    Specifies a boolean buffer.

.. member:: void \*dpiOracleDataBuffer.asString

    Specifies an OCIString handle buffer.

.. member:: void \*dpiOracleDataBuffer.asTimestamp

    Specifies an OCIDateTime handle buffer.

.. member:: void \*dpiOracleDataBuffer.asLobLocator

    Specifies an OCILobLocator handle buffer.

.. member:: void \*dpiOracleDataBuffer.asRaw

    Specifies a generic pointer which can be used to refer to the buffer as a
    whole.

