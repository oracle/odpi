.. _dpiPoolGetMode:

ODPI-C Public Enumeration dpiPoolGetMode
----------------------------------------

This enumeration identifies the mode to use when getting sessions from a
session pool.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_MODE_POOL_GET_WAIT       Specifies that the caller should block until a
                             session is available from the pool.
DPI_MODE_POOL_GET_NOWAIT     Specifies that the caller should return
                             immediately, regardless of whether a session is
                             available in the pool. If a session is not
                             available an error is returned.
DPI_MODE_POOL_GET_FORCEGET   Specifies that a new session should be created if
                             all of the sessions in the pool are busy, even if
                             this exceeds the maximum sessions allowable for
                             the session pool (see
                             :member:`dpiPoolCreateParams.maxSessions`)
===========================  ==================================================

