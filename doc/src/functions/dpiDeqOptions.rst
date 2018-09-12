.. _dpiDeqOptionsFunctions:

ODPI-C Dequeue Options Functions
--------------------------------

Dequeue option handles are used to represent the options specified when
dequeuing messages using advanced queueing. They are created by calling the
function :func:`dpiConn_newDeqOptions()` and are destroyed by releasing the
last reference by calling the function :func:`dpiDeqOptions_release()`.

.. function:: int dpiDeqOptions_addRef(dpiDeqOptions \*options)

    Adds a reference to the dequeue options. This is intended for situations
    where a reference to the dequeue options needs to be maintained
    independently of the reference returned when the handle was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- the dequeue options to which a reference is to be
    added. If the reference is NULL or invalid an error is returned.


.. function:: int dpiDeqOptions_getCondition(dpiDeqOptions \*options, \
        const char \** value, uint32_t \*valueLength)

    Returns the condition that must be satisfied in order for a message to be
    dequeued. See function :func:`dpiDeqOptions_setCondition()` for more
    information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the
    condition is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no condition, the pointer will be populated with
    the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no condition, the pointer will be populated with the value 0.


.. function:: int dpiDeqOptions_getConsumerName(dpiDeqOptions \*options, \
        const char \** value, uint32_t \*valueLength)

    Returns the name of the consumer that is dequeuing messages. See function
    :func:`dpiDeqOptions_setConsumerName()` for more information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the
    consumer name is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no consumer name, the pointer will be populated
    with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no consumer name, the pointer will be populated with the value 0.


.. function:: int dpiDeqOptions_getCorrelation(dpiDeqOptions \*options, \
        const char \** value, uint32_t \*valueLength)

    Returns the correlation of the message to be dequeued. See function
    :func:`dpiDeqOptions_setCorrelation()` for more information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the
    correlation is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no correlation, the pointer will be populated
    with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no correlation, the pointer will be populated with the value 0.


.. function:: int dpiDeqOptions_getMode(dpiDeqOptions \*options, \
        dpiDeqMode \*value)

    Returns the mode that is to be used when dequeuing messages.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the mode
    is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function. It will be one of the values from
    the enumeration :ref:`dpiDeqMode<dpiDeqMode>`.


.. function:: int dpiDeqOptions_getMsgId(dpiDeqOptions \*options, \
        const char \** value, uint32_t \*valueLength)

    Returns the identifier of the specific message that is to be dequeued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the
    message identifier is to be retrieved. If the reference is NULL or invalid
    an error is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function. If there is no message identifier,
    the pointer will be populated with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no message identifier, the pointer will be populated with the
    value 0.


.. function:: int dpiDeqOptions_getNavigation(dpiDeqOptions \*options, \
        dpiDeqNavigation \*value)

    Returns the position of the message that is to be dequeued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the
    navigation option is to be retrieved. If the reference is NULL or invalid
    an error is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function. It will be one of the values from
    the enumeration :ref:`dpiDeqNavigation<dpiDeqNavigation>`.


.. function:: int dpiDeqOptions_getTransformation(dpiDeqOptions \*options, \
        const char \** value, uint32_t \*valueLength)

    Returns the transformation of the message to be dequeued. See function
    :func:`dpiDeqOptions_setTransformation()` for more information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the
    transformation is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no transformation, the pointer will be populated
    with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no transformation, the pointer will be populated with the value 0.


.. function:: int dpiDeqOptions_getVisibility(dpiDeqOptions \*options, \
        dpiVisibility \*value)

    Returns whether the message being dequeued is part of the current
    transaction or constitutes a transaction on its own.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the
    visibility is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function. It will be one of the values from
    the enumeration :ref:`dpiVisibility<dpiVisibility>`.


.. function:: int dpiDeqOptions_getWait(dpiDeqOptions \*options, \
        uint32_t \*value)

    Returns the time to wait, in seconds, for a message matching the search
    criteria. See function :func:`dpiDeqOptions_setWait()` for more
    information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the wait
    time is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function.


.. function:: int dpiDeqOptions_release(dpiDeqOptions \*options)

    Releases a reference to the dequeue options. A count of the references to
    the dequeue options is maintained and when this count reaches zero, the
    memory associated with the options is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- the dequeue options from which a reference is to be
    released. If the reference is NULL or invalid an error is returned.


.. function:: int dpiDeqOptions_setCondition(dpiDeqOptions \*options, \
        const char \* value, uint32_t valueLength)

    Sets the condition which must be true for messages to be dequeued. The
    condition must be a valid boolean expression similar to the where clause
    of a SQL query. The expression can include conditions on message
    properties, user data properties and PL/SQL or SQL functions. User data
    properties must be prefixed with tab.user_data as a qualifier to indicate
    the specific column of the queue table that stores the message payload.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the
    condition is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- a byte string in the encoding used for CHAR data, or NULL
    if the condition is to be cleared.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiDeqOptions_setConsumerName(dpiDeqOptions \*options, \
        const char \* value, uint32_t valueLength)

    Sets the name of the consumer which will be dequeuing messages. This value
    should only be set if the queue is set up for multiple consumers.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the
    consumer name is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- a byte string in the encoding used for CHAR data, or NULL
    if the consumer name is to be cleared.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiDeqOptions_setCorrelation(dpiDeqOptions \*options, \
        const char \* value, uint32_t valueLength)

    Sets the correlation of the message to be dequeued. Special pattern
    matching characters such as the percent sign (%) and the underscore (_)
    can be used. If multiple messages satisfy the pattern, the order of
    dequeuing is undetermined.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the
    correlation is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- a byte string in the encoding used for CHAR data, or NULL
    if the correlation is to be cleared.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiDeqOptions_setDeliveryMode(dpiDeqOptions \*options, \
        dpiMessageDeliveryMode value)

    Sets the message delivery mode that is to be used when dequeuing messages.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the message
    delivery mode is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the mode that should be used. It should be one of the
    values from the enumeration
    :ref:`dpiMessageDeliveryMode<dpiMessageDeliveryMode>`.


.. function:: int dpiDeqOptions_setMode(dpiDeqOptions \*options, \
        dpiDeqMode value)

    Sets the mode that is to be used when dequeuing messages.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the mode is
    to be set. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- the mode that should be used. It should be one of the
    values from the enumeration :ref:`dpiDeqMode<dpiDeqMode>`.


.. function:: int dpiDeqOptions_setMsgId(dpiDeqOptions \*options, \
        const char \* value, uint32_t valueLength)

    Sets the identifier of the specific message to be dequeued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the message
    identifier to dequeue is to be set. If the reference is NULL or invalid an
    error is returned.

    **value** [IN] -- a pointer to the bytes making up the message identifier,
    or NULL if no specific message is to be dequeued.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiDeqOptions_setNavigation(dpiDeqOptions \*options, \
        dpiDeqNavigation value)

    Sets the position in the queue of the message that is to be dequeued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the
    navigation option is to be set. If the reference is NULL or invalid an
    error is returned.

    **value** [IN] -- the value that should be used. It should be one of the
    values from the enumeration :ref:`dpiDeqNavigation<dpiDeqNavigation>`.


.. function:: int dpiDeqOptions_setTransformation(dpiDeqOptions \*options, \
        const char \* value, uint32_t valueLength)

    Sets the transformation of the message to be dequeued. The transformation
    is applied after the message is dequeued but before it is returned to the
    application. It must be created using DBMS_TRANSFORM.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the
    transformation is to be set. If the reference is NULL or invalid an error
    is returned.

    **value** [IN] -- a byte string in the encoding used for CHAR data, or NULL
    if the transformation is to be cleared.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiDeqOptions_setVisibility(dpiDeqOptions \*options, \
        dpiVisibility value)

    Sets whether the message being dequeued is part of the current transaction
    or constitutes a transaction on its own.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options on which the
    visibility is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the value that should be used. It should be one of the
    values from the enumeration :ref:`dpiVisibility<dpiVisibility>`.


.. function:: int dpiDeqOptions_setWait(dpiDeqOptions \*options, \
        uint32_t value)

    Set the time to wait, in seconds, for a message matching the search
    criteria.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **options** [IN] -- a reference to the dequeue options from which the wait
    time is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the number of seconds to wait for a message matching the
    search criteria. Any integer is valid but the predefined constants
    DPI_DEQ_WAIT_NO_WAIT and DPI_DEQ_WAIT_FOREVER are provided as a
    convenience.

