.. _dpiBindVar:

ODPI-C Private Structure dpiBindVar
-----------------------------------

This private structure is used to represent a single bound variable. An array
of these is retained in the :ref:`dpiStmt<dpiStmt>` structure in order to
retain references to the variables that were bound to the statement. This
ensures that the statement can be executed without the fear of the variable
memory no longer being valid.  It also ensures that references are held only as
long as needed.

.. member:: dpiVar \*dpiBindVar.var

    Specifies a pointer to the :ref:`dpiVar<dpiVar>` structure which is the
    variable which has been bound.

.. member:: uint32_t dpiBindVar.pos

    Specifies the position to which this variable has been bound, if the
    variable was bound by position. If the variable was bound by name, this
    value will be 0.

.. member:: const char \*dpiBindVar.name

    Specifies a pointer to the array of bytes that make up the name to which
    this variable has been bound, if the variable was bound by name. If the
    variable was bound by position, this value will be NULL.

.. member:: uint32_t dpiBindVar.nameLength

    Specifies the length of the name to which this variable has been bound, in
    bytes. If the variable was bound by position, this value will be 0.

