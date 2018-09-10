.. _dpiSodaOutputOptions:

ODPI-C Public Structure dpiSodaOutputOptions
--------------------------------------------

This structure is used for passing information back from a SODA operation to
the application. It is used in the function :func:`dpiSodaColl_insertMany()`.

.. member:: uint64_t dpiSodaOutputOptions.numDocuments

    Specifies the number of documents that were processed by the operation.

