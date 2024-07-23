.. _dpiVectorFormat:

ODPI-C Enumeration dpiVectorFormat
----------------------------------

This enumeration identifies the storage format for a vector's dimensions.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiVectorFormat
     enumeration. The second column displays the description of the
     dpiVectorFormat enumeration value.

    * - Value
      - Description
    * - DPI_VECTOR_FORMAT_BINARY
      - The vector dimension storage format is single bits, represented in
        groups of 8 as single byte unsigned integers.
    * - DPI_VECTOR_FORMAT_FLOAT32
      - The vector dimension storage format is single-precision floating point
        numbers.
    * - DPI_VECTOR_FORMAT_FLOAT64
      - The vector dimension storage format is double-precision floating point
        numbers.
    * - DPI_VECTOR_FORMAT_INT8
      - The vector dimension storage format is single byte signed integers.
