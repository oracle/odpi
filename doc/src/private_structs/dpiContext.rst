.. _dpiContext:

ODPI-C Private Structure dpiContext
-----------------------------------

This private structure represents the context in which all activity in the
library takes place. The implementation for this type is found in dpiContext.c.
A context is created by calling the function :func:`dpiContext_create()` and is
destroyed by calling the function :func:`dpiContext_destroy()`. All of the
attributes of the structure :ref:`dpiBaseType<dpiBaseType>` are included in
this structure in addition to the ones specific to this structure described
below.

.. member:: dpiVersionInfo \*dpiContext.versionInfo

    Specifies the OCI client version that is being used. It is a pointer to a
    structure of type :ref:`dpiVersionInfo<dpiVersionInfo>`.

.. member:: uint8_t dpiContext.dpiMinorVersion

    Specifies the minor version of the ODPI-C library used by the application.
    This is used to adjust for public structure changes when needed.

