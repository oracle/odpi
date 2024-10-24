.. _dpiServerType:

ODPI-C Enumeration dpiServerType
--------------------------------

This enumeration identifies the type of server process associated with a
connection. It is only available with Oracle Client libraries 23.4 or higher.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiServerType
     enumeration. The second column displays the description of the
     dpiServerType enumeration value.

    * - Value
      - Description
    * - DPI_SERVER_TYPE_DEDICATED
      - A dedicated server process is being used with the connection.
    * - DPI_SERVER_TYPE_POOLED
      - A pooled server process (DRCP) is being used with the connection.
    * - DPI_SERVER_TYPE_SHARED
      - A shared server process is being used with the connection.
    * - DPI_SERVER_TYPE_UNKNOWN
      - The type of server process is unknown.
