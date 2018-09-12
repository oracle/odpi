.. _dpiSubscrMessageTable:

ODPI-C Structure dpiSubscrMessageTable
--------------------------------------

This structure is used for passing information on the tables that were changed
and resulted in the notification message of which this structure is a part.

.. member:: dpiOpCode dpiSubscrMessageTable.operation

    Specifies the operations that took place on the modified table. It will
    be one or more of the values from the enumeration
    :ref:`dpiOpCode<dpiOpCode>`, OR'ed together.

.. member:: const char \* dpiSubscrMessageRow.name

    Specifies the name of the table that was changed, in the encoding used for
    CHAR data.

.. member:: uint32_t dpiSubscrMessageRow.nameLength

    Specifies the length of the :member:`dpiSubscrMessageRow.name` member, in
    bytes.

.. member:: dpiSubscrMessageRow \* dpiSubscrMessageTable.rows

    Specifies a pointer to an array of
    :ref:`dpiSubscrMessageRow<dpiSubscrMessageRow>` structures representing the
    list of rows that were modified by the event which generated this
    notification.

.. member:: uint32_t dpiSubscrMessageTable.numRows

    Specifies the number of structures available in the
    :member:`dpiSubscrMessageTable.rows` member.

