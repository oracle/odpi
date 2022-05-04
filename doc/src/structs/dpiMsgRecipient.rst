.. _dpiMsgRecipient:

ODPI-C Structure dpiMsgRecipient
--------------------------------

This structure is used to set recipients for an AQ message.

A list of recipient names can be associated with a message at the time
a message is enqueued. This allows a limited set of recipients to
dequeue each message. The recipient list associated with the message
overrides the queue subscriber list, if there is one. The recipient
names need not be in the subscriber list but can be, if desired.

To dequeue a message, the consumername attribute can be set to one of
the recipient names. The original message recipient list is not
available on dequeued messages. All recipients have to dequeue a
message before it gets removed from the queue.

Subscribing to a queue is like subscribing to a magazine: each
subscriber can dequeue all the messages placed into a specific queue,
just as each magazine subscriber has access to all its articles. Being
a recipient, however, is like getting a letter: each recipient is a
designated target of a particular message.

.. member:: const char* dpiMsgRecipient.name

    Specifies the name of the message recipient, in the encoding used for
    CHAR data.

.. member:: uint32_t dpiMsgRecipient.nameLength

    Specifies the length of the :member:`dpiMsgRecipient.name` member, in
    bytes.
