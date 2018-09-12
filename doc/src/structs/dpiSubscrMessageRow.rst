.. _dpiSubscrMessageRow:

ODPI-C Structure dpiSubscrMessageRow
------------------------------------

This structure is used for passing information on the rows that were changed
and resulted in the notification message of which this structure is a part.

.. member:: dpiOpCode dpiSubscrMessageRow.operation

    Specifies the operations that took place on the registered query. It will
    be one or more of the values from the enumeration
    :ref:`dpiOpCode<dpiOpCode>`, OR'ed together.

.. member:: const char \* dpiSubscrMessageRow.rowid

    Specifies the rowid of the row that was changed, in the encoding used for
    CHAR data.

.. member:: uint32_t dpiSubscrMessageRow.rowidLength

    Specifies the length of the :member:`dpiSubscrMessageRow.rowid` member, in
    bytes.

