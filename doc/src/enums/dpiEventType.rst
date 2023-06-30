.. _dpiEventType:

ODPI-C Enumeration dpiEventType
-------------------------------

This enumeration identifies the types of events that can take place. The event
type is part of the messages that are sent to subscriptions.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiEventType
     enumeration. The second column displays the description of the
     dpiEventType enumeration value.

    * - Value
      - Description
    * - DPI_EVENT_AQ
      - Indicates that a queue has messages available to dequeue.
    * - DPI_EVENT_DEREG
      - Indicates that a subscription is no longer registered with the database
        and will no longer generate events.
    * - DPI_EVENT_NONE
      - Indicates that no event has taken place.
    * - DPI_EVENT_OBJCHANGE
      - Indicates that an object change has taken place.
    * - DPI_EVENT_QUERYCHANGE
      - Indicates that a query change has taken place.
    * - DPI_EVENT_SHUTDOWN
      - Indicates that a database is being shut down.
    * - DPI_EVENT_SHUTDOWN_ANY
      - Indicates that an instance of Oracle Real Application Clusters (RAC) is
        being shut down.
    * - DPI_EVENT_STARTUP
      - Indicates that a database is being started up.
