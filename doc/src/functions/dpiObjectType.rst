.. _dpiObjectTypeFunctions:

ODPI-C Object Type Functions
----------------------------

Object type handles are used to represent types such as those created by the
SQL command CREATE OR REPLACE TYPE. They are created using the function
:func:`dpiConn_getObjectType()` or implicitly when fetching from a column
containing objects by calling the function :func:`dpiStmt_getQueryInfo()`.
Object types are also retrieved when used as attributes in
another object by calling the function :func:`dpiObjectAttr_getInfo()` or as
the element type of a collection by calling the function
:func:`dpiObjectType_getInfo()`. They are destroyed when the last reference is
released by calling the function :func:`dpiObjectType_release()`.


.. function:: int dpiObjectType_addRef(dpiObjectType \*objType)

    Adds a reference to the object type. This is intended for situations where
    a reference to the object type needs to be maintained independently of the
    reference returned when the object type was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **objType** [IN] -- the object type to which a reference is to be added. If
    the reference is NULL or invalid an error is returned.


.. function:: int dpiObjectType_createObject(dpiObjectType \*objType, \
        dpiObject \**obj)

    Creates an object of the specified type and returns a reference to it.
    This reference should be released as soon as it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **objType** [IN] -- a reference to the object type whose information is to
    be retrieved. If the reference is NULL or invalid an error is returned.

    **obj** [OUT] -- a pointer to a reference to the created object, which will
    be populated when the function completes successfully.


.. function:: int dpiObjectType_getAttributes(dpiObjectType \*objType, \
        uint16_t numAttributes, dpiObjectAttr \**attributes)

    Returns the list of attributes that belong to the object type.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **objType** [IN] -- a reference to the object type whose attributes are to
    be retrieved. If the reference is NULL or invalid an error is returned.

    **numAttributes** [IN] -- the number of attributes which will be returned.
    This value can be determined using the function
    :func:`dpiObjectType_getInfo()`.

    **attributes** [OUT] -- an array of references to the object's attributes,
    which will be populated with attribute references upon successful
    completion of this function. It is assumed that the array is large enough
    to hold numAttributes attribute references. These references must be
    released when they are no longer required by calling the function
    :func:`dpiObjectAttr_release()`.


.. function:: int dpiObjectType_getInfo(dpiObjectType \*objType, \
        dpiObjectTypeInfo \*info)

    Returns information about the object type.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **objType** [IN] -- a reference to the object type whose information is to
    be retrieved. If the reference is NULL or invalid an error is returned.

    **info** [OUT] -- a pointer to a
    :ref:`dpiObjectTypeInfo<dpiObjectTypeInfo>` structure which will be
    populated with information about the object type when the function
    completes successfully.


.. function:: int dpiObjectType_release(dpiObjectType \*objType)

    Releases a reference to the object type. A count of the references to the
    object type is maintained and when this count reaches zero, the memory
    associated with the object type is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **objType** [IN] -- the object type from which a reference is to be
    released. If the reference is NULL or invalid an error is returned.

