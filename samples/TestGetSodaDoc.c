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
// TestGetSodaDoc.c
//   Tests getting a document from a SODA collection with the specified key.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const char *collectionName, *key, *temp, *encoding;
    dpiSodaOperOptions options;
    dpiSampleParams *params;
    uint32_t tempLength;
    dpiSodaColl *coll;
    dpiSodaDoc *doc;
    dpiSodaDb *db;

    // determine name of collection and key of document to retrieve
    if (argc != 3) {
        fprintf(stderr, "Usage: TestGetSodaDoc <name> <key>\n");
        return 1;
    }
    collectionName = argv[1];
    key = argv[2];

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

    // get document from the collection
    if (dpiSodaColl_findOne(coll, &options, DPI_SODA_FLAGS_DEFAULT, &doc) < 0)
        return dpiSamples_showError();
    if (!doc) {
        printf("No document with key %s\n", key);
        return 1;
    }

    // print some details about the retrieved document
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
    printf("Last Modified -> %.*s\n", tempLength, temp);

    // clean up
    dpiSodaDoc_release(doc);
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}

