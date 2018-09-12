.. _dpiSubscrFunctions:

ODPI-C Subscription Functions
-----------------------------

Subscription handles are used to represent subscriptions to events such as
continuous query notification and object change notification. They are created
by calling the function :func:`dpiConn_subscribe()` and are destroyed
by calling the function :func:`dpiConn_unsubscribe()` or releasing the last
reference by calling the function :func:`dpiSubscr_release()`.

.. function:: int dpiSubscr_addRef(dpiSubscr \*subscr)

    Adds a reference to the subscription. This is intended for situations where
    a reference to the subscription needs to be maintained independently of the
    reference returned when the subscription was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **subscr** [IN] -- the subscription to which a reference is to be added. If
    the reference is NULL or invalid an error is returned.


.. function:: int dpiSubscr_prepareStmt(dpiSubscr \*subscr, const char \*sql, \
        uint32_t sqlLength, dpiStmt \**stmt)

    Prepares a statement for registration on the subscription. The statement is
    then registered by calling the function :func:`dpiStmt_execute()`. The
    reference to the statement that is returned should be released as soon as
    it is no longer needed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **subscr** [IN] -- a reference to the subscription on which the statement
    is to be prepared for registration. If the reference is NULL or invalid an
    error is returned.

    **sql** [IN] -- the SQL that is to be prepared, as a byte string in the
    encoding used for CHAR data.

    **sqlLength** [IN] -- the length of the sql parameter, in bytes.

    **stmt** [OUT] -- a reference to the statement that was prepared, which
    will be populated when the function completes successfully.


.. function:: int dpiSubscr_release(dpiSubscr \*subscr)

    Releases a reference to the subscription. A count of the references to the
    subscription is maintained and when this count reaches zero, the memory
    associated with the subscription is freed. The subscription is also
    deregistered so that notifications are no longer sent, if this was not
    already done using the function :func:`dpiConn_unsubscribe()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **subscr** [IN] -- the subscription from which a reference is to be
    released. If the reference is NULL or invalid an error is returned.

