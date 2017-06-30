.. _dpiOracleType:

ODPI-C Private Structure dpiOracleType
--------------------------------------

This private structure is used to identify the different types of Oracle data
that the library supports. A list of these structures (defined as constants)
can be found in dpiOracleType.c. The enumeration
:ref:`dpiOracleTypeNum<dpiOracleTypeNum>` is used to identify the structures.

.. member:: dpiOracleTypeNum dpiOracleType.oracleTypeNum

    Specifies the value from the enumeration
    :ref:`dpiOracleTypeNum<dpiOracleTypeNum>` which identifies the type of data
    being represented.

.. member:: dpiNativeTypeNum dpiOracleType.defaultNativeTypeNum

    Specifies the default native type that is associated with the Oracle type.
    This will be one of the values from the enumeration
    :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`. Some of the Oracle types are
    capable of being represented using multiple native types but most are
    capable of being represented only by one of them.

.. member:: uint16_t dpiOracleType.oracleType

    Specifies the OCI type constant used to represent the Oracle type.

.. member:: uint8_t dpiOracleType.charsetForm

    Specifies the OCI character set form constant used to represent the Oracle
    type. This will be one of SQLCS_IMPLICIT (encoding) or SQLS_NCHAR (national
    encoding).

.. member:: uint32_t dpiOracleType.sizeInBytes

    Specifies the size in bytes of the value used by Oracle to represent the
    Oracle type. This value is zero for variable length data (such as strings
    and raw byte strings) where the size is provided by the calling application
    or driver.

.. member:: int dpiOracleType.isCharacterData

    Specifies if the type refers to character data (1) or not (0). This flag is
    used when calculating buffer sizes to determine if the size in characters
    should be multiplied by the maximum number of bytes for each character for
    the encoding or national encoding.

.. member:: int dpiOracleType.canBeInArray

    Specifies if the type is allowed to be found in a PL/SQL index-by table (1)
    or not (0).

.. member:: int dpiOracleType.requiresPreFetch

    Specifies if additional processing is required for the type when performing
    fetches (1) or not (0).

