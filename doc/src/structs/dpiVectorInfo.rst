.. _dpiVectorInfo:

ODPI-C Structure dpiVectorInfo
-------------------------------

This structure is used for providing or receiving information about a vector.

.. member:: uint8_t dpiVectorInfo.format

    Specifies the storage format for the vector. It should be one of the values
    from the enumeration :ref:`dpiVectorFormat<dpiVectorFormat>`.

.. member:: uint32_t dpiVectorInfo.numDimensions

    Specifies the number of dimensions the vector contains.

.. member:: uint8_t dpiVectorInfo.dimensionSize

    Specifies the size (in bytes) of each dimension of the vector. This value
    does not need to be specified on input but is populated on output as a
    convenience.

.. member:: dpiVectorDimensionBuffer dpiVectorInfo.dimensions

    Specifies the buffer for the dimensions. On input the buffer must remain
    valid for the duration of the call into ODPI-C. On output the buffer will
    remain valid as long as the vector object remains valid and the contents of
    the vector are not changed. See the
    :ref:`dpiVectorDimensionBuffer<dpiVectorDimensionBuffer>` union for more
    information.

    For dense vectors, the length of this array is defined by the
    :member:`~dpiVectorInfo.numDimensions` member. For sparse vectors, the
    length of this array is defined by the
    :member:`~dpiVectorInfo.numSparseValues` member.

.. member:: uint32_t dpiVectorInfo.numSparseValues

    Specifies the number of values that a sparse vector contains that are
    non-zero. If this value is 0, the vector is considered a dense vector.

.. member:: uint32_t* dpiVectorInfo.sparseIndices

    Specifies an array of sparse vector indices. Each element in this array is
    the index of a non-zero value in the vector. The length of this array is
    defined by the :member:`~dpiVectorInfo.numSparseValues` member. If the
    vector is not a sparse vector, this value will be NULL.
