.. _dpiJsonArray:

ODPI-C Structure dpiJsonArray
-----------------------------

This structure is used for passing JSON objects to and from the database.

.. member:: uint32_t dpiJsonArray.numElements

    Specifies the number of elements that are found in the JSON array. This
    also identifies the number of elements in each of the remaining arrays.

.. member:: dpiJsonNode* dpiJsonArray.elements

    Specifies an array of JSON nodes that are the values of the elements found
    in the JSON array. Each of these is a structure of type
    :ref:`dpiJsonNode<dpiJsonNode>`.

.. member:: dpiDataBuffer* dpiJsonArray.elementValues

    Specifies an array of buffers that contain the data for the values of the
    elements found in the JSON array. This member should not be used directly.
    Instead, the :member:`dpiJsonNode.value` member of the corresponding node
    in the member :member:`~dpiJsonArray.elements` should be used.
