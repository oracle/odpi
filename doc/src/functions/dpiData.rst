.. _dpiDataFunctions:

ODPI-C Data Functions
---------------------

All of these functions are used for getting and setting the various members of
the :ref:`dpiData<dpiData>` structure. The members of the structure can be
manipulated directly but some languages (such as Go) do not have the ability to
manipulate structures containing unions or the ability to process macros. For
this reason, none of these functions perform any error checking. They are
assumed to be replacements for direct manipulation of the various members of
the structure.

.. function:: int dpiData_getBool(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_BOOLEAN.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiBytes* dpiData_getBytes(dpiData* data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_BYTES.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: double dpiData_getDouble(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_DOUBLE.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: float dpiData_getFloat(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_FLOAT.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: int64_t dpiData_getInt64(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_INT64.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiIntervalDS* dpiData_getIntervalDS(dpiData* data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_DS.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiIntervalYM* dpiData_getIntervalYM(dpiData* data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_YM.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: int dpiData_getIsNull(dpiData* data)

    Returns whether the data refers to a null value (1) or not (0).

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the null indicator.

.. function:: dpiLob* dpiData_getLOB(dpiData* data)

    Returns the value of the data when the native type is DPI_NATIVE_TYPE_LOB.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiJson* dpiData_getJson(dpiData* data)

    Returns the value of the data when the native type is DPI_NATIVE_TYPE_JSON.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiJsonArray* dpiData_getJsonArray(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_JSON_ARRAY.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiJsonObject* dpiData_getJsonObject(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_JSON_OBJECT.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiObject* dpiData_getObject(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_OBJECT.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiStmt* dpiData_getStmt(dpiData* data)

    Returns the value of the data when the native type is DPI_NATIVE_TYPE_STMT.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiTimestamp* dpiData_getTimestamp(dpiData* data)

    Returns a pointer to the value of the data when the native type is
    DPI_NATIVE_TYPE_TIMESTAMP.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: uint64_t dpiData_getUint64(dpiData* data)

    Returns the value of the data when the native type is
    DPI_NATIVE_TYPE_UINT64.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: dpiVector* dpiData_getVector(dpiData* data)

    Returns the value of the data when the native type is DPI_NATIVE_TYPE_VECTOR.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure from which to
            get the value.

.. function:: void dpiData_setBool(dpiData* data, int value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_BOOLEAN.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``value``
          - IN
          - The value to set.

.. function:: void dpiData_setBytes(dpiData* data, char* ptr, uint32_t length)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_BYTES.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromBytes()`.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``ptr``
          - IN
          - The byte string containing the data to set.
        * - ``length``
          - IN
          - The length of the byte string.

.. function:: void dpiData_setDouble(dpiData* data, double value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_DOUBLE.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``value``
          - IN
          - The value to set.

.. function:: void dpiData_setFloat(dpiData* data, float value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_FLOAT.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``value``
          - IN
          - The value to set.

.. function:: void dpiData_setInt64(dpiData* data, int64_t value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_INT64.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``value``
          - IN
          - The value to set.

.. function:: void dpiData_setIntervalDS(dpiData* data, int32_t days, \
        int32_t hours, int32_t minutes, int32_t seconds, int32_t fseconds)

    Sets the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_DS.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``days``
          - IN
          - The number of days to set in the value.
        * - ``hours``
          - IN
          - The number of hours to set in the value.
        * - ``minutes``
          - IN
          - The number of minutes to set in the value.
        * - ``seconds``
          - IN
          - The number of seconds to set in the value.
        * - ``fseconds``
          - IN
          - The number of fractional seconds to set in the value.

.. function:: void dpiData_setIntervalYM(dpiData* data, int32_t years, \
        int32_t months)

    Sets the value of the data when the native type is
    DPI_NATIVE_TYPE_INTERVAL_YM.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``years``
          - IN
          - The number of years to set in the value.
        * - ``months``
          - IN
          - The number of months to set in the value.

.. function:: void dpiData_setLOB(dpiData* data, dpiLob* lob)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_LOB.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromLob()`.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``lob``
          - IN
          - A reference to the LOB to assign to the value.

.. function:: void dpiData_setNull(dpiData* data)

    Sets the value of the data to be the null value.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.

.. function:: void dpiData_setObject(dpiData* data, dpiObject* obj)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_OBJECT.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromObject()`.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``obj``
          - IN
          - A reference to the object to assign to the value.

.. function:: void dpiData_setStmt(dpiData* data, dpiStmt* stmt)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_STMT.
    Do *not* use this function when setting data for variables. Instead, use
    the function :func:`dpiVar_setFromStmt()`.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``stmt``
          - IN
          - A reference to the statement to assign to the value.

.. function:: void dpiData_setTimestamp(dpiData* data, int16_t year, \
        uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, \
        uint8_t second, uint32_t fsecond, int8_t tzHourOffset, \
        int8_t tzMinuteOffset)

    Sets the value of the data when the native type is
    DPI_NATIVE_TYPE_TIMESTAMP.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``year``
          - IN
          - The year to set in the value.
        * - ``month``
          - IN
          - The month to set in the value.
        * - ``day``
          - IN
          - The day to set in the value.
        * - ``hour``
          - IN
          - The hour to set in the value.
        * - ``minute``
          - IN
          - The minute to set in the value.
        * - ``second``
          - IN
          - The second to set in the value.
        * - ``fsecond``
          - IN
          - The fractional seconds to set in the value.
        * - ``tzHourOffset``
          - IN
          - The time zone hour offset to set in the value.
        * - ``tzMinuteOffset``
          - IN
          - The time zone minute offset to set in the value.

.. function:: void dpiData_setUint64(dpiData* data, uint64_t value)

    Sets the value of the data when the native type is DPI_NATIVE_TYPE_UINT64.

    .. parameters-table::

        * - ``data``
          - IN
          - A pointer to the :ref:`dpiData<dpiData>` structure to set.
        * - ``value``
          - IN
          - The value to set.
