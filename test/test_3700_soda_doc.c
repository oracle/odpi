//-----------------------------------------------------------------------------
// Copyright (c) 2018, 2022, Oracle and/or its affiliates.
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
// test_3700_soda_doc.c
//   Test suite for testing SODA Document Functions.
//-----------------------------------------------------------------------------

#include "TestLib.h"

//-----------------------------------------------------------------------------
// dpiTest_3700_nullHandle()
//   Call all public document functions with a NULL handle and verify the
// expected error is returned.
//-----------------------------------------------------------------------------
int dpiTest_3700_nullHandle(dpiTestCase *testCase, dpiTestParams *params)
{
    const char *expectedError = "DPI-1002:";

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
// dpiTest_3701_addRef()
//   Verify dpiSodaDoc_addRef() manages reference counts as expected.
//-----------------------------------------------------------------------------
int dpiTest_3701_addRef(dpiTestCase *testCase, dpiTestParams *params)
{
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
    if (dpiTestCase_expectError(testCase, "DPI-1002:") < 0)
        return DPI_FAILURE;
    if (dpiSodaDb_release(db) < 0)
        return dpiTestCase_setFailedFromError(testCase);

    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// dpiTest_3702_verifyDocFuncs()
//   Create a document and insert into a collection. Verify document public
// functions are working as expected. Also replace the document and verify
// version, lastmodified are changed and key, createdon are same as before.
//-----------------------------------------------------------------------------
int dpiTest_3702_verifyDocFuncs(dpiTestCase *testCase, dpiTestParams *params)
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
    dpiTestSuite_initialize(3700);
    dpiTestSuite_addCase(dpiTest_3700_nullHandle,
            "verify public document functions with NULL");
    dpiTestSuite_addCase(dpiTest_3701_addRef,
            "verify dpiSodaDoc_addRef() works as expected");
    dpiTestSuite_addCase(dpiTest_3702_verifyDocFuncs,
            "verify doc functions are working as expected");
    return dpiTestSuite_run();
}
