.. _dpiStmtInfo:

ODPI-C Structure dpiStmtInfo
----------------------------

This structure is used for passing information about a statement from ODPI-C. It
is used by the function :func:`dpiStmt_getInfo()`.

.. member:: int dpiStmtInfo.isQuery

    Specifies if the statement refers to a query (1) or not (0).

.. member:: int dpiStmtInfo.isPLSQL

    Specifies if the statement refers to a PL/SQL block (1) or not (0).

.. member:: int dpiStmtInfo.isDDL

    Specifies if the statement refers to DDL (data definition language) such
    as creating a table (1) or not (0).

.. member:: int dpiStmtInfo.isDML

    Specifies if the statement refers to DML (data manipulation language) such
    as inserting, updating and deleting (1) or not (0).

.. member:: dpiStatementType dpiStmtInfo.statementType

    Specifies the type of statement that has been prepared. The members
    :member:`dpiStmtInfo.isQuery`, :member:`dpiStmtInfo.isPLSQL`,
    :member:`dpiStmtInfo.isDDL` and :member:`dpiStmtInfo.isDML` are all
    categorizations of this value. It will be one of the values from the
    enumeration :ref:`dpiStatementType<dpiStatementType>`.

.. member:: int dpiStmtInfo.isReturning

    Specifies if the statement has a returning clause in it (1) or not (0).

.. member:: char* dpiStmtInfo.sqlId

    A pointer to a string containing the SQL_ID of the statement, if available.
    The SQL_ID uniquely identifies a SQL statement in the database. This value
    is only populated when using Oracle Client and Oracle Database 12.2, or
    later. The string is not null-terminated; use
    :member:`dpiStmtInfo.sqlIdLength` to determine its size. It will remain
    valid only as long as the statement from which this statement info was
    acquired remains valid.

.. member:: uint32_t dpiStmtInfo.sqlIdLength

    The length, in bytes, of the SQL_ID returned in
    :member:`dpiStmtInfo.sqlId`. This will be zero if the SQL_ID is not
    available.
