.. _dpiSodaDbFunctions:

ODPI-C SODA Database Functions
------------------------------

SODA database handles are used to represent connections to the SODA database.
They are created by calling the function :func:`dpiConn_getSodaDb()`. They are
destroyed when the last reference is released by calling
:func:`dpiSodaDb_release()`.

SODA support in ODPI-C is in Preview status and should not be used in
production. It will be supported with a future version of Oracle Client
libraries.

.. function:: int dpiSodaDb_addRef(dpiSodaDb \*db)

    Adds a reference to the SODA database. This is intended for situations
    where a reference to the SODA database needs to be maintained independently
    of the reference returned when the database was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **db** [IN] -- the database to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiSodaDb_createCollection(dpiSodaDb \*db, \
        const char \*name, uint32_t nameLength, const char \*metadata, \
        uint32_t metadataLength, uint32_t flags, dpiSodaColl \**coll)

    Creates a new SODA collection if a collection by that name does not exist.
    If a collection by that name already exists, the collection is opened if
    the metadata of the collection is equivalent to the supplied metadata;
    otherwise, an error is returned.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    NOTE: the creation of the collection is performed using an autonomous
    transaction. Any current transaction is left unchanged.

    **db** [IN] -- a reference to the database in which the SODA collection is
    to created. If the reference is NULL or invalid an error is returned.

    **name** [IN] -- the name of the collection which is to be created or
    opened. NOTE: the name is case sensitive.

    **nameLength** [IN] -- the length of the name parameter, in bytes.

    **metadata** [IN] -- the metadata to use when creating the collection, or
    NULL if default metadata should be used instead.

    **metadataLength** [IN] -- the length of the metadata parameter, in bytes,
    or 0 if metadata is NULL.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **coll** [OUT] -- a pointer to a reference to the newly created or opened
    SODA collection if the function completes successfully. The function
    :func:`dpiSodaColl_release()` should be called when the collection is no
    longer required.


.. function:: int dpiSodaDb_createDocument(dpiSodaDb \*db, const char \*key, \
        uint32_t keyLength, const char \*content, uint32_t contentLength, \
        const char \*mediaType, uint32_t mediaTypeLength, uint32_t flags, \
        dpiSodaDoc \**doc)

    Creates a SODA document that can later be inserted into a collection or
    used to replace an existing document in a collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **db** [IN] -- a reference to the database in which the SODA document is
    going to be inserted or replaced. If the reference is NULL or invalid an
    error is returned.

    **key** [IN] -- the key used to identify this document, or NULL if a key
    should be generated instead (and the collection metadata supports key
    generation).

    **keyLength** [IN] -- the length of the key used to identify the document,
    in bytes, or 0 if the key is NULL.

    **content** [IN] -- the content of the document to create, as a byte
    string. The type of content is controlled by the mediaType parameter. If
    the mediaType parameter is set to NULL or "application/json", the content
    must be a valid UTF-8 or UTF-16 encoded JSON string.

    **contentLength** [IN] -- the length of the document content, in bytes.

    **mediaType** [IN] -- the type of content that is found in the content
    parameter. This value may also be NULL, in which case the default value
    of "application/json" is assumed.

    **mediaTypeLength** [IN] -- the length of the mediaType parameter, in
    bytes, or 0 if the mediaType parameter is NULL.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **doc** [OUT] -- a pointer to a document reference that will be populated
    upon successful completion of this function. Call the function
    :func:`dpiSodaDoc_release()` when it is no longer needed.


.. function:: int dpiSodaDb_freeCollectionNames(dpiSodaDb \*db, \
        dpiSodaCollNames \*names)

    Frees the memory associated with the collection names allocated by the call
    to :func:`dpiSodaDb_getCollectionNames()`. This function should not be
    called without first calling :func:`dpiSodaDb_getCollectionNames()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **db** [IN] -- a reference to the database from which the collection names
    were retrieved.

    **names** [IN] -- a pointer to a structure of type
    :ref:`dpiSodaCollNames<dpiSodaCollNames>` which was previously used in a
    call to :func:`dpiSodaDb_getCollectionNames()`.


.. function:: int dpiSodaDb_getCollections(dpiSodaDb \*db, \
        const char \*startName, uint32_t startNameLength, uint32_t flags, \
        dpiSodaCollCursor \**cursor)

    Return a cursor to iterate over the SODA collections available in the
    database.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **db** [IN] -- a reference to the database to use for iterating over
    available SODA collections. If the reference is NULL or invalid an error is
    returned.

    **startName** [IN] -- a name from which to start iterating over collections
    available in the database or NULL if all collections should be returned.

    **startNameLength** [IN] -- the length of the startName parameter, in
    bytes, or 0 if startName is NULL.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **cursor** [OUT] -- a pointer to a reference to a newly allocated cursor if
    the function completes successfully. The function
    :func:`dpiSodaCollCursor_getNext()` should be used to get the next
    collection from the database and :func:`dpiSodaCollCursor_release()` should
    be used when the cursor is no longer required.


.. function:: int dpiSodaDb_getCollectionNames(dpiSodaDb \*db, \
        const char \*startName, uint32_t startNameLength, uint32_t limit, \
        uint32_t flags, dpiSodaCollNames \*names)

    Return an array of names of SODA collections available in the database.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **db** [IN] -- a reference to the database to use for getting the names of
    available SODA collections. If the reference is NULL or invalid an error is
    returned.

    **startName** [IN] -- the value from which to start getting the names of
    collections available in the database or NULL if the names of all
    collections should be returned.

    **startNameLength** [IN] -- the length of the startName parameter, in
    bytes, or 0 if startName is NULL.

    **limit** [IN] -- the maximum number of collection names to return, or 0 if
    all names matching the criteria should be returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **names** [IN] -- a pointer to structure of type
    :ref:`dpiSodaCollNames<dpiSodaCollNames>` which will be populated upon
    successful completion of this function. A call to the function
    :func:`dpiSodaDb_freeCollectionNames()` should be made once the names of
    the collections returned in this structure are no longer needed.


.. function:: int dpiSodaDb_openCollection(dpiSodaDb \*db, const char \*name, \
        uint32_t nameLength, uint32_t flags, dpiSodaColl \**coll)

    Opens an existing SODA collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **db** [IN] -- a reference to the database in which the SODA collection is
    to opened. If the reference is NULL or invalid an error is returned.

    **name** [IN] -- the name of the collection which is to be opened. NOTE:
    the name is case sensitive.

    **nameLength** [IN] -- the length of the name parameter, in bytes.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **coll** [OUT] -- a pointer to a reference to the newly opened SODA
    collection if the function completes successfully. The function
    :func:`dpiSodaColl_release()` should be called when the collection is no
    longer required. If the collection with the specified name does not exist,
    the returned reference is NULL and no error is returned.


.. function:: int dpiSodaDb_release(dpiSodaDb \*db)

    Releases a reference to the database. A count of the references to the
    database is maintained and when this count reaches zero, the memory
    associated with the database is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **db** [IN] -- the database from which a reference is to be released.
    If the reference is NULL or invalid an error is returned.

