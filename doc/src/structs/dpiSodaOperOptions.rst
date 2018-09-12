.. _dpiSodaOperOptions:

ODPI-C Structure dpiSodaOperOptions
-----------------------------------

This structure is used for passing search criteria to a number of SODA
operations. It is initialized using the function
:func:`dpiContext_initSodaOperOptions()` and used by the functions
:func:`dpiSodaColl_find()`, :func:`dpiSodaColl_findOne()`,
:func:`dpiSodaColl_getDocCount()`, :func:`dpiSodaColl_remove()`
and :func:`dpiSodaColl_replaceOne()`.

.. member:: uint32_t dpiSodaOperOptions.numKeys

    Specifies the number of elements in the members
    :member:`~dpiSodaOperOptions.keys` and
    :member:`~dpiSodaOperOptions.keyLengths`.

.. member:: const char \*\*dpiSodaOperOptions.keys

    Specifies an array of key values which documents in the collection must
    have in order to be processed by the operation. The length of this array
    is defined by the member :member:`~dpiSodaOperOptions.numKeys`.

.. member:: uint32_t \*dpiSodaOperOptions.keyLengths

    Specifies an array of key length values. Each element corresponds to the
    length of the key (in bytes) found in the member
    :member:`~dpiSodaOperOptions.keys` at the same element index. The length of
    this array is defined by the member :member:`~dpiSodaOperOptions.numKeys`.

.. member:: const char \*dpiSodaOperOptions.key

    Specifies the key which the document must have in order to be processed by
    the operation.

.. member:: uint32_t dpiSodaOperOptions.keyLength

    Specifies the length of the member :member:`~dpiSodaOperOptions.key`, in
    bytes.

.. member:: const char \*dpiSodaOperOptions.version

    Specifies the document version that documents must have in order to be
    processed by the operation. This is typically used in conjunction with
    :member:`~dpiSodaOperOptions.key` and allows for opportunistic locking,
    so that operations do not affect a document that someone else has already
    modified.

.. member:: uint32_t dpiSodaOperOptions.versionLength

    Specifies the length of the member :member:`~dpiSodaOperOptions.version`,
    in bytes.

.. member:: const char \*dpiSodaOperOptions.filter

    Specifies a filter value which limits the documents processed by the
    operation. See `Overview of SODA Filter Specifications (QBEs)
    <https://www.oracle.com/pls/topic/lookup?ctx=dblatest&
    id=GUID-CB09C4E3-BBB1-40DC-88A8-8417821B0FBE>`__.

.. member:: uint32_t dpiSodaOperOptions.filterLength

    Specifies the length of the member :member:`~dpiSodaOperOptions.filter`,
    in bytes.

.. member:: uint32_t dpiSodaOperOptions.skip

    Specifies the number of documents to skip before processing documents in
    the collection. A value of 0 will result in no documents being skipped.

.. member:: uint32_t dpiSodaOperOptions.limit

    Specifies the maximum number of documents to process in the collection. A
    value of 0 means no limit is enforced.

