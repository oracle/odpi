.. _dpiObjectTypeInfo:

ODPI-C Structure dpiObjectTypeInfo
----------------------------------

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

.. member:: dpiDataTypeInfo dpiObjectTypeInfo.elementTypeInfo

    Specifies the type of data of the elements in the collection. It is a
    structure of type :ref:`dpiDataTypeInfo<dpiDataTypeInfo>` which is only
    valid if the object type is a collection.

.. member:: uint16_t dpiObjectTypeInfo.numAttributes

    Specifies the number of attributes that the object type has.

