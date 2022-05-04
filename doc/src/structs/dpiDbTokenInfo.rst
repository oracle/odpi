.. _dpiDbTokenInfo:

ODPI-C Structure dpiDbTokenInfo
--------------------------------

This structure is used when creating connection pools and standalone
connections using token based authentication. Oracle Client libraries
must be 19.14 (or later), or 21.5 (or later).

The externalAuth must be set to 1 and in addition, homogeneous must be
set to 1 when creating a pool. The user (or username) and password properties
should not be set.

Both dbToken and dbTokenPrivateKey (and their lengths) must be set.

.. member:: const char* dpiDbTokenInfo.dbToken

    Specifies the database authentication token string used for token based
    authentication.

.. member:: uint32_t dpiDbTokenInfo.dbTokenLength

    Specifies the database authentication token string length used for token
    based authentication.

.. member:: const char* dpiDbTokenInfo.dbTokenPrivateKey

    Specifies the database authentication private key string used for token based
    authentication.

.. member:: uint32_t dpiDbTokenInfo.dbTokenPrivateKeyLength

    Specifies the database authentication private key string length used for token
    based authentication.
