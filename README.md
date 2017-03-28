# ODPI-C version 2.0.0-beta.2

Oracle Database Programming Interface for C (ODPI-C) is an open source
library of C code that simplifies the use of common
[Oracle Call Interface (OCI)](http://www.oracle.com/technetwork/database/features/oci/index.html)
features for Oracle Database drivers and user applications.

## Features

- 11.2, 12.1 and 12.2 Oracle Client support
- 9.2 and higher Oracle Database support (depending on Oracle Client version)
- SQL and PL/SQL execution
- Character datatypes (CHAR, VARCHAR2, NCHAR, NVARCHAR2, CLOB, NCLOB, LONG)
- Numeric datatypes (NUMBER, BINARY_FLOAT, BINARY_DOUBLE)
- Dates, Timestamps, Intervals
- Binary types (BLOB, BFILE, RAW, LONG RAW)
- PL/SQL datatypes (PLS_INTEGER, BOOLEAN, Collections, Records)
- JSON
- User Defined Types
- REF CURSOR, Nested cursors, Implicit Result Sets
- Array fetch
- Array bind/execute
- Session pools (homogeneous and non-homogeneous with proxy authentication)
- Standalone connections
- Database Resident Connection Pooling (DRCP)
- External authentication
- Statement caching (tagging)
- End-to-end tracing, mid-tier authentication and auditing (action, module,
  client identifier, client info, database operation)
- Edition Based Redefinition
- Scrollable cursors
- DML RETURNING
- Privileged connection support (SYSDBA, SYSOPER, SYSASM, PRELIM_AUTH)
- Database Startup/Shutdown
- Session Tagging
- Proxy authentication
- Batch Errors
- Array DML Row Counts
- Query Result Caching
- Application Continuity (with some limitations)
- Query Metadata
- Password Change
- Two Phase Commit
- OCI Client Version and Server Version
- Connection Validation (when acquired from session pool or DRCP)
- Continuous Query Notification
- Advanced Queuing


## Install

A sample Makefile is provided if you wish to build ODPI-C as a shared
library. Otherwise, include the ODPI-C source code in your project.  ODPI-C
uses the shared library loading mechanism available on each supported platform
to load the Oracle Client library at runtime. This allows code using ODPI-C to
be built only once, and then run using Oracle Client 11.2, 12.1 or 12.2
libraries. See
[Installation](https://oracle.github.io/odpi/doc/installation.html)
for more information.

Oracle client libraries are available in the free
[Oracle Instant Client](http://www.oracle.com/technetwork/database/features/instant-client/index.html).
The "Basic" or "Basic Light" package is required at runtime.  Client libraries
are also available in any Oracle Database installation or full Oracle Client
installation.

ODPI-C has been tested on Linux, Windows and macOS.  Other platforms should
also work but have not been tested.  On Windows, Visual Studio 2008 or higher
is required.  On macOS, Xcode 6 or higher is required.  On Linux, GCC 4.4 or
higher is required.

## Drivers Using ODPI-C

[cx_Oracle](https://oracle.github.io/python-cx_Oracle) driver for Python.

## Documentation

See [ODPI-C Documentation](https://oracle.github.io/odpi/doc/index.html)

## Help

Please report bugs and ask questions using [GitHub issues](https://github.com/oracle/odpi/issues).

## Samples

See [/samples](https://github.com/oracle/odpi/tree/master/samples)

## Contributing

See [CONTRIBUTING](https://github.com/oracle/odpi/blob/master/CONTRIBUTING.md)

## License

Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.

This program is free software: you can modify it and/or redistribute it under
the terms of:

(i)  the Universal Permissive License v 1.0 or at your option, any
     later version (<http://oss.oracle.com/licenses/upl>); and/or

(ii) the Apache License v 2.0. (<http://www.apache.org/licenses/LICENSE-2.0>)
