.. _dpiJsonNode:

ODPI-C Structure dpiJsonNode
----------------------------

This structure is used for passing JSON node data to and from the database in
the structure :ref:`dpiData<dpiData>`. Nodes can consist of scalar values,
objects, or arrays.

.. member:: dpiOracleTypeNum dpiJsonNode.oracleTypeNum

    The Oracle type of the data that is stored in the node. It will be one of
    the values from the enumeration :ref:`dpiOracleTypeNum<dpiOracleTypeNum>`.

.. member:: dpiNativeTypeNum dpiJsonNode.nativeTypeNum

    The native type of the data that is stored in the node. It will be one of
    the values from the enumeration :ref:`dpiNativeTypeNum<dpiNativeTypeNum>`.

.. member:: dpiDataBuffer* dpiJsonNode.value

    The value that is stored in the node. It is a union of type
    :ref:`dpiDataBuffer<dpiDataBuffer>`.
