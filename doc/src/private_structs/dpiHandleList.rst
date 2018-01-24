.. _dpiHandleList:

ODPI-C Private Structure dpiHandleList
--------------------------------------

This private structure is used to manage a list of shared handles in a
thread-safe manner. It is only currently used by the structure
:ref:`dpiConn <dpiConn>` to manage the list of open statements and LOBs
associated with the connection. These statements and LOBs are automatically
closed when the connection itself is closed. The functions for managing this
structure can be found in dpiHandleList.c.

.. member:: void \*\*dpiHandleList.handles

    Specifies the array of handles that are being managed by the list. The
    maximum number of handles in the array is specified by the member
    :member:`~dpiHandleList.numSlots`. Empty slots are represented by a NULL
    handle.

.. member:: uint32_t dpiHandleList.numSlots

    Specifies the maximum number of handles stored in the member
    :member:`~dpiHandleList.handles`.

.. member:: uint32_t dpiHandleList.numUsedSlots

    Specifies the actual number of handles in the list.

.. member:: uint32_t dpiHandleList.currentPos

    Specifies the position in the array which will next be searched for an
    empty slot.

.. member:: dpiMutexType dpiHandleList.mutex

    Specifies the mutex used to ensure thread safety when adding handles to the
    list or removing handles from the list.

