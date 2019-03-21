//-----------------------------------------------------------------------------
// Copyright (c) 2019, Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DemoInsertManySodaColl.c
//   Demos inserting many documents into a SODA collection with the specified
// contents.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const char *collectionName, *temp, *encoding;
    dpiSodaDoc **docs, **insertedDocs;
    uint32_t numDocs, i, tempLength;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // determine name of collection to insert into and number of documents
    if (argc < 3) {
        fprintf(stderr, "Usage: DemoInsertManySodaColl <name> <content> ...\n");
        return 1;
    }
    collectionName = argv[1];
    numDocs = (uint32_t) argc - 2;

    // connect to database
    db = dpiSamples_getSodaDb();

    // allocate memory for the document arrays
    docs = malloc(numDocs * sizeof(dpiSodaDoc*));
    insertedDocs = malloc(numDocs * sizeof(dpiSodaDoc*));
    if (!docs || !insertedDocs) {
        printf("Out of memory!\n");
        return 1;
    }

    // create documents for each of the contents provided
    for (i = 0; i < numDocs; i++) {
        if (dpiSodaDb_createDocument(db, NULL, 0, argv[i + 2],
                strlen(argv[i + 2]), NULL, 0, DPI_SODA_FLAGS_DEFAULT,
                &docs[i]) < 0)
            return dpiSamples_showError();
    }

    // open existing SODA collection
    if (dpiSodaDb_openCollection(db, collectionName, strlen(collectionName),
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiSamples_showError();
    if (!coll) {
        printf("No collection named %s\n", collectionName);
        return 1;
    }

    // insert documents into collection and return them
    printf("Inserting %u documents into collection %s\n\n", numDocs,
            collectionName);
    if (dpiSodaColl_insertMany(coll, numDocs, docs,
            DPI_SODA_FLAGS_DEFAULT, insertedDocs) < 0)
        return dpiSamples_showError();

    // display information about the documents that were just inserted
    for (i = 0; i < numDocs; i++) {
        dpiSodaDoc_release(docs[i]);
        printf("Document Details:\n");
        if (dpiSodaDoc_getKey(insertedDocs[i], &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Key -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getContent(insertedDocs[i], &temp, &tempLength,
                &encoding) < 0)
            return dpiSamples_showError();
        printf("Content -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getMediaType(insertedDocs[i], &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Media Type -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getVersion(insertedDocs[i], &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Version -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getCreatedOn(insertedDocs[i], &temp, &tempLength) < 0)
            return dpiSamples_showError();
        printf("Created -> %.*s\n", tempLength, temp);
        if (dpiSodaDoc_getLastModified(insertedDocs[i], &temp,
                &tempLength) < 0)
            return dpiSamples_showError();
        printf("Last Modified -> %.*s\n\n", tempLength, temp);
        dpiSodaDoc_release(insertedDocs[i]);
    }

    // clean up
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}
