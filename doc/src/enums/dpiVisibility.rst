.. _dpiVisibility:

ODPI-C Enumeration dpiVisibility
--------------------------------

This enumeration identifies the visibility of messages in advanced queuing.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_VISIBILITY_IMMEDIATE     The message is not part of the current transaction
                             but constitutes a transaction of its own.
DPI_VISIBILITY_ON_COMMIT     The message is part of the current transaction.
                             This is the default value.
===========================  ==================================================

