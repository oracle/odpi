.. _dpiIntervalDS:

ODPI-C Structure dpiIntervalDS
------------------------------

This structure is used for passing interval (days to seconds) data to and from
the database in the structure :ref:`dpiData<dpiData>`.

.. member:: int32_t dpiIntervalDS.days

    Specifies the number of days in the interval.

.. member:: int32_t dpiIntervalDS.hours

    Specifies the number of hours in the interval.

.. member:: int32_t dpiIntervalDS.minutes

    Specifies the number of minutes in the interval.

.. member:: int32_t dpiIntervalDS.seconds

    Specifies the number of seconds in the interval.

.. member:: int32_t dpiIntervalDS.fseconds

    Specifies the number of fractional seconds in the interval (in
    nanoseconds).

