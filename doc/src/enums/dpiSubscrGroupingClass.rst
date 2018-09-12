.. _dpiSubscrGroupingClass:

ODPI-C Enumeration dpiSubscrGroupingClass
-----------------------------------------

This enumeration identifies the grouping class. Instead of individual events
being delivered to the callback, events are grouped before being sent to the
callback. This enumeration is used in the
:ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure.

===============================  ==============================================
Value                            Description
===============================  ==============================================
DPI_SUBSCR_GROUPING_CLASS_TIME   Events are grouped by the period of time in
                                 which they are received.
===============================  ==============================================

