.. _dpiTpcEndFlags:

ODPI-C Enumeration dpiTpcEndFlags
---------------------------------

This enumeration identifies the flags that can be used when calling
:func:`dpiConn_tpcEnd()`.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_TPC_END_NORMAL           Normal ending of the transaction.
DPI_TPC_END_SUSPEND          Transaction is suspended and may be resumed at
                             some later point.
===========================  ==================================================
