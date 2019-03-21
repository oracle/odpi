.. _dpiData:

ODPI-C Structure dpiData
------------------------

This structure is used for passing data to and from the database in variables
and for getting and setting object attributes and collection values. The data
that is stored in the structure will remain valid for differing durations
depending on the source of the data.

For data that is supplied by the application (for passing data into the
database) the duration of the data is under the control of the application
itself and the application should ensure that it remains valid for as long as
any call that uses that data.

For data that is supplied by the database in a variable (either fetching data
or binding data out) the data will remain valid as long as a reference to the
variable is retained; the contents of the data may change however if a
statement to which the variable is bound or defined is executed or fetched
again.

For data that is acquired from an object attribute or a collection element the
data will remain valid as long as a reference to the object is retained; the
contents of the data may change however if the object is modified.

.. member:: int dpiData.isNull

    Specifies if the value refers to a null value (1) or not (0).

.. member:: dpiDataBuffer dpiData.value

    Specifies the value that is being passed or received. It is only examined
    when the member :member:`dpiData.isNull` is 0. It is a union of type
    :ref:`dpiDataBuffer<dpiDataBuffer>`.
