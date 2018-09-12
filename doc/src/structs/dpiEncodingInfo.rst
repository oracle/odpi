.. _dpiEncodingInfo:

ODPI-C Structure dpiEncodingInfo
--------------------------------

This structure is used for transferring encoding information from ODPI-C. All of
the information here remains valid as long as a reference is held to the
standalone connection or session pool from which the information was taken.

.. member:: const char \*dpiEncodingInfo.encoding

    The encoding used for CHAR data, as a null-terminated ASCII string.

.. member:: int32_t dpiEncodingInfo.maxBytesPerCharacter

    The maximum number of bytes required for each character in the encoding
    used for CHAR data. This value is used when calculating the size of
    buffers required when lengths in characters are provided.

.. member:: const char \*dpiEncodingInfo.nencoding

    The encoding used for NCHAR data, as a null-terminated ASCII string.

.. member:: int32_t dpiEncodingInfo.nmaxBytesPerCharacter

    The maximum number of bytes required for each character in the encoding
    used for NCHAR data. Since this information is not directly available
    from Oracle it is only accurate if the encodings used for CHAR and NCHAR
    data are identical or one of ASCII or UTF-8; otherwise a value of 4 is
    assumed. This value is used when calculating the size of buffers required
    when lengths in characters are provided.

