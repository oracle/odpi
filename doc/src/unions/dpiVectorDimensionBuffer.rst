.. _dpiVectorDimensionBuffer:

ODPI-C Union dpiVectorDimensionBuffer
--------------------------------------

This union is used for passing vector dimensions to and from the database
without requiring casts.

.. member:: void* dpiVectorDimensionBuffer.asPtr

    Raw pointer to dimension buffer.

.. member:: int8_t* dpiVectorDimensionBuffer.asInt8

    Pointer to dimension buffer when vector format is VECTOR_FORMAT_INT8.

.. member:: float* dpiVectorDimensionBuffer.asFloat

    Pointer to dimension buffer when vector format is VECTOR_FORMAT_FLOAT32.

.. member:: double* dpiVectorDimensionBuffer.asDouble

    Pointer to dimension buffer when vector format is VECTOR_FORMAT_FLOAT64.
