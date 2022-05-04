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
// DemoCreateSodaColl.c
//   Demos creating a SODA collection with the specified name.
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
        fprintf(stderr, "Usage: DemoCreateSodaColl <name>\n");
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
