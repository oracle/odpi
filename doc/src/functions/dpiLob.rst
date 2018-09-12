.. _dpiLobFunctions:

ODPI-C LOB Functions
--------------------

LOB handles are used to represent large objects (CLOB, BLOB, NCLOB, BFILE).
Both persistent and temporary large objects can be represented. LOB handles can
be created by calling the function :func:`dpiConn_newTempLob()` or are created
implicitly when a variable of type DPI_ORACLE_TYPE_CLOB, DPI_ORACLE_TYPE_NCLOB,
DPI_ORACLE_TYPE_BLOB or DPI_ORACLE_TYPE_BFILE is created and are destroyed when
the last reference is released by calling the function
:func:`dpiLob_release()`. They are used for reading and writing data to the
database in smaller pieces than is contained in the large object.

.. function:: int dpiLob_addRef(dpiLob \*lob)

    Adds a reference to the LOB. This is intended for situations where a
    reference to the LOB needs to be maintained independently of the reference
    returned when the LOB was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- the LOB to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiLob_closeResource(dpiLob \*lob)

    Closes the LOB resource. This should be done when a batch of writes has
    been completed so that the indexes associated with the LOB can be updated.
    It should only be performed if a call to function
    :func:`dpiLob_openResource()` has been performed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB which will be closed. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiLob_copy(dpiLob \*lob, dpiLob \**copiedLob)

    Creates an independent copy of a LOB and returns a reference to the newly
    created LOB. This reference should be released as soon as it is no longer
    needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- the LOB which is to be copied. If the reference is NULL or
    invalid an error is returned.

    **copiedLob** [OUT] -- a pointer to a reference to the LOB which is created
    as a copy of the first LOB, which is populated upon successful completion
    of this function.


.. function:: int dpiLob_getBufferSize(dpiLob \*lob, uint64_t sizeInChars, \
        uint64_t \*sizeInBytes)

    Returns the size of the buffer needed to hold the number of characters
    specified for a buffer of the type associated with the LOB. If the LOB does
    not refer to a character LOB the value is returned unchanged.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB in which the buffer is going to be
    used for transferring data to and from Oracle. If the reference is NULL or
    invalid an error is returned.

    **sizeInChars** [IN] -- the number of characters for which a buffer size
    needs to be determined.

    **sizeInBytes** [OUT] -- a pointer to the size in bytes which will be
    populated when the function has completed successfully.


.. function:: int dpiLob_getChunkSize(dpiLob \*lob, uint32_t \*size)

    Returns the chunk size of the internal LOB. Reading and writing to the LOB
    in multiples of this size will improve performance.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB from which the chunk size is to be
    retrieved. If the reference is NULL or invalid an error is returned.

    **size** [OUT] -- a pointer to the chunk size which will be populated when
    this function completes successfully.


.. function:: int dpiLob_getDirectoryAndFileName(dpiLob \*lob, \
        const char \**directoryAlias, uint32_t \*directoryAliasLength, \
        const char \**fileName, uint32_t \*fileNameLength)

    Returns the directory alias name and file name for a BFILE type LOB.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB from which the directory alias name
    and file name are to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **directoryAlias** [OUT] -- a pointer to the name of the directory alias,
    as a byte string in the encoding used for CHAR data, which will be
    populated upon successful completion of this function. The string returned
    will remain valid as long as a reference to the LOB is held.

    **directoryAliasLength** [OUT] -- a pointer to the length of the name of
    the directory alias, in bytes, which will be populated upon successful
    completion of this function.

    **fileName** [OUT] -- a pointer to the name of the file, as a byte string
    in the encoding used for CHAR data, which will be populated upon successful
    completion of this function. The string returned will remain valid as long
    as a reference to the LOB is held.

    **fileNameLength** [OUT] -- a pointer to the length of the name of the
    file, in bytes, which will be populated upon successful completion of this
    function.


.. function:: int dpiLob_getFileExists(dpiLob \*lob, int \*exists)

    Returns a boolean value indicating if the file referenced by the BFILE type
    LOB exists (1) or not (0).

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB which will be checked to see if the
    associated file exists. If the reference is NULL or invalid an error is
    returned.

    **exists** [OUT] -- a pointer to the boolean value which will be populated
    when this function completes successfully.


.. function:: int dpiLob_getIsResourceOpen(dpiLob \*lob, int \*isOpen)

    Returns a boolean value indicating if the LOB resource has been opened by
    making a call to the function :func:`dpiLob_openResource()` (1) or not (0).

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB which will be checked to see if it
    is open. If the reference is NULL or invalid an error is returned.

    **isOpen** [OUT] -- a pointer to the boolean value which will be populated
    when this function completes successfully.


.. function:: int dpiLob_getSize(dpiLob \*lob, uint64_t \*size)

    Returns the size of the data stored in the LOB. For character LOBs the size
    is in characters; for binary LOBs the size is in bytes.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB from which the size will be
    retrieved.  If the reference is NULL or invalid an error is returned.

    **size** [OUT] -- a pointer to the value which will be populated when this
    function completes successfully.


.. function:: int dpiLob_openResource(dpiLob \*lob)

    Opens the LOB resource for writing. This will improve performance when
    writing to the LOB in chunks and there are functional or extensible indexes
    associated with the LOB. If this function is not called, the LOB resource
    will be opened and closed for each write that is performed. A call to the
    function :func:`dpiLob_closeResource()` should be done before performing a
    call to the function :func:`dpiConn_commit()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB which will be opened. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiLob_readBytes(dpiLob \*lob, uint64_t offset, \
        uint64_t amount, char \*value, uint64_t \*valueLength)

    Reads data from the LOB at the specified offset into the provided buffer.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- the LOB from which data is to be read. If the reference is
    NULL or invalid an error is returned.

    **offset** [IN] -- the offset into the LOB data from which to start
    reading. The first position is 1. For character LOBs this represents the
    number of characters from the beginning of the LOB; for binary LOBS, this
    represents the number of bytes from the beginning of the LOB.

    **amount** [IN] -- the maximum number of characters (for character LOBs) or
    the maximum number of bytes (for binary LOBs) that will be read from the
    LOB.

    **value** [IN] -- the buffer into which the data is read. It is assumed to
    contain the number of bytes specified in the valueLength parameter.

    **valueLength** [IN/OUT] -- a pointer to the size of the value. When this
    function is called it must contain the maximum number of bytes in the
    buffer specified by the value parameter. After the function is completed
    successfully it will contain the actual number of bytes read into the
    buffer.


.. function:: int dpiLob_release(dpiLob \*lob)

    Releases a reference to the LOB. A count of the references to the LOB is
    maintained and when this count reaches zero, the memory associated with the
    LOB is freed. The LOB is also closed unless that has already taken place
    using the function :func:`dpiLob_close()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- the LOB from which a reference is to be released. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiLob_setDirectoryAndFileName(dpiLob \*lob, \
        const char \*directoryAlias, uint32_t directoryAliasLength, \
        const char \*fileName, uint32_t fileNameLength)

    Sets the directory alias name and file name for a BFILE type LOB.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- a reference to the LOB on which the directory alias name
    and file name are to be set. If the reference is NULL or invalid an error
    is returned.

    **directoryAlias** [IN] -- the name of the directory alias, as a byte
    string in the encoding used for CHAR data.

    **directoryAliasLength** [IN] -- the length of the directoryAlias
    parameter, in bytes.

    **fileName** [IN] -- the name of the file, as a byte string in the encoding
    used for CHAR data.

    **fileNameLength** [IN] -- the length of the fileName parameter, in bytes.


.. function:: int dpiLob_setFromBytes(dpiLob \*lob, const char \*value, \
        uint64_t valueLength)

    Replaces all of the data in the LOB with the contents of the provided
    buffer. The LOB will first be cleared and then the provided data will be
    written.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- the LOB to which data is to be written. If the reference is
    NULL or invalid an error is returned.

    **value** [IN] -- the buffer from which the data is written.

    **valueLength** [IN] -- the number of bytes which will be read from the
    buffer and written to the LOB.


.. function:: int dpiLob_trim(dpiLob \*lob, uint64_t newSize)

    Trims the data in the LOB so that it only contains the specified amount of
    data.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- the LOB which will be trimmed. If the reference is NULL or
    invalid an error is returned.

    **newSize** [IN] -- the new size of the data in the LOB. For character LOBs
    this value is in characters; for binary LOBs this value is in bytes.


.. function:: int dpiLob_writeBytes(dpiLob \*lob, uint64_t offset, \
        const char \*value, uint64_t valueLength)

    Write data to the LOB at the specified offset using the provided buffer as
    the source. If multiple calls to this function are planned, the LOB should
    first be opened using the function :func:`dpiLob_open()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **lob** [IN] -- the LOB to which data is to be written. If the reference is
    NULL or invalid an error is returned.

    **offset** [IN] -- the offset into the LOB data from which to start
    writing. The first position is 1. For character LOBs this represents the
    number of characters from the beginning of the LOB; for binary LOBS, this
    represents the number of bytes from the beginning of the LOB.

    **value** [IN] -- the buffer from which the data is written.

    **valueLength** [IN] -- the number of bytes which will be read from the
    buffer and written to the LOB.

