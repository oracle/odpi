.. _dpiAppContext:

ODPI-C Structure dpiAppContext
------------------------------

This structure is used for passing application context to the database during
the process of creating standalone connections. These values are ignored when
acquiring a connection from a session pool or when using DRCP (Database
Resident Connection Pooling). All values must be set to valid values prior to
being used in the :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure and
must remain valid until the execution of :func:`dpiConn_create()` completes.
Values set using this structure are available in logon triggers by using the
sys_context() SQL function.

.. member:: const char \*dpiAppContext.namespaceName

    Specifies the value of the "namespace" parameter to sys_context(). It is
    expected to be a byte string in the encoding specified in the
    :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure and must not be
    NULL.

.. member:: uint32_t dpiAppContext.namespaceNameLength

    Specifies the length of the :member:`dpiAppContext.namespaceName` member,
    in bytes.

.. member:: const char \*dpiAppContext.name

    Specifies the value of the "parameter" parameter to sys_context(). It is
    expected to be a byte string in the encoding specified in the
    :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure and must not be
    NULL.

.. member:: uint32_t dpiAppContext.nameLength

    Specifies the length of the :member:`dpiAppContext.name` member, in bytes.

.. member:: const char \*dpiAppContext.value

    Specifies the value that will be returned from sys_context(). It is
    expected to be a byte string in the encoding specified in the
    :ref:`dpiConnCreateParams<dpiConnCreateParams>` structure and must not be
    NULL.

.. member:: uint32_t dpiAppContext.valueLength

    Specifies the length of the :member:`dpiAppContext.value` member, in bytes.

