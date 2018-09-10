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
// TestGetAllSodaDocs.c
//   Tests getting all documents from a SODA collection.
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
        fprintf(stderr, "Usage: TestGetSodaDoc <name>\n");
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

