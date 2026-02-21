//-----------------------------------------------------------------------------
// Copyright (c) 2018, 2026, Oracle and/or its affiliates.
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
// DemoGetSodaCollNames.c
//   Demos getting the names of all SODA collections using the method
// dpiSodaDb_getCollectionNames().
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    uint32_t startingNameLength, i;
    const char *startingName;
    dpiSampleParams *params;
    dpiStringList collNames;
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
    params = dpiSamples_getParams();
    db = dpiSamples_getSodaDb();

    // get all collection names
    if (dpiSodaDb_getCollectionNames(db, startingName, startingNameLength, 0,
            DPI_SODA_FLAGS_DEFAULT, &collNames) < 0)
        return dpiSamples_showError();
    for (i = 0; i < collNames.numStrings; i++)
        printf("Collection: %.*s\n", collNames.stringLengths[i],
                collNames.strings[i]);

    // clean up
    dpiContext_freeStringList(params->context, &collNames);
    dpiSodaDb_release(db);

    printf("Done.\n");
    return 0;
}
