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
// DemoDropSodaColl.c
//   Demos dropping a SODA collection with the specified name.
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
        fprintf(stderr, "Usage: DemoDropSodaColl <name>\n");
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
