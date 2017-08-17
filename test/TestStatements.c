//-----------------------------------------------------------------------------
// Copyright (c) 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestStatements.c
//   Test suite for testing all the statement related cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_1100_releaseTwice()
//   Prepare any statement; call dpiStmt_release() twice (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1100_releaseTwice(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_release(stmt);
    return dpiTestCase_expectError(testCase,
            "DPI-1002: invalid dpiStmt handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1101_executeManyInvalidParams()
//   Prepare any query; call dpiStmt_executeMany() (error DPI-1013).
//-----------------------------------------------------------------------------
int dpiTest_1101_executeManyInvalidParams(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    const uint32_t numIters = 2;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, numIters);
    if (dpiTestCase_expectError(testCase, "DPI-1013: not supported") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1102_bindCountNoBinds()
//   Prepare any statement with no bind variables; call dpiStmt_getBindCount()
// and confirm that the value returned is 0 (no error).
//-----------------------------------------------------------------------------
int dpiTest_1102_bindCountNoBinds(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    uint32_t count;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBindCount(stmt, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, count, 0);
}


//-----------------------------------------------------------------------------
// dpiTest_1103_bindCountOneBind()
//   Prepare any statement with one bind variable; call dpiStmt_getBindCount()
// and confirm that the value returned is 1 (no error).
//-----------------------------------------------------------------------------
int dpiTest_1103_bindCountOneBind(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "select :1 from TestLongs";
    uint32_t count;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBindCount(stmt, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, count, 1);
}


//-----------------------------------------------------------------------------
// dpiTest_1104_bindNamesNoDuplicatesSql()
//   Prepare any statement with duplicate bind variable names; call
// dpiStmt_getBindNames() and verify that the names of the bind variables match
// what is expected, without duplicates (no error).
//-----------------------------------------------------------------------------
int dpiTest_1104_bindNamesNoDuplicatesSql(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select :a, :a, :xy, :xy from TestLongs", **bindNames;
    uint32_t numBindNames, *bindNameLengths;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBindCount(stmt, &numBindNames) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    bindNames = malloc(sizeof(const char *) * numBindNames);
    bindNameLengths = malloc(sizeof(uint32_t) * numBindNames);
    if (!bindNames || !bindNameLengths)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    if (dpiStmt_getBindNames(stmt, &numBindNames, bindNames,
            bindNameLengths) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numBindNames, 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, bindNames[0],
            strlen(bindNames[0]), "A", 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, bindNames[1],
            strlen(bindNames[1]), "XY", 2) < 0)
        return DPI_FAILURE;
    free(bindNames);
    free(bindNameLengths);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1105_stmtInfoSelect()
//   Prepare any query; call dpiStmt_getInfo() and verify that the isQuery
// value in the dpiStmtInfo structure is set to 1 and all other values are set
// to zero and that the statementType value is set to DPI_STMT_TYPE_SELECT (no
// error).
//-----------------------------------------------------------------------------
int dpiTest_1105_stmtInfoSelect(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_SELECT);
}


//-----------------------------------------------------------------------------
// dpiTest_1106_stmtInfoBegin()
//   Prepare any anonymous PL/SQL block without any declaration section; call
// dpiStmt_getInfo() and verify that the isPLSQL value in the dpiStmtInfo
// structure is set to 1 and all other values are set to zero and that the
// statementType value is set to DPI_STMT_TYPE_BEGIN (no error).
//-----------------------------------------------------------------------------
int dpiTest_1106_stmtInfoBegin(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "begin NULL; end;";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_BEGIN);
}


//-----------------------------------------------------------------------------
// dpiTest_1107_stmtInfoDeclare()
//   Prepare any anonymous PL/SQL block with a declaration section; call
// dpiStmt_getInfo() and verify that the isPLSQL value in the dpiStmtInfo
// structure is set to 1 and all other values are set to zero and that the
// statementType value is set to DPI_STMT_TYPE_DECLARE (no error).
//-----------------------------------------------------------------------------
int dpiTest_1107_stmtInfoDeclare(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "declare t number; begin NULL; end;";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_DECLARE);
}


//-----------------------------------------------------------------------------
// dpiTest_1108_stmtInfoInsert()
//   Prepare any insert statement; call dpiStmt_getInfo() and verify that the
// isDML value in the dpiStmtInfo structure is set to 1 and all other values
// are set to zero and that the statementType value is set to
// DPI_STMT_TYPE_INSERT (no error).
//-----------------------------------------------------------------------------
int dpiTest_1108_stmtInfoInsert(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "insert into TestLongs values (:1, :2)";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_INSERT);
}


//-----------------------------------------------------------------------------
// dpiTest_1109_stmtInfoUpdate()
//   Prepare any insert statement; call dpiStmt_getInfo() and verify that the
// isDML value in the dpiStmtInfo structure is set to 1 and all other values
// are set to zero and that the statementType value is set to
// DPI_STMT_TYPE_UPDATE (no error).
//-----------------------------------------------------------------------------
int dpiTest_1109_stmtInfoUpdate(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "update TestLongs set longcol = :1 where intcol = :2";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_UPDATE);
}


//-----------------------------------------------------------------------------
// dpiTest_1110_stmtInfoDelete()
//   Prepare any insert statement; call dpiStmt_getInfo() and verify that the
// isDML value in the dpiStmtInfo structure is set to 1 and all other values
// are set to zero and that the statementType value is set to
// DPI_STMT_TYPE_DELETE (no error).
//-----------------------------------------------------------------------------
int dpiTest_1110_stmtInfoDelete(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "delete TestLongs";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_DELETE);
}


//-----------------------------------------------------------------------------
// dpiTest_1111_stmtInfoCreate()
//   Prepare any insert statement; call dpiStmt_getInfo() and verify that the
// isDDL value in the dpiStmtInfo structure is set to 1 and all other values
// are set to zero and that the statementType value is set to
// DPI_STMT_TYPE_CREATE (no error).
//-----------------------------------------------------------------------------
int dpiTest_1111_stmtInfoCreate(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "create table Test (IntCol number(9))";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_CREATE);
}


//-----------------------------------------------------------------------------
// dpiTest_1112_stmtInfoDrop()
//   Prepare any insert statement; call dpiStmt_getInfo() and verify that the
// isDDL value in the dpiStmtInfo structure is set to 1 and all other values
// are set to zero and that the statementType value is set to
// DPI_STMT_TYPE_DROP (no error).
//-----------------------------------------------------------------------------
int dpiTest_1112_stmtInfoDrop(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "drop table Test";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_DROP);
}


//-----------------------------------------------------------------------------
// dpiTest_1113_stmtInfoAlter()
//   Prepare any insert statement; call dpiStmt_getInfo() and verify that the
// isDDL value in the dpiStmtInfo structure is set to 1 and all other values
// are set to zero and that the statementType value is set to
// DPI_STMT_TYPE_ALTER (no error).
//-----------------------------------------------------------------------------
int dpiTest_1113_stmtInfoAlter(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "alter table Test add X number";
    dpiStmtInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getInfo(stmt, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, info.isQuery, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDDL, 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isDML, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.isReturning, 0) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_ALTER);
}


//-----------------------------------------------------------------------------
// dpiTest_1114_numQueryColumnsForQuery()
//   Prepare and execute any query; call dpiStmt_getNumQueryColumns() and
// verify that the value returned matches the number of columns expected to be
// returned by the query (no error).
//-----------------------------------------------------------------------------
int dpiTest_1114_numQueryColumnsForQuery(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    uint32_t numQueryColumns;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getNumQueryColumns(stmt, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, numQueryColumns, 2);
}


//-----------------------------------------------------------------------------
// dpiTest_1115_numQueryColumnsForNonQuery()
//   Prepare and execute any non-query; call dpiStmt_getNumQueryColumns() and
// verify that the value returned is 0 (no error).
//-----------------------------------------------------------------------------
int dpiTest_1115_numQueryColumnsForNonQuery(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "delete from TestLongs";
    uint32_t numQueryColumns;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getNumQueryColumns(stmt, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, numQueryColumns, 0);
}


//-----------------------------------------------------------------------------
// dpiTest_1116_queryInfoNonQuery()
//   Prepare and execute any non-query; call dpiStmt_getQueryInfo() for any
// non-zero position (error DPI-1028).
//-----------------------------------------------------------------------------
int dpiTest_1116_queryInfoNonQuery(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "delete from TestLongs";
    uint32_t numQueryColumns;
    dpiQueryInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getQueryInfo(stmt, 1, &info);
    if (dpiTestCase_expectError(testCase,
            "DPI-1028: query position 1 is invalid") < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1117_queryInfoMetadata()
//   Prepare and execute any query; call dpiStmt_getQueryInfo() for each of the
// columns and verify that the metadata returned is accurate (no error).
//-----------------------------------------------------------------------------
int dpiTest_1117_queryInfoMetadata(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *col1 = "INTCOL", *col2 = "STRINGCOL";
    const char *sql = "select * from TestTempTable";
    uint32_t numQueryColumns;
    dpiQueryInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // verify column 1
    if (dpiStmt_getQueryInfo(stmt, 1, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.name, info.nameLength,
            col1, strlen(col1)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.oracleTypeNum,
            DPI_ORACLE_TYPE_NUMBER) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            info.typeInfo.defaultNativeTypeNum, DPI_NATIVE_TYPE_INT64) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            info.typeInfo.dbSizeInBytes, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            info.typeInfo.clientSizeInBytes, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.precision, 9) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.scale, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.nullOk, 0) < 0)
        return DPI_FAILURE;

    // verify column 2
    if (dpiStmt_getQueryInfo(stmt, 2, &info) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, info.name, info.nameLength,
            col2, strlen(col2)) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.oracleTypeNum,
            DPI_ORACLE_TYPE_VARCHAR) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase,
            info.typeInfo.defaultNativeTypeNum, DPI_NATIVE_TYPE_BYTES) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.dbSizeInBytes,
            100) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.clientSizeInBytes,
            100) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.precision, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.typeInfo.scale, 0) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, info.nullOk, 1) < 0)
        return DPI_FAILURE;

    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1118_executeManyDefaultMode()
//   Prepare any DML statement; call dpiStmt_executeMany() with an array of
// data and mode set to DPI_MODE_EXEC_DEFAULT; call dpiStmt_getRowCounts()
// (error ORA-24349).
//-----------------------------------------------------------------------------
int dpiTest_1118_executeManyDefaultMode(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *insertSql = "insert into TestTempTable values (:1, :2)";
    const char *truncateSql = "truncate table TestTempTable";
    uint32_t numRows = 5, i, numQueryColumns, numRowCounts;
    dpiVersionInfo *versionInfo;
    dpiData *intData, *strData;
    dpiVar *intVar, *strVar;
    uint64_t *rowCounts;
    char buffer[100];
    dpiConn *conn;
    dpiStmt *stmt;

    // only supported in 12.1 and higher
    dpiTestSuite_getClientVersionInfo(&versionInfo);
    if (versionInfo->versionNum < 12)
        return DPI_SUCCESS;

    // truncate table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare and bind insert statement
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &intVar, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 1, 0, NULL, &strVar, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate some dummy data
    for (i = 0; i < numRows; i++) {
        intData[i].isNull = 0;
        intData[i].value.asInt64 = i + 1;
        sprintf(buffer, "Test data %d", i + 1);
        if (dpiVar_setFromBytes(strVar, i, buffer, strlen(buffer)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // perform execute many in default mode and attempt to get row counts
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getRowCounts(stmt, &numRowCounts, &rowCounts);
    if (dpiTestCase_expectError(testCase,
            "ORA-24349: Array DML row counts not available") < 0)
        return DPI_FAILURE;
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1119_executeManyArrayDmlRowcounts()
//   Prepare any DML statement; call dpiStmt_executeMany() with an array of
// data and mode set to DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS; call
// dpiStmt_getRowCounts() and verify that the row counts returned matches
// expectations; ensure that a value other than 1 is returned for at least one
// of the rowcounts (no error).
//-----------------------------------------------------------------------------
int dpiTest_1119_executeManyArrayDmlRowcounts(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *deleteSql = "delete from TestTempTable where IntCol < :1";
    const char *insertSql = "insert into TestTempTable values (:1, :2)";
    const char *truncateSql = "truncate table TestTempTable";
    uint32_t numRows = 5, i, numQueryColumns, numRowCounts;
    dpiVersionInfo *versionInfo;
    dpiData *intData, *strData;
    dpiVar *intVar, *strVar;
    uint64_t *rowCounts;
    char buffer[100];
    dpiConn *conn;
    dpiStmt *stmt;

    // only supported in 12.1 and higher
    dpiTestSuite_getClientVersionInfo(&versionInfo);
    if (versionInfo->versionNum < 12)
        return DPI_SUCCESS;

    // truncate table
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare and bind insert statement
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &intVar, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 1, 0, NULL, &strVar, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // populate some dummy data
    for (i = 0; i < numRows; i++) {
        intData[i].isNull = 0;
        intData[i].value.asInt64 = i + 1;
        sprintf(buffer, "Dummy data %d", i + 1);
        if (dpiVar_setFromBytes(strVar, i, buffer, strlen(buffer)) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // perform insert and verify all row counts are 1
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS,
            numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getRowCounts(stmt, &numRowCounts, &rowCounts) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numRowCounts, numRows) < 0)
        return DPI_FAILURE;
    for (i = 0; i < numRows; i++) {
        if (dpiTestCase_expectUintEqual(testCase, rowCounts[i], 1) < 0)
            return DPI_FAILURE;
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // perform delete and verify row counts are as expected
    numRows = 2;
    if (dpiConn_prepareStmt(conn, 0, deleteSql, strlen(deleteSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &intVar, &intData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    intData[0].isNull = 0;
    intData[0].value.asInt64 = 2;
    intData[1].isNull = 0;
    intData[1].value.asInt64 = 5;
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_ARRAY_DML_ROWCOUNTS,
            numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getRowCounts(stmt, &numRowCounts, &rowCounts) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numRowCounts, numRows) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, rowCounts[0], 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, rowCounts[1], 3) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1120_bindCountDuplicateBindsSql()
//   Prepare any statement with at least one duplicate bind variable repeated
// in sql, call dpiStmt_getBindCount() and confirm that the value returned is
// the value expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1120_bindCountDuplicateBindsSql(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select :1, :1 from TestLongs";
    uint32_t count;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBindCount(stmt, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, count, 2);
}


//-----------------------------------------------------------------------------
// dpiTest_1121_executeManyInvalidNumIters()
//   Prepare any non query with more than one bind variable; call
// dpiStmt_executeMany() with the parameter numIters set to a value that is
// greater than the maxArraySize for at least one of the variables that
// were bound to the statement (error DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_1121_executeManyInvalidNumIters(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "insert into TestLongs values (:1, :2)";
    dpiData *intdata, *strData;
    dpiVar *intVar, *strVar;
    uint32_t numIters = 4;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numIters, 0, 0, 0, NULL, &intVar, &intdata) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numIters - 1, 100, 1, 0, NULL, &strVar, &strData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 2, strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, numIters);
    if (dpiTestCase_expectError(testCase,
            "DPI-1018: array size of 3 is too small") < 0)
        return DPI_FAILURE;
    if (dpiVar_release(intVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(strVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1122_bindCountDuplicateBindsPlsql()
//   Prepare any plsql statement with at least one duplicate bind variables
// call dpiStmt_getBindCount() and confirm that the value returned is the value
// expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1122_bindCountDuplicateBindsPlsql(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin select :1, :1 from TestLongs; end;";
    uint32_t count;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBindCount(stmt, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, count, 1);
}


//-----------------------------------------------------------------------------
// dpiTest_1123_bindNamesNoDuplicatesPlsql()
//   Prepare any PL/SQL statement; call dpiStmt_getBindNames() and verify that
// the names of the bind variables match what is expected, with duplicates
// (bind variable name repeated in SQL text) (no error).
//-----------------------------------------------------------------------------
int dpiTest_1123_bindNamesNoDuplicatesPlsql(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin :c := :a1 * :a1 + :a2 * :a2; end;", **bindNames;
    uint32_t numBindNames, *bindNameLengths;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBindCount(stmt, &numBindNames) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    bindNames = malloc(sizeof(const char *) * numBindNames);
    bindNameLengths = malloc(sizeof(uint32_t) * numBindNames);
    if (!bindNames || !bindNameLengths)
        return dpiTestCase_setFailed(testCase, "Out of memory!");
    if (dpiStmt_getBindNames(stmt, &numBindNames, bindNames,
            bindNameLengths) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, numBindNames, 3) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, bindNames[0],
            bindNameLengths[0], "C", 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, bindNames[1],
            bindNameLengths[1], "A1", 2) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, bindNames[2],
            bindNameLengths[2], "A2", 2) < 0)
        return DPI_FAILURE;
    free(bindNames);
    free(bindNameLengths);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(1100);
    dpiTestSuite_addCase(dpiTest_1100_releaseTwice,
            "dpiStmt_release() twice");
    dpiTestSuite_addCase(dpiTest_1101_executeManyInvalidParams,
            "dpiStmt_executeMany() with invalid parameters");
    dpiTestSuite_addCase(dpiTest_1102_bindCountNoBinds,
            "dpiStmt_getBindCount() with no binds");
    dpiTestSuite_addCase(dpiTest_1103_bindCountOneBind,
            "dpiStmt_getBindCount() with one bind");
    dpiTestSuite_addCase(dpiTest_1104_bindNamesNoDuplicatesSql,
            "dpiStmt_getBindNames() strips duplicates (SQL)");
    dpiTestSuite_addCase(dpiTest_1105_stmtInfoSelect,
            "dpiStmt_getInfo() for select statement");
    dpiTestSuite_addCase(dpiTest_1106_stmtInfoBegin,
            "dpiStmt_getInfo() for PL/SQL block starting with BEGIN");
    dpiTestSuite_addCase(dpiTest_1107_stmtInfoDeclare,
            "dpiStmt_getInfo() for PL/SQL block starting with DECLARE");
    dpiTestSuite_addCase(dpiTest_1108_stmtInfoInsert,
            "dpiStmt_getInfo() for insert statement");
    dpiTestSuite_addCase(dpiTest_1109_stmtInfoUpdate,
            "dpiStmt_getInfo() for update statement");
    dpiTestSuite_addCase(dpiTest_1110_stmtInfoDelete,
            "dpiStmt_getInfo() for delete statement");
    dpiTestSuite_addCase(dpiTest_1111_stmtInfoCreate,
            "dpiStmt_getInfo() for create statement");
    dpiTestSuite_addCase(dpiTest_1112_stmtInfoDrop,
            "dpiStmt_getInfo() for drop statement");
    dpiTestSuite_addCase(dpiTest_1113_stmtInfoAlter,
            "dpiStmt_getInfo() for alter statement");
    dpiTestSuite_addCase(dpiTest_1114_numQueryColumnsForQuery,
            "dpiStmt_getNumQueryColumn() for query");
    dpiTestSuite_addCase(dpiTest_1115_numQueryColumnsForNonQuery,
            "dpiStmt_getNumQueryColumn() for non-query");
    dpiTestSuite_addCase(dpiTest_1116_queryInfoNonQuery,
            "dpiStmt_getQueryInfo() for non-query");
    dpiTestSuite_addCase(dpiTest_1117_queryInfoMetadata,
            "dpiStmt_getQueryInfo() for query");
    dpiTestSuite_addCase(dpiTest_1118_executeManyDefaultMode,
            "dpiStmt_executeMany() without array DML row counts mode");
    dpiTestSuite_addCase(dpiTest_1119_executeManyArrayDmlRowcounts,
            "dpiStmt_executeMany() with array DML row counts mode");
    dpiTestSuite_addCase(dpiTest_1120_bindCountDuplicateBindsSql,
            "dpiStmt_getBindCount() with duplicate binds (SQL)");
    dpiTestSuite_addCase(dpiTest_1121_executeManyInvalidNumIters,
            "dpiStmt_executeMany() with invalid number of iterations");
    dpiTestSuite_addCase(dpiTest_1122_bindCountDuplicateBindsPlsql,
            "dpiStmt_getBindCount() with duplicate binds (PL/SQL)");
    dpiTestSuite_addCase(dpiTest_1123_bindNamesNoDuplicatesPlsql,
            "dpiStmt_getBindNames() strips duplicates (PL/SQL)");
    return dpiTestSuite_run();
}

