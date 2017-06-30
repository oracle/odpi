.. _dpiObject:

ODPI-C Private Structure dpiObject
----------------------------------

This private structure represents instances of the types created by the SQL
command CREATE OR REPLACE TYPE and is available by handle to a calling
application or driver. The implementation for this type is found in
dpiObject.c. An object is created by calling the function
:func:`dpiObjectType_createObject()` or by calling the function
:func:`dpiObject_copy()`. They are also created implicitly by creating a
variable of the type DPI_ORACLE_TYPE_OBJECT. Objects are destroyed when the
last reference is released by calling the function :func:`dpiObject_release()`.
All of the attributes of the structure :ref:`dpiBaseType<dpiBaseType>` are
included in this structure in addition to the ones specific to this structure
described below.

.. member:: dpiObjectType \*dpiObject.type

    Specifies a pointer to the :ref:`dpiObjectType<dpiObjectType>` structure
    which was used to create the object.

.. member:: dvoid \*dpiObject.instance

    Specifies a pointer to the object instance.

.. member:: dvoid \*dpiObject.indicator

    Specifies a pointer to the object indicator.

.. member:: int dpiObject.isIndependent

    Specifies whether the object is independent (1) or not (0). An object is
    independent if it was created directly, not indirectly by getting another
    object's attribute (where the child object remains part of the parent
    object's contents).

