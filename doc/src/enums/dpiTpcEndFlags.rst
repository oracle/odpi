.. _dpiTpcEndFlags:

ODPI-C Enumeration dpiTpcEndFlags
---------------------------------

This enumeration identifies the flags that can be used when calling
:func:`dpiConn_tpcEnd()`.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :width: 100%
    :summary: The first column displays the value of the dpiTpcEndFlags
     enumeration. The second column displays the description of the
     dpiTpcEndFlags enumeration value.

    * - Value
      - Description
    * - DPI_TPC_END_NORMAL
      - Normal ending of the transaction.
    * - DPI_TPC_END_SUSPEND
      - Transaction is suspended and may be resumed at some later point.
