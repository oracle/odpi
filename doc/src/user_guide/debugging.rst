.. _debugging:

ODPI-C Debugging
----------------

The environment variable DPI_DEBUG_LEVEL can be used to selectively turn on
the printing of various logging messages as defined in the following table. All
messages are printed to stderr. Each level is independent of all other levels
and the values of each level can be added to one another to choose which
messages will be printed.

.. list-table::
    :header-rows: 1

    * - Level
      - Hex Value
      - Decimal Value
      - Description
    * - DPI_DEBUG_LEVEL_FREES
      - 0x0001
      - 1
      - Prints messages when the freeing of OCI handles and descriptors fails
    * - DPI_DEBUG_LEVEL_REFS
      - 0x0002
      - 2
      - Prints messages when reference counts are added and released
    * - DPI_DEBUG_LEVEL_FNS
      - 0x0004
      - 4
      - Prints messages when a public ODPI-C function is called
    * - DPI_DEBUG_LEVEL_ERRORS
      - 0x0008
      - 8
      - Prints messages when an error is raised
    * - DPI_DEBUG_LEVEL_SQL
      - 0x0010
      - 16
      - Prints the text of all SQL that is prepared

