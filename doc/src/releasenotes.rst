Release notes
=============

Version 2.0.0-beta.3 (TBD)
--------------------------

1)  Add initial set of `functional test cases
    <https://github.com/oracle/odpi/tree/master/test>`__.
2)  Add support for smallint and float data types in Oracle objects, as
    `requested <https://github.com/oracle/python-cx_Oracle/issues/4>`__.
3)  Ensure that the actual array size is set to the number of rows returned in
    a DML returning statement.
4)  Remove unneeded function dpiVar_resize().
5)  Improve error message when specifying an invalid array position in a
    variable.
6)  Add structure :ref:`dpiVersionInfo` to pass version information, rather
    than separate parameters. This affects functions
    :func:`dpiContext_getClientVersion()` and
    :func:`dpiConn_getServerVersion()`.
7)  Rename functions that use an index to identify elements in a collection to
    include "ByIndex" in the name. This is clearer and also allows for
    functions that may be added in the future that will identify elements by
    other means. This affects functions
    :func:`dpiObject_deleteElementByIndex()`,
    :func:`dpiObject_getElementExistsByIndex()`,
    :func:`dpiObject_getElementValueByIndex()`, and
    :func:`dpiObject_setElementValueByIndex()`.
8)  The OCI function OCITypeByFullName() is supported on a 12.1 Oracle Client
    but will give the error "OCI-22351: This API is not supported by the ORACLE
    server" when used against an 11.2 Oracle Database. The function
    :func:`dpiConn_getObjectType()` now checks the server version and calls the
    correct routine as needed.


Version 2.0.0-beta.2 (March 28, 2017)
-------------------------------------

1)  Use dynamic loading at runtime to load the OCI library and eliminate the
    need for the OCI header files to be present when building ODPI-C.
2)  Improve sample Makefile as requested in `issue 1
    <https://github.com/oracle/odpi/issues/1>`__.
3)  Correct support for handling unsigned integers that are larger than the
    maximum size that can be represented by a signed integer. This corrects
    `issue 3 <https://github.com/oracle/odpi/issues/3>`__.
4)  Remove type DPI_ORACLE_TYPE_LONG_NVARCHAR which is not needed as noted in
    `issue 5 <https://github.com/oracle/odpi/issues/5>`__.
5)  Increase size of string which can be generated from an OCI number. This
    corrects `issue 6 <https://github.com/oracle/odpi/issues/6>`__.
6)  Ensure that zeroing the check integer on ODPI-C handles is not optimised
    away by the compiler.
7)  Silence compiler warnings from the Microsoft C++ compiler.
8)  Restore support for simple reference count tracing by the use of
    DPI_TRACE_REFS.
9)  Add additional error (ORA-56600: an illegal OCI function call was issued)
    to the list of errors that cause the session to be dropped from the session
    pool.
10) Changed LOB sample to include code to populate both CLOBs and BLOBs in
    addition to fetching them.

