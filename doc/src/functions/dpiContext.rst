.. _dpiContextFunctions:

ODPI-C Context Functions
------------------------

Context handles are the top level handles created by the library and are used
for all error handling as well as creating pools and standalone connections to
the database. The first call to ODPI-C by any application must be
:func:`dpiContext_createWithParams()` which will create the context as well as
validate the version used by the application. Context handles are destroyed by
using the function :func:`dpiContext_destroy()`.

.. function:: int dpiContext_createWithParams(unsigned int majorVersion, \
        unsigned int minorVersion, dpiContextCreateParams* params, \
        dpiContext** context, dpiErrorInfo* errorInfo)

    Creates a new context for interaction with the library. This is the first
    function that must be called and it must have completed successfully before
    any other functions can be called, including in other threads.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.
    If a failure occurs, the errorInfo structure is filled in with error
    information.

    .. note::

        The function ``dpiContext_create()`` was replaced by a macro in version
        4 which calls this function with params set to the value NULL.

    .. parameters-table::

        * - ``majorVersion``
          - IN
          - The major version of the ODPI-C library that is being used by the
            application. This should always be the constant value
            DPI_MAJOR_VERSION defined in the dpi.h being used by the
            application. It must match the major version of the ODPI-C library
            that is being linked to the application.
        * - ``minorVersion``
          - IN
          - The minor version of the ODPI-C library that is being used by the
            application. This should always be the constant value
            DPI_MINOR_VERSION defined in the dpi.h being used by the
            application. It must be less than or equal to the minor version of
            the ODPI-C library that is being linked to the application.
        * - ``params``
          - IN
          - A pointer to a
            :ref:`dpiContextCreateParams<dpiContextCreateParams>` structure
            containing parameters used to modify how ODPI-C loads the Oracle
            Client library. Although it is possible to create multiple
            contexts, only the first context created will use these
            parameters. This value can also be NULL in which case default
            parameters will be used.
        * - ``context``
          - OUT
          - A pointer to a context handle which will be populated upon
            successful completion of this function.
        * - ``errorInfo``
          - OUT
          - A pointer to a :ref:`dpiErrorInfo<dpiErrorInfo>` structure which
            will be populated with error information if an error takes place
            during the execution of this function. If no error takes place,
            the structure is not modified in any way. Note that the only
            members of the structure that should be examined when an error
            occurs are message, messageLength, encoding, fnName, and action.

.. function:: int dpiContext_destroy(dpiContext* context)

    Destroys the context that was earlier created with the function
    :func:`dpiContext_createWithParams()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle which should be destroyed. If the handle is NULL
            or invalid, an error is returned.

.. function:: int dpiContext_freeStringList(dpiContext* context, \
        dpiStringList* list)

    Frees the memory associated with the string list allocated by a call to
    one of the functions :func:`dpiSodaDb_getCollectionNames()`
    or :func:`dpiSodaColl_listIndexes()`. This function should not be
    called without first calling one of those functions first.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - A reference to the context in which the string list was allocated.
        * - ``list``
          - IN
          - A pointer to a structure of type
            :ref:`dpiStringList<dpiStringList>` which was previously
            used in a call to :func:`dpiSodaDb_getCollectionNames()` or
            :func:`dpiSodaColl_listIndexes()`.

.. function:: int dpiContext_getClientVersion(const dpiContext* context, \
        dpiVersionInfo* versionInfo)

    Return information about the version of the Oracle Client that is being
    used.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, an error is returned.
        * - ``versionInfo``
          - OUT
          - A pointer to a :ref:`dpiVersionInfo<dpiVersionInfo>` structure
            which will be populated with the version information of the Oracle
            Client being used.

.. function:: void dpiContext_getError(const dpiContext* context, \
        dpiErrorInfo* errorInfo)

    Returns information for the last error or warning that was raised by the
    library. This function must be called with the same thread that generated
    the error or warning. It must also be called before any other ODPI-C
    library calls are made on the calling thread since the error/warning
    information specific to that thread is cleared at the start of every ODPI-C
    function call.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, the error information is populated with an invalid context
            handle error instead.
        * - ``errorInfo``
          - OUT
          - A pointer to a :ref:`dpiErrorInfo<dpiErrorInfo>` structure which
            will be populated with information about the last error or warning
            that was raised. If a warning was raised, the
            :member:`dpiErrorInfo.isWarning` flag will be set to the value 1.

.. function:: int dpiContext_initCommonCreateParams( \
        const dpiContext* context, dpiContextParams* params)

    Initializes the :ref:`dpiCommonCreateParams<dpiCommonCreateParams>`
    structure to default values.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, an error is returned.
        * - ``params``
          - OUT
          - A pointer to a :ref:`dpiCommonCreateParams<dpiCommonCreateParams>`
            structure which will be populated with default values upon
            completion of this function.

.. function:: int dpiContext_initConnCreateParams( \
        const dpiContext* context, dpiConnCreateParams* params)

    Initializes the :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure
    to default values.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, an error is returned.
        * - ``params``
          - OUT
          - A pointer to a :ref:`dpiConnCreateParams<dpiConnCreateParams>`
            structure which will be populated with default values upon
            completion of this function.

.. function:: int dpiContext_initPoolCreateParams( \
        const dpiContext* context, dpiPoolCreateParams* params)

    Initializes the :ref:`dpiPoolCreateParams<dpiPoolCreateParams>` structure
    to default values.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, an error is returned.
        * - ``params``
          - OUT
          - A pointer to a :ref:`dpiPoolCreateParams<dpiPoolCreateParams>`
            structure which will be populated with default values upon
            completion of this function.

.. function:: int dpiContext_initSodaOperOptions( \
        const dpiContext* context, dpiSodaOperOptions* options)

    Initializes the :ref:`dpiSodaOperOptions<dpiSodaOperOptions>`
    structure to default values.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, an error is returned.
        * - ``options``
          - OUT
          - A pointer to a :ref:`dpiSodaOperOptions<dpiSodaOperOptions>`
            structure which will be populated with default values upon
            completion of this function.

.. function:: int dpiContext_initSubscrCreateParams( \
        const dpiContext* context, dpiSubscrCreateParams* params)

    Initializes the :ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>`
    structure to default values.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    .. parameters-table::

        * - ``context``
          - IN
          - The context handle created earlier using the function
            :func:`dpiContext_createWithParams()`. If the handle is NULL or
            invalid, an error is returned.
        * - ``params``
          - OUT
          - A pointer to a :ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>`
            structure which will be populated with default values upon completion
            of this function.
