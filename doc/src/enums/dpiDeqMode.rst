.. _dpiDeqMode:

ODPI-C Enumeration dpiDeqMode
-----------------------------

This enumeration identifies the modes that are possible when dequeuing messages
from a queue.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiDeqMode
     enumeration. The second column displays the description of the
     dpiDeqMode enumeration value.

    * - Value
      - Description
    * - DPI_MODE_DEQ_BROWSE
      - Read the message without acquiring a lock on the message (equivalent
        to a SELECT statement).
    * - DPI_MODE_DEQ_LOCKED
      - Read the message and obtain a write lock on the message (equivalent
        to a SELECT FOR UPDATE statement).
    * - DPI_MODE_DEQ_REMOVE
      - Read the message and update or delete it. This is the default mode.
        Note that the message may be retained in the queue table based on
        retention properties.
    * - DPI_MODE_DEQ_REMOVE_NO_DATA
      - Confirms receipt of the message but does not deliver the actual message
        content.
