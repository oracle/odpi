.. _dpiSodaCollNames:

ODPI-C Structure dpiSodaCollNames
---------------------------------

This structure is used for receiving an array of collection names from a SODA
database and is populated by the function
:func:`dpiSodaDb_getCollectionNames()`. The memory referenced by this structure
should be freed by the function :func:`dpiSodaDb_freeCollectionNames()` and
should not be manipulated directly.

.. member:: uint32_t dpiSodaCollNames.numNames

    Specifies the number of elements in the names and nameLengths arrays.

.. member:: const char \**dpiSodaCollNames.names

    Specifies an array of pointers to names. The length of this array is
    specified by the numNames attribute.

.. member:: uint32_t \*dpiSodaCollNames.nameLengths

    Specifies an array of lengths. Each element in this array is the length of
    the name found in the corresponding element in the names array, in bytes.
    The length of this array is specified by the numNames attribute.

