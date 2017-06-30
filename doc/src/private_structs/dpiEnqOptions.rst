.. _dpiEnqOptions:

ODPI-C Private Structure dpiEnqOptions
--------------------------------------

This private structure represents the available options for enqueuing messages
when using advanced queuing and is available by handle to a calling application
or driver. The implementation for this type is found in dpiEnqOptions.c.
Enqueue options are created by calling the function
:func:`dpiConn_newEnqOptions()` and are destroyed by releasing the last
reference when calling the function :func:`dpiEnqOptions_release()`. All of the
attributes of the structure :ref:`dpiBaseType<dpiBaseType>` are included in
this structure in addition to the ones specific to this structure described
below.

.. member:: dpiConn \*dpiEnqOptions.conn

    Specifies a pointer to the :ref:`dpiConn<dpiConn>` structure which was used
    to create this structure.

.. member:: OCIAqEnqOptions \*dpiEnqOptions.handle

    Specifies the OCI enqueue options handle.

