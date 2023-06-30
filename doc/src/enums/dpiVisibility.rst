.. _dpiVisibility:

ODPI-C Enumeration dpiVisibility
--------------------------------

This enumeration identifies the visibility of messages in advanced queuing.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiVisibility
     enumeration. The second column displays the description of the
     dpiVisibility enumeration value.

    * - Value
      - Description
    * - DPI_VISIBILITY_IMMEDIATE
      - The message is not part of the current transaction but constitutes
        a transaction of its own.
    * - DPI_VISIBILITY_ON_COMMIT
      - The message is part of the current transaction. This is the default
        value.
