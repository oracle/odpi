.. _dpiOracleData:

ODPI-C Private Union dpiOracleData
----------------------------------

This private union is used to avoid casts. The data referenced here is the data
that is actually bound to or fetched from statements and represents the array
of values that have been bound or will be fetched. It is used by the structure
:ref:`dpiVar<dpiVar>` and it is also used for getting data into and out of
Oracle object instances.

.. member:: void \*dpiOracleData.asRaw

    Specifies the pointer to the buffer allocated for the variable. The buffer
    allocated for the variable is a single contiguous piece of memory sectioned
    into array elements specific to the type of data being bound or fetched.

.. member:: char \*dpiOracleData.asBytes

    Specifies the buffer as a series of byte buffers of the length
    corresponding to the variable member :member:`dpiVar.sizeInBytes`.

.. member:: float \*dpiOracleData.asFloat

    Specifies an array of floats.

.. member:: double \*dpiOracleData.asDouble

    Specifies an array of doubles.

.. member:: int64_t \*dpiOracleData.asInt64

    Specifies an array of 64-bit integers.

.. member:: dpiOciNumber \*dpiOracleData.asNumber

    Specifies an array of OCINumber structures.

.. member:: dpiOciDate \*dpiOracleData.asDate

    Specifies an array of OCIDate structures.

.. member:: void \**dpiOracleData.asTimestamp

    Specifies an array of OCIDateTime handles.

.. member:: void \**dpiOracleData.asInterval

    Specifies an array of OCIInterval handles.

.. member:: void \**dpiOracleData.asLobLocator

    Specifies an array of OCILobLocator handles.

.. member:: void \**dpiOracleData.asString

    Specifies an array of OCIString handles.

.. member:: void \**dpiOracleData.asStmt

    Specifies an array of OCIStmt handles.

.. member:: void \**dpiOracleData.asRowid

    Specifies an array of OCIRowid handles.

.. member:: int \*dpiOracleData.asBoolean

    Specifies an array of booleans.

.. member:: void \**dpiOracleData.asObject

    Specifies an array of object instances.

.. member:: void \**dpiOracleData.asCollection

    Specifies an array of OCIColl handles.

