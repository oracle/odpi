.. _dpiOpCode:

ODPI-C Enumeration dpiOpCode
----------------------------

This enumeration identifies the types of operations that can take place during
object change and query change notification. It is used both as a filter when
determining which operations to consider when sending notifications as well as
identifying the operation that took place on a particular table or row when a
notification is sent. Multiple values can be OR'ed together to specify multiple
types of operations at the same time.

===========================  ==================================================
Value                        Description
===========================  ==================================================
DPI_OPCODE_ALL_OPS           Indicates that notifications should be sent for
                             all operations on the table or query.
DPI_OPCODE_ALL_ROWS          Indicates that all rows have been changed in the
                             table or query (or too many rows were changed or
                             row information was not requested).
DPI_OPCODE_INSERT            Indicates that an insert operation has taken place
                             in the table or query.
DPI_OPCODE_UPDATE            Indicates that an update operation has taken place
                             in the table or query.
DPI_OPCODE_DELETE            Indicates that a delete operation has taken place
                             in the table or query.
DPI_OPCODE_ALTER             Indicates that the registered table or query has
                             been altered.
DPI_OPCODE_DROP              Indicates that the registered table or query has
                             been dropped.
DPI_OPCODE_UNKNOWN           An unknown operation has taken place.
===========================  ==================================================

