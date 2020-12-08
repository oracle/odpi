.. _dpiJsonObject:

ODPI-C Structure dpiJsonObject
------------------------------

This structure is used for passing JSON objects to and from the database.

.. member:: uint32_t dpiJsonObject.numFields

    Specifies the number of fields that are found in the JSON object. This also
    identifies the number of elements in each of the remaining arrays.

.. member:: char** dpiJsonObject.fieldNames

    Specifies an array of field names that are found in the JSON object. Each
    element is a byte string encoded in UTF-8. The length of each byte string
    is found in the :member:`~dpiJsonObject.fieldNameLengths` member.

.. member:: uint32_t* dpiJsonObject.fieldNameLengths

    Specifies an array of lengths for the names of the fields found in the JSON
    object.

.. member:: dpiJsonNode* dpiJsonObject.fields

    Specifies an array of JSON nodes that are the values of the fields found in
    the JSON object. Each of these is a structure of type
    :ref:`dpiJsonNode<dpiJsonNode>`.

.. member:: dpiDataBuffer* dpiJsonObject.fieldValues

    Specifies an array of buffers that contain the data for the values of the
    fields found in the JSON object. This member should not be used directly.
    Instead, the :member:`dpiJsonNode.value` member of the corresponding node
    in the member :member:`~dpiJsonObject.fields` should be used.
