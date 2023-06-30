.. _dpiPurity:

ODPI-C Enumeration dpiPurity
----------------------------

This enumeration identifies the purity of the sessions that are acquired when
using connection classes during connection creation.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiPurity
     enumeration. The second column displays the description of the
     dpiPurity enumeration value.

    * - Value
      - Description
    * - DPI_PURITY_DEFAULT
      - Default value used when creating connections.
    * - DPI_PURITY_NEW
      - A connection is required that has not been tainted with any prior
        session state.
    * - DPI_PURITY_SELF
      - A connection is permitted to have prior session state.
