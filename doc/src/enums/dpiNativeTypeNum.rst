.. _dpiNativeTypeNum:

ODPI-C Enumeration dpiNativeTypeNum
-----------------------------------

This enumeration identifies the type of data that is being transferred to and
from the database. It is used in the structures
:ref:`dpiDataTypeInfo<dpiDataTypeInfo>` and
:ref:`dpiShardingKeyColumn<dpiShardingKeyColumn>`.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_NATIVE_TYPE_INT64        Data is passed as a 64-bit integer in the asInt64
                             member of dpiData.value.
DPI_NATIVE_TYPE_UINT64       Data is passed as an unsigned 64-bit integer in
                             the asUint64 member of dpiData.value.
DPI_NATIVE_TYPE_FLOAT        Data is passed as a single precision floating
                             point number in the asFloat member of
                             dpiData.value.
DPI_NATIVE_TYPE_DOUBLE       Data is passed as a double precision floating
                             point number in the asDouble member of
                             dpiData.value.
DPI_NATIVE_TYPE_BYTES        Data is passed as a byte string in the asBytes
                             member of dpiData.value.
DPI_NATIVE_TYPE_TIMESTAMP    Data is passed as a timestamp in the asTimestamp
                             member of dpiData.value.
DPI_NATIVE_TYPE_INTERVAL_DS  Data is passed as an interval (days to seconds)
                             in the asIntervalDS member of dpiData.value.
DPI_NATIVE_TYPE_INTERVAL_YM  Data is passed as an interval (years to months)
                             in the asIntervalYM member of dpiData.value.
DPI_NATIVE_TYPE_LOB          Data is passed as a reference to a LOB in the
                             asLOB member of dpiData.value.
DPI_NATIVE_TYPE_OBJECT       Data is passed as a reference to an object in the
                             asObject member of dpiData.value.
DPI_NATIVE_TYPE_STMT         Data is passed as a reference to a statement in
                             the asStmt member of dpiData.value.
DPI_NATIVE_TYPE_BOOLEAN      Data is passed as a boolean value in the
                             asBoolean member of dpiData.value.
DPI_NATIVE_TYPE_ROWID        Data is passed as a reference to a rowid in the
                             asRowid member of dpiData.value.
===========================  ==================================================

