.. _dpiStmtFunctions:

ODPI-C Statement Functions
--------------------------

Statement handles are used to represent statements of all types (queries, DML,
DDL and PL/SQL). They are created by calling the function
:func:`dpiConn_prepareStmt()` or the function :func:`dpiSubscr_prepareStmt()`.
They are also created implicitly when a variable of type DPI_ORACLE_TYPE_STMT
is created. Statement handles can be closed by calling the function
:func:`dpiStmt_close()` or by releasing the last reference to the statement by
calling the function :func:`dpiStmt_release()`.

.. function:: int dpiStmt_addRef(dpiStmt \*stmt)

    Adds a reference to the statement. This is intended for situations where a
    reference to the statement needs to be maintained independently of the
    reference returned when the statement was created.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- the statement to which a reference is to be added. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiStmt_bindByName(dpiStmt \*stmt, const char \*name, \
        uint32_t nameLength, dpiVar \*var)

    Binds a variable to a named placeholder in the statement. A reference to
    the variable is retained by the library and is released when the statement
    itself is released or a new variable is bound to the same name.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to have the variable
    bound. If the reference is NULL or invalid an error is returned.

    **name** [IN] -- a byte string in the encoding used for CHAR data giving
    the name of the placeholder which is to be bound.

    **nameLength** [IN] -- the length of the name parameter, in bytes.

    **var** [IN] -- a reference to the variable which is to be bound. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiStmt_bindByPos(dpiStmt \*stmt, uint32_t pos, dpiVar \*var)

    Binds a variable to a placeholder in the statement by position. A reference
    to the variable is retained by the library and is released when the
    statement itself is released or a new variable is bound to the same
    position.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to have the variable
    bound. If the reference is NULL or invalid an error is returned.

    **pos** [IN] -- the position which is to be bound. The position of a
    placeholder is determined by its location in the statement. Placeholders
    are numbered from left to right, starting from 1, and duplicate names do
    not count as additional placeholders.

    **var** [IN] -- a reference to the variable which is to be bound. If the
    reference is NULL or invalid an error is returned.


.. function:: int dpiStmt_bindValueByName(dpiStmt \*stmt, const char \*name, \
        uint32_t nameLength, dpiNativeTypeNum nativeTypeNum, dpiData \*data)

    Binds a value to a named placeholder in the statement without the need to
    create a variable directly. One is created implicitly and released when the
    statement is released or a new value is bound to the same name.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to have the variable
    bound. If the reference is NULL or invalid an error is returned.

    **name** [IN] -- a byte string in the encoding used for CHAR data giving the
    name of the placeholder which is to be bound.

    **nameLength** [IN] -- the length of the name parameter, in bytes.

    **nativeTypeNum** [IN] -- the type of data that is being bound. It is
    expected to be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **data** [IN] -- the data which is to be bound, as a pointer to a
    :ref:`dpiData<dpiData>` structure. A variable will be created based on the
    type of data being bound and a reference to this variable retained. Once
    the statement has been executed, this new variable will be released.


.. function:: int dpiStmt_bindValueByPos(dpiStmt \*stmt, uint32_t pos, \
        dpiNativeTypeNum nativeTypeNum, dpiData \*data)

    Binds a value to a placeholder in the statement without the need to create
    a variable directly. One is created implicitly and released when the
    statement is released or a new value is bound to the same position.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to have the variable
    bound. If the reference is NULL or invalid an error is returned.

    **pos** [IN] -- the position which is to be bound. The position of a
    placeholder is determined by its location in the statement. Placeholders
    are numbered from left to right, starting from 1, and duplicate names do
    not count as additional placeholders.

    **nativeTypeNum** [IN] -- the type of data that is being bound. It is
    expected to be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **data** [IN] -- the data which is to be bound, as a pointer to a
    :ref:`dpiData<dpiData>` structure. A variable will be created based on the
    type of data being bound and a reference to this variable retained. Once
    the statement has been executed, this new variable will be released.


.. function:: int dpiStmt_close(dpiStmt \*stmt, const char \*tag, \
        uint32_t tagLength)

    Closes the statement and makes it unusable for further work immediately,
    rather than when the reference count reaches zero.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to be closed. If the
    reference is NULL or invalid an error is returned.

    **tag** [IN] -- a key to associate the statement with in the statement
    cache, in the encoding used for CHAR data. NULL is also acceptable in which
    case the statement is not tagged. This value is ignored for statements that
    are acquired through bind variables (REF CURSOR) or implicit results.

    **tagLength** [IN] -- the length of the tag parameter, in bytes, or 0 if
    the tag parameter is NULL.


.. function:: int dpiStmt_define(dpiStmt \*stmt, uint32_t pos, dpiVar \*var)

    Defines the variable that will be used to fetch rows from the statement. A
    reference to the variable will be retained until the next define is
    performed on the same position or the statement is closed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement on which the variable is to
    be defined. If the reference is NULL or invalid an error is returned. Note
    that the statement must have already been executed or an error is returned.

    **pos** [IN] -- the position which is to be defined. The first position is
    1.

    **var** [IN] -- a reference to the variable which is to be used for
    fetching rows from the statement at the given position. If the reference is
    NULL or invalid an error is returned.


.. function:: int dpiStmt_defineValue(dpiStmt \*stmt, uint32_t pos, \
        dpiOracleTypeNum oracleTypeNum, dpiNativeTypeNum nativeTypeNum, \
        uint32_t size, int sizeIsBytes, dpiObjectType \*objType)

    Defines the type of data that will be used to fetch rows from the
    statement. This is intended for use with the function
    :func:`dpiStmt_getQueryValue()`, when the default data type derived from
    the column metadata needs to be overridden by the application. Internally,
    a variable is created with the specified data type and size.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement on which the define is to
    take place.  If the reference is NULL or invalid an error is returned. Note
    that the statement must have already been executed or an error is returned.

    **pos** [IN] -- the position which is to be defined. The first position is
    1.

    **oracleTypeNum** [IN] -- the type of Oracle data that is to be used. It
    should be one of the values from the enumeration
    :ref:`dpiOracleTypeNum<dpiOracleTypeNum>`.

    **nativeTypeNum** [IN] -- the type of native C data that is to be used. It
    should be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **size** [IN] -- the maximum size of the buffer used for transferring data
    to/from Oracle. This value is only used for variables transferred as byte
    strings. Size is either in characters or bytes depending on the value of
    the sizeIsBytes parameter. If the value is in characters, internally the
    value will be multipled by the maximum number of bytes for each character
    and that value used instead when determining the necessary buffer size.

    **sizeIsBytes** [IN] -- boolean value indicating if the size parameter
    refers to characters or bytes. This flag is only used if the variable
    refers to character data.

    **objType** [IN] -- a reference to the object type of the object that is
    being bound or fetched. This value is only used if the Oracle type is
    DPI_ORACLE_TYPE_OBJECT.


.. function:: int dpiStmt_execute(dpiStmt \*stmt, dpiExecMode mode, \
        uint32_t \*numQueryColumns)

    Executes the statement using the bound values. For queries this makes
    available metadata which can be acquired using the function
    :func:`dpiStmt_getQueryInfo()`. For non-queries, out and in-out variables
    are populated with their values.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to be executed. If
    the reference is NULL or invalid an error is returned.

    **mode** [IN] -- one or more of the values from the enumeration
    :ref:`dpiExecMode<dpiExecMode>`, OR'ed together.

    **numQueryColumns** [OUT] -- a pointer to the number of columns which are
    being queried, which will be populated upon successful execution of the
    statement. If the statement does not refer to a query, the value is set to
    0. This parameter may also be NULL.


.. function:: int dpiStmt_executeMany(dpiStmt \*stmt, dpiExecMode mode, \
        uint32_t numIters)

    Executes the statement the specified number of times using the bound
    values. Each bound variable must have at least this many elements allocated
    or an error is returned.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to be executed. If
    the reference is NULL or invalid an error is returned.

    **mode** [IN] -- one or more of the values from the enumeration
    :ref:`dpiExecMode<dpiExecMode>`, OR'ed together. Note that modes
    DPI_MODE_EXEC_BATCH_ERRORS and DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS are only
    supported with insert, update, delete and merge statements.

    **numIters** [IN] -- the number of times the statement is executed. Each
    iteration corresponds to one of the elements of the array that was
    bound earlier.


.. function:: int dpiStmt_fetch(dpiStmt \*stmt, int \*found, \
        uint32_t \*bufferRowIndex)

    Fetches a single row from the statement. If the statement does not refer to
    a query an error is returned. All columns that have not been defined prior
    to this call are implicitly defined using the metadata made available
    when the statement was executed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which a row is to be
    fetched.  If the reference is NULL or invalid an error is returned.

    **found** [OUT] -- a pointer to a boolean value indicating if a row was
    fetched or not, which will be populated upon successful completion of this
    function.

    **bufferRowIndex** [OUT] -- a pointer to the buffer row index which will be
    populated upon successful completion of this function if a row is found.
    This index is used as the array position for getting values from the
    variables that have been defined for the statement.


.. function:: int dpiStmt_fetchRows(dpiStmt \*stmt, uint32_t maxRows, \
        uint32_t \*bufferRowIndex, uint32_t \*numRowsFetched, int \*moreRows)

    Returns the number of rows that are available in the buffers defined for
    the query. If no rows are currently available in the buffers, an internal
    fetch takes place in order to populate them, if rows are available. If
    the statement does not refer to a query an error is returned. All columns
    that have not been defined prior to this call are implicitly defined using
    the metadata made available when the statement was executed.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which rows are to be
    fetched.  If the reference is NULL or invalid an error is returned.

    **maxRows** [IN] -- the maximum number of rows to fetch. If the number of
    rows available exceeds this value only this number will be fetched.

    **bufferRowIndex** [OUT] -- a pointer to the buffer row index which will be
    populated upon successful completion of this function. This index is used
    as the array position for getting values from the variables that have been
    defined for the statement.

    **numRowsFetched** [OUT] -- a pointer to the number of rows that have been
    fetched, populated after the call has completed successfully.

    **moreRows** [OUT] -- a pointer to a boolean value indicating if there are
    potentially more rows that can be fetched after the ones fetched by this
    function call.


.. function:: int dpiStmt_getBatchErrorCount(dpiStmt \*stmt, uint32_t \*count)

    Returns the number of batch errors that took place during the last
    execution with batch mode enabled. Batch errors are only available when
    both the client and the server are at 12.1.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the number of
    batch errors is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **count** [OUT] -- a pointer to the number of batch errors that took place,
    which is populated after successful completion of the function.


.. function:: int dpiStmt_getBatchErrors(dpiStmt \*stmt, uint32_t numErrors, \
        dpiErrorInfo \*errors)

    Returns the batch errors that took place during the last execution with
    batch mode enabled. Batch errors are only available when both the client
    and the server are at 12.1.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the batch errors
    are to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **numErrors** [IN] -- the size of the errors array in number of elements.
    The number of batch errors that are available can be determined using
    :func:`dpiStmt_getBatchErrorCount()`.

    **errors** [IN] -- a pointer to the first element of an array of
    :ref:`dpiErrorInfo<dpiErrorInfo>` structures which is assumed to contain
    the number of elements specified by the numErrors parameter.


.. function:: int dpiStmt_getBindCount(dpiStmt \*stmt, uint32_t \*count)

    Returns the number of bind variables in the prepared statement. In SQL
    statements this is the total number of bind variables whereas in PL/SQL
    statements this is the count of the **unique** bind variables.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the number of bind
    variables is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **count** [OUT] -- a pointer to the number of bind variables found in the
    statement, which is populated upon successful completion of the function.


.. function:: int dpiStmt_getBindNames(dpiStmt \*stmt, \
        uint32_t \*numBindNames, const char \**bindNames, \
        uint32_t \*bindNameLengths)

    Returns the names of the unique bind variables in the prepared statement.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the names of bind
    variables are to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **numBindNames** [OUT] -- a pointer to the size of the bindNames and
    bindNameLengths arrays in number of elements. This value must be large
    enough to hold all of the unique bind variables in the prepared statement
    or an error will be returned. The maximum number of bind variables can be
    determined by calling :func:`dpiStmt_getBindCount()`. Upon successful
    completion of this function, the actual number of unique bind variables
    in the prepared statement will be populated.

    **bindNames** [OUT] -- an array of pointers to byte strings in the encoding
    used for CHAR data. The size of the array is specified using the
    numBindNames parameter. When the function completes this array will be
    filled with the names of the unique bind variables in the statement.

    **bindNameLengths** [OUT] -- a pointer to the first element of an array of
    integers containing the lengths of the bind variable names which is
    filled in upon successful completion of the function. The number of
    elements is assumed to be specified by the numBindNames parameter.


.. function:: int dpiStmt_getFetchArraySize(dpiStmt \*stmt, \
        uint32_t \*arraySize)

    Gets the array size used for performing fetches.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the fetch array
    size is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **arraySize** [OUT] -- a pointer to the value which will be populated upon
    successful completion of this function.


.. function:: int dpiStmt_getImplicitResult(dpiStmt \*stmt, \
        dpiStmt \**implicitResult)

    Returns the next implicit result available from the last execution of the
    statement. Implicit results are only available when both the client and
    server are 12.1 or higher.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the next implicit
    result is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **implicitResult** [OUT] -- a pointer to a reference to a statement which
    will be populated with the next implicit result upon successful completion
    of the function. If no implicit results remain, the reference will be set
    to NULL. The reference that is returned must be released as soon as it is
    no longer needed.


.. function:: int dpiStmt_getInfo(dpiStmt \*stmt, dpiStmtInfo \*info)

    Returns information about the statement.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which information is to
    be retrieved. If the reference is NULL or invalid an error is returned.

    **info** [OUT] -- a pointer to a structure of type
    :ref:`dpiStmtInfo<dpiStmtInfo>` which will be filled in with information
    about the statement upon successful completion of the function.


.. function:: int dpiStmt_getNumQueryColumns(dpiStmt \*stmt, \
        uint32_t \*numQueryColumns)

    Returns the number of columns that are being queried.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the number of
    query columns is to be retrieved. If the reference is NULL or invalid an
    error is returned.

    **numQueryColumns** [OUT] -- a pointer to the number of columns which are
    being queried by the statement, which is filled in upon successful
    completion of the function. If the statement does not refer to a query, the
    value is populated with 0.


.. function:: int dpiStmt_getQueryInfo(dpiStmt \*stmt, uint32_t pos, \
        dpiQueryInfo \*info)

    Returns information about the column that is being queried.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the column
    metadata is to be retrieved. If the reference is NULL or invalid an error
    is returned.

    **pos** [IN] -- the position of the column whose metadata is to be
    retrieved. The first position is 1.

    **info** [OUT] -- a pointer to a :ref:`dpiQueryInfo<dpiQueryInfo>`
    structure which will be filled in upon successful completion of the
    function.


.. function:: int dpiStmt_getQueryValue(dpiStmt \*stmt, uint32_t pos, \
        dpiNativeTypeNum \*nativeTypeNum, dpiData \*data)

    Returns the value of the column at the given position for the currently
    fetched row, without needing to provide a variable. If the data type of
    the column needs to be overridden, the function
    :func:`dpiStmt_defineValue()` can be called to specify a different type
    after executing the statement but before fetching any data.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the column value
    is to be retrieved. If the reference is NULL or invalid an error is
    returned.

    **pos** [IN] -- the position of the column whose value is to be retrieved.
    The first position is 1.

    **nativeTypeNum** [OUT] -- a pointer to the native type that is used by the
    value, which will be populated upon successful completion of this function.
    It will be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

    **data** [OUT] -- a pointer to a :ref:`dpiData<dpiData>` structure which
    will be populated with the value of the column upon successful completion
    of the function. Note that any references to LOBs, statements, objects and
    rowids are owned by the statement. If you wish to retain these values
    independently of the statement, a reference must be added by calling one of
    :func:`dpiLob_addRef()`, :func:`dpiStmt_addRef()`,
    :func:`dpiObject_addRef()` or :func:`dpiRowid_addRef()`.


.. function:: int dpiStmt_getRowCount(dpiStmt \*stmt, uint64_t \*count)

    Returns the number of rows affected by the last DML statement that was
    executed or the number of rows currently fetched from a query. In all other
    cases 0 is returned.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the row count is
    to be retrieved. If the reference is NULL or invalid an error is returned.

    **count** [OUT] -- a pointer to the row count which will be populated upon
    successful completion of the function.


.. function:: int dpiStmt_getRowCounts(dpiStmt \*stmt, \
        uint32_t \*numRowCounts, uint64_t \**rowCounts)

    Returns an array of row counts affected by the last invocation of
    :func:`dpiStmt_executeMany()` with the array DML rowcounts mode enabled.
    This feature is only available if both client and server are at 12.1.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the row counts are
    to be retrieved. If the reference is NULL or invalid an error is returned.

    **numRowCounts** [OUT] -- a pointer to the size of the rowCounts array
    which is being returned. It is populated upon successful completion of the
    function.

    **rowCounts** [OUT] -- a pointer to an array of row counts which is
    populated upon successful completion of the function. This array should be
    considered read-only.


.. function:: int dpiStmt_getSubscrQueryId(dpiStmt \*stmt, uint64_t \*queryId)

    Returns the id of the query that was just registered on the subscription
    by calling :func:`dpiStmt_execute()` on a statement prepared by calling
    :func:`dpiSubscr_prepareStmt()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement from which the query id
    should be retrieved. This statement should have been prepared using the
    function :func:`dpiSubscr_prepareStmt()`. If the reference is NULL or
    invalid an error is returned.

    **queryId** [OUT] -- a pointer to the query id, which is filled in upon
    successful completion of the function.


.. function:: int dpiStmt_release(dpiStmt \*stmt)

    Releases a reference to the statement. A count of the references to the
    statement is maintained and when this count reaches zero, the memory
    associated with the statement is freed and the statement is closed if that
    has not already taken place using the function :func:`dpiStmt_close()`.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- the statement from which a reference is to be released. If
    the reference is NULL or invalid an error is returned.


.. function:: int dpiStmt_scroll(dpiStmt \*stmt, dpiFetchMode mode, \
        int32_t offset, int32_t rowCountOffset)

    Scrolls the statement to the position in the cursor specified by the mode
    and offset.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement which is to be scrolled to a
    particular row position. If the reference is NULL or invalid an error is
    returned.

    **mode** [IN] -- one of the values from the enumeration
    :ref:`dpiFetchMode<dpiFetchMode>`.

    **offset** [IN] -- a value which is used with the mode in order to
    determine the row position in the cursor.

    **rowCountOffset** [IN] -- an offset to the row count used when calculating
    the desired row to be fetched. This is needed when a client has fetched
    multiple rows but has not yet consumed all of them. If this is not being
    done by the client, the value 0 is appropriate.

.. function:: int dpiStmt_setFetchArraySize(dpiStmt \*stmt, uint32_t arraySize)

    Sets the array size used for performing fetches. All variables defined for
    fetching must have this many (or more) elements allocated for them. The
    higher this value is the less network round trips are required to fetch
    rows from the database but more memory is also required. A value of zero
    will reset the array size to the default value of
    DPI_DEFAULT_FETCH_ARRAY_SIZE.

    The function returns DPI_SUCCESS for success and DPI_FAILURE for failure.

    **stmt** [IN] -- a reference to the statement on which the fetch array size
    is to be set. If the reference is NULL or invalid an error is returned.

    **arraySize** [IN] -- the number of rows which should be fetched each time
    more rows need to be fetched from the database.

