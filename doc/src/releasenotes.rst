ODPI-C Release notes
====================

Version 2.0.3 (November 6, 2017)
--------------------------------

#)  Prevent use of unitialized data in certain cases (`cx_Oracle issue 77
    <https://github.com/oracle/python-cx_Oracle/issues/77>`__).
#)  Attempting to ping a database earlier than 10g results in error "ORA-1010:
    invalid OCI operation", but that implies a response from the database and
    therefore a successful ping, so treat it that way!
#)  Some values represented as double do not convert perfectly to float; use
    FLT_EPSILON to check the difference between the two after conversion,
    rather than expect the values to convert perfectly.
#)  Prevent use of NaN with Oracle numbers since it produces corrupt data
    (`cx_Oracle issue 91
    <https://github.com/oracle/python-cx_Oracle/issues/91>`__).
#)  Verify that Oracle objects bound to cursors, fetched from cursors, set in
    object attributes or appended to collection objects are of the correct
    type.
#)  Correct handling of NVARCHAR2 when used as attributes of Oracle objects or
    elements of collections
    (`issue 45 <https://github.com/oracle/odpi/issues/45>`__).


Version 2.0.2 (August 30, 2017)
-------------------------------

#)  Don't prevent connection from being explicitly closed when a fatal error
    has taken place (`cx_Oracle issue 67
    <https://github.com/oracle/python-cx_Oracle/issues/67>`__).
#)  Correct handling of objects when dynamic binding is performed.
#)  Process deregistration events without an error.
#)  Eliminate memory leak when calling dpiObjectType_createObject().


Version 2.0.1 (August 18, 2017)
-------------------------------

#)  Ensure that any allocated statement increments the open child count
    (`issue 27 <https://github.com/oracle/odpi/issues/27>`__).
#)  Correct parameter subscrId to :func:`dpiConn_newSubscription()` as
    `noted <https://github.com/oracle/odpi/issues/28>`__. There is no OCI
    documentation for this attribute and should never have been exposed. The
    parameter is therefore deprecated and will be removed in version 2.1. The
    value NULL can now also be passed to this parameter.
#)  Add script for running valgrind and correct issues found in test suite
    reported by valgrind
    (`issue 29 <https://github.com/oracle/odpi/issues/29>`__).
#)  Use posix_spawn() instead of system() in the test suite, as requested
    (`issue 30 <https://github.com/oracle/odpi/issues/30>`__).
#)  Add support for DML returning statements that require dynamically allocated
    variable data (such as CLOBs being returned as strings).


Version 2.0.0 (August 14, 2017)
-------------------------------

#)  Added new structure :ref:`dpiDataTypeInfo<dpiDataTypeInfo>` and modified
    structures :ref:`dpiQueryInfo<dpiQueryInfo>`,
    :ref:`dpiObjectAttrInfo<dpiObjectAttrInfo>` and
    :ref:`dpiObjectTypeInfo<dpiObjectTypeInfo>` in order to add support for
    fractional seconds precision on timestamps and interval day to second
    values (`issue 22 <https://github.com/oracle/odpi/issues/22>`__) and to
    support additional metadata on object types and attributes
    (`issue 23 <https://github.com/oracle/odpi/issues/23>`__).
#)  Prevent closing the connection when there are any open statements or LOBs
    and add new error "DPI-1054: connection cannot be closed when open
    statements or LOBs exist" when this situation is detected; this is needed
    to prevent crashes under certain conditions when statements or LOBs are
    being acted upon while at the same time (in another thread) a connection is
    being closed; it also prevents leaks of statements and LOBs when a
    connection is returned to a session pool.
#)  Added support for binding by value for rowids in
    :func:`dpiStmt_bindValueByPos()` and :func:`dpiStmt_bindValueByName()`.
#)  On platforms other than Windows, if the regular method for loading the
    Oracle Client libraries fails, try using $ORACLE_HOME/lib/libclntsh.so
    (`issue 20 <https://github.com/oracle/odpi/issues/20>`__).
#)  Use the environment variable DPI_DEBUG_LEVEL at runtime, not compile time,
    to add in :ref:`debugging <debugging>`.
#)  Added support for DPI_DEBUG_LEVEL_ERRORS (reports errors and has the value
    8) and DPI_DEBUG_LEVEL_SQL (reports prepared SQL statement text and has the
    value 16) in order to further improve the ability to debug issues.
#)  Ensure that any prefetch activities are performed prior to performing the
    fetch in :func:`dpiStmt_scroll()`.
#)  Provide means of disabling thread cleanup for situations where threads are
    created at startup and never terminated (such as takes place with Node.js).
#)  Add script sql/TestEnv.sql and read environment variables during test
    execution in order to simplify the running of tests.
#)  Add script sql/SampleEnv.sql and read environment variables during sample
    execution in order to simplify the running of samples.
#)  Adjust Makefile for tests and samples
    (`issue 24 <https://github.com/oracle/odpi/issues/24>`__).
#)  Added additional test cases.
#)  Documentation improvements.


Version 2.0.0-rc.2 (July 20, 2017)
----------------------------------

#)  Improved error messages when initial OCI environment cannot be created.
#)  On Windows, convert system message to Unicode first, then to UTF-8;
    otherwise, the error message returned could be in a mix of encodings.
#)  Added support for converting from text to number in object attributes and
    collection element values.
#)  Added checks on all pointers to ensure they are not NULL.
#)  Added checks on all pointer/length combinations to ensure that they are not
    NULL and non-zero at the same time.
#)  Ensure that the thread specific errors are freed properly.
#)  Corrected handling of connections using DRCP.
#)  Corrected issue where error getting a pooled connection was masked by
    error "DPI-1002: invalid OCI handle".
#)  PL/SQL boolean values are not supported until Oracle Client 12.1 so raise
    an unsupported error if an attempt is made to use them in Oracle Client
    11.2.
#)  Allow the parameter numQueryColumns to be null in :func:`dpiStmt_execute()`
    as a convenience to those who do not require that information.
#)  Added url fragment and whether a 32-bit or 64-bit library is expected to
    error "DPI-1047: Oracle Client library cannot be loaded" in order to
    improve the help provided.
#)  Added prefix "ODPI: " to start of all debug messages to aid in
    differentiating between them and other messages in log files.
#)  Added additional test cases.
#)  Documentation improvements.


Version 2.0.0-rc.1 (June 16, 2017)
----------------------------------

#)  OCI requires that both :member:`~dpiCommonCreateParams.encoding`
    and :member:`~dpiCommonCreateParams.nencoding` have values or that both
    encoding and encoding do not have values. The missing value is set to its
    default value if one of the values is set and the other is not
    (`issue 36 <https://github.com/oracle/python-cx_Oracle/issues/36>`__).
#)  Add optimization when client and server character sets are identical; in
    that case the size in bytes reported by the server is sufficient to hold
    the data that will be transferred from the server and there is no need to
    expand the buffer for character strings.
#)  Corrected handling of BFILE LOBs.
#)  Eliminated errors and warnings when using AIX compiler.
#)  Documentation improvements.


Version 2.0.0-beta.4 (May 24, 2017)
-----------------------------------

#)  Added support for getting/setting attributes of objects or element values
    in collections that contain LOBs, BINARY_FLOAT values, BINARY_DOUBLE values
    and NCHAR and NVARCHAR2 values. The error message for any types that are
    not supported has been improved as well.
#)  Enabled temporary LOB caching in order to avoid disk I/O as
    `suggested <https://github.com/oracle/odpi/issues/10>`__.
#)  Changed default native type to DPI_ORACLE_TYPE_INT64 if the column metadata
    indicates that the values are able to fit inside a 64-bit integer.
#)  Added function :func:`dpiStmt_defineValue()`, which gives the application
    the opportunity to specify the data type to use for fetching without having
    to create a variable.
#)  Added constant DPI_DEBUG_LEVEL as a set of bit flags which result in
    messages being printed to stderr. The following levels are defined:

    - 0x0001 - reports errors during free operations
    - 0x0002 - reports on reference count changes
    - 0x0004 - reports on public function calls

#)  An empty string is just as acceptable as NULL when enabling external
    authentication in :func:`dpiPool_create()`.
#)  Avoid changing the OCI actual length values for fixed length types in order
    to prevent error "ORA-01458: invalid length inside variable character
    string".
#)  Ensured that the length set in the dpiBytes structure by the caller is
    passed through to the actual length buffer used by OCI.
#)  Added missing documentation for function :func:`dpiVar_setFromBytes()`.
#)  Handle edge case when an odd number of zeroes trail the decimal point in a
    value that is effectively zero (`cx_Oracle issue 22
    <https://github.com/oracle/python-cx_Oracle/issues/22>`__).
#)  Eliminated resource leak when a standalone connection or pool is freed.
#)  Prevent attempts from binding the cursor being executed to itself.
#)  Corrected determination of unique bind variable names. The function
    :func:`dpiStmt_getBindCount()` returns a count of unique bind variable
    names for PL/SQL statements only. For SQL statements, this count is the
    total number of bind variables, including duplicates. The function
    :func:`dpiStmt_getBindNames()` has been adjusted to return the actual
    number of unique bind variable names (parameter numBindNames is now a
    pointer instead of a scalar value).
#)  Added additional test cases.
#)  Added check for Cygwin, as `suggested
    <https://github.com/oracle/odpi/issues/11>`__.


Version 2.0.0-beta.3 (April 18, 2017)
-------------------------------------

#)  Add initial set of `functional test cases
    <https://github.com/oracle/odpi/tree/master/test>`__.
#)  Add support for smallint and float data types in Oracle objects, as
    `requested <https://github.com/oracle/python-cx_Oracle/issues/4>`__.
#)  Ensure that the actual array size is set to the number of rows returned in
    a DML returning statement.
#)  Remove unneeded function dpiVar_resize().
#)  Improve error message when specifying an invalid array position in a
    variable.
#)  Add structure :ref:`dpiVersionInfo<dpiVersionInfo>` to pass version
    information, rather than separate parameters. This affects functions
    :func:`dpiContext_getClientVersion()` and
    :func:`dpiConn_getServerVersion()`.
#)  Rename functions that use an index to identify elements in a collection to
    include "ByIndex" in the name. This is clearer and also allows for
    functions that may be added in the future that will identify elements by
    other means. This affects functions
    :func:`dpiObject_deleteElementByIndex()`,
    :func:`dpiObject_getElementExistsByIndex()`,
    :func:`dpiObject_getElementValueByIndex()`, and
    :func:`dpiObject_setElementValueByIndex()`.
#)  The OCI function OCITypeByFullName() is supported on a 12.1 Oracle Client
    but will give the error "OCI-22351: This API is not supported by the ORACLE
    server" when used against an 11.2 Oracle Database. The function
    :func:`dpiConn_getObjectType()` now checks the server version and calls the
    correct routine as needed.
#)  Add parameter "exists" to functions :func:`dpiObject_getFirstIndex()` and
    :func:`dpiObject_getLastIndex()` which allow a calling program to avoid
    error "OCI-22166: collection is empty".


Version 2.0.0-beta.2 (March 28, 2017)
-------------------------------------

#)  Use dynamic loading at runtime to load the OCI library and eliminate the
    need for the OCI header files to be present when building ODPI-C.
#)  Improve sample Makefile as requested in `issue 1
    <https://github.com/oracle/odpi/issues/1>`__.
#)  Correct support for handling unsigned integers that are larger than the
    maximum size that can be represented by a signed integer. This corrects
    `issue 3 <https://github.com/oracle/odpi/issues/3>`__.
#)  Remove type DPI_ORACLE_TYPE_LONG_NVARCHAR which is not needed as noted in
    `issue 5 <https://github.com/oracle/odpi/issues/5>`__.
#)  Increase size of string which can be generated from an OCI number. This
    corrects `issue 6 <https://github.com/oracle/odpi/issues/6>`__.
#)  Ensure that zeroing the check integer on ODPI-C handles is not optimised
    away by the compiler.
#)  Silence compiler warnings from the Microsoft C++ compiler.
#)  Restore support for simple reference count tracing by the use of
    DPI_TRACE_REFS.
#)  Add additional error (ORA-56600: an illegal OCI function call was issued)
    to the list of errors that cause the session to be dropped from the session
    pool.
#)  Changed LOB sample to include code to populate both CLOBs and BLOBs in
    addition to fetching them.


Version 2.0.0-beta.1 (January 30, 2017)
---------------------------------------

#)  Initial release supporting the following features\:

    - 11.2, 12.1 and 12.2 Oracle Client support
    - 9.2 and higher Oracle Database support (depending on Oracle Client
      version)
    - SQL and PL/SQL execution
    - Character datatypes (CHAR, VARCHAR2, NCHAR, NVARCHAR2, CLOB, NCLOB, LONG)
    - Numeric datatypes (NUMBER, BINARY_FLOAT, BINARY_DOUBLE)
    - Dates, Timestamps, Intervals
    - Binary types (BLOB, BFILE, RAW, LONG RAW)
    - PL/SQL datatypes (PLS_INTEGER, BOOLEAN, Collections, Records)
    - JSON
    - User Defined Types
    - REF CURSOR, Nested cursors, Implicit Result Sets
    - Array fetch
    - Array bind/execute
    - Session pools (homogeneous and non-homogeneous with proxy authentication)
    - Standalone connections
    - Database Resident Connection Pooling (DRCP)
    - External authentication
    - Statement caching (tagging)
    - End-to-end tracing, mid-tier authentication and auditing (action, module,
      client identifier, client info, database operation)
    - Edition Based Redefinition
    - Scrollable cursors
    - DML RETURNING
    - Privileged connection support (SYSDBA, SYSOPER, SYSASM, PRELIM_AUTH)
    - Database Startup/Shutdown
    - Session Tagging
    - Proxy authentication
    - Batch Errors
    - Array DML Row Counts
    - Query Result Caching
    - Application Continuity (with some limitations)
    - Query Metadata
    - Password Change
    - Two Phase Commit
    - OCI Client Version and Server Version
    - Connection Validation (when acquired from session pool or DRCP)
    - Continuous Query Notification
    - Advanced Queuing
    - Easily extensible (via direct OCI calls)

