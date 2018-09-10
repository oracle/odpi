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
// TestRemoveSodaDoc.c
//   Tests removing a document from a SODA collection with the specified key.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const char *collectionName, *key;
    dpiSodaOperOptions options;
    dpiSampleParams *params;
    dpiSodaColl *coll;
    uint64_t numDocs;
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

    // remove document from the collection
    if (dpiSodaColl_remove(coll, &options, DPI_SODA_FLAGS_ATOMIC_COMMIT,
            &numDocs) < 0)
        return dpiSamples_showError();
    printf("%" PRIu64 " documents removed.\n", numDocs);

    // clean up
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}

