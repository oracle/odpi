.. _dpiTimestamp:

ODPI-C Structure dpiTimestamp
-----------------------------

This structure is used for passing timestamp data to and from the database in
the structure :ref:`dpiData<dpiData>`.

.. member:: int16_t dpiTimestamp.year

    Specifies the year for the timestamp.

.. member:: uint8_t dpiTimestamp.month

    Specifies the month for the timestamp. This should be between 1 and 12.

.. member:: uint8_t dpiTimestamp.day

    Specifies the day for the timestamp. This should be between 1 and 31.

.. member:: uint8_t dpiTimestamp.hour

    Specifies the hour for the timestamp. This should be between 0 and 23.

.. member:: uint8_t dpiTimestamp.minute

    Specifies the minute for the timestamp. This should be between 0 and 59.

.. member:: uint8_t dpiTimestamp.second

    Specifies the second for the timestamp. This should be between 0 and 59.

.. member:: uint32_t dpiTimestamp.fsecond

    Specifies the fractional seconds for the timestamp, in nanoseconds.

.. member:: int8_t dpiTimestamp.tzHourOffset

    Specifies the hours offset from UTC. This value is only used for timestamp
    with time zone and timestamp with local time zone columns.

.. member:: int8_t dpiTimestamp.tzMinuteOffset

    Specifies the minutes offset from UTC. This value is only used for
    timestamp with time zone and timestamp with local time zone columns. Note
    that this value will be negative in the western hemisphere. For example,
    when the timezone is -03:30, tzHourOffset will be -3 and tzMinuteOffset
    will be -30.

