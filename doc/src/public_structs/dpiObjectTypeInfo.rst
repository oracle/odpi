.. _dpiObjectTypeInfo:

dpiObjectTypeInfo
-----------------

This structure is used for passing information about an object type from ODPI-C.
It is used by the function :func:`dpiObjectType_getInfo()`.

.. member:: const char \*dpiObjectTypeInfo.schema

    Specifies the schema which owns the object type, as a byte string in the
    encoding used for CHAR data.

.. member:: uint32_t dpiObjectTypeInfo.schemaLength

    Specifies the length of the :member:`dpiObjectTypeInfo.schema` member, in
    bytes.

.. member:: const char \*dpiObjectTypeInfo.name

    Specifies the name of the object type, as a byte string in the encoding
    used for CHAR data.

.. member:: uint32_t dpiObjectTypeInfo.nameLength

    Specifies the length of the :member:`dpiObjectTypeInfo.name` member, in
    bytes.

.. member:: int dpiObjectTypeInfo.isCollection

    Specifies if the object type is a collection (1) or not (0).

.. member:: dpiOracleTypeNum dpiObjectTypeInfo.elementOracleTypeNum

    Specifies the Oracle type of the elements in the collection if the object
    type refers to a collection. It will be one of the values from the
    enumeration :ref:`dpiOracleTypeNum`.

.. member:: dpiNativeTypeNum dpiObjectTypeInfo.elementDefaultNativeTypeNum

    Specifies the default native type of the elements in the collection if the
    object type refers to a collection. It will be one of the values from the
    enumeration :ref:`dpiNativeTypeNum`.

.. member:: dpiObjectType \*dpiObjectTypeInfo.elementObjectType

    Specifies a reference to the object type of the elements in the collection
    if the object type on which info is being returned refers to a collection.

.. member:: uint16_t dpiObjectTypeInfo.numAttributes

    Specifies the number of attributes that the object type has.

