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
        DPI_SQLT_CHR,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NVARCHAR,           // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        DPI_SQLT_CHR,                       // internal Oracle type
        DPI_SQLCS_NCHAR,                    // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_CHAR,               // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        DPI_SQLT_AFC,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NCHAR,              // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        DPI_SQLT_AFC,                       // internal Oracle type
        DPI_SQLCS_NCHAR,                    // charset form
        0,                                  // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_ROWID,              // public Oracle type
        DPI_NATIVE_TYPE_ROWID,              // default native type
        DPI_SQLT_RDD,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        1,                                  // is character data
        1,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_RAW,                // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        DPI_SQLT_BIN,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        0,                                  // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NATIVE_FLOAT,       // public Oracle type
        DPI_NATIVE_TYPE_FLOAT,              // default native type
        DPI_SQLT_BFLOAT,                    // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(float),                      // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NATIVE_DOUBLE,      // public Oracle type
        DPI_NATIVE_TYPE_DOUBLE,             // default native type
        DPI_SQLT_BDOUBLE,                   // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(double),                     // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NATIVE_INT,         // public Oracle type
        DPI_NATIVE_TYPE_INT64,              // default native type
        DPI_SQLT_INT,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(int64_t),                    // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NUMBER,             // public Oracle type
        DPI_NATIVE_TYPE_DOUBLE,             // default native type
        DPI_SQLT_VNU,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        DPI_OCI_NUMBER_SIZE,                // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_DATE,               // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        DPI_SQLT_ODT,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(dpiOciDate),                 // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_TIMESTAMP,          // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        DPI_SQLT_TIMESTAMP,                 // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_TIMESTAMP_TZ,       // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        DPI_SQLT_TIMESTAMP_TZ,              // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_TIMESTAMP_LTZ,      // public Oracle type
        DPI_NATIVE_TYPE_TIMESTAMP,          // default native type
        DPI_SQLT_TIMESTAMP_LTZ,             // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_INTERVAL_DS,        // public Oracle type
        DPI_NATIVE_TYPE_INTERVAL_DS,        // default native type
        DPI_SQLT_INTERVAL_DS,               // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_INTERVAL_YM,        // public Oracle type
        DPI_NATIVE_TYPE_INTERVAL_YM,        // default native type
        DPI_SQLT_INTERVAL_YM,               // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        1,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_CLOB,               // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        DPI_SQLT_CLOB,                      // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        1,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NCLOB,              // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        DPI_SQLT_CLOB,                      // internal Oracle type
        DPI_SQLCS_NCHAR,                    // charset form
        sizeof(void*),                      // buffer size
        1,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_BLOB,               // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        DPI_SQLT_BLOB,                      // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_BFILE,              // public Oracle type
        DPI_NATIVE_TYPE_LOB,                // default native type
        DPI_SQLT_BFILE,                     // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_STMT,               // public Oracle type
        DPI_NATIVE_TYPE_STMT,               // default native type
        DPI_SQLT_RSET,                      // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_BOOLEAN,            // public Oracle type
        DPI_NATIVE_TYPE_BOOLEAN,            // default native type
        DPI_SQLT_BOL,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(int),                        // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_OBJECT,             // public Oracle type
        DPI_NATIVE_TYPE_OBJECT,             // default native type
        DPI_SQLT_NTY,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(void*),                      // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        1                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_LONG_VARCHAR,       // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        DPI_SQLT_CHR,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        DPI_MAX_BASIC_BUFFER_SIZE + 1,      // buffer size
        1,                                  // is character data
        0,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_LONG_RAW,           // public Oracle type
        DPI_NATIVE_TYPE_BYTES,              // default native type
        DPI_SQLT_BIN,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        DPI_MAX_BASIC_BUFFER_SIZE + 1,      // buffer size
        0,                                  // is character data
        0,                                  // can be in array
        0                                   // requires pre-fetch
    },
    {
        DPI_ORACLE_TYPE_NATIVE_UINT,        // public Oracle type
        DPI_NATIVE_TYPE_UINT64,             // default native type
        DPI_SQLT_UIN,                       // internal Oracle type
        DPI_SQLCS_IMPLICIT,                 // charset form
        sizeof(uint64_t),                   // buffer size
        0,                                  // is character data
        1,                                  // can be in array
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
const dpiOracleType *dpiOracleType__getFromObjectTypeInfo(uint16_t typeCode,
        uint8_t charsetForm, dpiError *error)
{
    switch(typeCode) {
        case DPI_SQLT_AFC:
            if (charsetForm == DPI_SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NCHAR, error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_CHAR, error);
        case DPI_SQLT_CHR:
        case DPI_SQLT_VCS:
            if (charsetForm == DPI_SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NVARCHAR,
                        error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_VARCHAR, error);
        case DPI_SQLT_INT:
        case DPI_OCI_TYPECODE_SMALLINT:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_INT, error);
        case DPI_SQLT_FLT:
        case DPI_SQLT_NUM:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NUMBER, error);
        case DPI_SQLT_IBFLOAT:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_FLOAT,
                    error);
        case DPI_SQLT_IBDOUBLE:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_DOUBLE,
                    error);
        case DPI_SQLT_DAT:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_DATE, error);
        case DPI_SQLT_TIMESTAMP:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP, error);
        case DPI_SQLT_TIMESTAMP_TZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_TZ,
                    error);
        case DPI_SQLT_TIMESTAMP_LTZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_LTZ,
                    error);
        case DPI_SQLT_NTY:
        case DPI_SQLT_REC:
        case DPI_SQLT_NCO:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_OBJECT, error);
        case DPI_SQLT_BOL:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BOOLEAN, error);
        case DPI_SQLT_CLOB:
            if (charsetForm == DPI_SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NCLOB, error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_CLOB, error);
        case DPI_SQLT_BLOB:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BLOB, error);
        case DPI_SQLT_BFILE:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BFILE, error);
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
        case DPI_SQLT_CHR:
            if (charsetForm == DPI_SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NVARCHAR,
                        error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_VARCHAR, error);
        case DPI_SQLT_NUM:
        case DPI_SQLT_VNU:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NUMBER, error);
        case DPI_SQLT_BIN:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_RAW, error);
        case DPI_SQLT_DAT:
        case DPI_SQLT_ODT:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_DATE, error);
        case DPI_SQLT_AFC:
            if (charsetForm == DPI_SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NCHAR, error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_CHAR, error);
        case DPI_SQLT_DATE:
        case DPI_SQLT_TIMESTAMP:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP, error);
        case DPI_SQLT_TIMESTAMP_TZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_TZ,
                    error);
        case DPI_SQLT_TIMESTAMP_LTZ:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_TIMESTAMP_LTZ,
                    error);
        case DPI_SQLT_INTERVAL_DS:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_INTERVAL_DS,
                    error);
        case DPI_SQLT_INTERVAL_YM:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_INTERVAL_YM,
                    error);
        case DPI_SQLT_CLOB:
            if (charsetForm == DPI_SQLCS_NCHAR)
                return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NCLOB, error);
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_CLOB, error);
        case DPI_SQLT_BLOB:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BLOB, error);
        case DPI_SQLT_BFILE:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_BFILE, error);
        case DPI_SQLT_RSET:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_STMT, error);
        case DPI_SQLT_NTY:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_OBJECT, error);
        case DPI_SQLT_BFLOAT:
        case DPI_SQLT_IBFLOAT:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_FLOAT,
                    error);
        case DPI_SQLT_BDOUBLE:
        case DPI_SQLT_IBDOUBLE:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_NATIVE_DOUBLE,
                    error);
        case DPI_SQLT_RDD:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_ROWID, error);
        case DPI_SQLT_LNG:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_LONG_VARCHAR,
                    error);
        case DPI_SQLT_LBI:
            return dpiOracleType__getFromNum(DPI_ORACLE_TYPE_LONG_RAW, error);
    }
    dpiError__set(error, "check query info", DPI_ERR_UNHANDLED_DATA_TYPE,
            oracleDataType);
    return NULL;
}

