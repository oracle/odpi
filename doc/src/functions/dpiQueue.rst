.. _dpiQueue:

ODPI-C Queue Functions
----------------------

Queue handles are used to represent Advanced Queueing (AQ) queues. They are
created by calling the function :func:`dpiConn_newQueue()` and are destroyed
when the last reference is released by calling the function
:func:`dpiQueue_release()`.

.. function:: int dpiQueue_addRef(dpiQueue* queue)

    Adds a reference to the queue. This is intended for situations where a
    reference to the queue needs to be maintained independently of the
    reference returned when the queue was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue to which a reference is to be added. If the reference is
            NULL or invalid, an error is returned.

.. function:: int dpiQueue_deqMany(dpiQueue* queue, uint32_t* numProps, \
        dpiMsgProps** props)

    Dequeues multiple messages from the queue.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue from which messages are to be dequeued. If the reference
            is NULL or invalid, an error is returned.
        * - ``numProps``
          - IN/OUT
          - A pointer to the number of elements in the props array. When the
            function is called, the value refers to the length of the props
            array and the maximum number of messages that should be dequeued.
            After the function has completed successfully, the value refers
            to the number of messages that were actually dequeued from the
            queue.
        * - ``props``
          - OUT
          - An array of references to message properties which will be
            populated upon successful completion of this function. Each of
            these references should be released when they are no longer
            needed by calling the function :func:`dpiMsgProps_release()`.

.. function:: int dpiQueue_deqOne(dpiQueue* queue, dpiMsgProps** props)

    Dequeues a single message from the queue.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue from which the messages is to be dequeued. If the
            reference is NULL or invalid, an error is returned.
        * - ``props``
          - OUT
          - A pointer to a reference to a message property which will be
            populated upon successful completion of this function. This
            reference should be released when it is no longer needed by calling
            the function :func:`dpiMsgProps_release()`. If no messages are
            available, this reference will be NULL.

.. function:: int dpiQueue_enqMany(dpiQueue* queue, uint32_t numProps, \
        dpiMsgProps** props)

    Enqueues multiple messages into the queue.

    Warning: calling this function in parallel on different connections
    acquired from the same pool may fail due to Oracle bug 29928074. Ensure
    that this function is not run in parallel, use standalone connections or
    connections from different pools, or make multiple calls to
    :func:`dpiQueue_enqOne()` instead. The function :func:`dpiQueue_deqMany()`
    call is not affected.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue into which the messages are to be enqueued. If the
            reference is NULL or invalid, an error is returned.
        * - ``numProps``
          - IN/OUT
          - The number of messages that are to be enqueued.
        * - ``props``
          - IN
          - An array of references to message properties that are to be
            enqueued. The length of the array is specified by the numProps
            parameter. Each of the message properties must have the right
            type of payload associated before calling this method or an error
            will occur.

.. function:: int dpiQueue_enqOne(dpiQueue* queue, dpiMsgProps* props)

    Enqueues a single mesasge into the queue.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue into which the message is to be enqueued. If the
            reference is NULL or invalid, an error is returned.
        * - ``props``
          - IN
          - A reference to the message that is to be enqueued. The message
            properties must have the right type of payload associated before
            calling this method or an error will occur.

.. function:: int dpiQueue_getDeqOptions(dpiQueue* queue, \
        dpiDeqOptions** options)

    Returns a reference to the dequeue options associated with the queue. These
    options affect how messages are dequeued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue from which the dequeue options are to be retrieved. If
            the reference is NULL or invalid, an error is returned.
        * - ``options``
          - OUT
          - A reference to the dequeue options associated with the queue which
            will be populated upon successful completion of this function.
            This reference is owned by the queue and will remain valid as
            long as a reference to the queue is held.

.. function:: int dpiQueue_getEnqOptions(dpiQueue* queue, \
        dpiEnqOptions** options)

    Returns a reference to the enqueue options associated with the queue. These
    options affect how messages are enqueued.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue from which the enqueue options are to be retrieved. If
            the reference is NULL or invalid, an error is returned.
        * - ``options``
          - OUT
          - A reference to the enqueue options associated with the queue which
            will be populated upon successful completion of this function. This
            reference is owned by the queue and will remain valid as long as a
            reference to the queue is held.

.. function:: int dpiQueue_release(dpiQueue* queue)

    Releases a reference to the queue. A count of the references to the queue
    is maintained and when this count reaches zero, the memory associated with
    the queue is freed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``queue``
          - IN
          - The queue from which a reference is to be released. If the
            reference is NULL or invalid, an error is returned.
