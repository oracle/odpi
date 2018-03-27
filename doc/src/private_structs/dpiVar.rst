.. _dpiVar:

ODPI-C Private Structure dpiVar
-------------------------------

This private structure represents memory areas used for transferring data to
and from the database and is available by handle to a calling application or
driver. The implementation of this type is found in dpiVar.c. Variables are
created by calling the function :func:`dpiConn_newVar()`. They are destroyed
when the last reference to the variable is released by calling the function
:func:`dpiVar_release()`. They are bound to statements by calling the function
:func:`dpiStmt_bindByName()` or the function :func:`dpiStmt_bindByPos()`. They
can also be used for fetching data from the database by calling the function
:func:`dpiStmt_define()`. All of the attributes of the structure
:ref:`dpiBaseType<dpiBaseType>` are included in this structure in addition to
the ones specific to this structure described below.

.. member:: dpiConn \*dpiVar.conn

    Specifies a pointer to the :ref:`dpiConn<dpiConn>` structure which was used
    to create the variable.

.. member:: const dpiOracleType \*dpiVar.type

    Specifies a pointer to a :ref:`dpiOracleType<dpiOracleType>` structure
    which identifies the type of Oracle data that is being represented by this
    variable.

.. member:: dpiNativeTypeNum dpiVar.nativeTypeNum

    Specifies the native type which will be used to transfer data from the
    calling application or driver to the Oracle database or which will be used
    to transfer data from the database. It will be one of the values from the
    enumeration :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

.. member:: int dpiVar.requiresPreFetch

    Specifies if the variable requires additional processing before each
    internal fetch is performed (1) or not (0).

.. member:: int dpiVar.isArray

    Specifies if the variable refers to a PL/SQL index-by table (1) or not (0).

.. member:: uint32_t dpiVar.sizeInBytes

    Specifies the size in bytes of the buffer used for transferring data to and
    from the Oracle database. This value is 0, however, if dynamic binding is
    being performed.

.. member:: int dpiVar.isDynamic

    Specifies if the variable uses dynamic bind or define techniques to bind or
    fetch data (1) or not (0).

.. member:: dpiObjectType \*dpiVar.objectType

    Specifies a pointer to a :ref:`dpiObjectType<dpiObjectType>` structure
    which is used when the type of data represented by the variable is of type
    DPI_ORACLE_TYPE_OBJECT. In all other cases this value is NULL. If
    specified, the reference is held for the duration of the variable's
    lifetime.

.. member:: dpiVarBuffer dpiVar.buffer

    Specifies the buffer used for transferring data to/from the database. This
    buffer is a :ref:`dpiVarBuffer<dpiVarBuffer>` structure and is used for all
    regular bind/define operations.

.. member:: dpiVarBuffer \*dpiVar.dynBindBuffers

    Specifies an array of structures of type :ref:`dpiVarBuffer<dpiVarBuffer>`
    which are used for DML returning statements. The length of the array
    corresponds to the maxArraySize property of the member
    :member:`dpiVarBuffer.buffer`, but only if a DML returning statement is
    being used and at least some data has been returned. In all other cases
    this value will be NULL.

.. member:: dpiError \*dpiVar.error

    Specifies a pointer to the :ref:`dpiError<dpiError>` structure used during
    dynamic bind and defines.

