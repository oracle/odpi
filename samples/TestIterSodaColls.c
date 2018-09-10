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
// TestIterSodaColls.c
//   Tests iterating over the SODA collections that are available and displays
// their names.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t startingNameLength, collectionNameLength;
    const char *startingName, *collectionName;
    dpiSodaCollCursor *cursor;
    dpiSodaColl *coll;
    dpiSodaDb *db;

    // determine starting name to use; no parameters means all collections
    if (argc < 2) {
        startingName = NULL;
        startingNameLength = 0;
        printf("Iterating all SODA collections\n");
    } else {
        startingName = argv[1];
        startingNameLength = strlen(startingName);
        printf("Iterating SODA collections starting with %s\n", startingName);
    }

    // connect to database
    db = dpiSamples_getSodaDb();

    // get cursor to iterate over SODA collections
    if (dpiSodaDb_getCollections(db, startingName, startingNameLength,
            DPI_SODA_FLAGS_DEFAULT, &cursor) < 0)
        return dpiSamples_showError();

    // iterate over collections until no more are found
    while (1) {

        // get next collection
        if (dpiSodaCollCursor_getNext(cursor, DPI_SODA_FLAGS_DEFAULT,
                &coll) < 0)
            return dpiSamples_showError();
        if (!coll)
            break;

        // get the name of the collection
        if (dpiSodaColl_getName(coll, &collectionName,
                &collectionNameLength) < 0)
            return dpiSamples_showError();
        printf("Collection: %.*s\n", collectionNameLength, collectionName);
        if (dpiSodaColl_release(coll) < 0)
            return dpiSamples_showError();
    }

    // clean up
    dpiSodaCollCursor_release(cursor);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}

