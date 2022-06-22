.. _dpiAccessToken:

ODPI-C Structure dpiAccessToken
-------------------------------

This structure is used when creating connection pools and standalone
connections using token based authentication. Oracle Client libraries
must be 19.14 (or later), or 21.5 (or later).

The externalAuth must be set to 1 and in addition, homogeneous must be
set to 1 when creating a pool. The user (or username) and password properties
should not be set.

Both token and privateKey (and their lengths) must be set while using IAM
for token based authentication.

Only token and it's length must be set while using OAuth for token based
authentication.

.. member:: const char* dpiAccessToken.token

    Specifies the database authentication token string used for token based
    authentication.

.. member:: uint32_t dpiAccessToken.tokenLength

    Specifies the database authentication token string length used for token
    based authentication.

.. member:: const char* dpiAccessToken.privateKey

    Specifies the database authentication private key string used for token
    based authentication.

.. member:: uint32_t dpiAccessToken.privateKeyLength

    Specifies the database authentication private key string length used for
    token based authentication.
