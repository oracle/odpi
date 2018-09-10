//-----------------------------------------------------------------------------
// Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
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
// dpiTest__callFunctionsWithError() [INTERNAL]
//   Test all public functions with the specified statement and expect the
// given error for each of them.
//-----------------------------------------------------------------------------
int dpiTest__callFunctionsWithError(dpiTestCase *testCase, dpiStmt *stmt,
        const char *expectedError)
{
    dpiStmt_bindByName(stmt, NULL, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_bindByPos(stmt, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_bindValueByName(stmt, NULL, 0, DPI_NATIVE_TYPE_INT64, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_bindValueByPos(stmt, 0, DPI_NATIVE_TYPE_INT64, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_define(stmt, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_defineValue(stmt, 0, DPI_ORACLE_TYPE_NATIVE_INT,
            DPI_NATIVE_TYPE_INT64, 0, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_executeMany(stmt, DPI_MODE_EXEC_DEFAULT, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_fetch(stmt, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_fetchRows(stmt, 1, NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getBatchErrorCount(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getBatchErrors(stmt, 1, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getBindCount(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getBindNames(stmt, NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getFetchArraySize(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getImplicitResult(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getInfo(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getNumQueryColumns(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getQueryInfo(stmt, 1, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getQueryValue(stmt, 1, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getRowCount(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getRowCounts(stmt, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_getSubscrQueryId(stmt, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_scroll(stmt, DPI_MODE_FETCH_FIRST, 0, 0);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiStmt_setFetchArraySize(stmt, 1);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifyBindCount() [INTERNAL]
//   Determines the number of bind variables for the given statement and
// verifies that it matches the expected value.
//-----------------------------------------------------------------------------
int dpiTest__verifyBindCount(dpiTestCase *testCase, dpiConn *conn,
        const char *sql, uint32_t expectedCount)
{
    uint32_t count;
    dpiStmt *stmt;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBindCount(stmt, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, count, expectedCount);
}


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
// dpiTest_1102_verifyCallStmtWorksAsExp()
//   Prepare any CALL statement; call dpiStmt_getInfo() and verify that the
// isPLSQL value in the dpiStmtInfo structure is set to 1 and that the
// statementType value is set to DPI_STMT_TYPE_CALL (no error).
//-----------------------------------------------------------------------------
int dpiTest_1102_verifyCallStmtWorksAsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "call proc_TestNoArgs();";
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
    if (dpiTestCase_expectUintEqual(testCase, info.isPLSQL, 1) < 0)
        return DPI_FAILURE;
    return dpiTestCase_expectUintEqual(testCase, info.statementType,
            DPI_STMT_TYPE_CALL);
}


//-----------------------------------------------------------------------------
// dpiTest_1103_bindCountWithVarBinds()
//   Prepare multiple statements with differing numbers of bind variables; call
// dpiStmt_getBindCount() and confirm that the value returned is as expected,
// for both SQL and PL/SQL statements (no error).
//-----------------------------------------------------------------------------
int dpiTest_1103_bindCountWithVarBinds(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiConn *conn;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "select * from TestDataTypes", 0) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "select :1 from TestDataTypes", 1) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "select :1, :2, :3, :4, :5, :6, :7 from TestDataTypes", 7) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "select :1, :2, :3, :4, :5, :6, :7, :8 from TestDataTypes", 8) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "select :1, :2, :3, :4, :5, :6, :7, :8, :9, :9 from TestDataTypes",
            10) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "select :1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, "
            ":13, :14, :15, :16, :17 from TestDataTypes", 17) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "begin select :1, :1 from dual; end;", 1) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "begin select :1, :2, :3, :4, :5, :6, :7 from dual; end;", 7) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "begin select :1, :2, :3, :4, :5, :6, :7, :8, :8 from dual; end;",
            8) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "begin select :1, :2, :3, :4, :5, :6, :7, :8, :8, :9, :9, :10 "
            "from dual; end;", 10) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifyBindCount(testCase, conn,
            "begin select :1, :2, :3, :4, :5, :6, :7, :8, :8, :9, :10, "
            ":11, :12, :13, :14, :15, :16, :17 from dual; end;", 17) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
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
            bindNameLengths[0], "A", 1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, bindNames[1],
            bindNameLengths[1], "XY", 2) < 0)
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
//   Prepare any update statement; call dpiStmt_getInfo() and verify that the
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
//   Prepare any delete statement; call dpiStmt_getInfo() and verify that the
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
//   Prepare any create statement; call dpiStmt_getInfo() and verify that the
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
//   Prepare any drop statement; call dpiStmt_getInfo() and verify that the
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
//   Prepare any alter statement; call dpiStmt_getInfo() and verify that the
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
    dpiData *intData, *strData;
    dpiVar *intVar, *strVar;
    uint64_t *rowCounts;
    char buffer[100];
    dpiConn *conn;
    dpiStmt *stmt;

    // only supported in 12.1 and higher
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;

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
    dpiData *intData, *strData;
    dpiVar *intVar, *strVar;
    uint64_t *rowCounts;
    char buffer[100];
    dpiConn *conn;
    dpiStmt *stmt;

    // only supported in 12.1 and higher
    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 12, 1) < 0)
        return DPI_FAILURE;

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
// dpiTest_1124_closeStmtAndcallStmtPubFuncs()
//   Prepare any statement; call dpiStmt_close() and then call each public
// function for dpiStmt except for dpiStmt_addRef() and dpiStmt_release()
// (error DPI-1039).
//-----------------------------------------------------------------------------
int dpiTest_1124_closeStmtAndcallStmtPubFuncs(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestLongs";
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_close(stmt, NULL, 0) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTest__callFunctionsWithError(testCase, stmt,
            "DPI-1039: statement was already closed");
}


//-----------------------------------------------------------------------------
// dpiTest_1125_callStmtPubFuncsWithNull()
//   Call each of the public functions for dpiStmt with the stmt parameter
// set to NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_1125_callStmtPubFuncsWithNull(dpiTestCase *testCase,
        dpiTestParams *params)
{
    return dpiTest__callFunctionsWithError(testCase, NULL,
            "DPI-1002: invalid dpiStmt handle");
}


//-----------------------------------------------------------------------------
// dpiTest_1126_verifyGetBindNamesWithLesserValue()
//   Prepare any statement; call dpiStmt_getBindNames() with the parameter
// numBindNames set to a value less than the number of bind names that are
// expected (error DPI-1018).
//-----------------------------------------------------------------------------
int dpiTest_1126_verifyGetBindNamesWithLesserValue(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *expectedError = "DPI-1018: array size of 1 is too small";
    const char *sql = "select :a, :xy from TestLongs", *bindName;
    uint32_t numBindNames = 1, bindNameLength;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getBindNames(stmt, &numBindNames, &bindName, &bindNameLength);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1127_rebindVariablesAndVerify()
//   Prepare and execute any statement with bind variables; rebind one or more
// of the bind variables with different variables and verify that the results
// match what is expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1127_rebindVariablesAndVerify(dpiTestCase *testCase,
        dpiTestParams *params)
{
    dpiData *inVarData1, *inVarData2, *inVarData3, *outData;
    const char *sql = "select :1 + :2 from dual";
    dpiVar *inVar1, *inVar2, *inVar3;
    dpiNativeTypeNum nativeTypeNum;
    uint32_t bufferRowIndex;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_setFetchArraySize(stmt, 1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64, 1,
            0, 0, 0, NULL, &inVar1, &inVarData1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setUint64(inVarData1, 23);
    if (dpiStmt_bindByPos(stmt, 1, inVar1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64, 1,
            0, 0, 0, NULL, &inVar2, &inVarData2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setUint64(inVarData2, 33);
    if (dpiStmt_bindByPos(stmt, 2, inVar2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_UINT64, 1,
            0, 0, 0, NULL, &inVar3, &inVarData3) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setUint64(inVarData3, 130);
    if (dpiStmt_bindByPos(stmt, 1, inVar3) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_defineValue(stmt, 1, DPI_ORACLE_TYPE_NUMBER,
            DPI_NATIVE_TYPE_UINT64, 0, 0, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (!found)
        return dpiTestCase_setFailed(testCase, "No rows found!");
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, outData->value.asUint64,
            163) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(inVar3) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1128_verifyStoredProcWithBindVars()
//   Call any PL/SQL stored procedure and verify the arguments are passed
// correctly (no error).
//-----------------------------------------------------------------------------
int dpiTest_1128_verifyStoredProcWithBindVars(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin proc_Test(:1, :2, :3); end;";
    dpiData *inOutValue, *outValue, inValue;
    dpiVar *inOutVar, *outVar;
    uint32_t numQueryColumns;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&inValue, "Test String", strlen("Test String"));
    if (dpiStmt_bindValueByPos(stmt, 1, DPI_NATIVE_TYPE_BYTES, &inValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &inOutVar, &inOutValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(inOutValue, 10);
    if (dpiStmt_bindByPos(stmt, 2, inOutVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &outVar, &outValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 3, outVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, inOutValue->value.asInt64,
            110) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectUintEqual(testCase, outValue->value.asInt64, 11) < 0)
        return DPI_FAILURE;
    if (dpiVar_release(inOutVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_release(outVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1129_verifyBindVarWithBatchErrorsExp()
//   Prepare and execute any statement with at least 17 bind variables with
// batch errors enabled and some batch errors expected (no error).
//-----------------------------------------------------------------------------
int dpiTest_1129_verifyBindVarWithBatchErrorsExp(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *truncateSql = "truncate table TestDataTypes";
    const char *insertSql = "insert into TestDataTypes (StringCol, "
            "UnicodeCol, FixedCharCol, FixedUnicodeCol, RawCol, FloatCol, "
            "DoublePrecCol, IntCol, NumberCol, DateCol, TimestampCol, "
            "TimestampTZCol, TimestampLTZCol, IntervalDSCol, IntervalYMCol, "
            "BinaryFltCol, BinaryDoubleCol, LongCol, UnconstrainedCol) values "
            "(:1, :2, :3, :4, '12AB', :5, :6, :7, :8, :9, :10, :11, :12, :13,"
            " :14, :15, :16, 1.454, :17)";
    uint32_t numQueryColumns, numRows = 3, numCols = 17, numErrs = 3, i, count;
    dpiErrorInfo errorInfo[3];
    char expectedError[512];
    dpiData *colData[17];
    dpiVar *colVar[17];
    dpiStmt *stmt;
    dpiConn *conn;

    // get connection
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;

    // truncate table
    if (dpiConn_prepareStmt(conn, 0, truncateSql, strlen(truncateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create variables
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 0, 0, NULL, &colVar[0], &colData[0]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 0, 0, NULL, &colVar[1], &colData[1]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 0, 0, NULL, &colVar[2], &colData[2]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            numRows, 100, 0, 0, NULL, &colVar[3], &colData[3]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE,
            numRows, 0, 0, 0, NULL, &colVar[4], &colData[4]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE,
            numRows, 0, 0, 0, NULL, &colVar[5], &colData[5]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64,
            numRows, 0, 0, 0, NULL, &colVar[6], &colData[6]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE,
            numRows, 0, 0, 0, NULL, &colVar[7], &colData[7]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_DATE, DPI_NATIVE_TYPE_TIMESTAMP,
            numRows, 0, 0, 0, NULL, &colVar[8], &colData[8]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP,
            DPI_NATIVE_TYPE_TIMESTAMP, numRows, 0, 0, 0, NULL, &colVar[9],
            &colData[9]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP_TZ,
            DPI_NATIVE_TYPE_TIMESTAMP, numRows, 0, 0, 0, NULL, &colVar[10],
            &colData[10]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_TIMESTAMP_LTZ,
            DPI_NATIVE_TYPE_TIMESTAMP, numRows, 0, 0, 0, NULL, &colVar[11],
            &colData[11]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_INTERVAL_DS,
            DPI_NATIVE_TYPE_INTERVAL_DS, numRows, 0, 0, 0, NULL, &colVar[12],
            &colData[12]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_INTERVAL_YM,
            DPI_NATIVE_TYPE_INTERVAL_YM, numRows, 0, 0, 0, NULL, &colVar[13],
            &colData[13]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_FLOAT,
            DPI_NATIVE_TYPE_FLOAT, numRows, 0, 0, 0, NULL, &colVar[14],
            &colData[14]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NATIVE_DOUBLE,
            DPI_NATIVE_TYPE_DOUBLE, numRows, 0, 0, 0, NULL, &colVar[15],
            &colData[15]) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_DOUBLE,
            numRows, 0, 0, 0, NULL, &colVar[16], &colData[16]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // prepare statement and perform binds
    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql),
            NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    for (i = 0; i < numCols; i++) {
        if (dpiStmt_bindByPos(stmt, i + 1, colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // populate data
    for (i = 0; i < numRows; i++) {
        dpiData_setBytes(&colData[0][i], "string", strlen("string"));
        dpiData_setBytes(&colData[1][i], "unistring", strlen("unistring"));
        dpiData_setBytes(&colData[2][i], "fixedchar", strlen("fixedchar"));
        dpiData_setBytes(&colData[3][i], "fixedunichar",
                strlen("fixedunichar"));
        dpiData_setDouble(&colData[4][i], 1.25);
        dpiData_setDouble(&colData[5][i], 1.44);
        if (i < 2)
            dpiData_setInt64(&colData[6][i], 1);
        else dpiData_setInt64(&colData[6][i], 12345678910);
        dpiData_setDouble(&colData[7][i], 1.35);
        dpiData_setTimestamp(&colData[8][i], 2017, 6, 1, 2, 2, 1, 0, 0, 0);
        dpiData_setTimestamp(&colData[9][i], 2017, 6, 1, 2, 2, 1, 0, 0, 0);
        dpiData_setTimestamp(&colData[10][i], 2017, 6, 1, 2, 2, 1, 0, 0, 0);
        dpiData_setTimestamp(&colData[11][i], 2017, 6, 1, 2, 2, 1, 0, 0, 0);
        dpiData_setIntervalDS(&colData[12][i], 3, 2, 1, 1, 0);
        dpiData_setIntervalYM(&colData[13][i], 1, 1);
        dpiData_setDouble(&colData[14][i], 1.34);
        dpiData_setDouble(&colData[15][i], 1.95);
        dpiData_setDouble(&colData[16][i], 999);
    }

    // attempt to insert and validate errors received match expected errors
    if (dpiStmt_executeMany(stmt, DPI_MODE_EXEC_BATCH_ERRORS, numRows) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBatchErrorCount(stmt, &count) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getBatchErrors(stmt, numErrs, errorInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    snprintf(expectedError, sizeof(expectedError),
            "ORA-00001: unique constraint (%.*s.TESTDATATYPES_PK) violated",
            params->mainUserNameLength, params->mainUserName);
    if (dpiTestCase_expectStringEqual(testCase, errorInfo[0].message,
            errorInfo[0].messageLength, expectedError,
            strlen(expectedError)) < 0)
        return DPI_FAILURE;
    strcpy(expectedError, "ORA-01438: value larger than specified precision "
            "allowed for this column");
    if (dpiTestCase_expectStringEqual(testCase, errorInfo[1].message,
            errorInfo[1].messageLength, expectedError,
            strlen(expectedError)) < 0)
        return DPI_FAILURE;

    // cleanup
    for (i = 0; i < numCols; i++) {
        if (dpiVar_release(colVar[i]) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1130_fetch1000ColsAndVerify()
//   Prepare and execute a query that fetches 1000 columns (no error)
//-----------------------------------------------------------------------------
int dpiTest_1130_fetch1000ColsAndVerify(dpiTestCase *testCase,
        dpiTestParams *params)
{
    uint32_t numQueryColumns;
    char sql[4000], *ptr;
    dpiConn *conn;
    dpiStmt *stmt;
    int i;

    // generate SQL statement
    strcpy(sql, "select ");
    ptr = sql + strlen(sql);
    for (i = 0; i < 1000; i++) {
        if (i > 0)
            *ptr++ = ',';
        ptr += sprintf(ptr, "%d", i + 1);
    }
    strcpy(ptr, " from dual");

    // execute it and verify the number of columns returned is as expected
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    return dpiTestCase_expectUintEqual(testCase, numQueryColumns, 1000);
}


//-----------------------------------------------------------------------------
// dpiTest_1131_stmtInfoMerge()
//   Prepare any merge statement; call dpiStmt_getInfo() and verify that the
// isDML value in the dpiStmtInfo structure is set to 1 and all other values
// are set to zero and that the statementType value is set to
// DPI_STMT_TYPE_MERGE (no error).
//-----------------------------------------------------------------------------
int dpiTest_1131_stmtInfoMerge(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *sql = "merge into t1 a using t1 b on (a.intcol = 1) "
                      "when matched then update set a.longcol = 1 "
                      "when not matched then insert values(1,2)";
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
            DPI_STMT_TYPE_MERGE);
}


//-----------------------------------------------------------------------------
// dpiTest_1132_verifyStoredFuncWithBindVars()
//   Call any PL/SQL stored function and verify the arguments are passed
// correctly and the return value is valid (no error).
//-----------------------------------------------------------------------------
int dpiTest_1132_verifyStoredFuncWithBindVars(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "begin :1 := func_Test(:2, :3); end;";
    char *testStr = "Test String";
    dpiData *retValue, inputData;
    uint32_t numQueryColumns;
    dpiVar *retVar;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setBytes(&inputData, testStr, strlen(testStr));
    if (dpiStmt_bindValueByPos(stmt, 2, DPI_NATIVE_TYPE_BYTES, &inputData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiData_setInt64(&inputData, 10);
    if (dpiStmt_bindValueByPos(stmt, 3, DPI_NATIVE_TYPE_INT64, &inputData) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_NUMBER, DPI_NATIVE_TYPE_INT64, 1,
            0, 0, 0, NULL, &retVar, &retValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, retVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, 0, &numQueryColumns) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectUintEqual(testCase, retValue->value.asInt64,
            strlen(testStr)+10) < 0)
        return DPI_FAILURE;
    if (dpiVar_release(retVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_1133_verifyQueryInfoReturnsNoMetaData()
//   Prepare and execute any query with mode set to DPI_MODE_EXEC_PARSE_ONLY.
// Call dpiStmt_getQueryInfo() and verify it does not return any metadata
// (error ORA-24338).
//-----------------------------------------------------------------------------
int dpiTest_1133_verifyQueryInfoReturnsNoMetaData(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *sql = "select * from TestTempTable";
    dpiQueryInfo info;
    dpiConn *conn;
    dpiStmt *stmt;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_PARSE_ONLY, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiStmt_getQueryInfo(stmt, 1, &info);
    if (dpiTestCase_expectError(testCase,
            "ORA-24338: statement handle not executed") < 0)
        return DPI_FAILURE;
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
    dpiTestSuite_addCase(dpiTest_1102_verifyCallStmtWorksAsExp,
            "dpiStmt_getInfo() for call statement");
    dpiTestSuite_addCase(dpiTest_1103_bindCountWithVarBinds,
            "dpiStmt_getBindCount() with variable binds");
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
    dpiTestSuite_addCase(dpiTest_1124_closeStmtAndcallStmtPubFuncs,
            "dpiStmt_close and call public functions");
    dpiTestSuite_addCase(dpiTest_1125_callStmtPubFuncsWithNull,
            "call pub functions with stmt set to NULL");
    dpiTestSuite_addCase(dpiTest_1126_verifyGetBindNamesWithLesserValue,
            "dpiStmt_getBindNames() with numBindNames set to less value");
    dpiTestSuite_addCase(dpiTest_1127_rebindVariablesAndVerify,
            "rebind the variable and verify");
    dpiTestSuite_addCase(dpiTest_1128_verifyStoredProcWithBindVars,
            "call PL/SQL procedure and verify the args are passed properly");
    dpiTestSuite_addCase(dpiTest_1129_verifyBindVarWithBatchErrorsExp,
            "bind many variables with batch errors enabled and verify");
    dpiTestSuite_addCase(dpiTest_1130_fetch1000ColsAndVerify,
            "execute query that fetches 1000 columns");
    dpiTestSuite_addCase(dpiTest_1131_stmtInfoMerge,
            "dpiStmt_getInfo() for merge statement");
    dpiTestSuite_addCase(dpiTest_1132_verifyStoredFuncWithBindVars,
            "call PL/SQL function & verify the args are passed properly");
    dpiTestSuite_addCase(dpiTest_1133_verifyQueryInfoReturnsNoMetaData,
            "verify getQueryInfo returns no metadata if mode is parse only");
    return dpiTestSuite_run();
}

