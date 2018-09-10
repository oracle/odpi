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
// TestDropSodaColl.c
//   Tests creating a SODA collection with the specified name.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiSodaColl *coll;
    dpiSodaDb *db;
    char *name;

    // determine name of collection to drop
    if (argc != 2) {
        fprintf(stderr, "Usage: TestDropSodaColl <name>\n");
        return 1;
    }
    name = argv[1];

    // connect to database
    db = dpiSamples_getSodaDb();

    // open the SODA collection first
    if (dpiSodaDb_openCollection(db, name, strlen(name),
            DPI_SODA_FLAGS_DEFAULT, &coll) < 0)
        return dpiSamples_showError();
    if (!coll) {
        printf("No collection named %s\n", name);
        return 1;
    }

    // drop collection
    printf("Dropping SODA collection named %s\n", name);
    if (dpiSodaColl_drop(coll, DPI_SODA_FLAGS_DEFAULT, NULL) < 0)
        return dpiSamples_showError();

    // clean up
    dpiSodaColl_release(coll);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}

