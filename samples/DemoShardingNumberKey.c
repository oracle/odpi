//-----------------------------------------------------------------------------
// Copyright (c) 2020, Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DemoShardingNumberKey.c
//   Demos simple use of sharding with a numeric key. The sample schema
// provided does not include suppoort for running this demo. A sharded database
// must first be created. Information on how to create a sharded database can
// be found in the documentation:
// https://www.oracle.com/pls/topic/lookup?ctx=dblatest&id=SHARD
//-----------------------------------------------------------------------------

#include "SampleLib.h"
#define SQL_TEXT            "select cust_id, cust_name from ShardedTable"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiShardingKeyColumn shardingKeyColumn;
    dpiConnCreateParams createParams;
    dpiNativeTypeNum nativeTypeNum;
    dpiData *intData, *strData;
    dpiSampleParams *params;
    uint32_t bufferRowIndex;
    long shardingKeyValue;
    dpiStmt *stmt;
    dpiConn *conn;
    int found;

    // verify parameters
    if (argc != 2) {
        fprintf(stderr, "Specify sharding key to use.\n");
        return 1;
    }
    shardingKeyValue = atol(argv[1]);
    printf("Using sharding key value %ld\n", shardingKeyValue);

    // connect to database
    params = dpiSamples_getParams();
    if (dpiContext_initConnCreateParams(params->context, &createParams) < 0)
        return dpiSamples_showError();
    createParams.shardingKeyColumns = &shardingKeyColumn;
    createParams.numShardingKeyColumns = 1;
    shardingKeyColumn.oracleTypeNum = DPI_ORACLE_TYPE_NUMBER;
    shardingKeyColumn.nativeTypeNum = DPI_NATIVE_TYPE_DOUBLE;
    shardingKeyColumn.value.asDouble = shardingKeyValue;
    if (dpiConn_create(params->context, params->mainUserName,
            params->mainUserNameLength, params->mainPassword,
            params->mainPasswordLength, params->connectString,
            params->connectStringLength, NULL, &createParams, &conn) < 0)
        return dpiSamples_showError();

    // perform query
    if (dpiConn_prepareStmt(conn, 0, SQL_TEXT, strlen(SQL_TEXT), NULL, 0,
            &stmt) < 0)
        return dpiSamples_showError();
    if (dpiStmt_execute(stmt, 0, NULL) < 0)
        return dpiSamples_showError();
    while (1) {
        if (dpiStmt_fetch(stmt, &found, &bufferRowIndex) < 0)
            return dpiSamples_showError();
        if (!found)
            break;
        if (dpiStmt_getQueryValue(stmt, 1, &nativeTypeNum, &intData) < 0)
            return dpiSamples_showError();
        if (dpiStmt_getQueryValue(stmt, 2, &nativeTypeNum, &strData) < 0)
            return dpiSamples_showError();
        printf("Row: ID = %g, Name = %.*s\n", intData->value.asDouble,
                strData->value.asBytes.length, strData->value.asBytes.ptr);
    }

    printf("Done.\n");
    return 0;
}
