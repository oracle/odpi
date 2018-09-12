.. _dpiErrorInfo:

ODPI-C Structure dpiErrorInfo
-----------------------------

This structure is used for transferring error information from ODPI-C. All of the
strings referenced here may become invalid as soon as the next ODPI-C call is
made.

.. member:: int32_t dpiErrorInfo.code

    The OCI error code if an OCI error has taken place. If no OCI error has
    taken place the value is 0.

.. member:: uint16_t dpiErrorInfo.offset

    The parse error offset (in bytes) when executing a statement or the row
    offset when fetching batch error information. If neither of these cases are
    true, the value is 0.

.. member:: const char \*dpiErrorInfo.message

    The error message as a byte string in the encoding specified by the
    :member:`dpiErrorInfo.encoding` member.

.. member:: uint32_t dpiErrorInfo.messageLength

    The length of the :member:`dpiErrorInfo.message` member, in bytes.

.. member:: const char \*dpiErrorInfo.encoding

    The encoding in which the error message is encoded as a null-terminated
    string. For OCI errors this is the CHAR encoding used when the connection
    was created. For ODPI-C specific errors this is UTF-8.

.. member:: const char \*dpiErrorInfo.fnName

    The public ODPI-C function name which was called in which the error took
    place. This is a null-terminated ASCII string.

.. member:: const char \*dpiErrorInfo.action

    The internal action that was being performed when the error took place.
    This is a null-terminated ASCII string.

.. member:: const char \*dpiErrorInfo.sqlState

    The SQLSTATE code associated with the error. This is a 5 character
    null-terminated string.

.. member:: int dpiErrorInfo.isRecoverable

    A boolean value indicating if the error is recoverable. This member always
    has a value of 0 unless both client and server are at release 12.1 or
    higher.

