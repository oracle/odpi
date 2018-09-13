ODPI-C Release notes
====================

Version 3.0.0 (September 13, 2018)
----------------------------------

#)  Added support for Oracle Client 18 libraries.
#)  Added support for SODA (as preview). See
    :ref:`SODA Database<dpiSodaDbFunctions>`,
    :ref:`SODA Collection<dpiSodaCollFunctions>` and
    :ref:`SODA Document<dpiSodaDocFunctions>` for more information.
#)  Added support for call timeouts available in Oracle Client 18.1 and higher.
    See functions :func:`dpiConn_setCallTimeout()` and
    :func:`dpiConn_getCallTimeout()`.
#)  Added support for setting a LOB attribute of an object with string/bytes
    using the function :func:`dpiObject_setAttributeValue()`.
#)  Added support for the packed decimal type used by object attributes with
    historical types DECIMAL and NUMERIC
    (`cx_Oracle issue 212
    <https://github.com/oracle/python-cx_Oracle/issues/212>`__).
#)  On Windows, first attempt to load oci.dll from the same directory as the
    module that contains ODPI-C.
#)  SQL Objects that are created or fetched from the database are now tracked
    and marked unusable when a connection is closed. This was done in order to
    avoid a segfault in some circumstances.
#)  Improved support for closing pools by ensuring that once a pool has closed,
    further attempts to use connections acquired from that pool will fail with
    error "DPI-1010: not connected".
#)  Re-enabled pool pinging functionality for Oracle Client 12.2 and higher
    to handle classes of connection errors such as resource profile limits.
#)  Improved error messages when the Oracle Client or Oracle Database need to
    be at a minimum version in order to support a particular feature.
#)  Use plain integers instead of enumerations in order to simplify code and
    reduce the requirement for casts. Typedefs have been included so that code
    does not need to be changed.
#)  Eliminated potential buffer overrun
    (`issue 69 <https://github.com/oracle/odpi/issues/69>`__).
#)  In the Makefile for non-Windows platforms, the version information for
    ODPI-C is acquired directly from include/dpi.h as suggested
    (`issue 66 <https://github.com/oracle/odpi/issues/66>`__).
#)  Removed function dpiConn_newSubscription(). Use function
    :func:`dpiConn_subscribe()` instead.
#)  Removed function dpiLob_flushBuffer(). This function never worked anyway.
#)  Removed function dpiSubscr_close(). Use function
    :func:`dpiConn_unsubscribe()` instead.
#)  Removed function dpiVar_getData(). Use function
    :func:`dpiVar_getReturnedData()` instead.
#)  Added additional test cases.
#)  Improved documentation.


Version 2.4.2 (July 9, 2018)
----------------------------

#)  Avoid buffer overrun due to improper calculation of length byte when
    converting some negative 39 digit numbers from string to the internal
    Oracle number format
    (`issue 67 <https://github.com/oracle/odpi/issues/67>`__).


Version 2.4.1 (July 2, 2018)
----------------------------

#)  Use the install_name_tool on macOS to adjust the library name, as suggested
    (`issue 65 <https://github.com/oracle/odpi/issues/65>`__).
#)  Even when dequeue fails OCI still provides a message id so make sure it is
    deallocated to avoid a memory leak.
#)  Ensure that the row count for queries is reset to zero when the statement
    is executed (`cx_Oracle issue 193
    <https://github.com/oracle/python-cx_Oracle/issues/193>`__).
#)  If the statement should be deleted from the statement cache, first check to
    see that there is a statement cache currently being used; otherwise, the
    error "ORA-24300: bad value for mode" will be raised under certain
    conditions.


Version 2.4 (June 6, 2018)
--------------------------

#)  Added support for grouping events for subscriptions. See attributes
    :member:`dpiSubscrCreateParams.groupingClass`,
    :member:`dpiSubscrCreateParams.groupingValue` and
    :member:`dpiSubscrCreateParams.groupingType`.
#)  Added support for specifying the IP address a subscription should use
    instead of having the Oracle Client libraries determine the IP address to
    use on its own. See attributes
    :member:`dpiSubscrCreateParams.ipAddress` and
    :member:`dpiSubscrCreateParams.ipAddressLength`.
#)  Added support for subscribing to notifications when messages are available
    to dequeue in an AQ queue. See attribute
    :member:`dpiSubscrCreateParams.subscrNamespace` and the enumeration
    :ref:`dpiSubscrNamespace<dpiSubscrNamespace>` as well as the attributes
    :member:`dpiSubscrMessage.queueName`,
    :member:`dpiSubscrMessage.queueNameLength`,
    :member:`dpiSubscrMessage.consumerName` and
    :member:`dpiSubscrMessage.consumerNameLength`.
#)  Added attribute :member:`dpiSubscrMessage.registered` to allow the
    application to know when a subscription is no longer registered with the
    database. Deregistration can take place when the
    :member:`dpiSubscrCreateParams.timeout` value is reached or when
    :member:`dpiSubscrCreateParams.qos` is set to the value
    DPI_SUBSCR_QOS_DEREG_NFY. Note that notifications are not sent when a
    subscription is explicitly deregistered.
#)  Added method :func:`dpiConn_subscribe()` to replace method
    dpiConn_newSubscription() and added method :func:`dpiConn_unsubscribe()` to
    replace method dpiSubscr_close(). The replaced methods are deprecated and
    will be removed in version 3.0. The new methods clarify the fact that
    subscriptions do not require the connection they were created with to
    remain open. A new connection with the same credentials can be used to
    unusbscribe from events in the database.
#)  Added support for the pool "get" mode of timed wait. See attributes
    :member:`dpiPoolCreateParams.getMode` and
    :member:`dpiPoolCreateParams.waitTimeout`. The wait timeout value can be
    acquired after pool creation using the new method
    :func:`dpiPool_getWaitTimeout()` and set after pool creation using the new
    method :func:`dpiPool_setWaitTimeout()`.
#)  Added support for setting the maximum lifetime session and timeout
    parameters when creating a pool. See attributes
    :member:`dpiPoolCreateParams.maxLifetimeSession` and
    :member:`dpiPoolCreateParams.timeout`.
#)  Added support for installing ODPI-C into a user-defined prefix on platforms
    other than Windows, as requested
    (`issue 59 <https://github.com/oracle/odpi/issues/59>`__).
#)  Added support for setting the SONAME for shared libraries on platforms
    other than Windows, as requested
    (`issue 44 <https://github.com/oracle/odpi/issues/44>`__).
#)  Improved error message when attempting to create a subscription without
    enabling events mode when the pool or standalone connection is created.
#)  Added checks for minimal Oracle Client version (12.1) when calling the
    methods :func:`dpiPool_getMaxLifetimeSession()` and
    :func:`dpiPool_setMaxLifetimeSession()`.
#)  Added check to prevent attempts to bind PL/SQL array variables using the
    method :func:`dpiStmt_executeMany()`.
#)  Ensure that method :func:`dpiStmt_getRowCount()` returns the value 0 for
    all statements other than queries and DML, as documented.
#)  Correct handling of ROWIDs and statements when used as bind variables
    during execution of DML RETURNING statements.
#)  Added additional test cases.
#)  Improved documentation.


Version 2.3.2 (May 7, 2018)
---------------------------

#)  Ensure that a call to unregister a subscription only occurs if the
    subscription is still registered.
#)  Ensure that before a statement is executed that any dynamic buffers created
    for DML returning statments are reset since the out bind callback is not
    called if no rows are returned!
#)  Silenced compilation warning in test suite.
#)  Added test cases for DML returning statements.


Version 2.3.1 (April 25, 2018)
------------------------------

#)  Fixed determination of the number of rows returned in a DML Returning
    statement when the same statement is executed multiple times in succession
    with less rows being returned in each succeeding execution.
#)  Stopped attempting to unregister a CQN subscription before it was
    completely registered. This prevents errors encountered during registration
    from being masked by an error stating that the subscription has not been
    registered!
#)  Fixed support for true heterogeneous session pools that use different
    user/password combinations for each session acquired from the pool.
#)  Added error message indicating that modes DPI_MODE_EXEC_BATCH_ERRORS and
    DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS are only supported with insert, update,
    delete and merge statements.
#)  Corrected comment
    (`issue 61 <https://github.com/oracle/odpi/issues/61>`__).
#)  Renamed internal method dpiStmt__preFetch() to dpiStmt__beforeFetch() in
    order to avoid confusion with OCI prefetch.


Version 2.3 (April 2, 2018)
---------------------------

#)  Corrected support for getting the OUT values of bind variables bound to a
    DML Returning statement when calling the function
    :func:`dpiStmt_executeMany()`. Since multiple rows can be returned for each
    iteration, a new function :func:`dpiVar_getReturnedData()` has been added
    and the original function :func:`dpiVar_getData()` has been deprecated and
    will be removed in version 3.
#)  Corrected binding of LONG data (values exceeding 32KB) when using the
    function :func:`dpiStmt_executeMany()`.
#)  Added code to verify that the CQN subscription is open before permitting it
    to be used. Error "DPI-1060: subscription was already closed" will now be
    raised if an attempt is made to use a subscription that was closed earlier.
#)  Added error "DPI-1061: edition is not supported when a new password is
    specified" to clarify the fact that specifying an edition and a new
    password at the same time is not supported. Previously the edition value
    was simply ignored.
#)  Query metadata is no longer fetched if executing a statement with mode
    DPI_MODE_EXEC_PARSE_ONLY.
#)  Added additional statement types (DPI_STMT_TYPE_EXPLAIN_PLAN,
    DPI_STMT_TYPE_ROLLBACK and DPI_STMT_TYPE_COMMIT) as well as one that covers
    statement types not currently identified (DPI_STMT_TYPE_UNKNOWN).
#)  Improved error message when older OCI client libraries are being used that
    don't have the method OCIClientVersion().
#)  Corrected the handling of ANSI types REAL and DOUBLE PRECISION as
    implemented by Oracle. These types are just subtypes of NUMBER and are
    different from BINARY_FLOAT and BINARY_DOUBLE (`cx_Oracle issue 163
    <https://github.com/oracle/python-cx_Oracle/issues/163>`__).
#)  Added check that the return value from OCI functions matches the expected
    value of OCI_ERROR, and if not, raises an error including the value that
    was actually returned.
#)  Added additional test cases.
#)  Removed unused error messages.


Version 2.2.1 (March 5, 2018)
-----------------------------

#)  Maintain a reference to the "parent" object and use the actual object
    instance instead of a copy, so that "child" objects can be manipulated
    in-place instead of having to be created externally and then set
    (attributes) or appended (collections).
#)  Correct handling of boundary numbers 1e126 and -1e126.
#)  Eliminate memory leak when calling :func:`dpiConn_deqObject()` and
    :func:`dpiConn_enqObject()`.
#)  Eliminate memory leak when setting NCHAR and NVARCHAR attributes of
    objects.
#)  Eliminate memory leak when fetching collection objects from the database.
#)  Prevent internal re-execution of statement from duplicating itself in the
    list of open statements maintained on the connection.
#)  Improved documentation.


Version 2.2 (February 14, 2018)
-------------------------------

#)  Keep track of open statements and LOBs and automatically close them when
    the connection is closed; this eliminates the need for users of the driver
    to do so and removes the error "DPI-1054: connection cannot be closed when
    open statements or LOBs exist".
#)  Ignore failures that occur during the implicit rollback performed when a
    connection is closed, but if an error does occur, ensure that the
    connection is dropped from the pool (if it was acquired from a pool); such
    failures are generally due to an inability to communicate with the server
    (such as when your session has been killed).
#)  Avoid a round trip to the database when a connection is released back to
    the pool by preventing a rollback from being called when there is no
    transaction in progress.
#)  Improve error message when the use of bind variables is attempted with DDL
    statements, which is not supported.
#)  Since rowid is returned as a handle, the size in bytes and characters was
    simply being returned as the size of a pointer; set these values to 0
    instead as is done with other handles that are returned.
#)  Ensure that the LOB locator returned from the object attribute or element
    is not used directly as freeing it will result in unexpected behavior when
    the object containing it is itself freed.
#)  Make a copy of any objects that are acquired from other objects (either as
    attributes or elements of collections) in order to prevent possible use of
    the object acquired in such a fashion after the object it came from has
    been freed.
#)  Protect global variables on destruction as well since dpiGlobal__finalize()
    may not be the last method that is called if other methods are registered
    with atexit().
#)  Use cast to avoid assertions with isspace() when using debug libraries on
    Windows (`issue 52 <https://github.com/oracle/odpi/issues/52>`__).
#)  Added file embed/dpi.c to simplify inclusion of ODPI-C in projects.
#)  Minor changes to satisfy pickier compilers and static analysis tools.
#)  Added additional test cases.
#)  Improved documentation.


Version 2.1 (December 12, 2017)
-------------------------------

#)  Connections

    - Support was added for accessing sharded databases via sharding keys (new
      in Oracle 12.2). NOTE: the underlying OCI library has a bug when using
      standalone connections. There is a small memory leak proportional to the
      number of connections created/dropped. There is no memory leak when using
      session pools, which is recommended.
    - Added options for authentication with SYSBACKUP, SYSDG, SYSKM and SYSRAC,
      as requested (`cx_Oracle issue 101
      <https://github.com/oracle/python-cx_Oracle/issues/101>`__).
    - Attempts to release statements or free LOBs after the connection has been
      closed (by, for example, killing the session) are now prevented.
    - An error message was added when specifying an edition and a connection
      class since this combination is not supported.
    - Attempts to close the session for connections created with an external
      handle are now prevented.
    - Attempting to ping a database earlier than 10g results in ORA-1010:
      invalid OCI operation, but that implies a response from the database and
      therefore a successful ping, so treat it that way!
      (see `<https://github.com/rana/ora/issues/224>`__ for more information).

#)  Objects

    - Support was added for converting numeric values in an object type
      attribute to integer and text, as requested (`issue 35
      <https://github.com/oracle/odpi/issues/35>`__).
    - Methods :func:`dpiDeqOptions_setMsgId()` and
      :func:`dpiMsgProps_setOriginalMsgId()` now set their values correctly.
    - The overflow check when using double values as input to float attributes
      of objects or elements of collections was removed as it didn't work
      anyway and is a well-known issue that cannot be prevented without
      removing desired functionality. The developer should ensure that the
      source value falls within the limits of floats, understand the consequent
      precision loss or use a different data type.

#)  Variables

    - Support was added for setting a LOB variable using
      :func:`dpiVar_setFromBytes()`.
    - Added support for the case when the time zone minute offset is negative,
      as requested (`issue 38 <https://github.com/oracle/odpi/issues/38>`__).
    - Variables of type DPI_NATIVE_TYPE_BYTES are restricted to 2 bytes less
      than 1 GB (1,073,741,822 bytes), since OCI cannot handle more than that
      currently.

#)  Miscellaneous

    - Support was added for identifying the id of the transaction which spawned
      a CQN subscription message, as requested
      (`issue 32 <https://github.com/oracle/odpi/issues/32>`__).
    - Corrected use of subscription port number (`cx_Oracle issue 115
      <https://github.com/oracle/python-cx_Oracle/issues/115>`__).
    - Added support for getting information about MERGE statements, as
      requested (`issue 40 <https://github.com/oracle/odpi/issues/40>`__).
    - Problems reported with the usage of FormatMessage() on Windows was
      addressed (`issue 47 <https://github.com/oracle/odpi/issues/47>`__).
    - On Windows, if oci.dll cannot be loaded because it is the wrong
      architecture (32-bit vs 64-bit), attempt to find the offending DLL and
      include the full path of the DLL in the message, as suggested
      (`issue 49 <https://github.com/oracle/odpi/issues/49>`__).

#)  Debugging

    - Support was added to the debugging infrastructure to print the thread id
      and the date/time of messages. Support for an environment variable
      DPI_DEBUG_PREFIX was also added. See :ref:`debugging`.
    - Support was added for debugging both entry and exit points of ODPI-C
      public functions and for memory allocation/deallocation.

#)  Infrastructure

    - Dependent libraries were moved to the main Makefile so that applications
      do not have to do that, as suggested (`issue 33
      <https://github.com/oracle/odpi/issues/33>`__).
    - Added Makefile.win32 for the use of nmake on Windows and reworked
      existing Makefiles to support only platforms other than Windows.
    - Ensure that ODPI-C extended initialization code takes place before any
      other ODPI-C code can take place, and that it takes place in only one
      thread. Code was also added to cleanup the global OCI environment on
      process exit.
    - The OCI wrapers for using mutexes were eliminated, which improves
      performance.
    - Force OCI prefetch to always use the value 2; the OCI default is 1 but
      setting the ODPI-C default to 2 ensures that single row fetches don't
      require an extra round trip to determine if there are more rows to fetch;
      this change also reduces the potential memory consumption when
      fetchArraySize was set to a large value and also avoids performance
      issues discovered with larger values of prefetch.
    - Unused parameters for internal functions were removed where possible and
      ``__attribute((unused))`` added where not possible, as requested
      (`issue 39 <https://github.com/oracle/odpi/issues/39>`__).
    - The use of OCIThreadKeyInit() in any code other than the global
      initialization code was removed in order to avoid bugs in the OCI
      library.
    - Compiler warnings and Parfait warnings were eliminated.
    - Added additional test cases.
    - Documentation improvements.

#)  Deprecations

    - The function dpiLob_flushBuffer() is deprecated and will be removed in
      version 3. It previously always returned an error anyway because of the
      inability to enable LOB buffering and now always returns the error
      "DPI-1013: not supported".


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
#)  Correct parameter subscrId to dpiConn_newSubscription() as
    `noted <https://github.com/oracle/odpi/issues/28>`__. There is no OCI
    documentation for this attribute and should never have been exposed. The
    parameter is therefore deprecated and will be removed in version 3. The
    value NULL can now also be passed to this parameter.
#)  Add script for running valgrind and correct issues found in test suite
    reported by valgrind
    (`issue 29 <https://github.com/oracle/odpi/issues/29>`__).
#)  Use posix_spawn() instead of system() in the test suite, as requested
    (`issue 30 <https://github.com/oracle/odpi/issues/30>`__).
#)  Add support for DML Returning statements that require dynamically allocated
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
    a DML Returning statement.
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

