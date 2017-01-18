//-----------------------------------------------------------------------------
// Copyright (c) 2016 Oracle and/or its affiliates.  All rights reserved.
// This program is free software: you can modify it and/or redistribute it
// under the terms of:
//
// (i)  the Universal Permissive License v 1.0 or at your option, any
//      later version (http://oss.oracle.com/licenses/upl); and/or
//
// (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// dpiOracleType.c
//   Implementation of variable types.
//-----------------------------------------------------------------------------

#include "dpiImpl.h"

//-----------------------------------------------------------------------------
// definition of Oracle types (MUST be in same order as enumeration)
//-----------------------------------------------------------------------------
static const dpiOracleType
        dpiAllOracleTypes[DPI_ORACLE_TYPE_MAX - DPI_ORACLE_TYPE_NONE - 1] = {
    {
        DPI_ORACLE_TYPE_VARCHAR,            // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_CHR,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NVARCHAR,           // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_CHR,                           // internal Oracle type
        SQLCS_NCHAR,                        // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_CHAR,               // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_AFC,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NCHAR,              // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_AFC,                           // internal Oracle type
        SQLCS_NCHAR,                        // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_ROWID,              // public Oracle type
        DPI_NATIVE_TYPE_ROWID,              // default native type
        SQLT_RDD,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIRowid*),                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_RAW,                // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_BIN,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        0,                                  // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NATIVE_FLOAT,       // public Oracle type
        DPI_NATIVE_TYPE_FLOAT,              // default native type
        SQLT_BFLOAT,                        // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(float),                      // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NATIVE_DOUBLE,      // public Oracle type
        DPI_NATIVE_TYPE_DOUBLE,             // default native type
        SQLT_BDOUBLE,                       // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(double),                     // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NATIVE_INT,         // public Oracle type
        DPI_NATIVE_TYPE_INT64,              // default native type
        SQLT_INT,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(int64_t),                    // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NUMBER,             // public Oracle type
        DPI_NATIVE_TYPE_DOUBLE,             // default native type
        SQLT_VNU,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCINumber),                  // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_DATE,               // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        SQLT_ODT,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIDate),                    // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_TIMESTAMP,          // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        SQLT_TIMESTAMP,                     // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIDateTime*),               // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_TIMESTAMP_TZ,       // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        SQLT_TIMESTAMP_TZ,                  // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIDateTime*),               // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_TIMESTAMP_LTZ,      // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        SQLT_TIMESTAMP_LTZ,                 // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIDateTime*),               // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_INTERVAL_DS,        // public Oracle type
        DPI_NATIVE_TYPE_INTERVAL_DS,        // default native type
        SQLT_INTERVAL_DS,                   // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIInterval*),               // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_INTERVAL_YM,        // public Oracle type
        DPI_NATIVE_TYPE_INTERVAL_YM,        // default native type
        SQLT_INTERVAL_YM,                   // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIInterval*),               // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_CLOB,               // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        SQLT_CLOB,                          // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCILobLocator*),             // buffer size
        1,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NCLOB,              // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        SQLT_CLOB,                          // internal Oracle type
        SQLCS_NCHAR,                        // charset form
        sizeof(OCILobLocator*),             // buffer size
        1,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_BLOB,               // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        SQLT_BLOB,                          // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCILobLocator*),             // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_BFILE,              // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        SQLT_BFILE,                         // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCILobLocator*),             // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_STMT,               // public Oracle type
        DPI_NATIVE_TYPE_STMT,               // default native type
        SQLT_RSET,                          // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(OCIStmt*),                   // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_BOOLEAN,            // public Oracle type
        DPI_NATIVE_TYPE_BOOLEAN,            // default native type
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
        SQLT_BOL,                           // internal Oracle type
#else
        SQLT_INT,                           // internal Oracle type
#endif
        SQLCS_IMPLICIT,                     // charset form
        sizeof(boolean),                    // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_OBJECT,             // public Oracle type
        DPI_NATIVE_TYPE_OBJECT,             // default native type
        SQLT_NTY,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        sizeof(dvoid*),                     // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_LONG_VARCHAR,       // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_CHR,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        DPI_MAX_BASIC_BUFFER_SIZE + 1,      // buffer size
        1,                                  // is character data
        0,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_LONG_NVARCHAR,      // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_CHR,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        DPI_MAX_BASIC_BUFFER_SIZE + 1,      // buffer size
        1,                                  // is character data
        0,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_LONG_RAW,           // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        SQLT_BIN,                           // internal Oracle type
        SQLCS_IMPLICIT,                     // charset form
        DPI_MAX_BASIC_BUFFER_SIZE + 1,      // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        0                                   // requires pre-fetch
    }
};


//-----------------------------------------------------------------------------
// dpiOracleType__getFromNum() [INTERNAL]
//   Return the variable type associated with the type number.
//-----------------------------------------------------------------------------
const dpiOracleType *dpiOracleType__getFromNum(dpiOracleTypeNum typeNum,
        dpiError *error)
{
    if (typeNum > DPI_ORACLE_TYPE_NONE && typeNum < DPI_ORACLE_TYPE_MAX)
        return &dpiAllOracleTypes[typeNum - DPI_ORACLE_TYPE_NONE - 1];
    dpiError__set(error, "check type", DPI_ERR_INVALID_ORACLE_TYPE, typeNum);
    return NULL;
}


//-----------------------------------------------------------------------------
// dpiOracleType__getFromObjectTypeInfo() [INTERNAL]
//   Return the variable type given the Oracle data type (used within object
// types).
//-----------------------------------------------------------------------------
const dpiOracleType *dpiOracleType__getFromObjectTypeInfo(OCITypeCode typeCode,
        dpiError *error)
{
    switch(typeCode) {
        case OCI_TYPECODE_CHAR:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_CHAR, error);
        case OCI_TYPECODE_VARCHAR:
        case OCI_TYPECODE_VARCHAR2:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_VARCHAR, error);
        case OCI_TYPECODE_INTEGER:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_INT, error);
        case OCI_TYPECODE_NUMBER:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NUMBER, error);
        case OCI_TYPECODE_DATE:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_DATE, error);
        case OCI_TYPECODE_TIMESTAMP:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP, error);
        case OCI_TYPECODE_TIMESTAMP_TZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_TZ,
                    error);
        case OCI_TYPECODE_TIMESTAMP_LTZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_LTZ,
                    error);
        case OCI_TYPECODE_OBJECT:
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
        case OCI_TYPECODE_RECORD:
#endif
        case OCI_TYPECODE_NAMEDCOLLECTION:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_OBJECT, error);
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12, 1)
        case OCI_TYPECODE_BOOLEAN:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BOOLEAN, error);
#endif
    }
    dpiError__set(error, "check object type info", DPI_ERR_UNHANDLED_DATA_TYPE,
            typeCode);
    return NULL;
}


//-----------------------------------------------------------------------------
// dpiOracleType__getFromQueryInfo() [INTERNAL]
//   Return the variable type given the Oracle data type (used within a query).
//-----------------------------------------------------------------------------
const dpiOracleType *dpiOracleType__getFromQueryInfo(uint16_t oracleDataType,
        uint8_t charsetForm, dpiError *error)
{
    switch(oracleDataType) {
        case SQLT_CHR:
            if (charsetForm == SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NVARCHAR,
                        error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_VARCHAR, error);
        case SQLT_NUM:
        case SQLT_VNU:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NUMBER, error);
        case SQLT_BIN:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_RAW, error);
        case SQLT_DAT:
        case SQLT_ODT:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_DATE, error);
        case SQLT_AFC:
            if (charsetForm == SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NCHAR, error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_CHAR, error);
        case SQLT_DATE:
        case SQLT_TIMESTAMP:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP, error);
        case SQLT_TIMESTAMP_TZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_TZ,
                    error);
        case SQLT_TIMESTAMP_LTZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_LTZ,
                    error);
        case SQLT_INTERVAL_DS:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_INTERVAL_DS,
                    error);
        case SQLT_INTERVAL_YM:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_INTERVAL_YM,
                    error);
        case SQLT_CLOB:
            if (charsetForm == SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NCLOB, error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_CLOB, error);
        case SQLT_BLOB:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BLOB, error);
        case SQLT_BFILE:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BFILE, error);
        case SQLT_RSET:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_STMT, error);
        case SQLT_NTY:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_OBJECT, error);
        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_FLOAT,
                    error);
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_DOUBLE,
                    error);
        case SQLT_RDD:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_ROWID, error);
        case SQLT_LNG:
            if (charsetForm == SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_LONG_NVARCHAR,
                        error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_LONG_VARCHAR,
                    error);
        case SQLT_LBI:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_LONG_RAW, error);
    }
    dpiError__set(error, "check query info", DPI_ERR_UNHANDLED_DATA_TYPE,
            oracleDataType);
    return NULL;
}

