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
// DemoReplaceSodaDoc.c
//   Demos replacing a document in a SODA collection with the specified key.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const char *collectionName, *key, *content, *temp, *encoding;
    dpiSodaDoc *doc, *replacedDoc;
    dpiSodaOperOptions options;
    dpiSampleParams *params;
    uint32_t tempLength;
    dpiSodaColl *coll;
    dpiSodaDb *db;
    int replaced;

    // determine name of collection to insert into and content of document
    if (argc != 4) {
        fprintf(stderr, "Usage: DemoReplaceSodaColl <name> <key> <content>\n");
        return 1;
    }
    collectionName = argv[1];
    key = argv[2];
    content = argv[3];

    // connect to database
    db = dpiSamples_getSodaDb();

    // open existing SODA collection
    if (dpiSodaDb_openCollection(db, collectionName, strlen(collectionName),
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiSamples_showError();
    if (!coll) {
        printf("No collection named %s\n", collectionName);
        return 1;
    }

    // build options
    params = dpiSamples_getParams();
    if (dpiContext_initSodaOperOptions(params->context, &options) < 0)
        return dpiSamples_showError();
    options.key = key;
    options.keyLength = strlen(key);

    // create SODA document using the provided content
    if (dpiSodaDb_createDocument(db, NULL, 0, content, strlen(content), NULL,
            0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiSamples_showError();

    // replace document in collection and return it
    if (dpiSodaColl_replaceOne(coll, &options, doc,
            DPI_SODA_FLAGS_ATOMIC_COMMIT, &replaced, &replacedDoc) < 0)
        return dpiSamples_showError();
    if (dpiSodaDoc_release(doc) < 0)
        return dpiSamples_showError();
    if (!replaced) {
        printf("No document with key %s\n", key);
        return 1;
    }

    // display information about the document that was just inserted
    printf("Document Details:\n");
    if (dpiSodaDoc_getKey(replacedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Key -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getContent(replacedDoc, &temp, &tempLength, &encoding) < 0)
        return dpiSamples_showError();
    printf("Content -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getMediaType(replacedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Media Type -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getVersion(replacedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Version -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getCreatedOn(replacedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Created -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getLastModified(replacedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Last Modified -> %.*s\n", tempLength, temp);

    // clean up
    dpiSodaDoc_release(replacedDoc);
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}
