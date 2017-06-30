.. _dpiSubscrMessage:

ODPI-C Public Structure dpiSubscrMessage
----------------------------------------

This structure is used for passing messages sent by notifications to
subscriptions. It is the second parameter to the callback method specified in
the :ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure.

.. member:: dpiEventType dpiSubscrMessage.eventType

    Specifies the type of event that took place which generated the
    notification. It will be one of the values from the enumeration
    :ref:`dpiEventType<dpiEventType>`.

.. member:: const char \* dpiSubscrMessage.dbName

    Specifies the name of the database which generated the notification, as a
    byte string in the encoding used for CHAR data.

.. member:: uint32_t dpiSubscrMessage.dbNameLength

    Specifies the length of the :member:`dpiSubscrMessage.dbName` member, in
    bytes.

.. member:: dpiSubscrMessageTable \* dpiSubscrMessage.tables

    Specifies a pointer to an array of
    :ref:`dpiSubscrMessageTable<dpiSubscrMessageTable>` structures representing
    the list of tables that were modified and generated this notification. This
    value will be NULL if the value of the :member:`dpiSubscrMessage.eventType`
    member is not equal to DPI_EVENT_OBJCHANGE.

.. member:: uint32_t dpiSubscrMessage.numTables

    Specifies the number of structures available in the
    :member:`dpiSubscrMessage.tables` member.

.. member:: dpiSubscrMessageQuery \* dpiSubscrMessage.queries

    Specifies a pointer to an array of
    :ref:`dpiSubscrMessageQuery<dpiSubscrMessageQuery>` structures representing
    the list of queries that were modified and generated this notification.
    This value will be NULL if the value of the
    :member:`dpiSubscrMessage.eventType` member is not equal to
    DPI_EVENT_QUERYCHANGE.

.. member:: uint32_t dpiSubscrMessage.numQueries

    Specifies the number of structures available in the
    :member:`dpiSubscrMessage.queries` member.

.. member:: dpiErrorInfo \* dpiSubscrMessage.errorInfo

    Specifies a pointer to a :ref:`dpiErrorInfo<dpiErrorInfo>` structure. This
    value will be NULL if no error has taken place. If this value is not NULL
    the other members in this structure may not contain valid values.

