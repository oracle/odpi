.. _dpiMessageDeliveryMode:

ODPI-C Enumeration dpiMessageDeliveryMode
-----------------------------------------

This enumeration identifies the delivery mode used for filtering messages
when dequeuing messages from a queue.

===================================  ==========================================
Value                                Description
===================================  ==========================================
DPI_MODE_MSG_PERSISTENT              Dequeue only persistent messages from the
                                     queue. This is the default mode.
DPI_MODE_MSG_BUFFERED                Dequeue only buffered messages from the
                                     queue.
DPI_MODE_MSG_PERSISTENT_OR_BUFFERED  Dequeue both persistent and buffered
                                     messages from the queue.
===================================  ==========================================

