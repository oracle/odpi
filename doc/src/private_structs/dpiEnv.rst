.. _dpiEnv:

ODPI-C Private Structure dpiEnv
-------------------------------

This private structure is used to represent the OCI environment. A pointer to
this structure is stored on each handle exposed publicly but it is created only
when a :ref:`dpiPool<dpiPool>` structure is created or when a
:ref:`dpiConn<dpiConn>` structure is created for a standalone connection.
Connections acquired from a session pool share the environment of the pool and
all other handles share the environment from the handle which created them. All
of the attributes are initialized when the environment is created and never
changed after that. The functions for managing the environment are found in
dpiEnv.c.

.. member:: const dpiContext \*dpiEnv.context

    Specifies a pointer to the :ref:`dpiContext<dpiContext>` structure which
    was used for the creation of the environment.

.. member:: void \*dpiEnv.handle

    Specifies the OCI environment handle.

.. member:: dpiMutexType dpiEnv.mutex

    Specifies the mutex used for controlling access to the reference count for
    each handle exposed publicly when the OCI environment is using OCI_THREADED
    mode.

.. member:: char dpiEnv.encoding[]

    The encoding used for CHAR data, as a null-terminated ASCII string.

.. member:: int32_t dpiEnv.maxBytesPerCharacter

    The maximum number of bytes required for each character in the encoding
    used for CHAR data. This value is used when calculating the size of
    buffers required when lengths in characters are provided.

.. member:: uint16_t dpiEnv.charsetId

    The Oracle character set id used for CHAR data.

.. member:: char dpiEnv.nencoding[]

    The encoding used for NCHAR data, as a null-terminated ASCII string.

.. member:: int32_t dpiEnv.nmaxBytesPerCharacter

    The maximum number of bytes required for each character in the encoding
    used for NCHAR data. Since this information is not directly available
    from Oracle it is only accurate if the encodings used for CHAR and NCHAR
    data are identical or one of ASCII or UTF-8; otherwise a value of 4 is
    assumed. This value is used when calculating the size of buffers required
    when lengths in characters are provided.

.. member:: uint16_t dpiEnv.ncharsetId

    The Oracle character set id used for NCHAR data.

.. member:: dpiHandlePool \*dpiEnv.errorHandles

    A pointer to a structure of type :ref:`dpiHandlePool <dpiHandlePool>`
    which manages a pool of OCI error handles in a thread-safe manner.

.. member:: void \*dpiEnv.versionInfo

    A pointer to a structure of type :ref:`dpiVersionInfo <dpiVersionInfo>`
    which contains the OCI client version that was used to create the
    environment.

.. member:: void \*dpiEnv.baseDate

    Specifies the base date (midnight on January 1, 1970 UTC) used for
    converting timestamps from Oracle into a number representing the number of
    seconds since the Unix "epoch".

.. member:: int dpiEnv.threaded

    Specifies whether the environment was created with DPI_OCI_THREADED mode
    (1) or not (0).

