.. _dpiObjectAttr:

dpiObjectAttr
-------------

This structure represents attributes of the types created by the SQL command
CREATE OR REPLACE TYPE and is available by handle to a calling application or
driver. The implementation for this type is found in dpiObjectAttr.c.
Attributes are created by a call to the function
:func:`dpiObjectType_getAttributes()` and are destroyed when the last reference
is released by calling the function :func:`dpiObjectAttr_release()`. All of the
attributes of the structure :ref:`dpiBaseType` are included in this structure
in addition to the ones specific to this structure described below.

.. member:: dpiObjectType \*dpiObjectAttr.belongsToType

    Specifies a pointer to the :ref:`dpiObjectType` structure to which this
    attribute belongs.

.. member:: const char \*dpiObjectAttr.name

    Specifies the name of the attribute, as a byte string in the encoding used
    for CHAR data.

.. member:: uint32_t dpiObjectAttr.nameLength

    Specifies the length of the :member:`dpiObjectAttr.name` member, in bytes.

.. member:: const dpiOracleType \*dpiObjectAttr.oracleType

    Specifies a pointer to the :ref:`dpiOracleType` structure which identifies
    the type of data stored in the attribute.

.. member:: dpiObjectType \*dpiObjectAttr.type

    Specifies a pointer to the :ref:`dpiObjectType` structure which identifies
    the type of object stored in the attribute, if the attribute refers to an
    object. In all other cases this value is NULL.

