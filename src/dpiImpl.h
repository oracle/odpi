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
// dpiImpl.h
//   Master include file for implementation of ODPI-C library.
//-----------------------------------------------------------------------------

#ifndef DPI_IMPL
#define DPI_IMPL

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <oci.h>
#include <orid.h>
#include <xa.h>
#include "dpi.h"

// validate OCI library
#if !defined(OCI_MAJOR_VERSION) || (OCI_MAJOR_VERSION < 11) || \
        ((OCI_MAJOR_VERSION == 11) && (OCI_MINOR_VERSION < 2))
#error Oracle 11.2 or later client libraries are required for building
#endif

// define simple way to respresent Oracle client version
#define DPI_ORACLE_CLIENT_VERSION(major, minor)    ((major << 8) | minor)
#define DPI_ORACLE_CLIENT_VERSION_HEX \
        DPI_ORACLE_CLIENT_VERSION(OCI_MAJOR_VERSION, OCI_MINOR_VERSION)

// define max error size
#ifdef OCI_ERROR_MAXMSG_SIZE2
#define DPI_MAX_ERROR_SIZE              OCI_ERROR_MAXMSG_SIZE2
#else
#define DPI_MAX_ERROR_SIZE              OCI_ERROR_MAXMSG_SIZE
#endif

// define size and methods to call for bind/define
#if DPI_ORACLE_CLIENT_VERSION_HEX >= DPI_ORACLE_CLIENT_VERSION(12,1)
    #define OCIBINDBYNAME               OCIBindByName2
    #define OCIBINDBYPOS                OCIBindByPos2
    #define OCIDEFINEBYPOS              OCIDefineByPos2
    #define DPI_ACTUAL_LENGTH_TYPE      uint32_t
#else
    #define OCIBINDBYNAME               OCIBindByName
    #define OCIBINDBYPOS                OCIBindByPos
    #define OCIDEFINEBYPOS              OCIDefineByPos
    #define DPI_ACTUAL_LENGTH_TYPE      uint16_t
#endif

// define context name for ping interval
#define DPI_CONTEXT_LAST_TIME_USED      "DPI_LAST_TIME_USED"

// define size of buffer used for numbers transferred to/from Oracle as text
#define DPI_NUMBER_AS_TEXT_CHARS        135

// define maximum number of digits possible in an Oracle number
#define DPI_NUMBER_MAX_DIGITS           40

// define maximum size in bytes supported by basic string handling
#define DPI_MAX_BASIC_BUFFER_SIZE       32767

// define internal chunk size used for dynamic binding/fetching
#define DPI_DYNAMIC_BYTES_CHUNK_SIZE    65536

// define well-known character sets
#define DPI_CHARSET_ID_ASCII            1
#define DPI_CHARSET_ID_UTF8             873
#define DPI_CHARSET_ID_UTF16            1000
#define DPI_CHARSET_NAME_ASCII          "ASCII"
#define DPI_CHARSET_NAME_UTF8           "UTF-8"
#define DPI_CHARSET_NAME_UTF16          "UTF-16"
#define DPI_CHARSET_NAME_UTF16LE        "UTF-16LE"
#define DPI_CHARSET_NAME_UTF16BE        "UTF-16BE"


//-----------------------------------------------------------------------------
// Enumerations
//-----------------------------------------------------------------------------

// error numbers
typedef enum {
    DPI_ERR_NO_ERR = 1000,
    DPI_ERR_NO_MEMORY,
    DPI_ERR_INVALID_HANDLE,
    DPI_ERR_ERR_NOT_INITIALIZED,
    DPI_ERR_GET_FAILED,
    DPI_ERR_CREATE_ENV,
    DPI_ERR_CONVERT_TEXT,
    DPI_ERR_QUERY_NOT_EXECUTED,
    DPI_ERR_UNHANDLED_DATA_TYPE,
    DPI_ERR_ARRAY_SIZE_EXCEEDED,
    DPI_ERR_NOT_CONNECTED,
    DPI_ERR_CONN_NOT_IN_POOL,
    DPI_ERR_INVALID_PROXY,
    DPI_ERR_NOT_SUPPORTED,
    DPI_ERR_UNHANDLED_CONVERSION,
    DPI_ERR_ARRAY_SIZE_TOO_BIG,
    DPI_ERR_INVALID_DATE,
    DPI_ERR_VALUE_IS_NULL,
    DPI_ERR_ARRAY_SIZE_TOO_SMALL,
    DPI_ERR_BUFFER_SIZE_TOO_SMALL,
    DPI_ERR_VERSION_NOT_SUPPORTED,
    DPI_ERR_INVALID_ORACLE_TYPE,
    DPI_ERR_WRONG_ATTR,
    DPI_ERR_NOT_COLLECTION,
    DPI_ERR_INVALID_INDEX,
    DPI_ERR_NO_OBJECT_TYPE,
    DPI_ERR_INVALID_CHARSET,
    DPI_ERR_SCROLL_OUT_OF_RS,
    DPI_ERR_QUERY_POSITION_INVALID,
    DPI_ERR_NO_ROW_FETCHED,
    DPI_ERR_TLS_ERROR,
    DPI_ERR_ARRAY_SIZE_ZERO,
    DPI_ERR_EXT_AUTH_WITH_CREDENTIALS,
    DPI_ERR_CANNOT_GET_ROW_OFFSET,
    DPI_ERR_CONN_IS_EXTERNAL,
    DPI_ERR_TRANS_ID_TOO_LARGE,
    DPI_ERR_BRANCH_ID_TOO_LARGE,
    DPI_ERR_COLUMN_FETCH,
    DPI_ERR_OCI_VERSION_NOT_SUPPORTED,
    DPI_ERR_STMT_CLOSED,
    DPI_ERR_LOB_CLOSED,
    DPI_ERR_INVALID_CHARSET_ID,
    DPI_ERR_INVALID_OCI_NUMBER,
    DPI_ERR_INVALID_NUMBER,
    DPI_ERR_NUMBER_TOO_LARGE,
    DPI_ERR_NUMBER_STRING_TOO_LONG,
    DPI_ERR_NULL_POINTER_PARAMETER,
    DPI_ERR_MAX
} dpiErrorNum;

// handle types
typedef enum {
    DPI_HTYPE_NONE = 4000,
    DPI_HTYPE_CONN,
    DPI_HTYPE_POOL,
    DPI_HTYPE_STMT,
    DPI_HTYPE_VAR,
    DPI_HTYPE_LOB,
    DPI_HTYPE_OBJECT,
    DPI_HTYPE_OBJECT_TYPE,
    DPI_HTYPE_OBJECT_ATTR,
    DPI_HTYPE_SUBSCR,
    DPI_HTYPE_DEQ_OPTIONS,
    DPI_HTYPE_ENQ_OPTIONS,
    DPI_HTYPE_MSG_PROPS,
    DPI_HTYPE_ROWID,
    DPI_HTYPE_MAX
} dpiHandleTypeNum;


//-----------------------------------------------------------------------------
// Internal implementation type definitions
//-----------------------------------------------------------------------------
typedef struct {
    int32_t code;
    uint16_t offset;
    dpiErrorNum dpiErrorNum;
    const char *fnName;
    const char *action;
    char encoding[OCI_NLS_MAXBUFSZ];
    char message[DPI_MAX_ERROR_SIZE];
    uint32_t messageLength;
    boolean isRecoverable;
} dpiErrorBuffer;

typedef struct {
    dpiErrorBuffer *buffer;
    OCIError *handle;
    const char *encoding;
    uint16_t charsetId;
} dpiError;

typedef struct {
    const dpiContext *context;
    OCIEnv *handle;
    OCIThreadMutex *mutex;
    OCIThreadKey *threadKey;
    OCIError *errorHandle;
    char encoding[OCI_NLS_MAXBUFSZ];
    int32_t maxBytesPerCharacter;
    uint16_t charsetId;
    char nencoding[OCI_NLS_MAXBUFSZ];
    int32_t nmaxBytesPerCharacter;
    uint16_t ncharsetId;
    OCIDateTime *baseDate;
    int threaded;
} dpiEnv;

typedef void (*dpiTypeFreeProc)(void*, dpiError*);

typedef struct {
    const char *name;
    size_t size;
    uint32_t checkInt;
    dpiTypeFreeProc freeProc;
} dpiTypeDef;

#define dpiType_HEAD \
    const dpiTypeDef *typeDef; \
    uint32_t checkInt; \
    unsigned refCount; \
    dpiEnv *env;

typedef struct {
    dpiType_HEAD
} dpiBaseType;

typedef struct dpiOracleType {
    dpiOracleTypeNum oracleTypeNum;
    dpiNativeTypeNum defaultNativeTypeNum;
    uint16_t oracleType;
    uint8_t charsetForm;
    uint32_t sizeInBytes;
    int isCharacterData;
    int canBeInArray;
    int requiresPreFetch;
} dpiOracleType;

typedef struct {
    char *ptr;
    uint32_t length;
    uint32_t allocatedLength;
} dpiDynamicBytesChunk;

typedef struct {
    uint32_t numChunks;
    uint32_t allocatedChunks;
    dpiDynamicBytesChunk *chunks;
} dpiDynamicBytes;

typedef struct {
    dpiVar *var;
    uint32_t pos;
    const char *name;
    uint32_t nameLength;
} dpiBindVar;


//-----------------------------------------------------------------------------
// External implementation type definitions
//-----------------------------------------------------------------------------
struct dpiPool {
    dpiType_HEAD
    OCISPool *handle;
    const char *name;
    uint32_t nameLength;
    int pingInterval;
    int pingTimeout;
    int homogeneous;
    int externalAuth;
};

struct dpiConn {
    dpiType_HEAD
    dpiPool *pool;
    OCISvcCtx *handle;
    OCIServer *serverHandle;
    OCISession *sessionHandle;
    const char *releaseString;
    uint32_t releaseStringLength;
    int versionNum;
    int releaseNum;
    int updateNum;
    int portReleaseNum;
    int portUpdateNum;
    uint32_t commitMode;
    int externalHandle;
    int dropSession;
    int standalone;
};

struct dpiContext {
    uint32_t checkInt;
    unsigned int majorVersion;
    unsigned int minorVersion;
};

struct dpiStmt {
    dpiType_HEAD
    dpiConn *conn;
    OCIStmt *handle;
    uint32_t fetchArraySize;
    uint32_t bufferRowCount;
    uint32_t bufferRowIndex;
    uint32_t numQueryVars;
    dpiVar **queryVars;
    dpiQueryInfo *queryInfo;
    uint32_t allocatedBindVars;
    uint32_t numBindVars;
    dpiBindVar *bindVars;
    uint32_t numBatchErrors;
    dpiErrorBuffer *batchErrors;
    uint64_t rowCount;
    uint64_t bufferMinRow;
    uint16_t statementType;
    int isOwned;
    int hasRowsToFetch;
    int scrollable;
    int isReturning;
    int deleteFromCache;
};

typedef union {
    void *asRaw;
    char *asBytes;
    float *asFloat;
    double *asDouble;
    int64_t *asInt64;
    OCINumber *asNumber;
    OCIDate *asDate;
    OCIDateTime **asTimestamp;
    OCIInterval **asInterval;
    OCILobLocator **asLobLocator;
    OCIString **asString;
    OCIStmt **asStmt;
    OCIRowid **asRowid;
    boolean *asBoolean;
    void **asObject;
    OCIColl **asCollection;
} dpiOracleData;

typedef union {
    int64_t asInt64;
    float asFloat;
    double asDouble;
    OCINumber asNumber;
    OCIDate asDate;
    boolean asBoolean;
    OCIString *asString;
    OCIDateTime *asTimestamp;
    void *asRaw;
} dpiOracleDataBuffer;

typedef union {
    void *asHandle;
    dpiObject *asObject;
    dpiStmt *asStmt;
    dpiLob *asLOB;
    dpiRowid *asRowid;
} dpiReferenceBuffer;

struct dpiVar {
    dpiType_HEAD
    dpiConn *conn;
    const dpiOracleType *type;
    dpiNativeTypeNum nativeTypeNum;
    uint32_t maxArraySize;
    uint32_t actualArraySize;
    int requiresPreFetch;
    int isArray;
    int16_t *indicator;
    uint16_t *returnCode;
    DPI_ACTUAL_LENGTH_TYPE *actualLength;
#if DPI_ORACLE_CLIENT_VERSION_HEX < DPI_ORACLE_CLIENT_VERSION(12,1)
    uint32_t *dynamicActualLength;
#endif
    uint32_t sizeInBytes;
    int isDynamic;
    dpiObjectType *objectType;
    dvoid **objectIndicator;
    dpiReferenceBuffer *references;
    dpiDynamicBytes *dynamicBytes;
    char *tempBuffer;
    dpiData *externalData;
    dpiOracleData data;
    dpiError *error;
};

struct dpiLob {
    dpiType_HEAD
    dpiConn *conn;
    const dpiOracleType *type;
    OCILobLocator *locator;
    char *buffer;
};

struct dpiObjectAttr {
    dpiType_HEAD
    dpiObjectType *belongsToType;
    const char *name;
    uint32_t nameLength;
    const dpiOracleType *oracleType;
    dpiObjectType *type;
};

struct dpiObjectType {
    dpiType_HEAD
    dpiConn *conn;
    OCIType *tdo;
    OCITypeCode typeCode;
    const char *schema;
    uint32_t schemaLength;
    const char *name;
    uint32_t nameLength;
    const dpiOracleType *elementOracleType;
    dpiObjectType *elementType;
    boolean isCollection;
    uint16_t numAttributes;
};

struct dpiObject {
    dpiType_HEAD
    dpiObjectType *type;
    dvoid *instance;
    dvoid *indicator;
    int isIndependent;
};

struct dpiRowid {
    dpiType_HEAD
    OCIRowid *handle;
    char *buffer;
    uint16_t bufferLength;
};

struct dpiSubscr {
    dpiType_HEAD
    dpiConn *conn;
    OCISubscription *handle;
    dpiSubscrQOS qos;
    dpiSubscrCallback callback;
    void *callbackContext;
};

struct dpiDeqOptions {
    dpiType_HEAD
    dpiConn *conn;
    OCIAQDeqOptions *handle;
};

struct dpiEnqOptions {
    dpiType_HEAD
    dpiConn *conn;
    OCIAQEnqOptions *handle;
};

struct dpiMsgProps {
    dpiType_HEAD
    dpiConn *conn;
    OCIAQMsgProperties *handle;
};


//-----------------------------------------------------------------------------
// definition of internal dpiContext methods
//-----------------------------------------------------------------------------
int dpiContext__initCommonCreateParams(const dpiContext *context,
        dpiCommonCreateParams *params, dpiError *error);
int dpiContext__initConnCreateParams(const dpiContext *context,
        dpiConnCreateParams *params, dpiError *error);
int dpiContext__initPoolCreateParams(const dpiContext *context,
        dpiPoolCreateParams *params, dpiError *error);
int dpiContext__initSubscrCreateParams(const dpiContext *context,
        dpiSubscrCreateParams *params, dpiError *error);
int dpiContext__startPublicFn(const dpiContext *context, const char *fnName,
        dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiData methods
//-----------------------------------------------------------------------------
int dpiData__fromOracleDate(dpiData *data, OCIDate *oracleValue);
int dpiData__fromOracleIntervalDS(dpiData *data, dpiEnv *env, dpiError *error,
        OCIInterval *oracleValue);
int dpiData__fromOracleIntervalYM(dpiData *data, dpiEnv *env, dpiError *error,
        OCIInterval *oracleValue);
int dpiData__fromOracleNumberAsDouble(dpiData *data, dpiEnv *env,
        dpiError *error, OCINumber *oracleValue);
int dpiData__fromOracleNumberAsInteger(dpiData *data, dpiEnv *env,
        dpiError *error, OCINumber *oracleValue);
int dpiData__fromOracleNumberAsText(dpiData *data, dpiVar *var,
        uint32_t pos, dpiError *error, OCINumber *oracleValue);
int dpiData__fromOracleTimestamp(dpiData *data, dpiEnv *env, dpiError *error,
        OCIDateTime *oracleValue, int withTZ);
int dpiData__fromOracleTimestampAsDouble(dpiData *data, dpiEnv *env,
        dpiError *error, OCIDateTime *oracleValue);
int dpiData__toOracleDate(dpiData *data, OCIDate *oracleValue);
int dpiData__toOracleIntervalDS(dpiData *data, dpiEnv *env, dpiError *error,
        OCIInterval *oracleValue);
int dpiData__toOracleIntervalYM(dpiData *data, dpiEnv *env, dpiError *error,
        OCIInterval *oracleValue);
int dpiData__toOracleNumberFromDouble(dpiData *data, dpiEnv *env,
        dpiError *error, OCINumber *oracleValue);
int dpiData__toOracleNumberFromInteger(dpiData *data, dpiEnv *env,
        dpiError *error, OCINumber *oracleValue);
int dpiData__toOracleNumberFromText(dpiData *data, dpiEnv *env,
        dpiError *error, OCINumber *oracleValue);
int dpiData__toOracleTimestamp(dpiData *data, dpiEnv *env, dpiError *error,
        OCIDateTime *oracleValue, int withTZ);
int dpiData__toOracleTimestampFromDouble(dpiData *data, dpiEnv *env,
        dpiError *error, OCIDateTime *oracleValue);


//-----------------------------------------------------------------------------
// definition of internal dpiEnv methods
//-----------------------------------------------------------------------------
void dpiEnv__free(dpiEnv *env, dpiError *error);
int dpiEnv__init(dpiEnv *env, const dpiContext *context,
        const dpiCommonCreateParams *params, dpiError *error);
int dpiEnv__getEncodingInfo(dpiEnv *env, dpiEncodingInfo *info);
int dpiEnv__initError(dpiEnv *env, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiError methods
//-----------------------------------------------------------------------------
int dpiError__check(dpiError *error, sword status, dpiConn *conn,
        const char *context);
int dpiError__getInfo(dpiError *error, dpiErrorInfo *info);
int dpiError__set(dpiError *error, const char *context, dpiErrorNum errorNum,
        ...);


//-----------------------------------------------------------------------------
// definition of internal dpiGen methods
//-----------------------------------------------------------------------------
int dpiGen__addRef(void *ptr, dpiHandleTypeNum typeNum, const char *fnName);
int dpiGen__allocate(dpiHandleTypeNum typeNum, dpiEnv *env, void **handle,
        dpiError *error);
int dpiGen__checkHandle(void *ptr, dpiHandleTypeNum typeNum,
        const char *context, dpiError *error);
int dpiGen__release(void *ptr, dpiHandleTypeNum typeNum, const char *fnName);
int dpiGen__setRefCount(void *ptr, dpiError *error, int increment);
int dpiGen__startPublicFn(void *ptr, dpiHandleTypeNum typeNum,
        const char *context, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiGlobal methods
//-----------------------------------------------------------------------------
int dpiGlobal__initError(const char *fnName, dpiError *error);
int dpiGlobal__lookupCharSet(const char *name, uint16_t *charsetId,
        dpiError *error);
int dpiGlobal__lookupEncoding(uint16_t charsetId, char *encoding,
        dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiOracleType methods
//-----------------------------------------------------------------------------
const dpiOracleType *dpiOracleType__getFromNum(dpiOracleTypeNum oracleTypeNum,
        dpiError *error);
const dpiOracleType *dpiOracleType__getFromQueryInfo(uint16_t oracleDataType,
        uint8_t charsetForm, dpiError *error);
const dpiOracleType *dpiOracleType__getFromObjectTypeInfo(OCITypeCode typeCode,
        dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiConn methods
//-----------------------------------------------------------------------------
void dpiConn__free(dpiConn *conn, dpiError *error);
int dpiConn__get(dpiConn *conn, const char *userName, uint32_t userNameLength,
        const char *password, uint32_t passwordLength,
        const char *connectString, uint32_t connectStringLength,
        dpiConnCreateParams *createParams, dpiPool *pool, dpiError *error);
int dpiConn__setAttributesFromCommonCreateParams(const dpiContext *context,
        void *handle, uint32_t handleType, const dpiCommonCreateParams *params,
        dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiPool methods
//-----------------------------------------------------------------------------
int dpiPool__acquireConnection(dpiPool *pool, const char *userName,
        uint32_t userNameLength, const char *password, uint32_t passwordLength,
        dpiConnCreateParams *params, dpiConn **conn, dpiError *error);
void dpiPool__free(dpiPool *pool, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiStmt methods
//-----------------------------------------------------------------------------
int dpiStmt__allocate(dpiConn *conn, int scrollable, dpiStmt **stmt,
        dpiError *error);
void dpiStmt__free(dpiStmt *stmt, dpiError *error);
int dpiStmt__init(dpiStmt *stmt, dpiError *error);
int dpiStmt__prepare(dpiStmt *stmt, const char *sql, uint32_t sqlLength,
        const char *tag, uint32_t tagLength, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiVar methods
//-----------------------------------------------------------------------------
int dpiVar__allocate(dpiConn *conn, dpiOracleTypeNum oracleTypeNum,
        dpiNativeTypeNum nativeTypeNum, uint32_t maxArraySize, uint32_t size,
        int sizeIsBytes, int isArray, dpiObjectType *objType, dpiVar **var,
        dpiData **data, dpiError *error);
int dpiVar__convertToLob(dpiVar *var, dpiError *error);
int dpiVar__copyData(dpiVar *var, uint32_t pos, dpiData *sourceData,
        dpiError *error);
int32_t dpiVar__defineCallback(dpiVar *var, OCIDefine *defnp, uint32_t iter,
        void **bufpp, uint32_t **alenpp, uint8_t *piecep, void **indpp,
        uint16_t **rcodepp);
int dpiVar__extendedPreFetch(dpiVar *var, dpiError *error);
void dpiVar__free(dpiVar *var, dpiError *error);
int32_t dpiVar__inBindCallback(dpiVar *var, OCIBind *bindp, uint32_t iter,
        uint32_t index, void **bufpp, uint32_t *alenp, uint8_t *piecep,
        void **indpp);
int dpiVar__getValue(dpiVar *var, uint32_t pos, dpiData *data,
        dpiError *error);
int dpiVar__setValue(dpiVar *var, uint32_t pos, dpiData *data,
        dpiError *error);
int32_t dpiVar__outBindCallback(dpiVar *var, OCIBind *bindp, uint32_t iter,
        uint32_t index, void **bufpp, uint32_t **alenpp, uint8_t *piecep,
        void **indpp, uint16_t **rcodepp);


//-----------------------------------------------------------------------------
// definition of internal dpiLob methods
//-----------------------------------------------------------------------------
int dpiLob__allocate(dpiConn *conn, const dpiOracleType *type, dpiLob **lob,
        dpiError *error);
int dpiLob__createTemporary(dpiLob *lob, dpiError *error);
void dpiLob__free(dpiLob *lob, dpiError *error);
int dpiLob__readBytes(dpiLob *lob, uint64_t offset, uint64_t amount,
        char *value, uint64_t *valueLength, dpiError *error);
int dpiLob__setFromBytes(dpiLob *lob, const char *value, uint64_t valueLength,
        dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiObject methods
//-----------------------------------------------------------------------------
int dpiObject__allocate(dpiObjectType *objType, dvoid *instance,
        dvoid *indicator, int isIndependent, dpiObject **obj, dpiError *error);
void dpiObject__free(dpiObject *obj, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiObjectType methods
//-----------------------------------------------------------------------------
int dpiObjectType__allocate(dpiConn *conn, OCIParam *param,
        uint32_t nameAttribute, dpiObjectType **objType, dpiError *error);
void dpiObjectType__free(dpiObjectType *objType, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiObjectAttr methods
//-----------------------------------------------------------------------------
int dpiObjectAttr__allocate(dpiObjectType *objType, OCIParam *param,
        dpiObjectAttr **attr, dpiError *error);
int dpiObjectAttr__check(dpiObjectAttr *attr, dpiError *error);
void dpiObjectAttr__free(dpiObjectAttr *attr, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiRowid methods
//-----------------------------------------------------------------------------
int dpiRowid__allocate(dpiConn *conn, dpiRowid **rowid, dpiError *error);
void dpiRowid__free(dpiRowid *rowid, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiSubscr methods
//-----------------------------------------------------------------------------
void dpiSubscr__free(dpiSubscr *subscr, dpiError *error);
int dpiSubscr__create(dpiSubscr *subscr, dpiConn *conn,
        dpiSubscrCreateParams *params, uint32_t *subscrId, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiDeqOptions methods
//-----------------------------------------------------------------------------
int dpiDeqOptions__create(dpiDeqOptions *options, dpiConn *conn,
        dpiError *error);
void dpiDeqOptions__free(dpiDeqOptions *options, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiEnqOptions methods
//-----------------------------------------------------------------------------
int dpiEnqOptions__create(dpiEnqOptions *options, dpiConn *conn,
        dpiError *error);
void dpiEnqOptions__free(dpiEnqOptions *options, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiMsgProps methods
//-----------------------------------------------------------------------------
int dpiMsgProps__create(dpiMsgProps *props, dpiConn *conn, dpiError *error);
void dpiMsgProps__free(dpiMsgProps *props, dpiError *error);


//-----------------------------------------------------------------------------
// definition of internal dpiUtils methods
//-----------------------------------------------------------------------------
int dpiUtils__getAttrStringWithDup(dpiError *error, const char *context,
        const void *ociHandle, uint32_t ociHandleType, uint32_t ociAttribute,
        const char **value, uint32_t *valueLength);
int dpiUtils__parseNumberString(const char *value, uint32_t valueLength,
        uint16_t charsetId, int *isNegative, int16_t *decimalPointIndex,
        uint8_t *numDigits, uint8_t *digits, dpiError *error);
int dpiUtils__parseOracleNumber(OCINumber *oracleValue, int *isNegative,
        int16_t *decimalPointIndex, uint8_t *numDigits, uint8_t *digits,
        dpiError *error);

#endif

