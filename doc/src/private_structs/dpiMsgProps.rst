.. _dpiMsgProps:

ODPI-C Private Structure dpiMsgProps
------------------------------------

This private structure represents the available properties for messages when
using advanced queuing and is available by handle to a calling application or
driver.  The implementation for this type is found in dpiMsgProps.c. Message
properties are created by calling the function :func:`dpiConn_newMsgProps()`
and are destroyed by releasing the last reference when calling the function
:func:`dpiMsgProps_release()`. All of the attributes of the structure
:ref:`dpiBaseType<dpiBaseType>` are included in this structure in addition to
the ones specific to this structure described below.

.. member:: dpiConn \*dpiMsgProps.conn

    Specifies a pointer to the :ref:`dpiConn<dpiConn>` structure which was used
    to create this structure.

.. member:: OCIAqMsgProperties \*dpiMsgProps.handle

    Specifies the OCI message properties handle.

