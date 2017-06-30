.. _dpiDynamicBytesChunk:

ODPI-C Private Structure dpiDynamicBytesChunk
---------------------------------------------

This private structure is used to represent a chunk of data that has been
allocated dynamically for long strings or raw byte strings. These are used for
LONG columns as well as when the calling application wishes to use strings or
raw byte srings directly instead of LOBs. An array of these chunks is found
within the structure :ref:`dpiDynamicBytes<dpiDynamicBytes>`.

.. member:: char \*dpiDynamicBytesChunk.ptr

    Specifies a pointer to the buffer containing the chunk. This value may be
    NULL if no memory has yet been allocated for this chunk.

.. member:: uint32_t dpiDynamicBytesChunk.length

    Specifies the actual length of the data found in the buffer, in bytes. This
    value will be 0 if the buffer pointer is NULL.

.. member:: uint32_t dpiDynamicBytesChunk.allocatedLength

    Specifies the allocated length of the buffer, in bytes. This value will be
    0 if the buffer pointer is NULL.

