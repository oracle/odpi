# Oracle Database Programming Interface for C (ODPI-C)

This is a C library that simplifies the use of common
[Oracle Call Interface (OCI)](http://www.oracle.com/technetwork/database/features/oci/index.html)
features for drivers and applications.

## Install

ODPI-C requires an Oracle Client version 11.2 or higher.

A sample Makefile is provided if you wish to build ODPI-C as a shared
library.  Otherwise, include the ODPI-C source code in your application
and build the application as you would build an OCI application.

## Features

- 11.2, 12.1 and 12.2 Oracle Client support
- SQL and PL/SQL execution
- REF cursors
- Large objects (CLOB, NCLOB, BLOB, BFILE)
- Timestamps (naive, with time zone, with local time zone)
- JSON objects
- Array fetch
- Array bind/execute
- Session pools (homogeneous and non-homogeneous with proxy authentication)
- Standalone connections
- Database Resident Connection Pooling (DRCP)
- External authentication
- Statement caching (tagging)
- End-to-end tracing, mid-tier authentication and auditing (action, module, client identifier, client info, database operation)
- PL/SQL arrays (index-by integer tables)
- Objects (types in SQL, records in PL/SQL)
- Editioning
- Scrollable cursors
- DML returning clause (uses dynamic binds)
- privileged connection support (SYSDBA, SYSOPER, SYSASM, PRELIM_AUTH)
- Database Startup/Shutdown
- Session Tagging
- Proxy authentication
- Batch Errors
- Array DML Row Counts
- Implicit Results
- Query Result Caching
- Application Continuity (with some limitations)
- Query Metadata
- Password Change
- Two Phase Commit
- OCI Client Version and Server Version
- Connection Validation (when acquired from session pool or DRCP)
- Continuous Query Notification
- Advanced Queuing

## Documentation

See [ODPI-C Documentation](https://github.com/oracle/odpi/doc).

## Help

Please report bugs and ask questions using [GitHub issues](https://github.com/oracle/odpi/issues).

## Tests

See [/test](https://github.com/oracle/odpi/test)

## Contributing

See [CONTRIBUTING](https://github.com/oracle/odpi/CONTRIBUTING.md)

## License

Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.

This program is free software: you can modify it and/or redistribute it under
the terms of:

(i)  the Universal Permissive License v 1.0 or at your option, any
     later version (<http://oss.oracle.com/licenses/upl>); and/or

(ii) the Apache License v 2.0. (<http://www.apache.org/licenses/LICENSE-2.0>)
