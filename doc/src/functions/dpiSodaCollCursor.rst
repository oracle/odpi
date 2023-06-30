.. _dpiSodaCollCursorFunctions:

ODPI-C SODA Collection Cursor Functions
---------------------------------------

SODA collection cursor handles are used to represent SODA collection cursors.
They are created by calling the function :func:`dpiSodaDb_getCollections()`
and are destroyed when the last reference is released by calling the function
:func:`dpiSodaCollCursor_release()`.

See `this tracking issue <https://github.com/oracle/odpi/issues/110>`__ for
known issues with SODA.

.. function:: int dpiSodaCollCursor_addRef(dpiSodaCollCursor* cursor)

    Adds a reference to the SODA collection cursor. This is intended for
    situations where a reference to the cursor needs to be maintained
    independently of the reference returned when the cursor was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``cursor``
          - IN
          - The cursor to which a reference is to be added. If the reference
            is NULL or invalid, an error is returned.

.. function:: int dpiSodaCollCursor_close(dpiSodaCollCursor* cursor)

    Closes the cursor and makes it unusable for further work immediately,
    rather than when the reference count reaches zero.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``cursor``
          - IN
          - A reference to the cursor which is to be closed. If the reference
            is NULL or invalid, an error is returned.

.. function:: int dpiSodaCollCursor_getNext(dpiSodaCollCursor* cursor, \
        uint32_t flags, dpiSodaColl** coll)

    Gets the next collection from the cursor, if there is one.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``cursor``
          - IN
          - The cursor from which the next collection is to be retrieved. The
            reference is NULL or invalid, an error is returned.
        * - ``flags``
          - IN
          - One or more of the values from the enumeration
            :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together. Only the value
            DPI_SODA_FLAGS_DEFAULT is currently supported.
        * - ``coll``
          - OUT
          - A pointer to a reference to the next collection in the cursor, if
            one exists. If no further collections are available from the
            cursor, NULL is returned instead. The function
            :func:`dpiSodaColl_release()` should be called when the collection
            is no longer required.

.. function:: int dpiSodaCollCursor_release(dpiSodaCollCursor* cursor)

    Releases a reference to the SODA collection cursor. A count of the
    references to the cursor is maintained and when this count reaches zero,
    the memory associated with the cursor is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``cursor``
          - IN
          - The cursor from which a reference is to be released. If the
            reference is NULL or invalid, an error is returned.
