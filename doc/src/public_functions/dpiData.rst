.. _dpiDataFunctions:

ODPI-C Public Data Functions
----------------------------

All of these functions are used for getting and setting the various members of
the :ref:`dpiData<dpiData>` structure. The members of the structure can be
manipulated directly but some languages (such as Go) do not have the ability to
manipulate structures containing unions or the ability to process macros. For
this reason, none of these functions perform any error checking. They are
assumed to be replacements for direct manipulation of the various members of
the structure.

.. function:: int dpiData_getBool(dpiData \*data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_BOOLEAN.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: dpiBytes \*dpiData_getBytes(dpiData \*data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_BYTES.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: double dpiData_getDouble(dpiData \*data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_DOUBLE.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: float dpiData_getFloat(dpiData \*data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_FLOAT.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: int64_t dpiData_getInt64(dpiData \*data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_INT64.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: dpiIntervalDS \*dpiData_getIntervalDS(dpiData \*data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_DS.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: dpiIntervalYM \*dpiData_getIntervalYM(dpiData \*data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_YM.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: dpiLob \*dpiData_getLOB(dpiData \*data)

    Returns the value of the data when the native type is DPI_NATIVE_TYPE_LOB.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: dpiObject \*dpiData_getObject(dpiData \*data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_OBJECT.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: dpiStmt \*dpiData_getStmt(dpiData \*data)

    Returns the value of the data when the native type is DPI_NATIVE_TYPE_STMT.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: dpiTimestamp \*dpiData_getTimestamp(dpiData \*data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_TIMESTAMP.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: uint64_t dpiData_getUint64(dpiData \*data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_UINT64.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure from which
    to get the value.


.. function:: void dpiData_setBool(dpiData \*data, int value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_BOOLEAN.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **value** -- the value to set.


.. function:: void dpiData_setBytes(dpiData \*data, char \*ptr, \
        uint32_t length)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_BYTES.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromBytes()`.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **ptr** -- the byte string containing the data to set.

    **length** -- the length of the byte string.


.. function:: void dpiData_setDouble(dpiData \*data, double value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_DOUBLE.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **value** -- the value to set.


.. function:: void dpiData_setFloat(dpiData \*data, float value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_FLOAT.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **value** -- the value to set.


.. function:: void dpiData_setInt64(dpiData \*data, int64_t value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_INT64.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **value** -- the value to set.


.. function:: void dpiData_setIntervalDS(dpiData \*data, int32_t days, \
        int32_t hours, int32_t minutes, int32_t seconds, int32_t fsceconds)

    Sets the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_DS.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **days** -- the number of days to set in the value.

    **hours** -- the number of hours to set in the value.

    **minutes** -- the number of minutes to set in the value.

    **seconds** -- the number of seconds to set in the value.

    **fseconds** -- the number of fractional seconds to set in the value.


.. function:: void dpiData_setIntervalYM(dpiData \*data, int32_t years, \
        int32_t months)

    Sets the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_YM.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **years** -- the number of years to set in the value.

    **months** -- the number of months to set in the value.


.. function:: void dpiData_setLOB(dpiData \*data, dpiLob \*lob)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_LOB.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromLob()`.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **lob** -- a reference to the LOB to assign to the value.


.. function:: void dpiData_setObject(dpiData \*data, dpiObject \*obj)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_OBJECT.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromObject()`.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **obj** -- a reference to the object to assign to the value.


.. function:: void dpiData_setStmt(dpiData \*data, dpiStmt \*stmt)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_STMT.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromStmt()`.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **stmt** -- a reference to the statement to assign to the value.


.. function:: void dpiData_setTimestamp(dpiData \*data, int16_t year, \
        uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, \
        uint8_t second, uint32_t fsecond, int8_t tzHourOffset, \
        int8_t tzMinuteOffset)

    Sets the value of the data when the native type is
    DPI_NATIVE_TYPE_TIMESTAMP.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **year** -- the year to set in the value.

    **month** -- the month to set in the value.

    **day** -- the day to set in the value.

    **hour** -- the hour to set in the value.

    **minute** -- the minute to set in the value.

    **second** -- the second to set in the value.

    **fsecond** -- the fractional seconds to set in the value.

    **tzHourOffset** -- the time zone hour offset to set in the value.

    **tzMinuteOffset** -- the time zone minute offset to set in the value.


.. function:: void dpiData_setUint64(dpiData \*data, uint64_t value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_UINT64.

    **data** -- a pointer to the :ref:`dpiData<dpiData>` structure to set.

    **value** -- the value to set.

