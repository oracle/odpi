.. _dpiObjectFunctions:

ODPI-C Object Functions
-----------------------

Object handles are used to represent instances of the types created by the SQL
command CREATE OR REPLACE TYPE. They are created by calling the function
:func:`dpiObjectType_createObject()` or calling the function
:func:`dpiObject_copy()` or implicitly by creating a variable of the type
DPI_ORACLE_TYPE_OBJECT. The are destroyed when the last reference is released
by calling the function :func:`dpiObject_release()`.

.. function:: int dpiObject_addRef(dpiObject \*obj)

    Adds a reference to the object. This is intended for situations where a
    reference to the object needs to be maintained independently of the
    reference returned when the object was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiObject_appendElement(dpiObject \*obj, \
        dpiNativeTypeNum nativeTypeNum, dpiData \*value)

    Sets the value of the element found at the specified index.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object to which the value is to be appended. If the
    reference is NULL or invalid an error is returned. Likewise, if the object
    does not refer to a collection an error is returned.

    **nativeTypeNum** [IN] -- the native type of the data that is to be
    appended. It should be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **value** [IN] -- a pointer to a :ref:`dpiData<dpiData>` structure which
    contains the value of the element to append to the collection.


.. function:: int dpiObject_copy(dpiObject \*obj, dpiObject \**copiedObj)

    Creates an independent copy of an object and returns a reference to the
    newly created object. This reference should be released as soon as it is
    no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object which is to be copied. If the reference is NULL
    or invalid an error is returned.

    **copiedObj** [OUT] -- a pointer to a reference to the object which is
    created as a copy of the first object, which is populated upon successful
    completion of this function.


.. function:: int dpiObject_deleteElementByIndex(dpiObject \*obj, \
        int32_t index)

    Deletes an element from the collection. Note that the position ordinals of
    the remaining elements are not changed. The delete operation creates
    *holes* in the collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the element is to be deleted. If the
    reference is NULL or invalid an error is returned. Likewise, if the object
    does not refer to a collection an error is returned.

    **index** [IN] -- the index of the element that is to be deleted. If no
    element exists at that index an error is returned.


.. function:: int dpiObject_getAttributeValue(dpiObject \*obj, \
        dpiObjectAttr \*attr, dpiNativeTypeNum nativeTypeNum, dpiData \*value)

    Returns the value of one of the object's attributes.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the attribute is to be retrieved. If
    the reference is NULL or invalid an error is returned.

    **attr** [IN] -- the attribute which is to be retrieved. The attribute must
    belong to the same type as the object; otherwise, an error is returned.

    **nativeTypeNum** [IN] -- the native type of the data that is to be
    retrieved.  It should be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **value** [IN/OUT] -- a pointer to a :ref:`dpiData<dpiData>` structure
    which will be populated with the value of the attribute when this function
    completes successfully. If the native type is DPI_NATIVE_TYPE_BYTES and
    the Oracle type of the attribute is DPI_ORACLE_TYPE_NUMBER, a buffer must
    be supplied in the value.asBytes.ptr attribute and the maximum length of
    that buffer must be supplied in the value.asBytes.length attribute before
    calling this function. For all other conversions, the buffer is supplied
    by the library and remains valid as long as a reference to the object is
    held.


.. function:: int dpiObject_getElementExistsByIndex(dpiObject \*obj, \
        int32_t index, int \*exists)

    Returns whether an element exists at the specified index.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object for which an element's existence is to be
    tested. If the reference is NULL or invalid an error is returned. Likewise,
    if the object does not refer to a collection an error is returned.

    **index** [IN] -- the index into the collection that is to be checked.

    **exists** [OUT] -- a pointer to a boolean value indicating if an element
    exists at the specified index (1) or not (0), which will be populated when
    this function completes successfully.


.. function:: int dpiObject_getElementValueByIndex(dpiObject \*obj, \
        int32_t index, dpiNativeTypeNum nativeTypeNum, dpiData \*value)

    Returns the value of the element found at the specified index.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the element is to be retrieved. If
    the reference is NULL or invalid an error is returned. Likewise, if the
    object does not refer to a collection an error is returned.

    **index** [IN] -- the index into the collection from which the element is
    to be retrieved. If no element exists at that index, an error is returned.

    **nativeTypeNum** [IN] -- the native type of the data that is to be
    retrieved.  It should be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **value** [OUT] -- a pointer to a :ref:`dpiData<dpiData>` structure which
    will be populated with the value of the element when this function
    completes successfully.


.. function:: int dpiObject_getFirstIndex(dpiObject \*obj, int32_t \*index, \
        int \*exists)

    Returns the first index used in a collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the first index is to be retrieved.
    If the reference is NULL or invalid an error is returned. Likewise, if the
    object does not refer to a collection an error is returned.

    **index** [OUT] -- a pointer to the first index used in the collection,
    which will be populated when the function completes successfully.

    **exists** [OUT] -- a pointer to a boolean value specifying whether a first
    index exists (1) or not (0), which will be populated when the function
    completes successfully.


.. function:: int dpiObject_getLastIndex(dpiObject \*obj, int32_t \*index, \
        int \*exists)

    Returns the last index used in a collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the last index is to be retrieved. If
    the reference is NULL or invalid an error is returned. Likewise, if the
    object does not refer to a collection an error is returned.

    **index** [OUT] -- a pointer to the last index used in the collection,
    which will be populated when the function completes successfully.

    **exists** [OUT] -- a pointer to a boolean value specifying whether a last
    index exists (1) or not (0), which will be populated when the function
    completes successfully.


.. function:: int dpiObject_getNextIndex(dpiObject \*obj, int32_t index, \
        int32_t \*nextIndex, int \*exists)

    Returns the next index used in a collection following the specified index.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the next index is to be retrieved. If
    the reference is NULL or invalid an error is returned. Likewise, if the
    object does not refer to a collection an error is returned.

    **index** [IN] -- the index after which the next index is to be determined.
    This does not have to be an actual index in the collection.

    **nextIndex** [OUT] -- a pointer to the next index used in the collection,
    which will be populated when the function completes successfully and the
    value of the exists parameter is 1.

    **exists** [OUT] -- a pointer to a boolean value specifying whether a next
    index exists following the specified index (1) or not (0), which will be
    populated when the function completes successfully.


.. function:: int dpiObject_getPrevIndex(dpiObject \*obj, int32_t index, \
        int32_t \*prevIndex, int \*exists)

    Returns the previous index used in a collection preceding the specified
    index.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the previous index is to be
    retrieved. If the reference is NULL or invalid an error is returned.
    Likewise, if the object does not refer to a collection an error is
    returned.

    **index** [IN] -- the index before which the previous index is to be
    determined.  This does not have to be an actual index in the collection.

    **prevIndex** [OUT] -- a pointer to the previous index used in the
    collection, which will be populated when the function completes
    successfully and the value of the exists parameter is 1.

    **exists** [OUT] -- a pointer to a boolean value specifying whether a
    previous index exists preceding the specified index (1) or not (0), which
    will be populated when the function completes successfully.


.. function:: int dpiObject_getSize(dpiObject \*obj, int32_t \*size)

    Returns the number of elements in a collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which the number of elements is to be
    retrieved.  If the reference is NULL or invalid an error is returned.
    Likewise, if the object does not refer to a collection an error is
    returned.

    **size** [OUT] -- a pointer to the number of elements in the collection,
    which will be populated when the function completes successfully.


.. function:: int dpiObject_release(dpiObject \*obj)

    Releases a reference to the object. A count of the references to the object
    is maintained and when this count reaches zero, the memory associated with
    the object is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which a reference is to be released. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiObject_setAttributeValue(dpiObject \*obj, \
        dpiObjectAttr \*attr, dpiNativeTypeNum nativeTypeNum, dpiData \*value)

    Sets the value of one of the object's attributes.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object on which the attribute is to be set. If the
    reference is NULL or invalid an error is returned.

    **attr** [IN] -- the attribute which is to be set. The attribute must
    belong to the same type as the object; otherwise, an error is returned.

    **nativeTypeNum** [IN] -- the native type of the data that is to be set. It
    should be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **value** [IN] -- a pointer to a :ref:`dpiData<dpiData>` structure which
    contains the value to which the attribute is to be set.


.. function:: int dpiObject_setElementValueByIndex(dpiObject \*obj, \
        int32_t index, dpiNativeTypeNum nativeTypeNum, dpiData \*value)

    Sets the value of the element found at the specified index.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object in which the element is to be set. If the
    reference is NULL or invalid an error is returned. Likewise, if the object
    does not refer to a collection an error is returned.

    **index** [IN] -- the index into the collection at which the element is to
    be set.

    **nativeTypeNum** [IN] -- the native type of the data that is to be set. It
    should be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **value** [IN] -- a pointer to a :ref:`dpiData<dpiData>` structure which
    contains the value of the element to place at the specified index.


.. function:: int dpiObject_trim(dpiObject \*obj, uint32_t numToTrim)

    Trims a number of elements from the end of a collection.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **obj** [IN] -- the object from which a number of elements are to be
    trimmed. If the reference is NULL or invalid an error is returned.
    Likewise, if the object does not refer to a collection an error is
    returned.

    **numToTrim** [IN] -- the number of elements to trim from the end of the
    collection. If the number of of elements to trim exceeds the current size
    of the collection an error is returned.

