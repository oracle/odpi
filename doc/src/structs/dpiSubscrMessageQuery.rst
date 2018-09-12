.. _dpiSubscrMessageQuery:

ODPI-C Structure dpiSubscrMessageQuery
--------------------------------------

This structure is used for passing information on query change notification
events and is part of the :ref:`dpiSubscrMessage<dpiSubscrMessage>` structure.

.. member:: uint64_t dpiSubscrMessageQuery.id

    Specifies the id of the query that was registered as part of the
    subscription that generated this notification.

.. member:: dpiOpCode dpiSubscrMessageQuery.operation

    Specifies the operations that took place on the registered query. It will
    be one or more of the values from the enumeration
    :ref:`dpiOpCode<dpiOpCode>`, OR'ed together.

.. member:: dpiSubscrMessageTable \* dpiSubscrMessageQuery.tables

    Specifies a pointer to an array of
    :ref:`dpiSubscrMessageTable<dpiSubscrMessageTable>` structures representing
    the list of tables that were modified by the event which generated this
    notification.

.. member:: uint32_t dpiSubscrMessageQuery.numTables

    Specifies the number of structures available in the
    :member:`dpiSubscrMessageQuery.tables` member.

