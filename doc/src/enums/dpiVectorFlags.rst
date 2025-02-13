.. _dpiVectorFlags:

ODPI-C Enumeration dpiVectorFlags
----------------------------------

This enumeration identifies the possible values for
:member:`dpiDataTypeInfo.vectorFlags`.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiVectorFlags
     enumeration. The second column displays the description of the
     dpiVectorFlags enumeration value.

    * - Value
      - Description
    * - DPI_VECTOR_FLAGS_FLEXIBLE_DIM
      - The vector column uses a flexible number of dimensions.
    * - DPI_VECTOR_FLAGS_SPARSE
      - The vector column contains sparse vectors
