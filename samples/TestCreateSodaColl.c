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
// TestCreateSodaColl.c
//   Tests creating a SODA collection with the specified name.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t tempLength;
    const char *temp;
    dpiSodaColl *coll;
    dpiSodaDb *db;
    char *name;

    // determine name of collection to create
    if (argc != 2) {
        fprintf(stderr, "Usage: TestCreateSodaColl <name>\n");
        return 1;
    }
    name = argv[1];
    printf("Creating SODA collection named %s\n", name);

    // connect to database
    db = dpiSamples_getSodaDb();

    // create new SODA collection
    if (dpiSodaDb_createCollection(db, name, strlen(name), NULL, 0,
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiSamples_showError();

    // get the name of the collection
    if (dpiSodaColl_getName(coll, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Name of created collection: %.*s\n", tempLength, temp);

    // get the metadata of the collection
    if (dpiSodaColl_getMetadata(coll, &temp, &tempLength) < 0)
        return dpiSamples_showError();
    printf("Metadata of created collection: %.*s\n", tempLength, temp);

    // clean up
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}

