.. _dpiErrorBuffer:

ODPI-C Private Structure dpiErrorBuffer
---------------------------------------

This private structure is used to save error information internally. A separate
structure is stored for each thread using the functions OCIThreadKeyGet() and
OCIThreadKeySet() with a globally created OCI environment handle. It is also
used when getting batch error information with the function
:func:`dpiStmt_getBatchErrors()`. This structure is not used directly but is
always used as part of the structure :ref:`dpiError<dpiError>`.

.. member:: int32_t dpiErrorBuffer.code

    Specifies the OCI error code of the last OCI error that was recorded or 0
    if no OCI error has been recorded.

.. member:: uint16_t dpiErrorBuffer.offset

    Specifies the parse error offset when executing a statement or the index of
    the row which generated the error when getting batch errors. In all other
    cases this value is 0.

.. member:: dpiErrorNum dpiErrorBuffer.errorNum

    Specifies the ODPI-C error number of the last ODPI-C error that was
    recorded or 0 if no ODPI-C error has been recorded.

.. member:: const char \*dpiErrorBuffer.fnName

    The public ODPI-C function name which was called in which the error took
    place. This is a null-terminated ASCII string.

.. member:: const char \*dpiErrorBuffer.action

    The internal action that was being performed when the error took place.
    This is a null-terminated ASCII string.

.. member:: char dpiErrorBuffer.encoding[]

    Specifies the encoding in which the :member:`dpiErrorBuffer.messageBuffer`
    member is encoded as a null-terminated ASCII string.

.. member:: char dpiErrorBuffer.message[]

    Specifies the buffer used for storing error messages.

.. member:: uint32_t dpiErrorBufferLength

    Specifies the length of the message found in the message buffer, in bytes.

.. member:: boolean dpiErrorBuffer.isRecoverable

    Specifies whether the error is recoverable (1) or not (0). This is only
    relevant for OCI errors for Oracle release 12.1 and higher. In all other
    cases the value is 0.

