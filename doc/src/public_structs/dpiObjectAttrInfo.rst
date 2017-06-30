.. _dpiObjectAttrInfo:

ODPI-C Public Structure dpiObjectAttrInfo
-----------------------------------------

This structure is used for passing information about an object type from ODPI-C.
It is used by the function :func:`dpiObjectAttr_getInfo()`.

.. member:: const char \*dpiObjectAttrInfo.name

    Specifies the name of the attribute, as a byte string in the encoding used
    for CHAR data.

.. member:: uint32_t dpiObjectAttrInfo.nameLength

    Specifies the length of the :member:`dpiObjectAttrInfo.name` member, in
    bytes.

.. member:: dpiOracleTypeNum dpiObjectAttrInfo.oracleTypeNum

    Specifices the Oracle type of the attribute. It will be one of the values
    from the enumeration :ref:`dpiOracleTypeNum<dpiOracleTypeNum>`.

.. member:: dpiNativeTypeNum dpiObjectAttrInfo.defaultNativeTypeNum

    Specifices the default native type of the attribute. It will be one of the
    values from the enumeration :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

.. member:: dpiObjectType \*dpiObjectAttrInfo.objectType

    Specifies a reference to the object type of the attribute, if the attribute
    refers to a named type; otherwise it is NULL.

