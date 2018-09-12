.. _dpiVersionInfo:

ODPI-C Structure dpiVersionInfo
-------------------------------

This structure is used for returning Oracle version information about the
Oracle Client (:func:`dpiContext_getClientVersion()`) and Oracle Database
(:func:`dpiConn_getServerVersion()`).

.. member:: int dpiVersionInfo.versionNum

    Specifies the major version of the Oracle Client or Database.

.. member:: int dpiVersionInfo.releaseNum

    Specifies the release version of the Oracle Client or Database.

.. member:: int dpiVersionInfo.updateNum

    Specifies the update version of the Oracle Client or Database.

.. member:: int dpiVersionInfo.portReleaseNum

    Specifies the port specific release version of the Oracle Client or
    Database.

.. member:: int dpiVersionInfo.portUpdateNum

    Specifies the port specific update version of the Oracle Client or
    Database.

.. member:: int dpiVersionInfo.fullVersionNum

    Specifies the full version (all five components) as a number that is
    suitable for comparison with the result of the macro
    DPI_ORACLE_VERSION_TO_NUMBER.

