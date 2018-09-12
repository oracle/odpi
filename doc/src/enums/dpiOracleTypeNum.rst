.. _dpiOracleTypeNum:

ODPI-C Enumeration dpiOracleTypeNum
-----------------------------------

This enumeration identifies the types of Oracle data that can be used for
binding data as arguments to a statement, fetching data from the database, or
getting and setting object attributes and element values.

=============================  ================================================
Value                          Description
=============================  ================================================
DPI_ORACLE_TYPE_VARCHAR        Default type used for VARCHAR2 columns in the
                               database. Data is transferred to/from Oracle as
                               byte strings in the encoding used for CHAR data.
DPI_ORACLE_TYPE_NVARCHAR       Default type used for NVARCHAR2 columns in the
                               database. Data is transferred to/from Oracle as
                               byte strings in the encoding used for NCHAR
                               data.
DPI_ORACLE_TYPE_CHAR           Default type used for CHAR columns in the
                               database. Data is transferred to/from Oracle as
                               byte strings in the encoding used for CHAR data.
DPI_ORACLE_TYPE_NCHAR          Default type used for NCHAR columns in the
                               database. Data is transferred to/from Oracle as
                               byte strings in the encoding used for NCHAR
                               data.
DPI_ORACLE_TYPE_ROWID          Default type used for the pseudocolumn "ROWID".
                               Data is transferred to/from Oracle as byte
                               strings, in the encoding used for CHAR data.
DPI_ORACLE_TYPE_RAW            Default type used for RAW columns in the
                               database. Data is transferred to/from Oracle as
                               raw byte strings.
DPI_ORACLE_TYPE_NATIVE_FLOAT   Default type used for BINARY_FLOAT columns in
                               the database. Data is transferred to/from Oracle
                               as the C float type.
DPI_ORACLE_TYPE_NATIVE_DOUBLE  Default type used for BINARY_DOUBLE columns in
                               the database. Data is transferred to/from Oracle
                               as the C double type.
DPI_ORACLE_TYPE_NATIVE_INT     Type available for binding native integers
                               directly in PL/SQL (such as PLS_INTEGER). Data
                               is transferred to/from Oracle as 64-bit
                               integers.
DPI_ORACLE_TYPE_NATIVE_UINT    Type available for binding native integers
                               directly in PL/SQL (such as PLS_INTEGER). Data
                               is transferred to/from Oracle as 64-bit
                               unsigned integers.
DPI_ORACLE_TYPE_NUMBER         Default type used for NUMBER columns in the
                               database. Data is transferred to/from Oracle in
                               Oracle's internal format.
DPI_ORACLE_TYPE_DATE           Default type used for DATE columns in the
                               database. Data is transferred to/from Oracle in
                               Oracle's internal format.
DPI_ORACLE_TYPE_TIMESTAMP      Default type used for TIMESTAMP columns in the
                               database. Data is transferred to/from Oracle in
                               Oracle's internal format.
DPI_ORACLE_TYPE_TIMESTAMP_TZ   Default type used for TIMESTAMP WITH TIME ZONE
                               columns in the database. Data is transferred
                               to/from Oracle in Oracle's internal format.
DPI_ORACLE_TYPE_TIMESTAMP_LTZ  Default type used for TIMESTAMP WITH LOCAL TIME
                               ZONE columns in the database. Data is
                               transferred to/from Oracle in Oracle's internal
                               format.
DPI_ORACLE_TYPE_INTERVAL_DS    Default type used for INTERVAL DAY TO SECOND
                               columns in the database. Data is transferred
                               to/from Oracle in Oracle's internal format.
DPI_ORACLE_TYPE_INTERVAL_YM    Default type used for INTERVAL YEAR TO MONTH
                               columns in the database. Data is transferred
                               to/from Oracle in Oracle's internal format.
DPI_ORACLE_TYPE_CLOB           Default type used for CLOB columns in the
                               database. Only a locator is transferred to/from
                               Oracle, which can subsequently be used via
                               dpiLob references to read/write from that
                               locator.
DPI_ORACLE_TYPE_NCLOB          Default type used for NCLOB columns in the
                               database. Only a locator is transferred to/from
                               Oracle, which can subsequently be used via
                               dpiLob references to read/write from that
                               locator.
DPI_ORACLE_TYPE_BLOB           Default type used for BLOB columns in the
                               database. Only a locator is transferred to/from
                               Oracle, which can subsequently be used via
                               dpiLob references to read/write from that
                               locator.
DPI_ORACLE_TYPE_BFILE          Default type used for BFILE columns in the
                               database. Only a locator is transferred to/from
                               Oracle, which can subsequently be used via
                               dpiLob references to read/write from that
                               locator.
DPI_ORACLE_TYPE_STMT           Used within PL/SQL for REF CURSOR or within SQL
                               for querying a CURSOR. Only a handle is
                               transferred to/from Oracle, which can
                               subsequently be used via dpiStmt for querying.
DPI_ORACLE_TYPE_BOOLEAN        Used within PL/SQL for boolean values. This is
                               only available in 12.1. Earlier releases simply
                               use the integer values 0 and 1 to represent a
                               boolean value. Data is transferred to/from
                               Oracle as an integer.
DPI_ORACLE_TYPE_OBJECT         Default type used for named type columns in the
                               database. Data is transferred to/from Oracle in
                               Oracle's internal format.
DPI_ORACLE_TYPE_LONG_VARCHAR   Default type used for LONG columns in the
                               database. Data is transferred to/from Oracle as
                               byte strings in the encoding used for CHAR data.
DPI_ORACLE_TYPE_LONG_RAW       Default type used for LONG RAW columns in the
                               database. Data is transferred to/from Oracle as
                               raw byte strings.
=============================  ================================================

