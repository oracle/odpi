.. _dpiShardingKeyColumn:

ODPI-C Structure dpiShardingKeyColumn
-------------------------------------

This structure is used for passing sharding key column values to the database.
It is part of the structure :ref:`dpiConnCreateParams<dpiConnCreateParams>`.

.. member:: dpiOracleTypeNum dpiShardingKeyColumn.oracleTypeNum

    Specifies the Oracle type of the column which makes up the sharding key. It
    is expected to be one of the values from the enumeration
    :ref:`dpiOracleTypeNum<dpiOracleTypeNum>` but currently only the value
    DPI_ORACLE_TYPE_VARCHAR is supported.

.. member:: dpiNativeTypeNum dpiShardingKeyColumn.nativeTypeNum

    Specifies the native type of the column which makes up the sharding key. It
    is expected to be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>` but currently only the value
    DPI_NATIVE_TYPE_BYTES is supported.

.. member:: dpiDataBuffer dpiShardingKeyColumn.value

    Specifies the value of the column which makes up the sharding key. It is a
    union of type :ref:`dpiDataBuffer<dpiDataBuffer>` and the member of the
    union that is set must correspond to the value of the member
    :member:`dpiShardingKeyColumn.nativeTypeNum`.

