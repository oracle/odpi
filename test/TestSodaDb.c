//-----------------------------------------------------------------------------
// Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestSodaDb.c
//   Test suite for testing SODA Database (dpiSodaDb) functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest__getCollections()
//   Iterate over the collections found in the database and verify that the
// names found match the expected names.
//-----------------------------------------------------------------------------
int dpiTest__getCollections(dpiTestCase *testCase, dpiSodaDb *db,
        const char *startName, uint32_t startNameLength,
        const char **expectedNames, uint32_t expectedNamesLength)
{
    dpiSodaCollCursor *cursor;
    uint32_t pos, nameLength;
    dpiSodaColl *coll;
    const char *name;

    // create cursor
    if (dpiSodaDb_getCollections(db, startName, startNameLength,
            DPI_SODA_FLAGS_DEFAULT, &cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // iterate over all collections
    for (pos = 0; pos < expectedNamesLength; pos++) {
        if (dpiSodaCollCursor_getNext(cursor, DPI_SODA_FLAGS_DEFAULT,
                &coll) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (!coll)
            return dpiTestCase_setFailed(testCase,
                    "too few collections found in the database");
        if (dpiSodaColl_getName(coll, &name, &nameLength) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiTestCase_expectStringEqual(testCase, name, nameLength,
                expectedNames[pos], strlen(expectedNames[pos])) < 0)
            return DPI_FAILURE;
        if (dpiSodaColl_release(coll) < 0)
             return dpiTestCase_setFailedFromError(testCase);
    }

    // no further collections should be found
    if (dpiSodaCollCursor_getNext(cursor, DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (coll)
        return dpiTestCase_setFailed(testCase,
                "too many collections found in the database");
    if (dpiSodaCollCursor_release(cursor) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2500_releaseTwice()
//   Call dpiSodaDb_release() twice and confirm correct error is returned.
//-----------------------------------------------------------------------------
int dpiTest_2500_releaseTwice(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDb handle";
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiSodaDb_release(db);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2501_addRef()
//   Verify dpiSodaDb_addRef() works as expected.
//-----------------------------------------------------------------------------
int dpiTest_2501_addRef(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDb handle";
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_addRef(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiSodaDb_release(db);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2502_nullHandle()
//   Call all public functions with invalid handle.
//-----------------------------------------------------------------------------
int dpiTest_2502_nullHandle(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDb handle";
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
         return dpiTestCase_setFailedFromError(testCase);
    dpiSodaDb_createCollection(db, NULL, 0, NULL, 0, DPI_SODA_FLAGS_DEFAULT,
            NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDb_createDocument(db, NULL, 0, NULL, 0, NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDb_getCollections(db, NULL, 0, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDb_openCollection(db, NULL, 0, DPI_SODA_FLAGS_DEFAULT, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDb_release(db);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2503_createCollection()
//   Create a collection and then attempt to create the collection a second
// time. It should open the existing collection without error.
//-----------------------------------------------------------------------------
int dpiTest_2503_createCollection(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *name = "ODPIC_COLL_2503", *tempName;
    uint32_t tempNameLength;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, name, strlen(name), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_getName(coll, &tempName, &tempNameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, tempName, tempNameLength,
            name, strlen(name)) < 0)
        return DPI_FAILURE;
    if (dpiSodaColl_release(coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create SODA collection again
    if (dpiSodaDb_createCollection(db, name, strlen(name), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_getName(coll, &tempName, &tempNameLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, tempName, tempNameLength,
            name, strlen(name)) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2504_getCollections()
//   Create a bunch of collections and then test various calls to
// dpiSodaDb_getCollections() and verify that the correct number of names is
// returned.
//-----------------------------------------------------------------------------
int dpiTest_2504_getCollections(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *allNames[] = {
        "ODPIC_COLL_2504_A", "ODPIC_COLL_2504_B", "ODPIC_COLL_2504_C",
        "ODPIC_COLL_2504_D", "ODPIC_COLL_2504_E", NULL
    };
    dpiSodaColl *coll;
    const char *name;
    dpiSodaDb *db;
    uint32_t i;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // remove all existing collections, if any
    if (dpiTestCase_dropAllSodaColls(testCase, db) < 0)
        return DPI_FAILURE;

    // create the collections in the array
    for (i = 0; ; i++) {
        name = allNames[i];
        if (!name)
            break;
        if (dpiSodaDb_createCollection(db, name, strlen(name), NULL, 0,
                DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
            return dpiTestCase_setFailedFromError(testCase);
        if (dpiSodaColl_release(coll) < 0)
            return dpiTestCase_setFailedFromError(testCase);
    }

    // test iterating over all collections
    if (dpiTest__getCollections(testCase, db, NULL, 0, allNames, 5) < 0)
        return DPI_FAILURE;

    // test iterating over collections from a starting point
    if (dpiTest__getCollections(testCase, db, allNames[1], strlen(allNames[1]),
            &allNames[1], 4) < 0)
        return DPI_FAILURE;
    if (dpiTest__getCollections(testCase, db, allNames[3], strlen(allNames[3]),
            &allNames[3], 2) < 0)
        return DPI_FAILURE;
    if (dpiTest__getCollections(testCase, db, allNames[4], strlen(allNames[4]),
            &allNames[4], 1) < 0)
        return DPI_FAILURE;

    // test iterating over no collections
    if (dpiTest__getCollections(testCase, db, "X", 1, allNames, 0) < 0)
        return DPI_FAILURE;

    // cleanup
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2505_getSodaDb()
//   Call dpiConn_getSodaDb() twice with the same connection and with different
// connections.
//-----------------------------------------------------------------------------
int dpiTest_2505_getSodaDb(dpiTestCase *testCase, dpiTestParams *params)
{
    dpiConn *conn1, *conn2;
    dpiSodaDb *db1, *db2;

    // create initial SODA database object (checks client/server versions)
    if (dpiTestCase_getSodaDb(testCase, &db1) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db1) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create connections
    if (dpiTestCase_getConnection(testCase, &conn1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_getConnection(testCase, &conn2) < 0)
        return DPI_FAILURE;

    // create two SODA databases on the same connection
    if (dpiConn_getSodaDb(conn1, &db1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getSodaDb(conn1, &db2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_release(db1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_release(db2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create two SODA databases but on different connections
    if (dpiConn_getSodaDb(conn2, &db1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_getSodaDb(conn1, &db2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_release(db1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDb_release(db2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiConn_release(conn1) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2506_createDocument()
//   Create document and verify that the content, key and media type were all
// saved properly.
//-----------------------------------------------------------------------------
int dpiTest_2506_createDocument(dpiTestCase *testCase,
        dpiTestParams *params)
{
    const char *content = "{\"test\":\"2506\"}", *temp, *encoding;
    const char *setMediaType = "text/plain", *key = "Key2506";
    const char *defaultMediaType = "application/json";
    uint32_t tempLength;
    dpiSodaDoc *doc;
    dpiSodaDb *db;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // set content but leave other attributes NULL
    if (dpiSodaDb_createDocument(db, NULL, 0, content, strlen(content), NULL,
            0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getContent(doc, &temp, &tempLength, &encoding) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, temp, tempLength,
            content, strlen(content)) < 0)
        return DPI_FAILURE;
    if (dpiSodaDoc_getKey(doc, &temp, &tempLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, temp, tempLength, NULL, 0) < 0)
        return DPI_FAILURE;
    if (dpiSodaDoc_getMediaType(doc, &temp, &tempLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, temp, tempLength,
            defaultMediaType, strlen(defaultMediaType)) < 0)
        return DPI_FAILURE;
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // set key and media type but not content
    if (dpiSodaDb_createDocument(db, key, strlen(key), NULL, 0, setMediaType,
            strlen(setMediaType), DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getContent(doc, &temp, &tempLength, &encoding) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, temp, tempLength, NULL, 0) < 0)
        return DPI_FAILURE;
    if (dpiSodaDoc_getKey(doc, &temp, &tempLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, temp, tempLength, key,
            strlen(key)) < 0)
        return DPI_FAILURE;
    if (dpiSodaDoc_getMediaType(doc, &temp, &tempLength) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_expectStringEqual(testCase, temp, tempLength, setMediaType,
            strlen(setMediaType)) < 0)
        return DPI_FAILURE;
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // cleanup
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2500);
    dpiTestSuite_addCase(dpiTest_2500_releaseTwice,
            "dpiSodaDb_release() twice");
    dpiTestSuite_addCase(dpiTest_2501_addRef,
            "dpiSodaDb_addRef() with valid parameters");
    dpiTestSuite_addCase(dpiTest_2502_nullHandle,
            "call SODA database functions with NULL handle");
    dpiTestSuite_addCase(dpiTest_2503_createCollection,
            "dpiSodaDb_createCollection() with valid parameters");
    dpiTestSuite_addCase(dpiTest_2504_getCollections,
            "dpiSodaDb_getCollections() with valid parameters");
    dpiTestSuite_addCase(dpiTest_2505_getSodaDb,
            "dpiConn_getSodaDb() with valid parameters");
    dpiTestSuite_addCase(dpiTest_2506_createDocument,
            "dpiSodaDb_createDocument() with valid parameters");
    return dpiTestSuite_run();
}

