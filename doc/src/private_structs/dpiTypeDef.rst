.. _dpiTypeDef:

ODPI-C Private Structure dpiTypeDef
-----------------------------------

This private structure is used to identify metadata for the different types of
handles that are exposed publicly. A list of these structures (defined as
constants) can be found in dpiGen.c. An enumeration called dpiHandleTypeNum is
used to identify the structures.

.. member:: const char \*dpiTypeDef.name

    Specifies the public name of the handle. This name is used in error
    messages such as when an invalid handle is received by the calling
    application.

.. member:: size_t dpiTypeDef.size

    Specifies the size of the structure. Memory corresponding to this size is
    allocated when a handle is created.

.. member:: uint32_t dpiTypeDef.checkInt

    Specifies the check integer that the handle is given when it is created.

.. member:: dpiTypeFreeProc dpiTypeDef.freeProc

    Specifies the procedure that is called to free the memory and resources
    associated with the handle when its reference count reaches zero.

