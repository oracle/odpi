.. _dpiStringList:

ODPI-C Structure dpiStringList
------------------------------

This structure is used for receiving a list of strings from such functions as
:func:`dpiSodaDb_getCollectionNames()` and :func:`dpiSodaColl_listIndexes()`.
The memory referenced by this structure should be freed by the function
:func:`dpiContext_freeStringList()` and should not be manipulated directly.

.. member:: uint32_t dpiStringList.numStrings

    Specifies the number of elements in the strings and stringLengths arrays.

.. member:: const char** dpiStringList.strings

    Specifies an array of pointers to strings. The length of this array is
    specified by the numStrings attribute.

.. member:: uint32_t* dpiStringList.stringLengths

    Specifies an array of lengths. Each element in this array is the length of
    the string found in the corresponding element in the strings array, in
    bytes. The length of this array is specified by the numStrings attribute.
