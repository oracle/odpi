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

.. member:: int16_t dpiDataTypeInfo.fsPrecision

    Specifies the fractional seconds precision of the data. This value is only
    populated for timestamp and interval day to second data. For all other
    data the value is zero.

.. member:: dpiObjectType \*dpiDataTypeInfo.objectType

    Specifies a reference to the type of the object. This value is only
    populated for named type data. For all other data the value is NULL. This
    reference is owned by the object attribute, object type or statement and a
    call to :func:`dpiObjectType_addRef()` must be made if the reference is
    going to be used beyond the lifetime of the owning object.

