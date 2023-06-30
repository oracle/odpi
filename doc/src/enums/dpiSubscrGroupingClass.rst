.. _dpiSubscrGroupingClass:

ODPI-C Enumeration dpiSubscrGroupingClass
-----------------------------------------

This enumeration identifies the grouping class. Instead of individual events
being delivered to the callback, events are grouped before being sent to the
callback. This enumeration is used in the
:ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiSubscrGroupingClass
     enumeration. The second column displays the description of the
     dpiSubscrGroupingClass enumeration value.

    * - Value
      - Description
    * - DPI_SUBSCR_GROUPING_CLASS_TIME
      - Events are grouped by the period of time in which they are received.
