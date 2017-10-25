.. _dpiHandlePool:

ODPI-C Private Structure dpiHandlePool
--------------------------------------

This private structure is used to manage a pool of shared handles in a
thread-safe manner. It is only currently used by the structure
:ref:`dpiEnv <dpiEnv>` to manage a pool of OCI error handles. The functions for
managing this structure can be found in dpiHandlePool.c.

.. member:: void \*\*dpiHandlePool.handles

    Specifies the array of handles that are being managed by the pool. The
    maximum number of handles in the array is specified by the member
    :member:`~dpiHandlePool.numSlots`.

.. member:: uint32_t dpiHandlePool.numSlots

    Specifies the maximum number of handles stored in the member
    :member:`~dpiHandlePool.handles`.

.. member:: uint32_t dpiHandlePool.numUsedSlots

    Specifies the actual number of handles in the pool.

.. member:: uint32_t dpiHandlePool.acquirePos

    Specifies the position in the array from where handles are acquired.

.. member:: uint32_t dpiHandlePool.releasePos

    Specifies the position in the array where handles are placed when they are
    released back to the pool.

.. member:: dpiMutexType dpiHandlePool.mutex

    Specifies the mutex used to ensure thread safety when acquiring handles
    from the pool or releasing handles back to the pool.

