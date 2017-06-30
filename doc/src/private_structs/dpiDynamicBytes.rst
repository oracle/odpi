.. _dpiDynamicBytes:

ODPI-C Private Structure dpiDynamicBytes
----------------------------------------

This private structure is used to represent a set of chunks allocated
dynamically. This structure is used for LONG columns as well as when the
calling application wishes to use strings or raw byte strings directly instead
of LOBs.

.. member:: uint32_t dpiDynamicBytes.numChunks

    Specifies the number of chunks that contain valid data.

.. member:: uint32_t dpiDynamicBytes.allocatedChunks

    Specifies the number of chunks that have been allocated.

.. member:: dpiDynamicBytesChunk \*dpiDynamicBytes.chunks

    Specifies a pointer to an array of
    :ref:`dpiDynamicBytesChunk<dpiDynamicBytesChunk>` structures. The array has
    the number of elements identified by the member
    :member:`dpiDynamicBytes.allocatedChunks`. When the number of allocated
    chunks is zero, this value is NULL.

