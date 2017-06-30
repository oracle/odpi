.. _dpiDeqOptions:

ODPI-C Private Structure dpiDeqOptions
--------------------------------------

This private structure represents the available options for dequeuing messages
when using advanced queuing and is available by handle to a calling application
or driver. The implementation for this type is found in dpiDeqOptions.c.
Dequeue options are created by calling the function
:func:`dpiConn_newDeqOptions()` and are destroyed by releasing the last
reference when calling the function :func:`dpiDeqOptions_release()`. All of the
attributes of the structure :ref:`dpiBaseType<dpiBaseType>` are included in
this structure in addition to the ones specific to this structure described
below.

.. member:: dpiConn \*dpiDeqOptions.conn

    Specifies a pointer to the :ref:`dpiConn<dpiConn>` structure which was used
    to create this structure.

.. member:: OCIAqDeqOptions \*dpiDeqOptions.handle

    Specifies the OCI dequeue options handle.

