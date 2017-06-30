.. _dpiError:

ODPI-C Private Structure dpiError
---------------------------------

This private structure is used for managing all errors that take place in the
library.  The implementation of the functions that use this structure are
included in dpiError.c. An instance of this structure is passed to each private
function that is called and the first thing that takes place in every public
function is a call to get the error structure. One instance is stored on every
environment that is created and, if the environment is threaded, an instance is
also stored for each thread using the OCI functions OCIThreadKeyGet() and
OCIThreadKeySet().

.. member:: dpiErrorBuffer \*dpiError.buffer

    Specifies a pointer to the :ref:`dpiErrorBuffer<dpiErrorBuffer>` structure
    where error information is to be stored. If this value is NULL, the error
    buffer for the current thread will be looked up and used when an actual
    error is being raised.

.. member:: OCIError \*dpiError.handle

    Specifies the OCI error handle which is used for all OCI calls that require
    one.

.. member:: const char \*dpiError.encoding

    Specifies the encoding that is used by all OCI errors that are raised. This
    is a pointer to the value of the :member:`dpiEncodingInfo.encoding` member
    associated with the environment used by this error structure.

.. member:: uint16_t dpiError.charsetId

    The Oracle character set id used for CHAR data. This is used to determine
    if the encoding is UTF-16 which requires special processing.

