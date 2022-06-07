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
//   DemoTokenPoolWithCallback.c
//
// DESCRIPTION
//   This demo file shows connection pooling with token based authentication to
//   Oracle Autonomous Database from Oracle Compute Infrastructure. It shows
//   how to create a connection pool and update expired tokens.
//
// PREREQUISITES
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
//      ODPIC_SAMPLES_EXPIRED_ACCESS_TOKEN_LOC
//-----------------------------------------------------------------------------

#include "SampleLib.h"

//-----------------------------------------------------------------------------
// tokenCallback()
//-----------------------------------------------------------------------------
int tokenCallback(void *context, dpiAccessToken *tokenRefresh)
{
    printf("Token callback is called.\n");

    // used for extracting the token and private key from files generated
    // through the OCI-CLI.
    dpiAccessToken *accessToken;
    accessToken = dpiSamples_getAccessToken();
    tokenRefresh->token = accessToken->token;
    tokenRefresh->tokenLength = accessToken->tokenLength;
    tokenRefresh->privateKey = accessToken->privateKey;
    tokenRefresh->privateKeyLength = accessToken->privateKeyLength;
    return DPI_SUCCESS;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    dpiConn *conn1, *conn2;
    dpiPool *pool;
    dpiSampleParams *params;
    dpiAccessToken *accessToken;

    accessToken = dpiSamples_getAccessToken();
    params = dpiSamples_getParams();

    dpiPoolCreateParams poolCreateParams;
    if (dpiContext_initPoolCreateParams(params->context,
            &poolCreateParams) < 0)
        return dpiSamples_showError();

    // params for token based authentication:
    // accessTokenCallback: A callback function to provide a refreshed token
    // externalAuth:    Must be set to true for token based authentication
    // homogeneous:     Must be set to true for token based authentication
    poolCreateParams.minSessions = 1;
    poolCreateParams.maxSessions = 10;
    poolCreateParams.sessionIncrement = 1;
    poolCreateParams.externalAuth = 1;
    poolCreateParams.homogeneous = 1;
    poolCreateParams.accessTokenCallback = tokenCallback;

    dpiCommonCreateParams commonCreateParams;
    if (dpiContext_initCommonCreateParams(params->context,
            &commonCreateParams) < 0)
        return dpiSamples_showError();
    commonCreateParams.accessToken = accessToken;

    if (dpiPool_create(params->context, NULL, 0, NULL, 0,
            params->connectString, params->connectStringLength,
            &commonCreateParams, &poolCreateParams, &pool) < 0)
       return dpiSamples_showError();
    printf("Connection pool is created.\n");

    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn1) < 0)
        return dpiSamples_showError();
    printf("Session is acquired from connection pool.\n");

    // set expired token and private key in connection pool
    dpiAccessToken expiredAccessToken;
    dpiSamples_populateAccessToken(&expiredAccessToken,
            "ODPIC_SAMPLES_EXPIRED_ACCESS_TOKEN_LOC");
    if (dpiPool_setAccessToken(pool, &expiredAccessToken) < 0)
        return dpiSamples_showError();

    // accessTokenCallback will get invoked to get refreshed tokens
    if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
            &conn2) < 0)
        return dpiSamples_showError();
    printf("Session is acquired from connection pool.\n");
    printf("Done.\n");
    return 0;
}
