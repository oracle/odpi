.. _dpiPoolCloseMode:

ODPI-C Enumeration dpiPoolCloseMode
-----------------------------------

This enumeration identifies the mode to use when closing pools.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_MODE_POOL_CLOSE_DEFAULT  Default value used when closing pools. If there
                             are any active sessions in the pool an error will
                             be raised.
DPI_MODE_POOL_CLOSE_FORCE    Causes all of the active connections in the pool
                             to be closed before closing the pool itself.
===========================  ==================================================

