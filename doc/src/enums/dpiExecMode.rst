.. _dpiExecMode:

ODPI-C Enumeration dpiExecMode
------------------------------

This enumeration identifies the available modes for executing statements
using :func:`dpiStmt_execute()` and :func:`dpiStmt_executeMany()`.

=================================  ============================================
Value                              Description
=================================  ============================================
DPI_MODE_EXEC_DEFAULT              Default mode for execution. Metadata is made
                                   available after queries are executed.
DPI_MODE_EXEC_DESCRIBE_ONLY        Do not execute the statement but simply
                                   acquire the metadata for the query.
DPI_MODE_EXEC_COMMIT_ON_SUCCESS    If execution completes successfully, the
                                   current active transaction is committed.
DPI_MODE_EXEC_BATCH_ERRORS         Enable batch error mode. This permits an
                                   an array DML operation to succeed even if
                                   some of the individual operations fail. The
                                   errors can be retrieved using the function
                                   :func:`dpiStmt_getBatchErrors()`.
DPI_MODE_EXEC_PARSE_ONLY           Do not execute the statement but only parse
                                   it and return any parse errors.
DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS  Enable getting row counts for each DML
                                   operation when performing an array DML
                                   execution. The actual row counts can be
                                   retrieved using the function
                                   :func:`dpiStmt_getRowCounts()`.
=================================  ============================================

