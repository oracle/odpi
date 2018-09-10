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
// TestInsertSodaColl.c
//   Tests inserting a document into a SODA collection with the specified name
// and content.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const char *collectionName, *content, *temp, *encoding;
    dpiSodaDoc *doc, *insertedDoc;
    uint32_t tempLength;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // determine name of collection to insert into and content of document
    if (argc != 3) {
        fprintf(stderr, "Usage: TestInsertSodaColl <name> <content>\n");
        return 1;
    }
    collectionName = argv[1];
    content = argv[2];

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

    // create SODA document using the provided content
    if (dpiSodaDb_createDocument(db, NULL, 0, content, strlen(content), NULL,
            0, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiSamples_showError();

    // insert document into collection and return inserted document
    if (dpiSodaColl_insertOne(coll, doc, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &insertedDoc) , 0)
        return dpiSamples_showError();
    if (dpiSodaDoc_release(doc) < 0)
        return dpiSamples_showError();

    // display information about the document that was just inserted
    printf("Document Details:\n");
    if (dpiSodaDoc_getKey(insertedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Key -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getContent(insertedDoc, &temp, &tempLength, &encoding) < 0)
        return dpiSamples_showError();
    printf("Content -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getMediaType(insertedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Media Type -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getVersion(insertedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Version -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getCreatedOn(insertedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Created -> %.*s\n", tempLength, temp);
    if (dpiSodaDoc_getLastModified(insertedDoc, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Last Modified -> %.*s\n", tempLength, temp);

    // clean up
    dpiSodaDoc_release(insertedDoc);
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}

