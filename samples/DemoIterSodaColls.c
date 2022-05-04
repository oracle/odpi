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
// DemoIterSodaColls.c
//   Demos iterating over the SODA collections that are available and displays
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
