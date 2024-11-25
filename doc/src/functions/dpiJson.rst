.. _dpiJsonFunctions:

ODPI-C JSON Functions
---------------------

JSON handles are used to represent JSON values stored in the database. They are
only available from Oracle Client and Database 20 and higher. They cannot be
created directly but are created implicitly when a variable of type
DPI_ORACLE_TYPE_JSON is created. They are destroyed when the last reference is
released by a call to the function :func:`dpiJson_release()`.

.. function:: int dpiJson_addRef(dpiJson* json)

    Adds a reference to the JSON value. This is intended for situations where a
    reference to the JSON value needs to be maintained independently of the
    reference returned when the JSON value was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``json``
          - IN
          - The JSON value to which a reference is to be added. If the
            reference is NULL or invalid, an error is returned.

.. function:: int dpiJson_getValue(dpiJson* json, uint32_t options, \
        dpiJsonNode** topNode)

    Returns the top node of a hierarchy of nodes containing the data stored in
    the JSON value.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``json``
          - IN
          - The JSON value from which the top node is to be returned. If the
            reference is NULL or invalid, an error is returned.
        * - ``options``
          - IN
          - One or more of the values from the
            :ref:`dpiJsonOptions<dpiJsonOptions>` enumerations, OR'ed together.
        * - ``topNode``
          - OUT
          - A pointer to the top node of the JSON value, which will be
            populated upon successful completion of this function. It is a
            structure of type :ref:`dpiJsonNode<dpiJsonNode>` and will remain
            valid as long as a reference is held to the JSON value and as long
            as the JSON value is not modified by call to
            :func:`dpiJson_setValue()`.

.. function:: int dpiJson_release(dpiJson* json)

    Releases a reference to the JSON value. A count of the references to the
    JSON value is maintained and when this count reaches zero, the memory
    associated with the JSON value is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``json``
          - IN
          - The JSON value from which a reference is to be released. If the
            reference is NULL or invalid, an error is returned.

.. function:: int dpiJson_setFromText(dpiJson* json, \
        const char* value, uint64_t valueLength, uint32_t flags)

    Sets the JSON value from the JSON string passed as a byte string.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``json``
          - IN
          - The JSON value which is to be modified to contain the data from
            input JSON string. If the reference is NULL or invalid, an error
            is returned.
        * - ``value``
          - IN
          - A pointer to the JSON string which contains the data to be set.
            This value can be NULL if the valueLength parameter is 0.
        * - ``valueLength``
          - IN
          - The length of the data to be set, in bytes.
        * - ``flags``
          - IN
          - Flags which can be used in the future to adjust how text is
            converted to JSON. Pass 0 for now.

.. function:: int dpiJson_setValue(dpiJson* json, dpiJsonNode* topNode)

    Sets the JSON value to the data found in the hierarchy of nodes pointed to
    by the top level node.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``json``
          - IN
          - The JSON value which is to be modified to contain the data found
            in the hierarchy of nodes pointed to by the top node. If the
            reference is NULL or invalid, an error is returned.
        * - ``topNode``
          - IN
          - A pointer to the top node of the data which will replace the data
            in the JSON value. It is a structure of type
            :ref:`dpiJsonNode<dpiJsonNode>`.
