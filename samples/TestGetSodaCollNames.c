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
// TestGetSodaCollNames.c
//   Tests getting the names of all SODA collections using the method
// dpiSodaDb_getCollectionNames().
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t startingNameLength, i;
    dpiSodaCollNames collNames;
    const char *startingName;
    dpiSodaDb *db;

    // determine starting name to use; no parameters means all collections
    if (argc < 2) {
        startingName = NULL;
        startingNameLength = 0;
        printf("Getting all SODA collection names\n");
    } else {
        startingName = argv[1];
        startingNameLength = strlen(startingName);
        printf("Getting all SODA collection names starting with %s\n",
                startingName);
    }

    // connect to database
    db = dpiSamples_getSodaDb();

    // get all collection names
    if (dpiSodaDb_getCollectionNames(db, startingName, startingNameLength, 0,
            DPI_SODA_FLAGS_DEFAULT, &collNames) < 0)
        return dpiSamples_showError();
    for (i = 0; i < collNames.numNames; i++)
        printf("Collection: %.*s\n", collNames.nameLengths[i],
                collNames.names[i]);

    // clean up
    dpiSodaDb_freeCollectionNames(db, &collNames);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}

