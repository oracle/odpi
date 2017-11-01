.. _debugging:

ODPI-C Debugging
----------------

Level
=====

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
      - Prints the text of all SQL that is prepared.  Bind values are not
        displayed.
    * - DPI_DEBUG_LEVEL_MEM
      - 0x0020
      - 32
      - Prints the location and size of all memory allocations and frees
        performed by the library.


Prefix
======

The environment variable DPI_DEBUG_PREFIX can be used to specify the prefix
that is printed in front of each debugging message. If not set, the default
prefix is ``"ODPI [%i] %d %t: "``. The maximum length of the prefix string that
can be set is 64 characters. If an attempt is made to set a value longer than
that, it is ignored and the default prefix is used instead.

The following directives can be used within the prefix:

.. list-table::
    :header-rows: 1

    * - Directive
      - Comments
    * - %i
      - Replaced by the identifier of the thread that is printing the message
    * - %d
      - Replaced by the date the message is printed, in the format YYYY/MM/DD
    * - %t
      - Replaced by the time the message is printed, in the format HH:MM:SS.SSS

All other characters in the prefix are copied unchanged to the output.

