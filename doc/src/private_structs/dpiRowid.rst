.. _dpiRowid:

ODPI-C Private Structure dpiRowid
---------------------------------

This private structure is used to represent the unique identifier of a row in
the database and is available by handle to a calling application or driver. The
implementation for this type is found in dpiRowid.c. Rowids cannot be created
or set directly but are created implicitly when a variable of type
DPI_ORACLE_TYPE_ROWID is created. They are destroyed when the last reference is
released by a call to the function :func:`dpiRowid_release()`. All of the
attributes of the structure :ref:`dpiBaseType<dpiBaseType>` are included in
this structure in addition to the ones specific to this structure described
below.

.. member:: OCIRowid \*dpiRowid.handle

    Specifies the OCIRowid descriptor handle.

.. member:: char \*dpiRowid.buffer

    Specifies a buffer used for storing the string representation of the rowid,
    when that information is requested by means of calling the function
    :func:`dpiRowid_getStringValue()`. In all other cases this value is NULL.

.. member:: uint16_t dpiRowid.bufferLength

    Specifies the length of the string representation of the rowid, in bytes.
    If the buffer is NULL because no call has been made to the function
    :func:`dpiRowid_getStringValue()`, this value will be 0.

