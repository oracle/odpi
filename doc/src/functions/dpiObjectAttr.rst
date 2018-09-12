.. _dpiObjectAttrFunctions:

ODPI-C Object Attribute Functions
---------------------------------

Object attribute handles are used to represent the attributes of types such as
those created by the SQL command CREATE OR REPLACE TYPE. They are created by
calling the function :func:`dpiObjectType_getAttributes()` and are destroyed
when the last reference is released by calling the function
:func:`dpiObjectAttr_release()`.

.. function:: int dpiObjectAttr_addRef(dpiObjectAttr \*attr)

    Adds a reference to the attribute. This is intended for situations where a
    reference to the attribute needs to be maintained independently of the
    reference returned when the attribute was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **attr** [IN] -- the attribute to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiObjectAttr_getInfo(dpiObjectAttr \*attr, \
        dpiObjectAttrInfo \*info)

    Returns information about the attribute.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **attr** [IN] -- a reference to the attribute whose information is to be
    retrieved. If the reference is NULL or invalid an error is returned.

    **info** [OUT] -- a pointer to a
    :ref:`dpiObjectAttrInfo<dpiObjectAttrInfo>` structure which will be
    populated with information about the attribute.


.. function:: int dpiObjectAttr_release(dpiObjectAttr \*attr)

    Releases a reference to the attribute. A count of the references to the
    attribute is maintained and when this count reaches zero, the memory
    associated with the attribute is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **attr** [IN] -- the attribute from which a reference is to be released. If
    the reference is NULL or invalid an error is returned.

