.. _dpiXid:

ODPI-C Structure dpiXid
-------------------------------

This structure is used for identifying a TPC (two-phase commit) transaction. It
is used by the TPC functions :func:`dpiConn_tpcBegin()`,
:func:`dpiConn_tpcCommit()`, :func:`dpiConn_tpcEnd()`,
:func:`dpiConn_tpcForget()`, :func:`dpiConn_tpcPrepare()` and
:func:`dpiConn_tpcRollback()`.

.. member:: long dpiXid.formatId

    An integer identifying the format the transaction id is expected to take.
    A value of -1 implies that the XID value is NULL.

.. member:: const char* dpiXid.globalTransactionId

    Specifies the global transaction identifier of the XID.

.. member:: uint32_t dpiXid.globalTransactionIdLength

    Specifies the length of the :data:`dpiXid.globalTransactionId` member, in
    bytes. The maximum supported length is 64 bytes.

.. member:: const char* dpiXid.branchQualifier

    Specifies the branch qualifier of the XID.

.. member:: uint32_t dpiXid.branchQualifierLength

    Specifies the length of the :data:`dpiXid.branchQualifier` member, in
    bytes. The maximum supported length is 64 bytes.
