.. _dpiEnv:

ODPI-C Private Structure dpiEnv
-------------------------------

This private structure is used to represent the OCI environment. A pointer to
this structure is stored on each handle exposed publicly but it is created only
when a :ref:`dpiPool<dpiPool>` structure is created or when a
:ref:`dpiConn<dpiConn>` structure is created for a standalone connection.
Connections acquired from a session pool share the environment of the pool and
all other handles share the environment from the handle which created them. All
of the attributes are initialized when the environment is created and never
changed after that. The functions for managing the environment are found in
dpiEnv.c.

.. member:: dpiContext \*dpiEnv.context

    Specifies a pointer to the :ref:`dpiContext<dpiContext>` structure which
    was used for the creation of the environment.

.. member:: OCIEnv \*dpiEnv.handle

    Specifies the OCI environment handle.

.. member:: OCIThreadMutex \*dpiEnv.mutex

    Specifies the OCI thread mutex handle used for controlling access to the
    reference count for each handle exposed publicly when the OCI environment
    is using OCI_THREADED mode. If the environment is not using OCI_THREADED
    mode the mutex handle will be NULL.

.. member:: OCIThreadKey \*dpiEnv.threadKey

    Specifies the OCI thread key handle used for storing error structures in a
    thread safe manner when the OCI environment is using OCI_THREADED mode. If
    the environment is not using OCI_THREADED mode the thread key handle will
    be null.

.. member:: OCIError \*dpiEnv.errorHandle

    Specifies the OCI error handle used for all errors when the environment is
    not in OCI_THREADED mode. When the environment is in OCI_THREADED mode, the
    error handle is only used for looking up the thread specific error handle.

.. member:: char dpiEnv.encoding[]

    The encoding used for CHAR data, as a null-terminated ASCII string.

.. member:: int32_t dpiEnv.maxBytesPerCharacter

    The maximum number of bytes required for each character in the encoding
    used for CHAR data. This value is used when calculating the size of
    buffers required when lengths in characters are provided.

.. member:: uint16_t dpiEnv.charsetId

    The Oracle character set id used for CHAR data.

.. member:: char dpiEnv.nencoding[]

    The encoding used for NCHAR data, as a null-terminated ASCII string.

.. member:: int32_t dpiEnv.nmaxBytesPerCharacter

    The maximum number of bytes required for each character in the encoding
    used for NCHAR data. Since this information is not directly available
    from Oracle it is only accurate if the encodings used for CHAR and NCHAR
    data are identical or one of ASCII or UTF-8; otherwise a value of 4 is
    assumed. This value is used when calculating the size of buffers required
    when lengths in characters are provided.

.. member:: uint16_t dpiEnv.ncharsetId

    The Oracle character set id used for NCHAR data.

.. member:: const char \*dpiEnv.numberToStringFormat

    Specifies the format used to convert numbers to strings.

.. member:: uint32_t dpiEnv.numberToStringFormatLength

    Specifies the length of the :member:`dpiEnv.numberToStringFormat` member,
    in bytes.

.. member:: const char \*dpiEnv.numberFromStringFormat

    Specifies the format used to convert to numbers from strings.

.. member:: uint32_t dpiEnv.numberFromStringFormatLength

    Specifies the length of the :member:`dpiEnv.numberFromStringFormat` member,
    in bytes.

.. member:: const char \*dpiEnv.nlsNumericChars

    Specifies the NLS numeric characters value used for converting numbers to
    strings.

.. member:: uint32_t dpiEnv.nlsNumericCharsLength

    Specifies the length of the :member:`dpiEnv.nlsNumericChars` member,
    in bytes.

.. member:: OCIDateTime \*dpiEnv.baseDate

    Specifies the base date (midnight on January 1, 1970 UTC) used for
    converting timestamps from Oracle into a number representing the number of
    seconds since the Unix "epoch".

.. member:: int dpiEnv.threaded

    Specifies whether the environment is in OCI_THREADED mode (1) or not (0).

