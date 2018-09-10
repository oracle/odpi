//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
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

