.. _dpiJsonOptions:

ODPI-C Enumeration dpiJsonOptions
---------------------------------

This enumeration identifies the options that can be used when calling
:func:`dpiJson_getValue()`.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiJsonOptions
     enumeration. The second column displays the description of the
     dpiJsonOptions enumeration value.

    * - Value
      - Description
    * - DPI_JSON_OPT_DATE_AS_DOUBLE
      - Convert values that are stored as Oracle dates and timestamps in the
        JSON value into double values (number of milliseconds since
        January 1, 1970).
    * - DPI_JSON_OPT_DEFAULT
      - Default value.
    * - DPI_JSON_OPT_NUMBER_AS_STRING
      - Convert values that are stored as Oracle numbers in the JSON value into
        strings in order to retain precision.
