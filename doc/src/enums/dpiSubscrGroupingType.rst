.. _dpiSubscrGroupingType:

ODPI-C Enumeration dpiSubscrGroupingType
----------------------------------------

This enumeration identifies the grouping type. It is used in the
:ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiSubscrGroupingType
     enumeration. The second column displays the description of the
     dpiSubscrGroupingType enumeration value.

    * - Value
      - Description
    * - DPI_SUBSCR_GROUPING_TYPE_LAST
      - The last event in the group is sent.
    * - DPI_SUBSCR_GROUPING_TYPE_SUMMARY
      - A summary of all events in the group is sent. This is also the default
        value.
