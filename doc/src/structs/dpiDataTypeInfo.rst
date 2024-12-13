.. _dpiDataTypeInfo:

ODPI-C Structure dpiDataTypeInfo
--------------------------------

This structure is used for passing data type information from ODPI-C. It is
part of the structures :ref:`dpiObjectAttrInfo<dpiObjectAttrInfo>`,
:ref:`dpiObjectTypeInfo<dpiObjectTypeInfo>` and
:ref:`dpiQueryInfo<dpiQueryInfo>`.

.. member:: dpiOracleTypeNum dpiDataTypeInfo.oracleTypeNum

    Specifies the type of the data. It will be one of the values from the
    enumeration :ref:`dpiOracleTypeNum<dpiOracleTypeNum>`, or 0 if the type
    is not supported by ODPI-C.

.. member:: dpiNativeTypeNum dpiDataTypeInfo.defaultNativeTypeNum

    Specifies the default native type for the data. It will be one of the
    values from the enumeration :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`, or 0
    if the type is not supported by ODPI-C.

.. member:: uint16_t dpiDataTypeInfo.ociTypeCode

    Specifies the OCI type code for the data, which can be useful if the type
    is not supported by ODPI-C.

.. member:: uint32_t dpiDataTypeInfo.dbSizeInBytes

    Specifies the size in bytes (from the database's perspective) of the data.
    This value is only populated for strings and binary data. For all other
    data the value is zero.

.. member:: uint32_t dpiDataTypeInfo.clientSizeInBytes

    Specifies the size in bytes (from the client's perspective) of the data.
    This value is only populated for strings and binary data. For all other
    data the value is zero.

.. member:: uint32_t dpiDataTypeInfo.sizeInChars

    Specifies the size in characters of the data. This value is only populated
    for string data. For all other data the value is zero.

.. member:: int16_t dpiDataTypeInfo.precision

    Specifies the precision of the data. This value is only populated for
    numeric and interval data. For all other data the value is zero.

.. member:: int8_t dpiDataTypeInfo.scale

    Specifies the scale of the data. This value is only populated for numeric
    data. For all other data the value is zero.

.. member:: uint8_t dpiDataTypeInfo.fsPrecision

    Specifies the fractional seconds precision of the data. This value is only
    populated for timestamp and interval day to second data. For all other
    data the value is zero.

.. member:: dpiObjectType* dpiDataTypeInfo.objectType

    Specifies a reference to the type of the object. This value is only
    populated for named type data. For all other data the value is NULL. This
    reference is owned by the object attribute, object type or statement and a
    call to :func:`dpiObjectType_addRef()` must be made if the reference is
    going to be used beyond the lifetime of the owning object.

.. member:: int dpiDataTypeInfo.isJson

    Specifies if the data contains JSON. This is set to true when columns are
    fetched that are of type ``DPI_ORACLE_TYPE_JSON`` or if the column fetched
    has the check constraint "IS JSON" enabled.

.. member:: const char* dpiDataTypeInfo.domainSchema

    Specifies the schema of the `SQL domain
    <https://docs.oracle.com/en/database/oracle/oracle-database/23/sqlrf/create-domain.html#GUID-17D3A9C6-D993-4E94-BF6B-CACA56581F41>`__
    associated with the column which is being queried, as a byte string in the
    encoding used for CHAR data.

.. member:: uint32_t dpiDataTypeInfo.domainSchemaLength

    Specifies the length of the :member:`dpiDataTypeInfo.domainSchema` member,
    in bytes.

.. member:: const char* dpiDataTypeInfo.domainName

    Specifies the name of the `SQL domain
    <https://docs.oracle.com/en/database/oracle/oracle-database/23/sqlrf/create-domain.html#GUID-17D3A9C6-D993-4E94-BF6B-CACA56581F41>`__
    associated with the column which is being queried, as a byte string in the
    encoding used for CHAR data.

.. member:: uint32_t dpiDataTypeInfo.domainNameLength

    Specifies the length of the :member:`dpiDataTypeInfo.domainName` member,
    in bytes.

.. member:: uint32_t dpiDataTypeInfo.numAnnotations

    Specifies the number of annotations associated with the column which is
    being fetched.  It is the length of :member:`dpiDataTypeInfo.annotations`.

.. member:: dpiAnnotation* dpiDataTypeInfo.annotations

    Specifies an array of structures of type
    :ref:`dpiAnnotation<dpiAnnotation>`. The length of the array is specified
    in the :member:`dpiDataTypeInfo.numAnnotations` member.

.. member:: int dpiDataTypeInfo.isOson

    Specifies if the data contains binary encoded OSON. This is set to true
    when columns are fetched that have the check constraint "IS JSON FORMAT
    OSON" enabled.

.. member:: uint32_t dpiDataTypeInfo.vectorDimensions

    Specifies the number of dimensions that a vector column contains. This
    value will be 0 if :member:`~dpiDataTypeInfo.oracleTypeNum` is not
    DPI_ORACLE_TYPE_VECTOR or the vector columns uses a flexible number of
    dimensions.

.. member:: uint8_t dpiDataTypeInfo.vectorFormat

    Specifies the storage format of the vector column. This value will be 0 if
    :member:`~dpiDataTypeInfo.oracleTypeNum` is not DPI_ORACLE_TYPE_VECTOR or
    the vector column uses a flexible storage format. If a value is set it will
    be one of the values in the eneumeration
    :ref:`dpiVectorFormat<dpiVectorFormat>`.

.. member:: uint8_t dpiDataTypeInfo.vectorFlags

    Specifies flags for the vector column. This value will be 0 if
    :member:`~dpiDataTypeInfo.oracleTypeNum` is not DPI_ORACLE_TYPE_VECTOR or
    no flags have been set. If flags have been set, they will be one or more of
    the values in the enumeration :ref:`dpiVectorFlags<dpiVectorFlags>`, or'ed
    together.
