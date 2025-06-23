//-----------------------------------------------------------------------------
// Copyright (c) 2025, Oracle and/or its affiliates.
//
// This software is dual-licensed to you under the Universal Permissive License
// (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
// 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
// either license.
//
// If you elect to accept the software under the Apache License, Version 2.0,
// the following applies:
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// test_4400_json.c
//   Test suite for all the Vector related test cases.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__compareVectors()
//   Compare two vectors to ensure they have the same values.
//-----------------------------------------------------------------------------
int dpiTest__compareVectors(dpiTestCase *testCase, dpiVectorInfo *vector1,
        dpiVectorInfo *vector2)
{
    uint32_t i;
    int status;

    // ensure the number of dimensions matches first
    if (vector1->numDimensions != vector2->numDimensions) {
        return dpiTestCase_setFailed(testCase,
                "vector dimension counts do not match");
    }

    // compare arrays
    for (i = 0; i < vector1->numDimensions; i++) {
        switch(vector1->format) {
            case DPI_VECTOR_FORMAT_INT8:
                if (vector2->format == DPI_VECTOR_FORMAT_INT8) {
                    status = dpiTestCase_expectIntEqual(testCase,
                            vector1->dimensions.asInt8[i],
                            vector2->dimensions.asInt8[i]);
                } else if (vector2->format == DPI_VECTOR_FORMAT_FLOAT32) {
                    status = dpiTestCase_expectIntEqual(testCase,
                            vector1->dimensions.asInt8[i],
                            vector2->dimensions.asFloat[i]);
                } else {
                    status = dpiTestCase_expectIntEqual(testCase,
                            vector1->dimensions.asInt8[i],
                            vector2->dimensions.asDouble[i]);
                }
                break;
            case DPI_VECTOR_FORMAT_FLOAT32:
                if (vector2->format == DPI_VECTOR_FORMAT_INT8) {
                    status = dpiTestCase_expectDoubleEqual(testCase,
                            vector1->dimensions.asFloat[i],
                            vector2->dimensions.asInt8[i]);
                } else if (vector2->format == DPI_VECTOR_FORMAT_FLOAT32) {
                    status = dpiTestCase_expectDoubleEqual(testCase,
                            vector1->dimensions.asFloat[i],
                            vector2->dimensions.asFloat[i]);
                } else {
                    status = dpiTestCase_expectDoubleEqual(testCase,
                            vector1->dimensions.asFloat[i],
                            vector2->dimensions.asDouble[i]);
                }
                break;
            case DPI_VECTOR_FORMAT_FLOAT64:
                if (vector2->format == DPI_VECTOR_FORMAT_INT8) {
                    status = dpiTestCase_expectDoubleEqual(testCase,
                            vector1->dimensions.asDouble[i],
                            vector2->dimensions.asInt8[i]);
                } else if (vector2->format == DPI_VECTOR_FORMAT_FLOAT32) {
                    status = dpiTestCase_expectDoubleEqual(testCase,
                            vector1->dimensions.asDouble[i],
                            vector2->dimensions.asFloat[i]);
                } else {
                    status = dpiTestCase_expectDoubleEqual(testCase,
                            vector1->dimensions.asDouble[i],
                            vector2->dimensions.asDouble[i]);
                }
                break;
        }
        if (status < 0)
            return DPI_FAILURE;
    }

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__deleteVectorTable()
//   Delete the vector table.
//-----------------------------------------------------------------------------
int dpiTest__deleteVectorTable(dpiTestCase *testCase, dpiConn *conn)
{
    const char *sql = "delete TestVectors";
    dpiStmt *stmt;

    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__getVectorInfo()
//   Populate the vector info structure.
//-----------------------------------------------------------------------------
void dpiTest__getVectorInfo(dpiVectorInfo *info, uint8_t format,
        uint32_t numDimensions, void *values)
{
    memset(info, 0, sizeof(dpiVectorInfo));
    info->format = format;
    info->numDimensions = numDimensions;
    info->dimensions.asPtr = values;
}


//-----------------------------------------------------------------------------
// dpiTest__populateTable()
//   Insert data into the table.
//-----------------------------------------------------------------------------
int dpiTest__populateTable(dpiTestCase *testCase, const char *columnName,
        dpiVectorInfo *vectorInfo, const char *errExp)
{
    dpiVector *vec;
    char sql[200];
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 4) < 0)
        return DPI_FAILURE;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__deleteVectorTable(testCase, conn) < 0)
        return DPI_FAILURE;

    if (dpiConn_newVector(conn, vectorInfo, &vec) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VECTOR, DPI_NATIVE_TYPE_VECTOR,
            1, 0, 0, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromVector(inVar, 0, vec) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    snprintf(sql, sizeof(sql),
            "insert into TestVectors (IntCol, %s) values(1, :1)", columnName);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_COMMIT_ON_SUCCESS, NULL) < 0) {
        if (errExp) {
            if (dpiTestCase_expectError(testCase, errExp) < 0)
                return DPI_FAILURE;
        } else {
            return dpiTestCase_setFailedFromError(testCase);
        }
    }

    // cleanup
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVector_release(vec) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__populateTableViaType()
//   Insert data into the table via a type other than native vector.
//-----------------------------------------------------------------------------
int dpiTest__populateTableViaType(dpiTestCase *testCase,
        const char *columnName, const char *vector, dpiOracleTypeNum oraType,
        dpiNativeTypeNum nativeType, const char *errExp)
{
    char sql[200];
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 4) < 0)
        return DPI_FAILURE;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__deleteVectorTable(testCase, conn) < 0)
        return DPI_FAILURE;

    if (dpiConn_newVar(conn, oraType, nativeType, 1, 100, 1, 0, NULL, &inVar,
            &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    snprintf(sql, sizeof(sql),
            "insert into TestVectors (IntCol, %s) values(1, :1)", columnName);
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromBytes(inVar, 0, vector, strlen(vector)) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_COMMIT_ON_SUCCESS, NULL) < 0) {
        if (errExp) {
            if (dpiTestCase_expectError(testCase, errExp) < 0)
                return DPI_FAILURE;
        } else {
            return dpiTestCase_setFailedFromError(testCase);
        }
    }

    // cleanup
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest__verifySelect()
//   Select vector column from the table and verify the values are as expected.
//-----------------------------------------------------------------------------
int dpiTest__verifySelect(dpiTestCase *testCase, const char *sql,
        dpiVectorInfo *inVectorInfo)
{
    dpiNativeTypeNum nativeTypeNum;
    dpiVectorInfo outVectorInfo;
    uint32_t bufferRowIndex;
    dpiData *outValue;
    dpiConn *conn;
    dpiStmt *stmt;
    int found;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 4) < 0)
        return DPI_FAILURE;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_prepareStmt(conn, 0, sql, strlen(sql), NULL, 0, &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_DEFAULT, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &outValue) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVector_getValue(outValue->value.asVector, &outVectorInfo) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__compareVectors(testCase, inVectorInfo, &outVectorInfo) < 0)
        return DPI_FAILURE;
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4400()
//   Call each of the dpiVector public functions with vector parameter set to
// NULL (error DPI-1002).
//-----------------------------------------------------------------------------
int dpiTest_4400(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 4) < 0)
        return DPI_FAILURE;
    dpiVector_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiVector_getValue(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiVector_setValue(NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiVar_setFromVector(NULL, 0, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiConn_newVector(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiVector_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4401()
//   Insert and fetch from int8 vector column and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4401(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector8Col from TestVectors";
    int8_t values[] = {-128, -10, 0, 40, 50, 60, 70, 80, 90, 127};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 10, values);
    if (dpiTest__populateTable(testCase, "Vector8Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4402()
//   Insert and fetch from float32 vector column and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4402(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector32Col from TestVectors";
    float values[] = {-99.999, -104.1, -0.1234, 1.9, 222.222, 5.5, 0.0099,
                       999.999, 12345.1, 333333.333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 10, values);
    if (dpiTest__populateTable(testCase, "Vector32Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4403()
//   Insert and fetch from float64 vector column and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4403(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector64Col from TestVectors";
    double values[] = {-999.999, -104.1, -0.1234, 1.9, 222.222, 5.5, 0.0099,
                        999.999, 12345.1, 3333333.33333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 10, values);
    if (dpiTest__populateTable(testCase, "Vector64Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4404()
//   Insert and fetch from flex vector column as int8 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4404(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlex8Col from TestVectors";
    int8_t values[] = {-128, -10, 0, 40, 50, 60, 90, 127};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 10, values);
    if (dpiTest__populateTable(testCase, "VectorFlex8Col", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4405()
//   Insert and fetch from flex vector column as float32 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4405(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlex32Col from TestVectors";
    float values[] = {-99.999, -104.1, -0.1234, 1.9, 222.222, 0.0099,
                       999.999, 12345.1, 333333.333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 10, values);
    if (dpiTest__populateTable(testCase, "VectorFlex32Col", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4406()
//   Insert and fetch from flex vector column as float64 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4406(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlex64Col from TestVectors";
    double values[] = {-999.999, -104.1, -0.1234, 222.222, 5.5, 0.0099,
                        999.999, 3333333.33333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 8, values);
    if (dpiTest__populateTable(testCase, "VectorFlex64Col", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4407()
//   Insert and fetch from flex type column as int8 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4407(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexTypeCol from TestVectors";
    int8_t values[] = {-128, 127};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 2, values);
    if (dpiTest__populateTable(testCase, "VectorFlexTypeCol", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4408()
//   Insert and fetch from flex type column as float32 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4408(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexTypeCol from TestVectors";
    float values[] = {-99.999, 333333.333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 2, values);
    if (dpiTest__populateTable(testCase, "VectorFlexTypeCol", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4409()
//   Insert and fetch from flex type column as float64 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4409(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexTypeCol from TestVectors";
    double values[] = {-99999.999, 3333333.33333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 2, values);
    if (dpiTest__populateTable(testCase, "VectorFlexTypeCol", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4410()
//   Insert and fetch from flex all column as int8 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4410(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexAllCol from TestVectors";
    int8_t values[] = {-128, 127, 0};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 3, values);
    if (dpiTest__populateTable(testCase, "VectorFlexAllCol", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4411()
//   Insert and fetch from flex all column as float32 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4411(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexAllCol from TestVectors";
    float values[] = {-99.999, 3333.333, 1.0, -5.9};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 4, values);
    if (dpiTest__populateTable(testCase, "VectorFlexAllCol", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4412()
//   Insert and fetch from flex all column as float64 and verify the values.
//-----------------------------------------------------------------------------
int dpiTest_4412(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexAllCol from TestVectors";
    double values[] = {-99999.999, 3333333.33333, 0.0, 1.333, -555.555};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 5, values);
    if (dpiTest__populateTable(testCase, "VectorFlexAllCol", &vectorInfo,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4413()
//   Verify int8 vector column with wrong dimension count (error ORA-51803).
//-----------------------------------------------------------------------------
int dpiTest_4413(dpiTestCase *testCase, dpiTestParams *params)
{
    int8_t values[] = {-128, -10, 0, 40, 50, 60, 70, 80, 90, 127};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 9, values);
    if (dpiTest__populateTable(testCase, "Vector8Col", &vectorInfo,
            "ORA-51803:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4414()
//   Verify float32 vector column with wrong dimension count (error ORA-51803).
//-----------------------------------------------------------------------------
int dpiTest_4414(dpiTestCase *testCase, dpiTestParams *params)
{
    float values[] = {-99.999, -104.1, -0.1234, 1.9, 222.222, 5.5, 0.0099,
                       999.999, 12345.1};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 9, values);
    if (dpiTest__populateTable(testCase, "Vector32Col", &vectorInfo,
            "ORA-51803:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4415()
//   Verify float64 vector column with wrong dimension count (error ORA-51803).
//-----------------------------------------------------------------------------
int dpiTest_4415(dpiTestCase *testCase, dpiTestParams *params)
{
    double values[] = {-999.999, -104.1, -0.1234, 1.9, 222.222, 5.5, 0.0099,
                        999.999, 12345.1, 3333333.33333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 9, values);
    if (dpiTest__populateTable(testCase, "Vector64Col", &vectorInfo,
            "ORA-51803:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4416()
//   Insert int8 vector into float32 column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4416(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector32Col from TestVectors";
    int8_t values[] = {28, -10, 0, 40, 50, 60, 70, 80, 90, 127};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 10, values);
    if (dpiTest__populateTable(testCase, "Vector32Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4417()
//   Insert Int8 Vector into Float64 column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4417(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector64Col from TestVectors";
    int8_t values[] = {28, -10, 0, 40, 50, 60, 70, 80, 90, 127};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 10, values);
    if (dpiTest__populateTable(testCase, "Vector64Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4418()
//   Insert float32 Vector into Int8 column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4418(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector8Col from TestVectors";
    float values[] = {-99, -104, 0, 1, 2, 5, -99, 9, 12, 33};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 10, values);
    if (dpiTest__populateTable(testCase, "Vector8Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4419()
//   Insert float32 Vector into float64 column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4419(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector64Col from TestVectors";
    float values[] = {-99.9, -104.1, -0.1234, 1.9, 222.222, 5.5, 0.0099,
                       999.999, 12345.1, 333333.333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 10, values);
    if (dpiTest__populateTable(testCase, "Vector64Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4420()
//   Insert float64 Vector into Int8 column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4420(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector8Col from TestVectors";
    double values[] = {-99, -104, 0, 1, 2, 5, -99, 9, 12, 33};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 10, values);
    if (dpiTest__populateTable(testCase, "Vector8Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4421()
//   Insert float64 Vector into float32 column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4421(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select Vector32Col from TestVectors";
    double values[] = {-91, -104, -0, 1, 2, 5, 0.0,
                          93333, 1112, 333333};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 10, values);
    if (dpiTest__populateTable(testCase, "Vector32Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4422()
//   Insert varchar into Vector column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4422(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexAllCol from TestVectors";
    char *insVector = "[1, 2, 3]";
    int8_t values[] = {1, 2, 3};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 3, values);
    if (dpiTest__populateTableViaType(testCase, "VectorFlexAllCol",
            insVector, DPI_ORACLE_TYPE_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4423()
//   Insert char into Vector column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4423(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlex32Col from TestVectors";
    const char *insVector = "[-99991.1111111, -22.33, 333.999]";
    float values[] = {-99991.1111111, -22.33, 333.999};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 3, values);
    if (dpiTest__populateTableViaType(testCase, "VectorFlex32Col",
            insVector, DPI_ORACLE_TYPE_CHAR, DPI_NATIVE_TYPE_BYTES, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4424()
//   Insert nchar into Vector column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4424(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlex8Col from TestVectors";
    const char *insVector = "[-1, 0, 127]";
    int8_t values[] = {-1, 0, 127};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 3, values);
    if (dpiTest__populateTableViaType(testCase, "VectorFlex8Col",
            insVector, DPI_ORACLE_TYPE_NCHAR, DPI_NATIVE_TYPE_BYTES, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4425()
//   Insert nvarchar into Vector column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4425(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlex64Col from TestVectors";
    const char *insVector = "[-0.123456789101112, 0.123456789101112,"
            "-1234567.12345678, 9999999.99999999]";
    double values[] = {-0.123456789101112, 0.123456789101112,
            -1234567.12345678, 9999999.99999999};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT64, 4, values);
    if (dpiTest__populateTableViaType(testCase, "VectorFlex64Col",
            insVector, DPI_ORACLE_TYPE_NVARCHAR, DPI_NATIVE_TYPE_BYTES,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4426()
//   Insert raw into Vector column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4426(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexAllCol from TestVectors";
    const char *insVector = "[-99991.1111111, -22.33, 333.999]";
    float values[] = {-99991.1111111, -22.33, 333.999};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 3, values);
    if (dpiTest__populateTableViaType(testCase, "VectorFlexAllCol",
            insVector, DPI_ORACLE_TYPE_LONG_VARCHAR, DPI_NATIVE_TYPE_BYTES,
            NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4427()
//   Insert unsupported datatype (RAW) into Vector column and verify
// (error ORA-00932:).
//-----------------------------------------------------------------------------
int dpiTest_4427(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *insVector = "[-99991.1111111, -22.33, 333.999]";

    if (dpiTest__populateTableViaType(testCase, "VectorFlexAllCol",
            insVector, DPI_ORACLE_TYPE_RAW, DPI_NATIVE_TYPE_BYTES,
            "ORA-00932:") < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4428()
//   Insert CLOB into Vector column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4428(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlexAllCol from TestVectors";
    const char *insVector = "[-99991.1111111, -22.33, 333.999]";
    float values[] = {-99991.1111111, -22.33, 333.999};
    dpiVectorInfo vectorInfo;

    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 3, values);
    if (dpiTest__populateTableViaType(testCase, "VectorFlexAllCol",
            insVector, DPI_ORACLE_TYPE_CLOB, DPI_NATIVE_TYPE_LOB, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4429()
//   Insert Json array into Vector column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4429(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *selectSql = "select VectorFlex32Col from TestVectors";
    const char *insertSql =
            "insert into TestVectors (IntCol, VectorFlex32Col) values(1, :1)";
    float values[] = {-0.12345, -99991.11111, -22.33, 333.999};
    dpiDataBuffer inNodeData[5];
    uint8_t i, numElements = 4;
    dpiVectorInfo vectorInfo;
    dpiJsonNode inNodes[5];
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;

    if (dpiTestCase_setSkippedIfVersionTooOld(testCase, 0, 23, 4) < 0)
        return DPI_FAILURE;

    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiTest__deleteVectorTable(testCase, conn) < 0)
        return DPI_FAILURE;

    // prepare node structure
    memset(inNodes, 0, sizeof(inNodes));
    memset(inNodeData, 0, sizeof(inNodeData));
    for (i = 0; i < numElements; i++) {
        inNodes[i].value = &inNodeData[i];
        inNodes[i].oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
        inNodes[i].nativeTypeNum = DPI_NATIVE_TYPE_DOUBLE;
        inNodeData[i].asDouble = values[i];
    }
    inNodes[numElements].value = &inNodeData[numElements];
    inNodes[numElements].oracleTypeNum = DPI_ORACLE_TYPE_JSON_ARRAY;
    inNodes[numElements].nativeTypeNum = DPI_NATIVE_TYPE_JSON_ARRAY;
    inNodeData[numElements].asJsonArray.numElements = numElements;
    inNodeData[numElements].asJsonArray.elements = &inNodes[0];
    inNodeData[numElements].asJsonArray.elementValues = &inNodeData[0];

    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_JSON, DPI_NATIVE_TYPE_JSON,
            1, 100, 1, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    data->isNull = 0;
    if (dpiJson_setValue(dpiData_getJson(data), &inNodes[numElements]) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    if (dpiConn_prepareStmt(conn, 0, insertSql, strlen(insertSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_COMMIT_ON_SUCCESS, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // query and validate the data
    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_FLOAT32, 4, values);
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_4430()
//   Update Vector Int8 column and verify.
//-----------------------------------------------------------------------------
int dpiTest_4430(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *updateSql =
            "update TestVectors set Vector8Col = :1 where IntCol = 1";
    const char *selectSql = "select Vector8Col from TestVectors";
    int8_t values[] = {28, -10, 0, 40, 50, 60, 70, 80, 90, 127};
    int8_t updValues[] = {-128, -10, -5, -1, 0, 10, 5, 100, 1, 2};
    dpiVectorInfo vectorInfo;
    dpiVector *vec;
    dpiData *data;
    dpiConn *conn;
    dpiStmt *stmt;
    dpiVar *inVar;

    // insert and verify inserted vector is correct
    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 10, values);
    if (dpiTest__populateTable(testCase, "Vector8Col", &vectorInfo, NULL) < 0)
        return DPI_FAILURE;
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    // update table and verify update is correct
    dpiTest__getVectorInfo(&vectorInfo, DPI_VECTOR_FORMAT_INT8, 10, updValues);
    if (dpiTestCase_getConnection(testCase, &conn) < 0)
        return DPI_FAILURE;
    if (dpiConn_newVector(conn, &vectorInfo, &vec) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_newVar(conn, DPI_ORACLE_TYPE_VECTOR, DPI_NATIVE_TYPE_VECTOR,
            1, 0, 0, 0, NULL, &inVar, &data) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVar_setFromVector(inVar, 0, vec) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_prepareStmt(conn, 0, updateSql, strlen(updateSql), NULL, 0,
            &stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_bindByPos(stmt, 1, inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_execute(stmt, DPI_MODE_EXEC_COMMIT_ON_SUCCESS, NULL) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTest__verifySelect(testCase, selectSql, &vectorInfo) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiVar_release(inVar) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiStmt_release(stmt) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiVector_release(vec) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(4400);
    dpiTestSuite_addCase(dpiTest_4400,
            "call public functions with Vector set to NULL");
    dpiTestSuite_addCase(dpiTest_4401,
            "insert and fetch int8 vector col");
    dpiTestSuite_addCase(dpiTest_4402,
            "insert and fetch float32 vector col");
    dpiTestSuite_addCase(dpiTest_4403,
            "insert and fetch float64 vector col");
    dpiTestSuite_addCase(dpiTest_4404,
            "insert and fetch flex int8 vector col");
    dpiTestSuite_addCase(dpiTest_4405,
            "insert and fetch flex float32 vector col");
    dpiTestSuite_addCase(dpiTest_4406,
            "insert and fetch flex float64 vector col");
    dpiTestSuite_addCase(dpiTest_4407,
            "insert and fetch flex type (int8) vector col");
    dpiTestSuite_addCase(dpiTest_4408,
            "insert and fetch flex type (float32) vector col");
    dpiTestSuite_addCase(dpiTest_4409,
            "insert and fetch flex type (float64) vector col");
    dpiTestSuite_addCase(dpiTest_4410,
            "insert and fetch flex All (int8) vector col");
    dpiTestSuite_addCase(dpiTest_4411,
            "insert and fetch flex All (float32) vector col");
    dpiTestSuite_addCase(dpiTest_4412,
            "insert and fetch flex All (float64) vector col");
    dpiTestSuite_addCase(dpiTest_4413,
            "verify int8 vector col num dimensions");
    dpiTestSuite_addCase(dpiTest_4414,
            "verify float32 vector col num dimensions");
    dpiTestSuite_addCase(dpiTest_4415,
            "verify float64 vector col num dimensions");
    dpiTestSuite_addCase(dpiTest_4416,
            "insert int8 vector into float32 col");
    dpiTestSuite_addCase(dpiTest_4417,
            "insert int8 vector into float64 col");
    dpiTestSuite_addCase(dpiTest_4418,
            "insert float32 vector into int8 col");
    dpiTestSuite_addCase(dpiTest_4419,
            "insert float32 vector into float64 col");
    dpiTestSuite_addCase(dpiTest_4420,
            "insert float64 vector into int8 col");
    dpiTestSuite_addCase(dpiTest_4421,
            "insert float64 vector into float32 col");
    dpiTestSuite_addCase(dpiTest_4422,
            "insert varchar into vector");
    dpiTestSuite_addCase(dpiTest_4423,
            "insert char into vector");
    dpiTestSuite_addCase(dpiTest_4424,
            "insert nchar into vector");
    dpiTestSuite_addCase(dpiTest_4425,
            "insert nvarchar into vector");
    dpiTestSuite_addCase(dpiTest_4426,
            "insert long varchar into vector");
    dpiTestSuite_addCase(dpiTest_4427,
            "insert long varchar into vector");
    dpiTestSuite_addCase(dpiTest_4428,
            "insert CLOB into vector");
    dpiTestSuite_addCase(dpiTest_4429,
            "insert Json array into vector");
    dpiTestSuite_addCase(dpiTest_4430,
            "update vector and verify");
    return dpiTestSuite_run();
}

