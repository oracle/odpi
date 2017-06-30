.. _dpiSubscrCreateParams:

ODPI-C Public Structure dpiSubscrCreateParams
---------------------------------------------

This structure is used for creating subscriptions to messages sent for object
change notification, query change notification or advanced queuing. All members
are initialized to default values using the
:func:`dpiContext_initSubscrCreateParams()` function.

.. member:: dpiSubscrNamespace dpiSubscrCreateParams.subscrNamespace

    Specifies the namespace in which the subscription is created. It is
    expected to be one of the values from the enumeration
    :ref:`dpiSubscrNamespace<dpiSubscrNamespace>`. The default value is
    DPI_SUBSCR_NAMESPACE_DBCHANGE.

.. member:: dpiSubscrProtocol dpiSubscrCreateParams.protocol

    Specifies the protocol used for sending notifications for the subscription.
    It is expected to be one of the values from the enumeration
    :ref:`dpiSubscrProtocol<dpiSubscrProtocol>`. The default value is
    DPI_SUBSCR_PROTO_CALLBACK.

.. member:: dpiSubscrQOS dpiSubscrCreateParams.qos

    Specifies the quality of service flags to use with the subscription. It is
    expected to be one or more of the values from the enumeration
    :ref:`dpiSubscrQOS<dpiSubscrQOS>`, OR'ed together. The default value is to
    have no flags set.

.. member:: dpiOpCode dpiSubscrCreateParams.operations

    Specifies which operations on the registered tables or queries should
    result in notifications. It is expected to be one or more of the values
    from the enumeration :ref:`dpiOpCode<dpiOpCode>`, OR'ed together. The
    default value is DPI_OPCODE_ALL_OPS.

.. member:: uint32_t dpiSubscrCreateParams.portNumber

    Specifies the port number on which to receive notifications. The default
    value is 0, which means that a port number will be selected by the Oracle
    client.

.. member:: uint32_t dpiSubscrCreateParams.timeout

    Specifies the length of time, in seconds, before the subscription is
    unregistered. If the value is 0, the subscription remains active until
    explicitly unregistered. The default value is 0.

.. member:: const char \* dpiSubscrCreateParams.name

    Specifies the name of the subscription, as a byte string in the encoding
    used for CHAR data. This name must be consistent with the namespace
    identified in the :member:`dpiSubscrCreateParams.subscrNamespace` member.
    The default value is NULL.

.. member:: uint32_t dpiSubscrCreateParams.nameLength

    Specifies the length of the :member:`dpiSubscrCreateParams.name` member, in
    bytes. The default value is 0.

.. member:: dpiSubscrCallback dpiSubscrCreateParams.callback

    Specifies the callback that will be called when a notification is sent to
    the subscription, if the :member:`dpiSubscrCreateParams.protocol` member
    is set to DPI_SUBSCR_PROTO_CALLBACK. The callback accepts the following
    arguments:

        **context** -- the value of the
        :member:`dpiSubscrCreateParams.callbackContext` member.

        **message** -- a pointer to the message that is being sent. The message
        is in the form :ref:`dpiSubscrMessage<dpiSubscrMessage>`.

    The default value is NULL. If a callback is specified and a notification is
    sent, this will be performed on a separate thread. If database operations
    are going to take place, ensure that the create mode
    DPI_MODE_CREATE_THREADED is set in the structure
    :ref:`dpiCommonCreateParams<dpiCommonCreateParams>` when creating the
    session pool or standalone connection that will be used in this callback.

.. member:: void \* dpiSubscrCreateParams.callbackContext

    Specifies the value that will be used as the first argument to the callback
    specified in the :member:`dpiSubscrCreateParams.callback` member. The
    default value is NULL.

.. member:: const char \* dpiSubscrCreateParams.recipientName

    Specifies the name of the recipient to which notifications are sent when
    the :member:`dpiSubscrCreateParams.protocol` member is not set to
    DPI_SUBSCR_PROTO_CALLBACK. The value is expected to be a byte string in the
    encoding used for CHAR data. The default value is NULL.

.. member:: uint32_t dpiSubscrCreateParams.recipientNameLength

    Specifies the length of the :member:`dpiSubscrCreateParams.recipientName`
    member, in bytes. The default value is 0.

