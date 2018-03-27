.. _dpiVarBuffer:

ODPI-C Private Structure dpiVarBuffer
-------------------------------------

This private structure represents memory areas used for transferring data to
and from the database and is used internally by the structure
:ref:`dpiVar<dpiVar>`.

.. member:: uint32_t dpiVarBuffer.maxArraySize

    Specifies the number of rows in the buffers used for transferring data to
    and from the database. This value corresponds to the maximum size of any
    PL/SQL index-by table that can be represented by this variable or the
    maximum number of rows that can be fetched into this variable or the
    maximum number of iterations that can be processed using the function
    :func:`dpiStmt_executeMany()`.

.. member:: uint32_t dpiVarBuffer.actualArraySize

    Specifies the actual number of elements in a PL/SQL index-by table when
    the member :member:`dpiVar.isArray` is set to 1; otherwise, this value is
    set to the same value as the member :member:`dpiVarBuffer.maxArraySize`.

.. member:: int16_t \*dpiVarBuffer.indicator

    Specifies an array of indicator values. The size of this array corresponds
    to the value in the member :member:`dpiVarBuffer.maxArraySize`. These
    values indicate if the element in the array is null (OCI_IND_NULL) or not
    (OCI_IND_NOTNULL).

.. member:: uint16_t \*dpiVarBuffer.returnCode

    Specifies an array of return code values. The size of this array
    corresponds to the value in the member :member:`dpiVarBuffer.maxArraySize`.
    These values are checked before returning a value to the calling
    application or driver. If the value is non-zero an exception is raised.
    This array is only allocated for variable length data (strings and raw byte
    strings). In all other cases this value is NULL.

.. member:: uint16_t \*dpiVarBuffer.actualLength16

    Specifies an array of actual lengths used for Oracle 11g. The size of this
    array corresponds to the value in the member
    :member:`dpiVarBuffer.maxArraySize`. This array is only allocated for
    variable length data (strings and raw byte strings) and only for Oracle
    11g. In all other cases this value is NULL.

.. member:: uint32_t \*dpiVarBuffer.actualLength32

    Specifies an array of actual lengths used for Oracle 12c and higher or
    when dynamic binding takes place for Oracle 11g. The size of this
    array corresponds to the value in the member
    :member:`dpiVarBuffer.maxArraySize`. This array is only allocated for
    variable length data (strings and raw byte strings). It is also only
    allocated for 12c and higher or when dynamic binding takes place for Oracle
    11g. In all other cases this value is NULL.

.. member:: dvoid \**dpiVarBuffer.objectIndicator

    Specifies an array of object indicator arrays which uses used when the type
    of data represented by the variable is of type DPI_ORACLE_TYPE_OBJECT. The
    size of this array corresponds to the value in the member
    :member:`dpiVarBuffer.maxArraySize`. In all other cases this value is NULL.

.. member:: dpiReferenceBuffer \*dpiVarBuffer.references

    Specifies an array of reference buffers of type
    :ref:`dpiReferenceBuffer<dpiReferenceBuffer>`. The size of this array
    corresponds to the value in the member :member:`dpiVarBuffer.maxArraySize`.
    These buffers are stored when the type of data represented by the variable
    is of type DPI_ORACLE_TYPE_OBJECT, DPI_ORACLE_TYPE_STMT or
    DPI_ORACLE_TYPE_CLOB, DPI_ORACLE_TYPE_BLOB, DPI_ORACLE_TYPE_NCLOB or
    DPI_ORACLE_TYPE_BFILE. In all other cases this value is NULL.

.. member:: dpiDynamicBytes \*dpiVarBuffer.dynamicBytes

    Specifies an array of :ref:`dpiDynamicBytes<dpiDynamicBytes>` structures.
    The size of this array corresponds to the value in the member
    :member:`dpiVarBuffer.maxArraySize`. This array is allocated when long
    strings or long raw byte strings (lengths of more than 32K) are being used
    to transfer data to and from the Oracle database. In all other cases this
    value is NULL.

.. member:: char \*dpiVarBuffer.tempBuffer

    Specifies a set of temporary buffers which are used to handle conversion
    from the Oracle data type OCINumber to a string, in other words when the
    Oracle data type is DPI_ORACLE_TYPE_NUMBER and the native type is
    DPI_NATIVE_TYPE_BYTES. In all other cases this value is NULL.

.. member:: dpiData \*dpiVarBuffer.externalData

    Specifies an array of :ref:`dpiData<dpiData>` structures which are used to
    transfer data from native types to Oracle data types. The size of this
    array corresponds to the value in the member
    :member:`dpiVarBuffer.maxArraySize`.  This array is made available to the
    calling application or driver to simplify and streamline data transfer.

.. member:: dpiOracleData dpiVarBuffer.data

    Specifies the buffers used by OCI to transfer data to and from the Oracle
    database using the structure :ref:`dpiOracleData<dpiOracleData>`. After
    execution or internal fetches are performed, the data in these buffers is
    transferred to and from the array found in the member
    :member:`dpiVarBuffer.externalData`.

