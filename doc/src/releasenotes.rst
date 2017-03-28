Release notes
=============

Version 2.0.0-beta.2 (March 28, 2017)
-------------------------------------

1)  Use dynamic loading at runtime to load the OCI library and eliminate the
    need for the OCI header files to be present when building ODPI-C.
2)  Improve sample Makefile as requested in issue 1
    (https://github.com/oracle/odpi/issues/1).
3)  Correct support for handling unsigned integers that are larger than the
    maximum size that can be represented by a signed integer. This corrects
    issue 3 (https://github.com/oracle/odpi/issues/3).
4)  Remove type DPI_ORACLE_TYPE_LONG_NVARCHAR which is not needed
    (https://github.com/oracle/odpi/issues/5).
5)  Increase size of string which can be generated from an OCI number. This
    corrects issue #6 (https://github.com/oracle/odpi/issues/6).
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

