.. _dpiEventType:

ODPI-C Public Enumeration dpiEventType
--------------------------------------

This enumeration identifies the types of events that can take place. The event
type is part of the messages that are sent to subscriptions.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_EVENT_NONE               Indicates that no event has taken place.
DPI_EVENT_STARTUP            Indicates that a database is being started up.
DPI_EVENT_SHUTDOWN           Indicates that a database is being shut down.
DPI_EVENT_SHUTDOWN_ANY       Indicates that a database is being shut down.
DPI_EVENT_DROP_DB            Indicates that a database is being dropped.
DPI_EVENT_DEREG              Indicates that a registered query is being
                             deregistered.
DPI_EVENT_OBJCHANGE          Indicates that an object change has taken place.
DPI_EVENT_QUERYCHANGE        Indicates that a query change has taken place.
===========================  ==================================================

