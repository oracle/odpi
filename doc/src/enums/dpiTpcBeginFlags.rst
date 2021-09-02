.. _dpiTpcBeginFlags:

ODPI-C Enumeration dpiTpcBeginFlags
-----------------------------------

This enumeration identifies the flags that can be used when calling
:func:`dpiConn_tpcBegin()`.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_TPC_BEGIN_JOIN           Joins an existing global transaction.
DPI_TPC_BEGIN_NEW            Creates a new global transaction.
DPI_TPC_BEGIN_PROMOTE        Promotes a local transaction to a global
                             transaction.
DPI_TPC_BEGIN_RESUME         Resumes an existing global transaction.
===========================  ==================================================
