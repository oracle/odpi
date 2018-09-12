.. _dpiRowidFunctions:

ODPI-C Rowid Functions
----------------------

Rowid handles are used to represent the unique identifier of a row in the
database. They cannot be created or set directly but are created implicitly
when a variable of type DPI_ORACLE_TYPE_ROWID is created. They are destroyed
when the last reference is released by a call to the function
:func:`dpiRowid_release()`.

.. function:: int dpiRowid_addRef(dpiRowid \*rowid)

    Adds a reference to the rowid. This is intended for situations where a
    reference to the rowid needs to be maintained independently of the
    reference returned when the rowid was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **rowid** [IN] -- the rowid to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiRowid_getStringValue(dpiRowid \*rowid, \
        const char \**value, uint32_t \*valueLength)

    Returns the sting (base64) representation of the rowid.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **rowid** [IN] -- the rowid from which the string representation is to be
    returned. If the reference is NULL or invalid an error is returned.

    **value** [OUT] -- a pointer to the value as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. The string returned will remain valid as long as a reference
    is held to the rowid.

    **valueLength** [OUT] -- a pointer to the length of the value parameter, in
    bytes, which will be populated upon successful completion of this function.


.. function:: int dpiRowid_release(dpiRowid \*rowid)

    Releases a reference to the rowid. A count of the references to the rowid
    is maintained and when this count reaches zero, the memory associated with
    the rowid is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **rowid** [IN] -- the rowid from which a reference is to be released. If
    the reference is NULL or invalid an error is returned.

