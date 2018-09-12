.. _dpiStatementType:

ODPI-C Enumeration dpiStatementType
-----------------------------------

This enumeration identifies the type of statement that has been prepared. It is
available as part of the structure :ref:`dpiStmtInfo<dpiStmtInfo>`.

==========================  ===================================================
Value                       Description
==========================  ===================================================
DPI_STMT_TYPE_UNKNOWN       Indicates that the statement type is unknown.
DPI_STMT_TYPE_SELECT        Identifies a select statement. The member
                            :member:`dpiStmtInfo.isQuery` will be set to 1.
DPI_STMT_TYPE_UPDATE        Identifies an update statement. The member
                            :member:`dpiStmtInfo.isDML` will be set to 1.
DPI_STMT_TYPE_DELETE        Identifies a delete statement. The member
                            :member:`dpiStmtInfo.isDML` will be set to 1.
DPI_STMT_TYPE_INSERT        Identifies an insert statement. The member
                            :member:`dpiStmtInfo.isDML` will be set to 1.
DPI_STMT_TYPE_CREATE        Identifies a create statement. The member
                            :member:`dpiStmtInfo.isDDL` will be set to 1.
DPI_STMT_TYPE_DROP          Identifies a drop statement. The member
                            :member:`dpiStmtInfo.isDDL` will be set to 1.
DPI_STMT_TYPE_ALTER         Identifies an alter statement. The member
                            :member:`dpiStmtInfo.isDDL` will be set to 1.
DPI_STMT_TYPE_BEGIN         Identifies an anonymous PL/SQL block starting with
                            the keyword begin. The member
                            :member:`dpiStmtInfo.isPLSQL` will be set to 1.
DPI_STMT_TYPE_DECLARE       Identifies an anonymous PL/SQL block starting with
                            the keyword declare. The member
                            :member:`dpiStmtInfo.isPLSQL` will be set to 1.
DPI_STMT_TYPE_CALL          Identifies a CALL statement used for calling stored
                            procedures and functions.  The member
                            :member:`dpiStmtInfo.isPLSQL` will be set to 1.
DPI_STMT_TYPE_MERGE         Identifies a merge statement. The member
                            :member:`dpiStmtInfo.isDML` will be set to 1.
DPI_STMT_TYPE_EXPLAIN_PLAN  Identifies an explain plan statement. The member
                            :member:`dpiStmtInfo.isDML` will be set to 1.
DPI_STMT_TYPE_COMMIT        Identifies a commit statement.
DPI_STMT_TYPE_ROLLBACK      Identifies a rollback statement.
==========================  ===================================================

