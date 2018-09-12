.. _dpiEnqOptionsFunctions:

ODPI-C Enqueue Options Functions
--------------------------------

Enqueue option handles are used to represent the options specified when
enqueuing messages using advanced queueing. They are created by calling the
function :func:`dpiConn_newEnqOptions()` and are destroyed by releasing the
last reference by calling the function :func:`dpiEnqOptions_release()`.

.. function:: int dpiEnqOptions_addRef(dpiEnqOptions \*options)

    Adds a reference to the enqueue options. This is intended for situations
    where a reference to the enqueue options needs to be maintained
    independently of the reference returned when the handle was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- the enqueue options to which a reference is to be
    added. If the reference is NULL or invalid an error is returned.


.. function:: int dpiEnqOptions_getTransformation(dpiEnqOptions \*options, \
        const char \** value, uint32_t \*valueLength)

    Returns the transformation of the message to be enqueued. See function
    :func:`dpiEnqOptions_setTransformation()` for more information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the enqueue options from which the
    transformation is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no transformation, the pointer will be populated
    with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no transformation, the pointer will be populated with the value 0.


.. function:: int dpiEnqOptions_getVisibility(dpiEnqOptions \*options, \
        dpiVisibility \*value)

    Returns whether the message being enqueued is part of the current
    transaction or constitutes a transaction on its own.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the enqueue options from which the
    visibility is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function. It will be one of the values from
    the enumeration :ref:`dpiVisibility<dpiVisibility>`.


.. function:: int dpiEnqOptions_release(dpiEnqOptions \*options)

    Releases a reference to the enqueue options. A count of the references to
    the enqueue options is maintained and when this count reaches zero, the
    memory associated with the options is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- the enqueue options from which a reference is to be
    released. If the reference is NULL or invalid an error is returned.


.. function:: int dpiEnqOptions_setDeliveryMode(dpiEnqOptions \*options, \
        dpiMessageDeliveryMode value)

    Sets the message delivery mode that is to be used when enqueuing messages.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the enqueue options on which the message
    delivery mode is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the mode that should be used. It should be one of the
    values from the enumeration
    :ref:`dpiMessageDeliveryMode<dpiMessageDeliveryMode>`.


.. function:: int dpiEnqOptions_setTransformation(dpiEnqOptions \*options, \
        const char \* value, uint32_t valueLength)

    Sets the transformation of the message to be enqueued. The transformation
    is applied after the message is enqueued but before it is returned to the
    application. It must be created using DBMS_TRANSFORM.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the enqueue options on which the
    transformation is to be set. If the reference is NULL or invalid an error
    is returned.

    **value** [IN] -- a byte string in the encoding used for CHAR data, or NULL
    if the transformation is to be cleared.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiEnqOptions_setVisibility(dpiEnqOptions \*options, \
        dpiVisibility value)

    Sets whether the message being enqueued is part of the current transaction
    or constitutes a transaction on its own.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the enqueue options on which the
    visibility is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the value that should be used. It should be one of the
    values from the enumeration :ref:`dpiVisibility<dpiVisibility>`.

