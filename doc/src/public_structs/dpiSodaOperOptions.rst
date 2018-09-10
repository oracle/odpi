.. _dpiSodaOperOptions:

ODPI-C Public Structure dpiSodaOperOptions
------------------------------------------

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

    Specifies an array of key values which identify documents in the
    collection. The length of this array is defined by the member
    :member:`~dpiSodaOperOptions.numKeys`.

.. member:: uint32_t \*dpiSodaOperOptions.keyLengths

    Specifies an array of key length values. Each element corresponds to the
    length of the key (in bytes) found in the member
    :member:`~dpiSodaOperOptions.keys` at the same element index. The length of
    this array is defined by the member :member:`~dpiSodaOperOptions.numKeys`.

.. member:: const char \*dpiSodaOperOptions.key

    Specifies a key value which identifies a document in the collection.

.. member:: uint32_t dpiSodaOperOptions.keyLength

    Specifies the length of the member :member:`~dpiSodaOperOptions.key`, in
    bytes.

.. member:: const char \*dpiSodaOperOptions.version

    Specifies a version value which limits the documents processed by the
    operation.

.. member:: uint32_t dpiSodaOperOptions.versionLength

    Specifies the length of the member :member:`~dpiSodaOperOptions.version`,
    in bytes.

.. member:: const char \*dpiSodaOperOptions.filter

    Specifies a filter value which limits the documents processed by the
    operation.

.. member:: uint32_t dpiSodaOperOptions.filterLength

    Specifies the length of the member :member:`~dpiSodaOperOptions.filter`,
    in bytes.

.. member:: uint32_t dpiSodaOperOptions.skip

    Specifies the number of documents to skip before processing documents in
    the collection. A value of 0 will result in no documents being skipped.

.. member:: uint32_t dpiSodaOperOptions.limit

    Specifies the maximum number of documents to process in the collection. A
    value of 0 means no limit is enforced.

