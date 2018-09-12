.. _dpiObjectAttrInfo:

ODPI-C Structure dpiObjectAttrInfo
----------------------------------

This structure is used for passing information about an object type from ODPI-C.
It is used by the function :func:`dpiObjectAttr_getInfo()`.

.. member:: const char \*dpiObjectAttrInfo.name

    Specifies the name of the attribute, as a byte string in the encoding used
    for CHAR data.

.. member:: uint32_t dpiObjectAttrInfo.nameLength

    Specifies the length of the :member:`dpiObjectAttrInfo.name` member, in
    bytes.

.. member:: dpiDataTypeInfo dpiObjectAttrInfo.typeInfo

    Specifies the type of data of the attribute. It is a structure of type
    :ref:`dpiDataTypeInfo<dpiDataTypeInfo>`.

