.. _dpiAnnotation:

ODPI-C Structure dpiAnnotation
------------------------------

This structure is part of the structure :ref:`dpiDataTypeInfo<dpiDataTypeInfo>`
and provides information about `annotations
<https://docs.oracle.com/en/database/oracle/oracle-database/23/sqlrf/annotations_clause.html#GUID-1AC16117-BBB6-4435-8794-2B99F8F68052>`__
associated with columns that are fetched from the database. Annotations are
only available with Oracle Database 23ai (or later). Oracle Client libraries
must also be 23.1 (or later).

.. member:: const char* dpiAnnotation.key

    Specifies the annoation key.

.. member:: uint32_t dpiAnnotation.keyLength

    Specifies the length of the annotation key, in bytes.

.. member:: const char* dpiAnnotation.value

    Specifies the annoation value.

.. member:: uint32_t dpiAnnotation.valueLength

    Specifies the length of the annotation value, in bytes.
