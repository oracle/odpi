.. _dpiSubscrNamespace:

ODPI-C Enumeration dpiSubscrNamespace
-------------------------------------

This enumeration identifies the namespaces supported by subscriptions.

.. list-table-with-summary::
    :header-rows: 1
    :class: wy-table-responsive
    :widths: 15 35
    :summary: The first column displays the value of the dpiSubscrNamespace
     enumeration. The second column displays the description of the
     dpiSubscrNamespace enumeration value.

    * - Value
      - Description
    * - DPI_SUBSCR_NAMESPACE_AQ
      - Identifies the namespace used for receiving notifications when messages
        are available to be dequeued in advanced queueing (AQ).
    * - DPI_SUBSCR_NAMESPACE_DBCHANGE
      - Identifies the namespace used for receiving notifications for database
        object changes and query changes (CQN).
