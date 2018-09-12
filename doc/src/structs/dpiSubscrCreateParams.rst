.. _dpiSubscrCreateParams:

ODPI-C Structure dpiSubscrCreateParams
--------------------------------------

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

.. member:: const char \* dpiSubscrCreateParams.ipAddress

    Specifies the IP address on which the subscription listens to receive
    notifications. The IP address can be an IPv4 address in dotted decimal
    format such as 192.1.2.34 or an IPv6 address in hexadecimal format such as
    2001:0db8:0000:0000:0217:f2ff:fe4b:4ced. The default value is NULL which
    means that an IP address will be selected by the Oracle client.

.. member:: uint32_t dpiSubscrCreateParams.ipAddressLength

    Specifies the length of the :member:`dpiSubscrCreateParams.ipAddress`
    member, in bytes. The default value is 0.

.. member:: uint8_t dpiSubscrCreateParams.groupingClass

    Specifies the grouping class to use when grouping events. It should be one
    of the values from the enumeration
    :ref:`dpiSubscrGroupingClass<dpiSubscrGroupingClass>`. The default value is
    0. If this value is 0, the groupingValue and groupingType attributes are
    ignored.

.. member:: uint32_t dpiSubscrCreateParams.groupingValue

    Specifies the value to be used in grouping. The meaning of this value
    depends on the value of the groupingClass attribute. For time based
    grouping this value refers to the number of seconds during which events
    will be grouped together. The default value is 0.

.. member:: uint8_t dpiSubscrCreateParams.groupingType

    Specifies the type of grouping to use when grouping events. It should be
    one of the values from the enumeration
    :ref:`dpiSubscrGroupingType<dpiSubscrGroupingType>`. The default value is
    DPI_SUBSCR_GROUPING_TYPE_SUMMARY.

