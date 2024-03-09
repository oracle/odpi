.. _dpiSodaDocFunctions:

ODPI-C SODA Document Functions
------------------------------

SODA document handles are used to represent SODA documents in the database.
They are created by calling one of the functions
:func:`dpiSodaDb_createDocument()`, :func:`dpiSodaDocCursor_getNext()`,
:func:`dpiSodaColl_findOne()`, :func:`dpiSodaColl_getDataGuide()`,
:func:`dpiSodaColl_insertMany()`, :func:`dpiSodaColl_insertOne()`,
:func:`dpiSodaColl_replaceOne()` or :func:`dpiSodaColl_save()` and
are destroyed when the last reference is released by calling the function
:func:`dpiSodaDoc_release()`.

See `this tracking issue <https://github.com/oracle/odpi/issues/110>`__ for
known issues with SODA.

.. function:: int dpiSodaDoc_addRef(dpiSodaDoc* doc)

    Adds a reference to the SODA document. This is intended for situations
    where a reference to the document needs to be maintained independently of
    the reference returned when the document was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - The document to which a reference is to be added. If the reference
            is NULL or invalid, an error is returned.

.. function:: int dpiSodaDoc_getContent(dpiSodaDoc* doc, \
        const char** value, uint32_t* valueLength, const char** encoding)

    Returns the content of the document. If the document contains JSON an
    exception will be thrown. Use :func:`dpiSodaDoc_getIsJson()` to determine
    whether to call this function or :func:`dpiSodaDoc_getJsonContent()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document from which the content is to be
            retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the document content, as a byte string, which will be
            populated upon successful comletion of this function. The pointer
            returned will remain valid as long as a reference to the document
            is held.
        * - ``valueLength``
          - OUT
          - A pointer to the length of the document content, in bytes, which
            will be populated upon successful completion of this function.
        * - ``encoding``
          - OUT
          - A pointer to the the encoding in which the content was stored,
            which will be populated upon successful completion of this
            function. If the document content is not "application/json", a
            NULL value will be returned.

.. function:: int dpiSodaDoc_getCreatedOn(dpiSodaDoc* doc, \
        const char** value, uint32_t* valueLength)

    Returns the timestamp when the document was created, as a string, in ISO
    format. This value will be empty if the metadata used to create the
    collection in which the document is found does not support the storage of
    this attribute, or if the document was created using
    :func:`dpiSodaDb_createDocument()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document from which the creation timestamp is
            to be retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the document creation timestamp, as a byte string in
            the encoding used for CHAR data, which will be populated upon
            successful completion of this function. The pointer returned will
            remain valid as long as a reference to the document is held.
        * - ``valueLength``
          - OUT
          - A pointer to the length of the document creation timestamp, in
            bytes, which will be populated upon successful completion of this
            function.

.. function:: int dpiSodaDoc_getIsJson(dpiSodaDoc* doc, int* isJson)

    Returns a boolean value indicating if the document contains JSON or not.
    This method should be used to determine if :func:`dpiSodaDoc_getContent()`
    or :func:`dpiSodaDoc_getJsonContent()` should be called to get the content
    of the document.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document which will be checked to see if it
            contains JSON content. If the reference is NULL or invalid, an
            error is returned.
        * - ``isJson``
          - OUT
          - A pointer to a boolean value which will be populated upon
            successful completion of this function.

.. function:: int dpiSodaDoc_getJsonContent(dpiSodaDoc* doc, dpiJson** value)

    Returns the content of the document. If the document does not contain JSON
    an exception will be thrown. Use :func:`dpiSodaDoc_getIsJson()` to
    determine whether to call this function or :func:`dpiSodaDoc_getContent()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document which will be checked to see if it
            contains JSON content. If the reference is NULL or invalid, an
            error is returned.
        * - ``value``
          - OUT
          - A pointer to a dpiJson reference which will be populated upon
            successful completion of this function. This reference will remain
            valid as long as the SODA document itself is valid.

.. function:: int dpiSodaDoc_getKey(dpiSodaDoc* doc, const char** value, \
        uint32_t* valueLength)

    Returns the key that uniquely identifies the document in the collection.
    This value will be empty if no key was supplied when the document was
    created using :func:`dpiSodaDb_createDocument()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document from which the key is to be retrieved.
            If the reference is NULL or invalid, an error is returned.
        * - ``value``
          - OUT
          - A pointer to the document key, as a byte string in the encoding
            used for CHAR data, which will be populated upon successful
            completion of this function. The pointer returned will remain
            valid as long as a reference to the document is held.
        * - ``valueLength``
          - OUT
          - A pointer to the length of the document key, in bytes, which will
            be populated upon successful completion of this function.

.. function:: int dpiSodaDoc_getLastModified(dpiSodaDoc* doc, \
        const char** value, uint32_t* valueLength)

    Returns the timestamp when the document was last modified, as a string, in
    ISO format. This value will be empty if the metadata used to create the
    collection in which the document is found does not support the storage of
    this attribute, or if the document was created using
    :func:`dpiSodaDb_createDocument()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document from which the last modified timestamp
            is to be retrieved. If the reference is NULL or invalid, an error
            is returned.
        * - ``value``
          - OUT
          - A pointer to the document last modified timestamp, as a byte
            string in the encoding used for CHAR data, which will be populated
            upon successful completion of this function. The pointer returned
            will remain valid as long as a reference to the document is held.
        * - ``valueLength``
          - OUT
          - A pointer to the length of the document last modified timestamp,
            in bytes, which will be populated upon successful completion of
            this function.

.. function:: int dpiSodaDoc_getMediaType(dpiSodaDoc* doc, \
        const char** value, uint32_t* valueLength)

    Returns the media type of the document.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document from which the media type is to be
            retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the media type, as a byte string in the encoding
            used for CHAR data, which will be populated upon successful
            completion of this function. The pointer returned will remain
            valid as long as a reference to the document is held.
        * - ``valueLength``
          - OUT
          - A pointer to the length of the media type, in bytes, which will
            be populated upon successful completion of this function.

.. function:: int dpiSodaDoc_getVersion(dpiSodaDoc* doc, \
        const char** value, uint32_t* valueLength)

    Returns the current version of the document. This value will be empty if
    the metadata used to create the collection in which the document is found
    does not support the storage of this attribute, or if the document was
    created using :func:`dpiSodaDb_createDocument()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - A reference to the document from which the version is to be
            retrieved. If the reference is NULL or invalid, an error is
            returned.
        * - ``value``
          - OUT
          - A pointer to the version, as a byte string in the encoding used
            for CHAR data, which will be populated upon successful completion
            of this function. The pointer returned will remain valid as long
            as a reference to the document is held.
        * - ``valueLength``
          - OUT
          - A pointer to the length of the version, in bytes, which will be
            populated upon successful completion of this function.

.. function:: int dpiSodaDoc_release(dpiSodaDoc* doc)

    Releases a reference to the SODA document. A count of the references to
    the document is maintained and when this count reaches zero, the memory
    associated with the document is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``doc``
          - IN
          - The document from which a reference is to be released. If the
            reference is NULL or invalid, an error is returned.
