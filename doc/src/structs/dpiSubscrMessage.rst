.. _dpiSubscrMessage:

ODPI-C Structure dpiSubscrMessage
---------------------------------

This structure is used for passing messages sent by notifications to
subscriptions. It is the second parameter to the callback method specified in
the :ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure.

.. member:: dpiEventType dpiSubscrMessage.eventType

    Specifies the type of event that took place which generated the
    notification. It will be one of the values from the enumeration
    :ref:`dpiEventType<dpiEventType>`.

.. member:: const char \* dpiSubscrMessage.dbName

    Specifies the name of the database which generated the notification, as a
    byte string in the encoding used for CHAR data. This value is not
    populated when the :member:`~dpiSubscrMessage.eventType` member is set to
    the values DPI_EVENT_AQ or DPI_EVENT_DEREG.

.. member:: uint32_t dpiSubscrMessage.dbNameLength

    Specifies the length of the :member:`~dpiSubscrMessage.dbName` member, in
    bytes.

.. member:: dpiSubscrMessageTable \* dpiSubscrMessage.tables

    Specifies a pointer to an array of
    :ref:`dpiSubscrMessageTable<dpiSubscrMessageTable>` structures representing
    the list of tables that were modified and generated this notification. This
    value is only populated when the value of the
    :member:`~dpiSubscrMessage.eventType` member is set to DPI_EVENT_OBJCHANGE.

.. member:: uint32_t dpiSubscrMessage.numTables

    Specifies the number of structures available in the
    :member:`~dpiSubscrMessage.tables` member.

.. member:: dpiSubscrMessageQuery \* dpiSubscrMessage.queries

    Specifies a pointer to an array of
    :ref:`dpiSubscrMessageQuery<dpiSubscrMessageQuery>` structures representing
    the list of queries that were modified and generated this notification.
    This value is only populated when the value of the
    :member:`~dpiSubscrMessage.eventType` member is set to
    DPI_EVENT_QUERYCHANGE.

.. member:: uint32_t dpiSubscrMessage.numQueries

    Specifies the number of structures available in the
    :member:`~dpiSubscrMessage.queries` member.

.. member:: dpiErrorInfo \* dpiSubscrMessage.errorInfo

    Specifies a pointer to a :ref:`dpiErrorInfo<dpiErrorInfo>` structure. This
    value will be NULL if no error has taken place. If this value is not NULL
    the other members in this structure will not be populated.

.. member:: const void \* dpiSubscrMessage.txId

    Specifies the id of the transaction which generated the notification, as a
    series of bytes. This value is not populated when the
    :member:`~dpiSubscrMessage.eventType` member is set to the values
    DPI_EVENT_AQ or DPI_EVENT_DEREG.

.. member:: uint32_t dpiSubscrMessage.txIdLength

    Specifies the length of the :member:`~dpiSubscrMessage.txId` member, in
    bytes.

.. member:: int dpiSubscrMessage.registered

    Specifies whether the subscription is registered with the database (1) or
    not (0). The subscription is automatically deregistered with the database
    when the subscription timeout value is reached or when the first
    notification is sent (when the quality of service flag
    DPI_SUBSCR_QOS_DEREG_NFY is used).

.. member:: const char \* dpiSubscrMessage.queueName

    Specifies the name of the queue which has messages available to dequeue, as
    a byte string in the encoding used for CHAR data. This value is only
    populated when the :member:`~dpiSubscrMessage.eventType` member is set to
    the value DPI_EVENT_AQ.

.. member:: uint32_t dpiSubscrMessage.queueNameLength

    Specifies the length of the :member:`~dpiSubscrMessage.queueName` member,
    in bytes.

.. member:: const char \* dpiSubscrMessage.consumerName

    Specifies the consumer name of the queue which has messages available to
    dequeue, as a byte string in the encoding used for CHAR data. This value is
    only populated when the :member:`~dpiSubscrMessage.eventType` member is set
    to the value DPI_EVENT_AQ. It is also only populated if the queue that has
    messages to dequeue is a multi-consumer queue.

.. member:: uint32_t dpiSubscrMessage.consumerNameLength

    Specifies the length of the :member:`~dpiSubscrMessage.consumerName`
    member, in bytes.

