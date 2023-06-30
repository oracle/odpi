.. _dpiMessageState:

ODPI-C Enumeration dpiMessageState
----------------------------------

This enumeration identifies the possible states for messages in a queue.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :width: 100%
    :summary: The first column displays the value of the dpiMessageState
     enumeration. The second column displays the description of the
     dpiMessageState enumeration value.

    * - Value
      - Description
    * - DPI_MSG_STATE_EXPIRED
      - The message has been moved to the exception queue.
    * - DPI_MSG_STATE_PROCESSED
      - The message has already been processed and is retained.
    * - DPI_MSG_STATE_READY
      - The message is ready to be processed.
    * - DPI_MSG_STATE_WAITING
      - The message is waiting for the delay time to expire.
