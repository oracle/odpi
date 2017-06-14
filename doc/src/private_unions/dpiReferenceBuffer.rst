.. _dpiReferenceBuffer:

ODPI-C Private Union dpiReferenceBuffer
---------------------------------------

This private union is used to avoid casts. It is used by the :ref:`dpiVar`
structure to store references to LOBs, objects and statements that are bound to
statements or fetched from the database.

.. member:: void \*dpiReferenceBuffer.asHandle

    Specifies a generic handle pointer.

.. member:: dpiObject \*dpiReferenceBuffer.asObject

    Specifies a pointer to a :ref:`dpiObject` structure.

.. member:: dpiStmt \*dpiReferenceBuffer.asStmt

    Specifies a pointer to a :ref:`dpiStmt` structure.

.. member:: dpiLob \*dpiReferenceBuffer.asLOB

    Specifies a pointer to a :ref:`dpiLob` structure.

.. member:: dpiRowid \*dpiReferenceBuffer.asRowid

    Specifies a pointer to a :ref:`dpiRowid` structure.

