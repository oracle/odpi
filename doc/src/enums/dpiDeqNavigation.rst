.. _dpiDeqNavigation:

ODPI-C Enumeration dpiDeqNavigation
-----------------------------------

This enumeration identifies the method used for determining which message is to
be dequeued from a queue.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiDeqNavigation
     enumeration. The second column displays the description of the
     dpiDeqNavigation enumeration value.

    * - Value
      - Description
    * - DPI_DEQ_NAV_FIRST_MSG
      - Retrieves the first available message that matches the search criteria.
        This resets the position to the beginning of the queue.
    * - DPI_DEQ_NAV_NEXT_MSG
      - Retrieves the next available message that matches the search criteria.
        This is the default method.
    * - DPI_DEQ_NAV_NEXT_TRANSACTION
      - Skips the remainder of the current transaction group (if any) and
        retrieves the first message of the next transaction group. This option
        can only be used if message grouping is enabled for the queue.
