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
// SampleLib.c
//   Common code used in all samples.
//-----------------------------------------------------------------------------

#include "SampleLib.h"

static dpiContext *gContext = NULL;
static dpiSampleParams gParams;
static dpiAccessToken *gAccessToken = NULL;

//-----------------------------------------------------------------------------
// dpiSamples__fatalError() [INTERNAL]
//   Called when a fatal error is encountered from which recovery is not
// possible. This simply prints a message to stderr and exits the program with
// a non-zero exit code to indicate an error.
//-----------------------------------------------------------------------------
static void dpiSamples__fatalError(const char *message)
{
    fprintf(stderr, "FATAL: %s\n", message);
    exit(1);
}


//-----------------------------------------------------------------------------
// dpiSamples__finalize() [INTERNAL]
//   Destroy context upon process exit.
//-----------------------------------------------------------------------------
static void dpiSamples__finalize(void)
{
    dpiContext_destroy(gContext);
    if (gAccessToken) {
        if (gAccessToken->token)
            free((char*) gAccessToken->token);
        if (gAccessToken->privateKey)
            free((char*) gAccessToken->privateKey);
        free(gAccessToken);
    }
}


//-----------------------------------------------------------------------------
// dpiSamples__getEnvValue()
//   Get parameter value from the environment or use supplied default value if
// the value is not set in the environment. Memory is allocated to accommodate
// the value.
//-----------------------------------------------------------------------------
static void dpiSamples__getEnvValue(const char *envName,
        const char *defaultValue, const char **value, uint32_t *valueLength,
        int convertToUpper)
{
    const char *source;
    uint32_t i;
    char *ptr;

    source = getenv(envName);
    if (!source)
        source = defaultValue;
    *valueLength = strlen(source);
    *value = malloc(*valueLength);
    if (!*value)
        dpiSamples__fatalError("Out of memory!");
    memcpy((void*) *value, source, *valueLength);
    if (convertToUpper) {
        ptr = (char*) *value;
        for (i = 0; i < *valueLength; i++)
            ptr[i] = toupper(ptr[i]);
    }
}


//-----------------------------------------------------------------------------
// dpiSamples_getConn()
//   Connect to the database using the supplied parameters. The DPI library
// will also be initialized, if needed.
//-----------------------------------------------------------------------------
dpiConn *dpiSamples_getConn(int withPool, dpiCommonCreateParams *commonParams)
{
    dpiConn *conn;
    dpiPool *pool;

    // perform initialization
    dpiSamples_getParams();

    // create a pool and acquire a connection
    if (withPool) {
        if (dpiPool_create(gContext, gParams.mainUserName,
                gParams.mainUserNameLength, gParams.mainPassword,
                gParams.mainPasswordLength, gParams.connectString,
                gParams.connectStringLength, commonParams, NULL, &pool) < 0) {
            dpiSamples_showError();
            dpiSamples__fatalError("Unable to create pool.");
        }
        if (dpiPool_acquireConnection(pool, NULL, 0, NULL, 0, NULL,
                    &conn) < 0) {
            dpiSamples_showError();
            dpiSamples__fatalError("Unable to acquire connection from pool.");
        }
        dpiPool_release(pool);

    // or create a standalone connection
    } else if (dpiConn_create(gContext, gParams.mainUserName,
            gParams.mainUserNameLength, gParams.mainPassword,
            gParams.mainPasswordLength, gParams.connectString,
            gParams.connectStringLength, commonParams, NULL, &conn) < 0) {
        dpiSamples_showError();
        dpiSamples__fatalError("Unable to create connection.");
    }

    return conn;
}


//-----------------------------------------------------------------------------
// dpiSamples_getParams()
//   Get parameters set in the environment. The DPI library will also be
// initialized if needed.
//-----------------------------------------------------------------------------
dpiSampleParams *dpiSamples_getParams(void)
{
    dpiErrorInfo errorInfo;

    if (!gContext) {
        if (dpiContext_createWithParams(DPI_MAJOR_VERSION, DPI_MINOR_VERSION,
                NULL, &gContext, &errorInfo) < 0) {
            fprintf(stderr, "ERROR: %.*s (%s : %s)\n", errorInfo.messageLength,
                    errorInfo.message, errorInfo.fnName, errorInfo.action);
            dpiSamples__fatalError("Cannot create DPI context.");
        }
        atexit(dpiSamples__finalize);
    }

    dpiSamples__getEnvValue("ODPIC_SAMPLES_MAIN_USER", "odpicdemo",
            &gParams.mainUserName, &gParams.mainUserNameLength, 1);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_MAIN_PASSWORD", "welcome",
            &gParams.mainPassword, &gParams.mainPasswordLength, 0);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_PROXY_USER", "odpicdemo_proxy",
            &gParams.proxyUserName, &gParams.proxyUserNameLength, 1);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_PROXY_PASSWORD", "welcome",
            &gParams.proxyPassword, &gParams.proxyPasswordLength, 0);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_CONNECT_STRING", "localhost/orclpdb",
            &gParams.connectString, &gParams.connectStringLength, 0);
    dpiSamples__getEnvValue("ODPIC_SAMPLES_DIR_NAME", "odpicdemo_dir",
            &gParams.dirName, &gParams.dirNameLength, 1);
    gParams.context = gContext;

    return &gParams;
}


//-----------------------------------------------------------------------------
// dpiSamples_getSodaDb()
//   Connect to the database and then acquire the SODA database object.
//-----------------------------------------------------------------------------
dpiSodaDb *dpiSamples_getSodaDb(void)
{
    dpiSodaDb *db;
    dpiConn *conn;

    conn = dpiSamples_getConn(0, NULL);
    if (dpiConn_getSodaDb(conn, &db) < 0) {
        dpiSamples_showError();
        dpiSamples__fatalError("Unable to acquire SODA database.");
    }
    dpiConn_release(conn);

    return db;
}


//-----------------------------------------------------------------------------
// dpiSamples__strRemove() [INTERNAL]
//   Removes substring from buffer
//-----------------------------------------------------------------------------
void dpiSamples__strRemove(char *str, const char *sub, size_t dataLen)
{
    char *r;
    size_t len = 0;

    r = strstr(str, sub);
    if (r) {
        len = strlen(sub);
        memmove(r, r + len + 1, dataLen - (r - str + len));
    }
}


//-----------------------------------------------------------------------------
// dpiSamples__getTokenData() [INTERNAL]
//   Read files from location specified
//-----------------------------------------------------------------------------
void dpiSamples__getTokenData(const char *dirName, const char *fileName,
        char **value, uint32_t *valueLength, int type)
{
    FILE *fp = NULL;
    char *fullFileName = NULL;
    size_t len = 0;

    fullFileName = malloc(strlen(dirName) + strlen(fileName) + 2);
    if (!fullFileName)
        dpiSamples__fatalError("Out of memory!");

    sprintf(fullFileName, "%s/%s", dirName, fileName);

    fp = fopen(fullFileName, "r");
    free(fullFileName);
    fullFileName = NULL;

    if (!fp) {
        *value = NULL;
        *valueLength = 0;
        return;
    }

    *value = malloc(TOKENBUFLEN);
    if (!*value)
        dpiSamples__fatalError("Out of memory!");

    len = fread(*value, sizeof(char), TOKENBUFLEN - 1, fp);
    (*value)[len] = '\0';

    *valueLength = strlen(*value);
    if (ferror(fp))
        dpiSamples__fatalError("Error in reading from file.");

    if (type) {
        dpiSamples__strRemove(*value,
             "-----BEGIN PRIVATE KEY-----", len);
        dpiSamples__strRemove(*value,
             "-----END PRIVATE KEY-----", len);
    }

    fclose(fp);
}


//-----------------------------------------------------------------------------
// dpiSamples_populateAccessToken()
//   Populates structure with token and private key
// ----------------------------------------------------------------------------
void dpiSamples_populateAccessToken(dpiAccessToken* accessToken,
        const char* envName)
{
    const char *privateKeyFileName = "oci_db_key.pem";
    const char *tokenFileName = "token";
    char *dbLocation = NULL;

    dbLocation = getenv(envName);
    if (!dbLocation) {
        const char *format = "Set environment variable %s to the directory "
                "where the database token and private key files are found.";
        char *errorMessage = malloc(strlen(format) + strlen(envName) + 1);
        if (!errorMessage)
            dpiSamples__fatalError("Out of memory!");
        sprintf(errorMessage, format, envName);
        dpiSamples__fatalError(errorMessage);
    }

    dpiSamples__getTokenData(dbLocation, tokenFileName,
            (char **)(&accessToken->token),
            &accessToken->tokenLength, 0);

    dpiSamples__getTokenData(dbLocation, privateKeyFileName,
            (char **)(&accessToken->privateKey),
            &accessToken->privateKeyLength, 1);
}


//-----------------------------------------------------------------------------
// dpiSamples_getAccessToken()
//   Read the authentication token and key from files
// ----------------------------------------------------------------------------
dpiAccessToken *dpiSamples_getAccessToken(void)
{
    if (!gAccessToken) {
        gAccessToken = malloc(sizeof(dpiAccessToken));
        if (!gAccessToken)
            dpiSamples__fatalError("Out of memory!");

        dpiSamples_populateAccessToken(gAccessToken,
                "ODPIC_SAMPLES_ACCESS_TOKEN_LOC");
    }

    return gAccessToken;
}


//-----------------------------------------------------------------------------
// dpiSamples_showError()
//   Display the error to stderr.
//-----------------------------------------------------------------------------
int dpiSamples_showError(void)
{
    dpiErrorInfo info;

    dpiContext_getError(gContext, &info);
    fprintf(stderr, "ERROR: %.*s (%s: %s), offset: %u\n", info.messageLength,
            info.message, info.fnName, info.action, info.offset);
    return -1;
}
