.. _dpiContext:

ODPI-C Private Structure dpiContext
-----------------------------------

This private structure represents the context in which all activity in the
library takes place. The implementation for this type is found in dpiContext.c.
A context is created by calling the function :func:`dpiContext_create()` and is
destroyed by calling the function :func:`dpiContext_destroy()`.

.. member:: uint32_t dpiContext.checkInt

    Specifies a check integer which is used to verify that the handle passed by
    the application or driver is valid. It is set to 0 when the context is
    destroyed and to the value DPI_CONTEXT_CHECK_INT when the context is
    created.

.. member:: dpiVersionInfo \*dpiContext.versionInfo

    Specifies the OCI client version that is being used. It is a pointer to a
    structure of type :ref:`dpiVersionInfo<dpiVersionInfo>`.

.. member:: uint8_t dpiContext.dpiMinorVersion

    Specifies the minor version of the ODPI-C library used by the application.
    This is used to adjust for public structure changes when needed.

