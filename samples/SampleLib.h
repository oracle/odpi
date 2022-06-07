//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2022, Oracle and/or its affiliates.
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
// SampleLib.h
//   Header file for common code used in all samples.
//-----------------------------------------------------------------------------

#include <dpi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _MSC_VER
#if _MSC_VER < 1900
#define PRId64                  "I64d"
#define PRIu64                  "I64u"
#endif
#endif

#ifndef PRIu64
#include <inttypes.h>
#endif

// buffer length for token and private key
#define TOKENBUFLEN 8000

typedef struct {
    const char *mainUserName;
    uint32_t mainUserNameLength;
    const char *mainPassword;
    uint32_t mainPasswordLength;
    const char *proxyUserName;
    uint32_t proxyUserNameLength;
    const char *proxyPassword;
    uint32_t proxyPasswordLength;
    const char *connectString;
    uint32_t connectStringLength;
    const char *dirName;
    uint32_t dirNameLength;
    dpiContext *context;
} dpiSampleParams;

// connect to the database
dpiConn *dpiSamples_getConn(int withPool, dpiCommonCreateParams *commonParams);

// acquire parameters
dpiSampleParams *dpiSamples_getParams(void);

// acquire SODA database
dpiSodaDb *dpiSamples_getSodaDb(void);

// show error to stderr
int dpiSamples_showError(void);

// get token and private key
dpiAccessToken *dpiSamples_getAccessToken(void);

// populate access token
void dpiSamples_populateAccessToken(dpiAccessToken* accessToken,
        const char* envName);
