.. _dpiCreateMode:

ODPI-C Enumeration dpiCreateMode
--------------------------------

This enumeration identifies the mode to use when creating connections to the
database. Note that the OCI objects mode is always enabled.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiCreateMode
     enumeration. The second column displays the description of the
     dpiCreateMode enumeration value.

    * - Value
      - Description
    * - DPI_MODE_CREATE_DEFAULT
      - Default value used when creating connections.
    * - DPI_MODE_CREATE_EVENTS
      - Enables events mode which is required for the use of advanced queuing
        (AQ) and continuous query notification (CQN).
    * - DPI_MODE_CREATE_THREADED
      - Enables threaded mode. Internal OCI structures not exposed to the user
        are protected from concurrent access by multiple threads. Error
        information is also managed in thread local storage.
