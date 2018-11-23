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
// TestSodaDoc.c
//   Test suite for testing SODA Document Functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_2800_nullHandle()
//   Call all public document functions with a NULL handle and verify the
// expected error is returned.
//-----------------------------------------------------------------------------
int dpiTest_2800_nullHandle(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDoc handle";

    dpiSodaDoc_addRef(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getContent(NULL, NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getCreatedOn(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getKey(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getLastModified(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getMediaType(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_getVersion(NULL, NULL, NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    dpiSodaDoc_release(NULL);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2801_addRef()
//   Verify dpiSodaDoc_addRef() manages reference counts as expected.
//-----------------------------------------------------------------------------
int dpiTest_2801_addRef(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002: invalid dpiSodaDoc handle";
    dpiSodaDoc *doc;
    dpiSodaDb *db;

    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_createDocument(db, NULL, 0, NULL, 0, NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
       return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_addRef(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    dpiSodaDoc_release(doc);
    if (dpiTestCase_expectError(testCase, expectedError) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_2802_verifyDocFuncs()
//   Create a document and insert into a collection. Verify document public
// functions are working as expected. Also replace the document and verify
// version, lastmodified are changed and key, createdon are same as before.
//-----------------------------------------------------------------------------
int dpiTest_2802_verifyDocFuncs(dpiTestCase *testCase, dpiTestParams *params)
{
    uint32_t createdOnLength1, lastModifiedLength1, keyLength1, versionLength1;
    uint32_t createdOnLength2, lastModifiedLength2, keyLength2, versionLength2;
    const char *content = "{\"test\":\"2802 original content\"}";
    const char *replaceContent = "{\"test\":\"2802 replaced\"}";
    const char *createdOn1, *lastModified1, *key1, *version1;
    const char *createdOn2, *lastModified2, *key2, *version2;
    dpiSodaDoc *doc, *insertedDoc, *replacedDoc;
    const char *collName = "ODPIC_COLL_2602";
    dpiSodaOperOptions options;
    dpiContext *context;
    dpiSodaColl *coll;
    dpiSodaDb *db;
    int replaced;

    // get SODA database
    if (dpiTestCase_getSodaDb(testCase, &db) < 0)
        return DPI_FAILURE;

    // create SODA collection
    if (dpiSodaDb_createCollection(db, collName, strlen(collName), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // create a document and insert into collection
    if (dpiSodaDb_createDocument(db, NULL, 0, content, strlen(content), NULL,
            0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_insertOne(coll, doc, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &insertedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // retrieve attributes of inserted document
    if (dpiSodaDoc_getKey(insertedDoc, &key1, &keyLength1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getVersion(insertedDoc, &version1, &versionLength1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getCreatedOn(insertedDoc, &createdOn1,
            &createdOnLength1) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getLastModified(insertedDoc, &lastModified1,
            &lastModifiedLength1) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // replace document with new content
    dpiTestSuite_getContext(&context);
    if (dpiContext_initSodaOperOptions(context, &options) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    options.key = key1;
    options.keyLength = keyLength1;
    if (dpiSodaDb_createDocument(db, NULL, 0, replaceContent,
            strlen(replaceContent), NULL, 0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaColl_replaceOne(coll, &options, doc,
            DPI_SODA_FLAGS_ATOMIC_COMMIT, &replaced, &replacedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(doc) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // retrieve attributes of replaced document
    if (dpiSodaDoc_getKey(replacedDoc, &key2, &keyLength2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getVersion(replacedDoc, &version2, &versionLength2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getCreatedOn(replacedDoc, &createdOn2,
            &createdOnLength2) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_getLastModified(replacedDoc, &lastModified2,
            &lastModifiedLength2) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    // compare results
    if (dpiTestCase_expectStringEqual(testCase, key2, keyLength2, key1,
            keyLength1) < 0)
        return DPI_FAILURE;
    if (dpiTestCase_expectStringEqual(testCase, createdOn2, createdOnLength2,
            createdOn1, createdOnLength1) < 0)
        return DPI_FAILURE;
    if (versionLength2 == versionLength1 &&
            strncmp(version2, version1, versionLength1) == 0)
        return dpiTestCase_setFailed(testCase,
                "created and replaced document versions are matching");
    if (lastModifiedLength2 == lastModifiedLength1 &&
            strncmp(lastModified2, lastModified1, lastModifiedLength1) == 0)
        return dpiTestCase_setFailed(testCase,
                "created and replaced document last modified are matching");

    // cleanup
    if (dpiSodaDoc_release(insertedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiSodaDoc_release(replacedDoc) < 0)
        return dpiTestCase_setFailedFromError(testCase);
    if (dpiTestCase_cleanupSodaColl(testCase, coll) < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiTestSuite_initialize(2800);
    dpiTestSuite_addCase(dpiTest_2800_nullHandle,
            "verify public document functions with NULL");
    dpiTestSuite_addCase(dpiTest_2801_addRef,
            "verify dpiSodaDoc_addRef() works as expected");
    dpiTestSuite_addCase(dpiTest_2802_verifyDocFuncs,
            "verify doc functions are working as expected");
    return dpiTestSuite_run();
}

