.. _dpiData:

ODPI-C Structure dpiData
------------------------

This structure is used for passing data to and from the database for variables
and for manipulating object attributes and collection values.

.. member:: int dpiData.isNull

    Specifies if the value refers to a null value (1) or not (0).

.. member:: dpiDataBuffer dpiData.value

    Specifies the value that is being passed or received. It is only examined
    when the member :member:`dpiData.isNull` is 1. It is a union of type
    :ref:`dpiDataBuffer<dpiDataBuffer>`.

