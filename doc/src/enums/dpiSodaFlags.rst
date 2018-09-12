.. _dpiSodaFlags:

ODPI-C Enumeration dpiSodaFlags
-------------------------------

This enumeration identifies the flags that can be used with SODA functions.

================================  =============================================
Value                             Description
================================  =============================================
DPI_SODA_FLAGS_DEFAULT            Default value.
DPI_SODA_FLAGS_ATOMIC_COMMIT      If the function completes successfully the
                                  current transaction is committed. If it does
                                  not complete successfully, no changes to the
                                  database are made and the existing
                                  transaction is left untouched.
DPI_SODA_FLAGS_CREATE_COLL_MAP    Create collection in MAP mode. This mode is
                                  only supported in
                                  :func:`dpiSodaDb_createCollection()`.
DPI_SODA_FLAGS_INDEX_DROP_FORCE   Forcibly drop the index. This mode is only
                                  supported in
                                  :func:`dpiSodaColl_dropIndex()`.
================================  =============================================

