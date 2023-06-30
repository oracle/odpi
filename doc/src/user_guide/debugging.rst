.. _debugging:

****************
ODPI-C Debugging
****************

.. _level:

Level
=====

The environment variable DPI_DEBUG_LEVEL can be used to selectively turn on
the printing of various logging messages as defined in the following table. All
messages are printed to stderr. Each level is independent of all other levels
and the values of each level can be added to one another to choose which
messages will be printed.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 10 5 7 30
    :summary: The first column displays the level. The second column displays
     the hex value. The third column displays the decimal value. The fourth
     column displays the description of the debug level.

    * - Level
      - Hex Value
      - Decimal Value
      - Description
    * - DPI_DEBUG_LEVEL_UNREPORTED_ERRORS
      - 0x0001
      - 1
      - Prints messages when unreported errors take place (such as when an
        error takes place during the freeing of OCI handles and descriptors).
    * - DPI_DEBUG_LEVEL_REFS
      - 0x0002
      - 2
      - Prints messages when reference counts are added and released.
    * - DPI_DEBUG_LEVEL_FNS
      - 0x0004
      - 4
      - Prints messages when a public ODPI-C function is called.
    * - DPI_DEBUG_LEVEL_ERRORS
      - 0x0008
      - 8
      - Prints messages when an error is raised.
    * - DPI_DEBUG_LEVEL_SQL
      - 0x0010
      - 16
      - Prints the text of all SQL that is prepared. Bind values are not
        displayed.
    * - DPI_DEBUG_LEVEL_MEM
      - 0x0020
      - 32
      - Prints the location and size of all memory allocations and frees
        performed by the library.
    * - DPI_DEBUG_LEVEL_LOAD_LIB
      - 0x0040
      - 64
      - Prints the methods and locations searched for the Oracle Client library
        in addition to any errors that took place.

.. _prefix:

Prefix
======

The environment variable DPI_DEBUG_PREFIX can be used to specify the prefix
that is printed in front of each debugging message. If not set, the default
prefix is ``"ODPI [%i] %d %t: "``. The maximum length of the prefix string that
can be set is 64 characters. If an attempt is made to set a value longer than
that, it is ignored and the default prefix is used instead.

The following directives can be used within the prefix:

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 30
    :summary: The first column displays the directive. The second column displays
     the comments.

    * - Directive
      - Comments
    * - %i
      - Replaced by the identifier of the thread that is printing the message
    * - %d
      - Replaced by the date the message is printed, in the format YYYY/MM/DD
    * - %t
      - Replaced by the time the message is printed, in the format HH:MM:SS.SSS

All other characters in the prefix are copied unchanged to the output.

.. _memtracing:

Memory Tracing
==============

The Python script `mem_leak.py
<https://github.com/oracle/odpi/blob/main/util/tracing/mem_leak.py>`__ can be used to
search for memory leaks. The script's argument should be the name of a file
containing trace output when DPI_DEBUG_LEVEL is set to DPI_DEBUG_LEVEL_MEM::

    export DPI_DEBUG_LEVEL=32
    ./myprog >& mem.log
    python mem_leak.py mem.log
