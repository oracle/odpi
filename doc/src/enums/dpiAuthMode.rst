.. _dpiAuthMode:

ODPI-C Enumeration dpiAuthMode
------------------------------

This enumeration identifies the mode to use when authorizing connections to the
database.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_MODE_AUTH_DEFAULT        Default value used when creating connections.
DPI_MODE_AUTH_SYSDBA         Authenticates with SYSDBA access.
DPI_MODE_AUTH_SYSOPER        Authenticates with SYSOPER access.
DPI_MODE_AUTH_PRELIM         Used together with DPI_MODE_AUTH_SYSDBA or
                             DPI_MODE_AUTH_SYSOPER to authenticate for
                             certain administrative tasks (such as starting up
                             or shutting down the database).
DPI_MODE_AUTH_SYSASM         Authenticates with SYSASM access. The use of this
                             value requires Oracle Client 12.1 or higher.
DPI_MODE_AUTH_SYSBKP         Authenticates with SYSBACKUP access. The use of
                             this value requires Oracle Client 12.1 or higher.
DPI_MODE_AUTH_SYSDGD         Authenticates with SYSDG access. The use of this
                             value requires Oracle Client 12.1 or higher.
DPI_MODE_AUTH_SYSKMT         Authenticates with SYSKM access. The use of this
                             value requires Oracle Client 12.1 or higher.
DPI_MODE_AUTH_SYSRAC         Authenticates with SYSRAC access. The use of this
                             value requires Oracle Client 12.2 or higher.
===========================  ==================================================

