ODPI-C Release notes
====================

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
#)  Add structure :ref:`dpiVersionInfo` to pass version information, rather
    than separate parameters. This affects functions
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

