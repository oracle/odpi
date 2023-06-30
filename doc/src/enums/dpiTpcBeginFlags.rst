.. _dpiTpcBeginFlags:

ODPI-C Enumeration dpiTpcBeginFlags
-----------------------------------

This enumeration identifies the flags that can be used when calling
:func:`dpiConn_tpcBegin()`.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :width: 100%
    :summary: The first column displays the value of the dpiTpcBeginFlags
     enumeration. The second column displays the description of the
     dpiTpcBeginFlags enumeration value.

    * - Value
      - Description
    * - DPI_TPC_BEGIN_JOIN
      - Joins an existing global transaction.
    * - DPI_TPC_BEGIN_NEW
      - Creates a new global transaction.
    * - DPI_TPC_BEGIN_PROMOTE
      - Promotes a local transaction to a global transaction.
    * - DPI_TPC_BEGIN_RESUME
      - Resumes an existing global transaction.
