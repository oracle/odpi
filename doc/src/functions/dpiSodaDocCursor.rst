.. _dpiSodaDocCursorFunctions:

ODPI-C SODA Document Cursor Functions
-------------------------------------

SODA document cursor handles are used to represent SODA document cursors.
They are created by calling the function :func:`dpiSodaColl_find()`
and are destroyed when the last reference is released by calling the function
:func:`dpiSodaDocCursor_release()`.

SODA support in ODPI-C is in Preview status and should not be used in
production. It will be supported with a future version of Oracle Client
libraries.

.. function:: int dpiSodaDocCursor_addRef(dpiSodaDocCursor \*cursor)

    Adds a reference to the SODA document cursor. This is intended for
    situations where a reference to the cursor needs to be maintained
    independently of the reference returned when the cursor was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **cursor** [IN] -- the cursor to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiSodaDocCursor_close(dpiSodaDocCursor \*cursor)

    Closes the cursor and makes it unusable for further work immediately,
    rather than when the reference count reaches zero.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **cursor** [IN] -- a reference to the cursor which is to be closed. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiSodaDocCursor_getNext(dpiSodaDocCursor \*cursor, \
        uint32_t flags, dpiSodaDoc \**doc)

    Gets the next document from the cursor, if there is one.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **cursor** [IN] -- the cursor from which the next document is to be
    retrieved. The the reference is NULL or invalid an error is returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together. Only the value
    DPI_SODA_FLAGS_DEFAULT is currently supported.

    **doc** [OUT] -- a pointer to a reference to the next document in the
    cursor, if one exists. If no further documents are available from the
    cursor, NULL is returned instead. The function :func:`dpiSodaDoc_release()`
    should be called when the document is no longer required.


.. function:: int dpiSodaDocCursor_release(dpiSodaDocCursor \*cursor)

    Releases a reference to the SODA document cursor. A count of the
    references to the cursor is maintained and when this count reaches zero,
    the memory associated with the cursor is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **cursor** [IN] -- the cursor from which a reference is to be released.
    If the reference is NULL or invalid an error is returned.

