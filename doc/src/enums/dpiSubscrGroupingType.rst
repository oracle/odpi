.. _dpiSubscrGroupingType:

ODPI-C Enumeration dpiSubscrGroupingType
----------------------------------------

This enumeration identifies the grouping type. It is used in the
:ref:`dpiSubscrCreateParams<dpiSubscrCreateParams>` structure.

================================  =============================================
Value                             Description
================================  =============================================
DPI_SUBSCR_GROUPING_TYPE_SUMMARY  A summary of all events in the group is sent.
                                  This is also the default value.
DPI_SUBSCR_GROUPING_TYPE_LAST     The last event in the group is sent.
================================  =============================================

