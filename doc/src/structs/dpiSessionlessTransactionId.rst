.. _dpiSessionlessTransactionId:

ODPI-C Structure dpiSessionlessTransactionId
--------------------------------------------

This structure is used for identifying a sessionless transaction. It
is used by the functions :func:`dpiConn_beginSessionlessTransaction()`,
:func:`dpiConn_resumeSessionlessTransaction()`, and
:func:`suspendSessionlessTransaction()`.

.. member:: const char* dpiSessionlessTransactionId.value

    Specifies the value of the identifier for the sessionless transaction. This
    value may be up to 64 bytes.

.. member:: uint32_t dpiSessionlessTransactionId.length

    Specifies the length of the :data:`dpiSessionlessTransactionId.value`
    member, in bytes.
