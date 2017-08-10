.. _dpiObjectType:

ODPI-C Private Structure dpiObjectType
--------------------------------------

This private structure represents types such as those created by the SQL
command CREATE OR REPLACE TYPE and is available by handle to a calling
application or driver.  The implementation for this type is found in
dpiObjectType.c. Object types are created by calling the function
:func:`dpiConn_getObjectType()` or implicitly when calling the function
:func:`dpiStmt_getQueryInfo()` on a query column that contains objects. They
are also created implicitly by calling the function
:func:`dpiObjectAttr_getInfo()` on an attribute that contains objects or by
calling the function :func:`dpiObjectType_getInfo()` on a collection that
contains objects. Object types are destroyed when the last reference to them is
released by the call :func:`dpiObjectType_release()`. All of the attributes of
the structure :ref:`dpiBaseType<dpiBaseType>` are included in this structure in
addition to the ones specific to this structure described below.

.. member:: dpiConn \*dpiObjectType.conn

    Specifies a pointer to the :ref:`dpiConn<dpiConn>` structure which was used
    to create this structure.

.. member:: OCIType \*dpiObjectType.tdo

    Specifies the OCI TDO (type descriptor) handle.

.. member:: OCITypeCode dpiObjectType.typeCode

    Specifies the OCI type code.

.. member:: const char \*dpiObjectType.schema

    Specifies the schema of the type, as a byte string in the encoding used
    for CHAR data.

.. member:: uint32_t dpiObjectType.schemaLength

    Specifies the length of the :member:`dpiObjectType.schema` member, in
    bytes.

.. member:: const char \*dpiObjectType.name

    Specifies the name of the type, as a byte string in the encoding used for
    CHAR data.

.. member:: uint32_t dpiObjectType.nameLength

    Specifies the length of the :member:`dpiObjectType.name` member, in bytes.

.. member:: dpiDataTypeInfo dpiObjectType.elementTypeInfo

    Specifies the type of data of the elements of the collection. This is a
    structure of type :ref:`dpiDataTypeInfo<dpiDataTypeInfo>` and is only valid
    if the object type refers to a collection.

.. member:: boolean dpiObjectType.isCollection

    Specifies if the type refers to a collection (1) or not (0).

.. member:: uint16_t dpiObjectType.numAttributes

    Specifies how many attributes the type has.

