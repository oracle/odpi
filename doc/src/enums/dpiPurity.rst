.. _dpiPurity:

ODPI-C Enumeration dpiPurity
----------------------------

This enumeration identifies the purity of the sessions that are acquired when
using connection classes during connection creation.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_PURITY_DEFAULT           Default value used when creating connections.
DPI_PURITY_NEW               A connection is required that has not been tainted
                             with any prior session state.
DPI_PURITY_SELF              A connection is permitted to have prior session
                             state.
===========================  ==================================================

