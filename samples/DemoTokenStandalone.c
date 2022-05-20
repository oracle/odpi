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
// NAME
//   DemoTokenStandalone.c
//
// DESCRIPTION
//   This Demo file shows standalone connection using token based authentication
//   to Oracle Autonomous Database from Oracle Compute Infrastructure.
//
// PREQUISITES
//   - Oracle Client libraries 19.14 (or later), or 21.5 (or later).
//
//   - The Oracle Cloud Infrastructure command line interface (OCI-CLI).  The
//     command line interface (CLI) is a tool that enables you to work with
//     Oracle Cloud Infrastructure objects and services at a command line, see
//     https://docs.oracle.com/en-us/iaas/Content/API/Concepts/cliconcepts.htm
//
//   - Set these environment variables (see the code explanation):
//      ODPIC_SAMPLES_ACCESS_TOKEN_LOC
//      ODPIC_SAMPLES_CONNECT_STRING
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiConn *conn;
    dpiSampleParams *params;
    dpiAccessToken *accessToken;

    accessToken = dpiSamples_getAccessToken();
    params = dpiSamples_getParams();

    dpiConnCreateParams connCreateParams;
    if (dpiContext_initConnCreateParams(params->context,
            &connCreateParams) < 0)
        return dpiSamples_showError();

    // params for token based authentication:
    // externalAuth:    Must be set to true for token based authentication
    connCreateParams.externalAuth = 1;

    dpiCommonCreateParams commonCreateParams;
    if (dpiContext_initCommonCreateParams(params->context,
            &commonCreateParams) < 0)
        return dpiSamples_showError();
    commonCreateParams.accessToken = accessToken;

    if (dpiConn_create(params->context, NULL, 0, NULL, 0,
            params->connectString, params->connectStringLength,
            &commonCreateParams, &connCreateParams, &conn) < 0)
       return dpiSamples_showError();

    printf("Standalone connection is created\n");
    printf("Done.\n");
    return 0;
}
