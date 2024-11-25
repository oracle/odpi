.. _dpiVectorFunctions:

ODPI-C Vector Functions
-----------------------

Vector handles are used to represent vector values stored in the database. They
are only available from Oracle Client and Database 23.4 and higher. They can be
created explicitly by a call to :func:`dpiConn_newVector()` or implicitly when
a variable of type DPI_ORACLE_TYPE_VECTOR is created. They are destroyed when
the last reference is released by a call to the function
:func:`dpiVector_release()`.

.. function:: int dpiVector_addRef(dpiVector* vector)

    Adds a reference to the vector value. This is intended for situations where
    a reference to the vector value needs to be maintained independently of the
    reference returned when the vector value was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``vector``
          - IN
          - The vector value to which a reference is to be added. If the
            reference is NULL or invalid, an error is returned.

.. function:: int dpiVector_getValue(dpiVector* vector, dpiVectorInfo* info)

    Returns information about the vector.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``vector``
          - IN
          - The vector value from which the information is to be extracted. If
            the reference is NULL or invalid, an error is returned.
        * - ``info``
          - OUT
          - A pointer to a structure of type
            :ref:`dpiVectorInfo<dpiVectorInfo>` which will be populated upon
            successful completion of this function. The structure's contents
            will remain valid as long as a reference is held to the vector
            value and the vector value itself is not modified.

.. function:: int dpiVector_release(dpiVector* vector)

    Releases a reference to the vector value. A count of the references to the
    vector value is maintained and when this count reaches zero, the memory
    associated with the vector value is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``vector``
          - IN
          - The vector value from which a reference is to be released. If the
            reference is NULL or invalid, an error is returned.

.. function:: int dpiVector_setValue(dpiVector* vector, dpiVectorInfo* info)

    Sets the vector value from the supplied information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``vector``
          - IN
          - The vector value which is to be modified using the supplied
            information. If the reference is NULL or invalid, an error is
            returned.
        * - ``info``
          - IN
          - A pointer to a structure of type
            :ref:`dpiVectorInfo<dpiVectorInfo>` which contains the information
            to be set on the vector.
