.. _dpiBytes:

ODPI-C Structure dpiBytes
-------------------------

This structure is used for passing byte strings to and from the database in
the structure :ref:`dpiData<dpiData>`.

.. member:: char \*dpiBytes.ptr

    Specifies the pointer to the memory allocated by ODPI-C for the variable.
    For strings, data written to this memory should be in the encoding
    appropriate to the type of data being transferred. When data is transferred
    from the database it will be in the correct encoding already.

.. member:: uint32_t dpiBytes.length

    Specifies the length of the byte string, in bytes.

.. member:: const char \*dpiBytes.encoding

    Specifies the encoding for character data. This value is populated when
    data is transferred from the database. It is ignored when data is being
    transferred to the database.

