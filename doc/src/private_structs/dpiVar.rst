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

.. member:: uint32_t dpiVar.maxArraySize

    Specifies the number of rows in the buffers used for transferring data to
    and from the database. This value corresponds to the maximum size of any
    PL/SQL index-by table that can be represented by this variable or the
    maximum number of rows that can be fetched into this variable or the
    maximum number of iterations that can be processed using the function
    :func:`dpiStmt_executeMany()`.

.. member:: uint32_t dpiVar.actualArraySize

    Specifies the actual number of elements in a PL/SQL index-by table when
    the member :member:`dpiVar.isArray` is set to 1; otherwise, if the variable
    is one of the output bind variables of a DML returning statement, this
    value is set to the number of rows returned by the DML returning statement.
    In all other cases, this value is set to the same value as the member
    :member:`dpiVar.maxArraySize`.

.. member:: int dpiVar.requiresPreFetch

    Specifies if the variable requires additional processing before each
    internal fetch is performed (1) or not (0).

.. member:: int dpiVar.isArray

    Specifies if the variable refers to a PL/SQL index-by table (1) or not (0).

.. member:: int16_t \*dpiVar.indicator

    Specifies an array of indicator values. The size of this array corresponds
    to the value in the member :member:`dpiVar.maxArraySize`. These values
    indicate if the element in the array is null (OCI_IND_NULL) or not
    (OCI_IND_NOTNULL).

.. member:: uint16_t \*dpiVar.returnCode

    Specifies an array of return code values. The size of this array
    corresponds to the value in the member :member:`dpiVar.maxArraySize`. These
    values are checked before returning a value to the calling application or
    driver. If the value is non-zero an exception is raised. This array is only
    allocated for variable length data (strings and raw byte strings). In all
    other cases this value is NULL.

.. member:: DPI_ACTUAL_LENGTH_TYPE \*dpiVar.actualLength

    Specifies an array of actual lengths. The size of this array corresponds to
    the value in the member :member:`dpiVar.maxArraySize`. For releases prior
    to 12.1, these are 16-bit integers and for 12.1 and higher these are 32-bit
    integers. This array is only allocated for variable length data (strings
    and raw byte strings). In all other cases this value is NULL.

.. member:: uint32_t \*dpiVar.dynamicActualLength

    Specifies an array of actual lengths that is used during dynamic binds.
    This array is only present in the structure for releases prior to 12.1,
    since the normal actual lengths those releases support are only 16-bit.

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

.. member:: dvoid \**dpiVar.objectIndicator

    Specifies an array of object indicator arrays which uses used when the type
    of data represented by the variable is of type DPI_ORACLE_TYPE_OBJECT. The
    size of this array corresponds to the value in the member
    :member:`dpiVar.maxArraySize`. In all other cases this value is NULL.

.. member:: dpiReferenceBuffer \*dpiVar.references

    Specifies an array of reference buffers of type
    :ref:`dpiReferenceBuffer<dpiReferenceBuffer>`. The size of this array
    corresponds to the value in the member :member:`dpiVar.maxArraySize`. These
    buffers are stored when the type of data represented by the variable is of
    type DPI_ORACLE_TYPE_OBJECT, DPI_ORACLE_TYPE_STMT or DPI_ORACLE_TYPE_CLOB,
    DPI_ORACLE_TYPE_BLOB, DPI_ORACLE_TYPE_NCLOB or DPI_ORACLE_TYPE_BFILE. In
    all other cases this value is NULL.

.. member:: dpiDynamicBytes \*dpiVar.dynamicBytes

    Specifies an array of :ref:`dpiDynamicBytes<dpiDynamicBytes>` structures.
    The size of this array corresponds to the value in the member
    :member:`dpiVar.maxArraySize`. This array is allocated when long strings or
    long raw byte strings (lengths of more than 32K) are being used to transfer
    data to and from the Oracle database. In all other cases this value is
    NULL.

.. member:: char \*dpiVar.tempBuffer

    Specifies a set of temporary buffers which are used to handle conversion
    from the Oracle data type OCINumber to a string, in other words when the
    Oracle data type is DPI_ORACLE_TYPE_NUMBER and the native type is
    DPI_NATIVE_TYPE_BYTES. In all other cases this value is NULL.

.. member:: dpiData \*dpiVar.externalData

    Specifies an array of :ref:`dpiData<dpiData>` structures which are used to
    transfer data from native types to Oracle data types. The size of this
    array corresponds to the value in the member :member:`dpiVar.maxArraySize`.
    This array is made available to the calling application or driver to
    simplify and streamline data transfer.

.. member:: dpiOracleData dpiVar.data

    Specifies the buffers used by OCI to transfer data to and from the Oracle
    database using the structure :ref:`dpiOracleData<dpiOracleData>`. After
    execution or internal fetches are performed the data in these buffers is
    transferred to and from the array found in the member
    :member:`dpiVar.externalData`.

.. member:: dpiError \*dpiVar.error

    Specifies a pointer to the :ref:`dpiError<dpiError>` structure used during
    dynamic bind and defines.

