.. _dpiSubscr:

ODPI-C Private Structure dpiSubscr
----------------------------------

This private structure represents subscriptions to events such as continuous
query notification and object change notification and is available by handle to
a calling application or driver. The implementation for this type is found in
dpiSubscr.c. Subscriptions are created by calling the function
:func:`dpiConn_newSubscription()` and are destroyed by calling the function
:func:`dpiSubscr_close()` or by releasing the last reference when calling the
function :func:`dpiSubscr_release()`. All of the attributes of the structure
:ref:`dpiBaseType<dpiBaseType>` are included in this structure in addition to
the ones specific to this structure described below.

.. member:: dpiConn \*dpiSubscr.conn

    Specifies a pointer to the :ref:`dpiConn<dpiConn>` structure which was used
    to create this structure.

.. member:: OCISubscription \*dpiSubscr.handle

    Specifies the OCI subscription handle.

.. member:: dpiSubscrQOS \*dpiSubscr.qos

    Specifies the quality of service flags used by the subscription. This will
    be one or more of the values from the enumeration
    :ref:`dpiSubscrQOS<dpiSubscrQOS>`, OR'ed together.

.. member:: dpiSubscrCallback dpiSubscr.callback

    Specifies the callback that will be called when an event is propagated for
    this subscription. For more information see
    :member:`dpiSubscrCreateParams.callback`.

.. member:: void \*dpiSubscr.callbackContext

    Specifies the user-defined callback context pointer that is passed to the
    callback when it is executed. For more information see
    :member:`dpiSubscrCreateParams.callbackContext`.

