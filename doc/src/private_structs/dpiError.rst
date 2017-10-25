.. _dpiError:

ODPI-C Private Structure dpiError
---------------------------------

This private structure is used for managing all errors that take place in the
library.  The implementation of the functions that use this structure are
included in dpiError.c. An instance of this structure is passed to each private
function that is called and the first thing that takes place in every public
function is a call to get the error structure.

.. member:: dpiErrorBuffer \*dpiError.buffer

    Specifies a pointer to the :ref:`dpiErrorBuffer<dpiErrorBuffer>` structure
    where error information is to be stored. If this value is NULL, the error
    buffer for the current thread will be looked up and used when an actual
    error is being raised.

.. member:: void \*dpiError.handle

    Specifies the OCI error handle which is used for all OCI calls that require
    one.

.. member:: dpiEnv \*dpiError.env

    Specifies a pointer to the :ref:`dpiEnv <dpiEnv>` structure which was used
    to create the OCI error handle, or NULL if no OCI error handle is required
    for the currently executing function.

