.. _dpiMsgPropsFunctions:

ODPI-C Message Properties Functions
-----------------------------------

Message properties handles are used to represent the properties of messages
that are enqueued and dequeued using advanced queuing. They are created by
calling the function :func:`dpiConn_newMsgProps()` and are destroyed by
releasing the last reference by calling the function
:func:`dpiMsgProps_release()`.

.. function:: int dpiMsgProps_addRef(dpiMsgProps \*props)

    Adds a reference to the message properties. This is intended for situations
    where a reference to the message properties needs to be maintained
    independently of the reference returned when the handle was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- the message properties to which a reference is to be
    added. If the reference is NULL or invalid an error is returned.


.. function:: int dpiMsgProps_getNumAttempts(dpiMsgProps \*props, \
        int32_t \*value)

    Returns the number of attempts that have been made to dequeue a message.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    number of attempts is to be retrieved. If the reference is NULL or invalid
    an error is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function.


.. function:: int dpiMsgProps_getCorrelation(dpiMsgProps \*props, \
        const char \** value, uint32_t \*valueLength)

    Returns the correlation supplied by the producer when the message was
    enqueued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    correlation is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no correlation, the pointer will be populated
    with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no correlation, the pointer will be populated with the value 0.


.. function:: int dpiMsgProps_getDelay(dpiMsgProps \*props, int32_t \*value)

    Returns the number of seconds the enqueued message will be delayed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    delay is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function.


.. function:: int dpiMsgProps_getDeliveryMode(dpiMsgProps \*props, \
        dpiMessageDeliveryMode \*value)

    Returns the mode that was used to deliver the message.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    message delivery mode is to be retrieved. If the reference is NULL or
    invalid an error is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function. It will be one of the values from
    the enumeration :ref:`dpiMessageDeliveryMode<dpiMessageDeliveryMode>`.


.. function:: int dpiMsgProps_getEnqTime(dpiMsgProps \*props, \
        dpiTimestamp \*value)

    Returns the time that the message was enqueued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    enqueue time is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to a :ref:`dpiTimestamp<dpiTimestamp>`
    structure, which will be populated upon successful completion of this
    function.


.. function:: int dpiMsgProps_getExceptionQ(dpiMsgProps \*props, \
        const char \** value, uint32_t \*valueLength)

    Returns the name of the queue to which the message is moved if it cannot be
    processed successfully. See function :func:`dpiMsgProps_setExceptionQ()`
    for more information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the name
    of the exception queue is to be retrieved. If the reference is NULL or
    invalid an error is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no exception queue name, the pointer will be
    populated with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no exception queue name, the pointer will be populated with the
    value 0.


.. function:: int dpiMsgProps_getExpiration(dpiMsgProps \*props, \
        int32_t \*value)

    Returns the number of seconds the message is available to be dequeued.
    See function :func:`dpiMsgProps_setExpiration()` for more information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    expiration is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function.


.. function:: int dpiMsgProps_getOriginalMsgId(dpiMsgProps \*props, \
        const char \** value, uint32_t \*valueLength)

    Returns the id of the message in the last queue that generated this
    message. See function :func:`dpiMsgProps_setOriginalMsgId()` for more
    information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    original message id is to be retrieved. If the reference is NULL or invalid
    an error is returned.

    **value** [OUT] -- a pointer to the value, as a byte string in the encoding
    used for CHAR data, which will be populated upon successful completion of
    this function. If there is no original message id, the pointer will be
    populated with the value NULL.

    **valueLength** [OUT] -- a pointer to the length of the value, in bytes,
    which will be populated upon successful completion of this function. If
    there is no original message id, the pointer will be populated with the
    value 0.


.. function:: int dpiMsgProps_getPriority(dpiMsgProps \*props, int32_t \*value)

    Returns the priority assigned to the message. See function
    :func:`dpiMsgProps_setPriority()` for more information.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    priority is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function.


.. function:: int dpiMsgProps_getState(dpiMsgProps \*props, \
        dpiMessageState \*value)

    Returns the state of the message at the time of dequeue.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties from which the
    message state is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **value** [OUT] -- a pointer to the value, which will be populated upon
    successful completion of this function. It will be one of the values from
    the enumeration :ref:`dpiMessageState<dpiMessageState>`.


.. function:: int dpiMsgProps_release(dpiMsgProps \*props)

    Releases a reference to the message properties. A count of the references
    to the message properties is maintained and when this count reaches zero,
    the memory associated with the properties is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- the message properties from which a reference is to be
    released. If the reference is NULL or invalid an error is returned.


.. function:: int dpiMsgProps_setCorrelation(dpiMsgProps \*props, \
        const char \* value, uint32_t valueLength)

    Sets the correlation of the message to be dequeued. Special pattern
    matching characters such as the percent sign (%) and the underscore (_)
    can be used. If multiple messages satisfy the pattern, the order of
    dequeuing is undetermined.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties on which the
    correlation is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- a byte string in the encoding used for CHAR data, or NULL
    if the correlation is to be cleared.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiMsgProps_setDelay(dpiMsgProps \*props, int32_t value)

    Sets the number of seconds to delay the message before it can be dequeued.
    Messages enqueued with a delay are put into the DPI_MSG_STATE_WAITING
    state. When the delay expires the message is put into the
    DPI_MSG_STATE_READY state. Dequeuing directly by message id overrides this
    delay specification. Note that delay processing requires the queue monitor
    to be started.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties on which the delay
    is to be set. If the reference is NULL or invalid an error is returned.

    **value** [IN] -- the value to set.


.. function:: int dpiMsgProps_setExceptionQ(dpiMsgProps \*props, \
        const char \* value, uint32_t valueLength)

    Sets the name of the queue to which the message is moved if it cannot be
    processed successfully. Messages are moved if the number of unsuccessful
    dequeue attempts has reached the maximum allowed number or if the message
    has expired. All messages in the exception queue are in the
    DPI_MSG_STATE_EXPIRED state.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties on which the name
    of the exception queue is to be set. If the reference is NULL or invalid an
    error is returned.

    **value** [IN] -- a byte string in the encoding used for CHAR data, or NULL
    if the exception queue name is to be cleared. If not NULL, the value must
    refer to a valid queue name.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiMsgProps_setExpiration(dpiMsgProps \*props, int32_t value)

    Sets the number of seconds the message is available to be dequeued. This
    value is an offset from the delay. Expiration processing requires the queue
    monitor to be running. Until this time elapses, the messages are in the
    queue in the state DPI_MSG_STATE_READY. After this time elapses messages
    are moved to the exception queue in the DPI_MSG_STATE_EXPIRED state.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties on which the
    expiration is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the value to set.


.. function:: int dpiMsgProps_setOriginalMsgId(dpiMsgProps \*props, \
        const char \* value, uint32_t valueLength)

    Sets the id of the message in the last queue that generated this
    message.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties on which the
    original message identifier is to be set. If the reference is NULL or
    invalid an error is returned.

    **value** [IN] -- a pointer to the bytes making up the message identifier,
    or NULL if no identifier is to be specified.

    **valueLength** [IN] -- the length of the value parameter in bytes, or 0 if
    the value parameter is NULL.


.. function:: int dpiMsgProps_setPriority(dpiMsgProps \*props, int32_t value)

    Sets the priority assigned to the message. A smaller number indicates a
    higher priority. The priority can be any number, including negative
    numbers.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **props** [IN] -- a reference to the message properties on which the
    priority is to be set. If the reference is NULL or invalid an error is
    returned.

    **value** [IN] -- the value to set.

