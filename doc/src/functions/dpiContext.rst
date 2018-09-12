.. _dpiContextFunctions:

ODPI-C Context Functions
------------------------

Context handles are the top level handles created by the library and are used
for all error handling as well as creating pools and standalone connections to
the database. The first call to ODPI-C by any application must be
:func:`dpiContext_create()` which will create the context as well as validate
the version used by the application. Context handles are destroyed by using
the function :func:`dpiContext_destroy()`.

.. function:: int dpiContext_create(unsigned int majorVersion, \
        unsigned int minorVersion, dpiContext \**context, \
        dpiErrorInfo \*errorInfo)

    Creates a new context for interaction with the library. This is the first
    function that must be called and it must have completed successfully before
    any other functions can be called, including in other threads.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.
    If a failure occurs, the errorInfo structure is filled in with error
    information.

    **majorVersion** [IN] -- the major version of the ODPI-C library that is
    being used by the application. This should always be the constant value
    DPI_MAJOR_VERSION defined in the dpi.h being used by the application. It
    must match the major version of the ODPI-C library that is being linked to
    the application.

    **minorVersion** [IN] -- the minor version of the ODPI-C library that is
    being used by the application. This should always be the constant value
    DPI_MINOR_VERSION defined in the dpi.h being used by the application. It
    must be less than or equal to the minor version of the ODPI-C library that
    is being linked to the application.

    **context** [OUT] -- a pointer to a context handle which will be populated
    upon successful completion of this function.

    **errorInfo** [OUT] -- a pointer to a :ref:`dpiErrorInfo<dpiErrorInfo>`
    structure which will be populated with error information if an error takes
    place during the execution of this function. If no error takes place, the
    structure is not modified in any way.


.. function:: int dpiContext_destroy(dpiContext \*context)

    Destroys the context that was earlier created with the function
    :func:`dpiContext_create()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **context** [IN] -- the context handle which should be destroyed. If the
    handle is NULL or invalid an error is returned.


.. function:: void dpiContext_getClientVersion(const dpiContext \*context, \
        dpiVersionInfo \*versionInfo)

    Return information about the version of the Oracle Client that is being
    used.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **versionInfo** [OUT] -- a pointer to a
    :ref:`dpiVersionInfo<dpiVersionInfo>` structure which will be populated
    with the version information of the Oracle Client being used.


.. function:: void dpiContext_getError(const dpiContext \*context, \
        dpiErrorInfo \*errorInfo)

    Returns error information for the last error that was raised by the
    library. This function must be called with the same thread that generated
    the error. It must also be called before any other ODPI-C library calls are
    made on the calling thread since the error information specific to that
    thread is cleared at the start of every ODPI-C function call.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid the error
    information is populated with an invalid context handle error instead.

    **errorInfo** [OUT] -- a pointer to a :ref:`dpiErrorInfo<dpiErrorInfo>`
    structure which will be populated with information about the last error
    that was raised.


.. function:: int dpiContext_initCommonCreateParams( \
        const dpiContext \*context, dpiContextParams \*params)

    Initializes the :ref:`dpiCommonCreateParams<dpiCommonCreateParams>`
    structure to default values.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **params** [OUT] -- a pointer to a
    :ref:`dpiCommonCreateParams<dpiCommonCreateParams>` structure which will be
    populated with default values upon completion of this function.

.. function:: int dpiContext_initConnCreateParams( \
        const dpiContext \*context, dpiConnCreateParams \*params)

    Initializes the :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure
    to default values.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **params** [OUT] -- a pointer to a
    :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure which will be
    populated with default values upon completion of this function.


.. function:: int dpiContext_initPoolCreateParams( \
        const dpiContext \*context, dpiPoolCreateParams \*params)

    Initializes the :ref:`dpiPoolCreateParams<dpiPoolCreateParams>` structure
    to default values.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **params** [OUT] -- a pointer to a
    :ref:`dpiPoolCreateParams<dpiPoolCreateParams>` structure which will be
    populated with default values upon completion of this function.


.. function:: int dpiContext_initSodaOperOptions( \
        const dpiContext \*context, dpiSodaOperOptions \*options)

    Initializes the :ref:`dpiSodaOperOptions<dpiSodaOperOptions>`
    structure to default values.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **options** [OUT] -- a pointer to a
    :ref:`dpiSodaOperOptions<dpiSodaOperOptions>` structure which will be
    populated with default values upon completion of this function.


.. function:: int dpiContext_initSubscrCreateParams( \
        const dpiContext \*context, dpiSubscrCreateParams \*params)

    Initializes the :ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>`
    structure to default values.

    **context** [IN] -- the context handle created earlier using the function
    :func:`dpiContext_create()`. If the handle is NULL or invalid an error is
    returned.

    **params** [OUT] -- a pointer to a
    :ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure which will be
    populated with default values upon completion of this function.

