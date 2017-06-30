.. _dpiBaseType:

ODPI-C Private Structure dpiBaseType
------------------------------------

This private structure contains the base attributes that all handles exposed
publicly have. Generic functions for checking and manipulating handles are
found in the file dpiGen.c.

.. member:: const dpiTypeDef \*dpiBaseType.typeDef

    Specifies a pointer to the :ref:`dpiTypeDef<dpiTypeDef>` structure which
    identifies the type of handle.

.. member:: uint32_t dpiBaseType.checkInt

    Specifies the check integer that the handle was given when it was created.
    In order for the handle to be valid it must match the value of the
    :member:`dpiTypeDef.checkInt` corresponding to the type identified by the
    :member:`dpiBaseType.typeDef` member. When the handle is freed, this check
    integer is set to zero so that subsequent attempts to use the handle will
    result in an invalid handle error.

.. member:: unsigned dpiBaseType.refCount

    Specifies the number of references that are held to the handle. These
    references can be held internally by the library or externally by the
    calling application or driver. When a handle is created it starts with a
    reference count of 1. When the reference count reaches zero, the free
    procedure associated with the type is called.

.. member:: dpiEnv \*dpiBaseType.env

    Specifies a pointer to the :ref:`dpiEnv<dpiEnv>` structure which was used
    to create this handle.

