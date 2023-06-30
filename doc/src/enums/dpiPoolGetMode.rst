.. _dpiPoolGetMode:

ODPI-C Enumeration dpiPoolGetMode
---------------------------------

This enumeration identifies the mode to use when getting sessions from a
session pool.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiPoolGetMode
     enumeration. The second column displays the description of the
     dpiPoolGetMode enumeration value.

    * - Value
      - Description
    * - DPI_MODE_POOL_GET_FORCEGET
      - Specifies that a new session should be created if all of the sessions
        in the pool are busy, even if this exceeds the maximum sessions
        allowable for the session pool (see
        :member:`dpiPoolCreateParams.maxSessions`).
    * - DPI_MODE_POOL_GET_NOWAIT
      - Specifies that the caller should return immediately, regardless of
        whether a session is available in the pool. If a session is not
        available an error is returned.
    * - DPI_MODE_POOL_GET_TIMEDWAIT
      - Specifies that the caller should block until a session is available
        from the pool, but only for the specified length of time defined in
        :member:`dpiPoolCreateParams.waitTimeout`. If a session is not
        available within the specified period of time an error is returned.
    * - DPI_MODE_POOL_GET_WAIT
      - Specifies that the caller should block until a session is available
        from the pool.
