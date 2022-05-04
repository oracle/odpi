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
// DemoGetAllSodaDocs.c
//   Demos getting all documents from a SODA collection.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const char *collectionName, *temp, *encoding;
    dpiSodaDocCursor *cursor;
    uint32_t tempLength;
    dpiSodaColl *coll;
    dpiSodaDoc *doc;
    dpiSodaDb *db;

    // determine name of collection and key of document to retrieve
    if (argc != 2) {
        fprintf(stderr, "Usage: DemoGetSodaDoc <name>\n");
        return 1;
    }
    collectionName = argv[1];

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

    // find all documents in the collection
    if (dpiSodaColl_find(coll, NULL, DPI_SODA_FLAGS_DEFAULT, &cursor) < 0)
        return dpiSamples_showError();

    // iterate over the cursor until no more documents are found
    while (1) {

        // get next document from the cursor
        if (dpiSodaDocCursor_getNext(cursor, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
            return dpiSamples_showError();
        if (!doc)
            break;

        // print details about the document
        printf("Document Details:\n");
        if (dpiSodaDoc_getKey(doc, &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Key -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getContent(doc, &temp, &tempLength, &encoding) < 0)
            return dpiSamples_showError();
        printf("Content -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getMediaType(doc, &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Media Type -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getVersion(doc, &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Version -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getCreatedOn(doc, &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Created -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getLastModified(doc, &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Last Modified -> %.*s\n\n", tempLength, temp);

        // free document
        if (dpiSodaDoc_release(doc) < 0)
            return dpiSamples_showError();

    }

    // clean up
    dpiSodaDocCursor_release(cursor);
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}
