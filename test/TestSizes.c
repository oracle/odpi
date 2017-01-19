//-----------------------------------------------------------------------------
// Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TestSizes.c
//   Tests whether the sizes used by the C99 standard integer types and the
// sizes used by the Oracle integer types (ub8, ub4, etc.) match.
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <oci.h>
#include "dpi.h"

//-----------------------------------------------------------------------------
// ValidateSize()
//   Validate that the size of the C99 standard integer type and the equivalent
// Oracle type match.
//-----------------------------------------------------------------------------
int ValidateSize(const char *standardName, int standardSize,
        const char *oracleName, int oracleSize)
{
    if (standardSize != oracleSize) {
        printf("*** ERROR *** Types %s (%d) and %s (%d) do not match!\n",
                standardName, standardSize, oracleName, oracleSize);
        return -1;
    }
    printf("Types %s and %s match\n", standardName, oracleName);
    return 0;
}


//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    if (ValidateSize("uint64_t", sizeof(uint64_t), "ub8", sizeof(ub8)) < 0)
        return 1;
    if (ValidateSize("uint32_t", sizeof(uint32_t), "ub4", sizeof(ub4)) < 0)
        return 1;
    if (ValidateSize("uint16_t", sizeof(uint16_t), "ub2", sizeof(ub2)) < 0)
        return 1;
    if (ValidateSize("uint8_t", sizeof(uint8_t), "ub1", sizeof(ub1)) < 0)
        return 1;

    if (ValidateSize("int64_t", sizeof(int64_t), "sb8", sizeof(sb8)) < 0)
        return 1;
    if (ValidateSize("int32_t", sizeof(int32_t), "sb4", sizeof(sb4)) < 0)
        return 1;
    if (ValidateSize("int16_t", sizeof(int16_t), "sb2", sizeof(sb2)) < 0)
        return 1;
    if (ValidateSize("int8_t", sizeof(int8_t), "sb1", sizeof(sb1)) < 0)
        return 1;

    return 0;
}

