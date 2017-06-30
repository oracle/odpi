.. _dpiLob:

ODPI-C Private Structure dpiLob
-------------------------------

This private structure represents large objects (CLOB, BLOB, NCLOB and BFILE)
and is available by handle to a calling application or driver. The
implementation for this type is found in dpiLob.c. A temporary LOB can be
created by calling the function :func:`dpiConn_newTempLob()` but generally LOBs
are created implicitly by creating a variable of type DPI_ORACLE_TYPE_CLOB,
DPI_ORACLE_TYPE_NCLOB, DPI_ORACLE_TYPE_BLOB or DPI_ORACLE_TYPE_BFILE is
created. They are destroyed when the last reference is released by calling the
function :func:`dpiLob_release()`. All of the attributes of the structure
:ref:`dpiBaseType<dpiBaseType>` are included in this structure in addition to
the ones specific to this structure described below.

.. member:: dpiConn \*dpiLob.conn

    Specifies a pointer to the :ref:`dpiConn<dpiConn>` structure which was used
    to create the LOB.

.. member:: const dpiOracleType \*dpiLob.type

    Specifies a pointer to a :ref:`dpiOracleType<dpiOracleType>` structure
    which identifies the type of Oracle data that is being represented by this
    LOB.

.. member:: OCILobLocator \*dpiLob.locator

    Specifies the OCILobLocator handle.

.. member:: char \*dpiLob.buffer

    Specifies a buffer used for storing the directory alias name and file name
    of a BFILE type LOB, when that information is requested by means of calling
    the function :func:`dpiLob_getDirectoryAndFileName()`. In all other cases
    this value is NULL.

