.. _dpiShutdownMode:

ODPI-C Enumeration dpiShutdownMode
----------------------------------

This enumeration identifies the mode to use when shutting down a database
using :func:`dpiConn_shutdownDatabase()`.

======================================  =======================================
Value                                   Description
======================================  =======================================
DPI_MODE_SHUTDOWN_DEFAULT               Further connections to the database are
                                        prohibited. Wait for users to
                                        disconnect from the database.
DPI_MODE_SHUTDOWN_TRANSACTIONAL         Further connections to the database are
                                        prohibited and no new transactions are
                                        allowed to be started. Wait for active
                                        transactions to complete.
DPI_MODE_SHUTDOWN_TRANSACTIONAL_LOCAL   Behaves the same way as
                                        DPI_MODE_SHUTDOWN_TRANSACTIONAL but
                                        only waits for local transactions to
                                        complete.
DPI_MODE_SHUTDOWN_IMMEDIATE             All uncommitted transactions are
                                        terminated and rolled back and all
                                        connections to the database are closed
                                        immediately.
DPI_MODE_SHUTDOWN_ABORT                 All uncommitted transactions are
                                        terminated and are not rolled back.
                                        This is the fastest way to shut down
                                        the database but the next database
                                        startup may require instance recovery.
DPI_MODE_SHUTDOWN_FINAL                 Shuts down the database. This mode
                                        should only be used in the second call
                                        to :func:`dpiConn_shutdownDatabase()`.
======================================  =======================================

