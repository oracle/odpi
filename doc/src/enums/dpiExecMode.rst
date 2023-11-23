.. _dpiExecMode:

ODPI-C Enumeration dpiExecMode
------------------------------

This enumeration identifies the available modes for executing statements
using :func:`dpiStmt_execute()` and :func:`dpiStmt_executeMany()`.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiExecMode
     enumeration. The second column displays the description of the
     dpiExecMode enumeration value.

    * - Value
      - Description
    * - DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS
      - Enable getting row counts for each DML operation when performing an
        array DML execution. The actual row counts can be retrieved using the
        function :func:`dpiStmt_getRowCounts()`.
    * - DPI_MODE_EXEC_BATCH_ERRORS
      - Enable batch error mode. This permits an an array DML operation to
        succeed even if some of the individual operations fail. The errors can
        be retrieved using the function :func:`dpiStmt_getBatchErrors()`.
    * - DPI_MODE_EXEC_COMMIT_ON_SUCCESS
      - If execution completes successfully, the current active transaction is
        committed.
    * - DPI_MODE_EXEC_DEFAULT
      - Default mode for execution. Metadata is made available after queries
        are executed.
    * - DPI_MODE_EXEC_DESCRIBE_ONLY
      - Do not execute the statement but simply acquire the metadata for the
        query.
    * - DPI_MODE_EXEC_PARSE_ONLY
      - Do not execute the statement but only parse it and return any parse
        errors. Note that using this mode with a DDL statement will result in
        the statement being executed.
