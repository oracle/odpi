.. _dpiSodaCollFunctions:

ODPI-C SODA Collection Functions
--------------------------------

SODA collection handles are used to represent SODA collections in the database.
They are created by calling one of the functions
:func:`dpiSodaDb_createCollection()`, :func:`dpiSodaDb_openCollection()` or
:func:`dpiSodaCollCursor_getNext()` and are destroyed when the last reference
is released by calling the function :func:`dpiSodaColl_release()`.

SODA support in ODPI-C is in Preview status and should not be used in
production. It will be supported with a future version of Oracle Client
libraries.

.. function:: int dpiSodaColl_addRef(dpiSodaColl \*coll)

    Adds a reference to the SODA collection. This is intended for situations
    where a reference to the collection needs to be maintained independently of
    the reference returned when the collection was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection to which a reference is to
    be added. If the reference is NULL or invalid an error is returned.


.. function:: int dpiSodaColl_createIndex(dpiSodaColl \*coll, \
        const char \*indexSpec, uint32_t indexSpecLength, uint32_t flags)

    Create an index on the collection, which can improve the performance of
    SODA query-by-examples (QBE). An index is defined by a specification,
    which is a JSON-encoded string that specifies how particular QBE patterns
    are to be indexed for quicker matching. See
    `Overview of SODA indexing
    <https://www.oracle.com/pls/topic/lookup?ctx=dblatest&
    id=GUID-4848E6A0-58A7-44FD-8D6D-A033D0CCF9CB>`__.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection on which the index is to be
    created. If the reference is NULL or invalid an error is returned.

    **indexSpec** [IN] -- the JSON specification of the index as a byte string
    in the encoding used for CHAR data.

    **indexSpecLength** [IN] -- the length of the specification of the index,
    in bytes.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.


.. function:: int dpiSodaColl_drop(dpiSodaColl \*coll, uint32_t flags, \
        int \*isDropped)

    Drops a SODA collection from the database.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection which is to be dropped. If
    the reference is NULL or invalid an error is returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **isDropped** [OUT] -- a pointer to a boolean which indicates whether the
    collection was dropped (1) or not (0). If this information is not needed,
    NULL can also be passed.


.. function:: int dpiSodaColl_dropIndex(dpiSodaColl \*coll, \
        const char \*name, uint32_t nameLength, uint32_t flags, \
        int \*isDropped)

    Drop an index on the collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection on which the index is to be
    dropped. If the reference is NULL or invalid an error is returned.

    **name** [IN] -- the name of the index that is to be dropped as a byte
    string in the encoding used for CHAR data.

    **indexSpecLength** [IN] -- the length of the index name, in bytes.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **isDropped** [OUT] -- a pointer to a boolean which indicates whether the
    specified index was dropped (1) or not (0). If this information is not
    needed, NULL can also be passed.


.. function:: int dpiSodaColl_find(dpiSodaColl \*coll, \
        const dpiSodaOperOptions \*options, uint32_t flags, \
        dpiSodaDocCursor \**cursor)

    Finds all of the documents in the collection that match the specified
    options.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection which is to be searched. If
    the reference is NULL or invalid an error is returned.

    **options** [IN] -- a pointer to a structure of type
    :ref:`dpiSodaOperOptions<dpiSodaOperOptions>` which specifies options for
    restricting the number of documents returned. This value can also be NULL
    which will result in all documents in the collection being returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **cursor** [OUT] -- a pointer to a reference to the cursor that is
    populated if the function completes successfully. The reference
    that is returned should be released using
    :func:`dpiSodaDocCursor_release()` when it is no longer needed.


.. function:: int dpiSodaColl_findOne(dpiSodaColl \*coll, \
        const dpiSodaOperOptions \*options, uint32_t flags, dpiSodaDoc \**doc)

    Finds a single document in the collection that matches the specified
    options.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection which is to be searched. If
    the reference is NULL or invalid an error is returned.

    **options** [IN] -- a pointer to a structure of type
    :ref:`dpiSodaOperOptions<dpiSodaOperOptions>` which specifies options for
    restricting the document that is returned. This value can also be NULL
    which will result in the first document in the collection being returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **doc** [OUT] -- a pointer to a reference to the document that is
    populated if the function completes successfully. The reference
    that is returned should be released using :func:`dpiSodaDoc_release()` when
    it is no longer needed. If no document matches the specified options, the
    value NULL is returned instead.


.. function:: int dpiSodaColl_getDataGuide(dpiSodaColl \*coll, \
        uint32_t flags, dpiSodaDoc \**doc)

    Returns a document containing the data guide for the collection. See
    `JSON Data Guide <https://www.oracle.com/pls/topic/lookup?ctx=dblatest&
    id=GUID-219FC30E-89A7-4189-BC36-7B961A24067C>`__.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection from which the data guide is
    to be retrieved. If the reference is NULL or invalid an error is returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **doc** [OUT] -- a pointer to a reference to the document that is populated
    if the function completes successfully. The document reference that is
    returned should be released using :func:`dpiSodaDoc_release()` when it is
    no longer needed. If no data guide is available because the collection
    contains no documents, then a NULL reference is returned.


.. function:: int dpiSodaColl_getDocCount(dpiSodaColl \*coll, \
        const dpiSodaOperOptions \*options, uint32_t flags, uint64_t \*count)

    Returns the number of documents in the collection that match the specified
    options.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection which is to be searched. If
    the reference is NULL or invalid an error is returned.

    **options** [IN] -- a pointer to a structure of type
    :ref:`dpiSodaOperOptions<dpiSodaOperOptions>` which specifies options for
    restricting the number of documents found. This value can also be NULL
    which will result in all of the documents in the collection being counted.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **count** [OUT] -- a pointer to a count of the number of documents found
    that is populated if the function completes successfully.


.. function:: int dpiSodaColl_getMetadata(dpiSodaColl \*coll, \
        const char \**value, uint32_t \*valueLength)

    Returns the metadata for the collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection from which the metadata is
    to be retrieved. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- a pointer to the collection metadata, as a byte string in
    the encoding used for CHAR data, which will be populated upon successful
    completion of this function. The string returned will remain valid as long
    as a reference to the collection is held.

    **valueLength** [OUT] -- a pointer to the length of the collection
    metadata, in bytes, which will be populated upon successful completion of
    this function.


.. function:: int dpiSodaColl_getName(dpiSodaColl \*coll, \
        const char \**value, uint32_t \*valueLength)

    Returns the name of the collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection from which the name is to be
    retrieved. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- a pointer to the collection name, as a byte string in the
    encoding used for CHAR data, which will be populated upon successful
    completion of this function. The string returned will remain valid as long
    as a reference to the collection is held.

    **valueLength** [OUT] -- a pointer to the length of the collection name, in
    bytes, which will be populated upon successful completion of this function.


.. function:: int dpiSodaColl_insertOne(dpiSodaColl \*coll, dpiSodaDoc \*doc, \
        uint32_t flags, dpiSodaDoc \**insertedDoc)

    Inserts a document into the collection and optionally returns it.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection into which the document is
    to be inserted. If the reference is NULL or invalid an error is returned.

    **doc** [IN] -- a reference to the document which is to be inserted into
    the collection. If the reference is NULL or invalid an error is returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **insertedDoc** [OUT] -- a pointer to a document reference that will be
    populated upon successful completion of this function. Call the function
    :func:`dpiSodaDoc_release()` when it is no longer needed. This parameter
    can also be NULL if this information is not required.


.. function:: int dpiSodaColl_release(dpiSodaColl \*coll)

    Releases a reference to the SODA collection. A count of the references to
    the collection is maintained and when this count reaches zero, the memory
    associated with the collection is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection from which a reference is to
    be released. If the reference is NULL or invalid an error is returned.


.. function:: int dpiSodaColl_remove(dpiSodaColl \*coll, \
        const dpiSodaOperOptions \*options, uint32_t flags, uint64_t \*count)

    Removes all of the documents in the collection that match the specified
    options.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **coll** [IN] -- a reference to the collection from which documents are to
    be removed. If the reference is NULL or invalid an error is returned.

    **options** [IN] -- a pointer to a structure of type
    :ref:`dpiSodaOperOptions<dpiSodaOperOptions>` which specifies options for
    restricting the number of documents removed. This value can also be NULL
    which will result in all documents in the collection being removed.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **count** [OUT] -- a pointer to a count of the number of documents removed
    that is populated if the function completes successfully.


.. function:: int dpiSodaColl_replaceOne(dpiSodaColl \*coll, \
        const dpiSodaOperOptions \*options, dpiSodaDoc \*doc, uint32_t flags, \
        int \*replaced, dpiSodaDoc \**replacedDoc)

    Replaces the single document in the collection matching the given options
    with the new document.

    **coll** [IN] -- a reference to the collection in which the document is to
    be replaced.  If the reference is NULL or invalid an error is returned.

    **options** [IN] -- a pointer to a structure of type
    :ref:`dpiSodaOperOptions<dpiSodaOperOptions>` which identifies options for
    specifying the document which is to be replaced. Currently at least the
    :member:`dpiSodaOperOptions.key` member must be specified or the error
    "ORA-40734: key for the document to replace must be specified using the key
    attribute" will be raised.

    **doc** [IN] -- a reference to the document which is to replace the
    document in the collection found using the provided options. If the
    reference is NULL or invalid an error is returned.

    **flags** [IN] -- one or more of the values from the enumeration
    :ref:`dpiSodaFlags<dpiSodaFlags>`, OR'ed together.

    **replaced** [OUT] -- a pointer to a boolean value indicating if the
    document was replaced (1) or not (0). This parameter may also be NULL if
    this information is not required.

    **replacedDoc** [OUT] -- a pointer to a document reference which will be
    populated when this function returns successfully. Call the function
    :func:`dpiSodaDoc_release()` when the reference is no longer needed. This
    parameter can also be NULL if this information is not required.

