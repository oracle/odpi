This directory contains tests for ODPI-C. All of the test executables
are built using the supplied Makefile. The test executables will be
placed in the subdirectory "build".

See the top level [README](../README.md) for the platforms and compilers that
have been tested and are known to work.

To run the tests:

  - On Linux, set LD_LIBRARY_PATH to the location of the Oracle client
    libraries and to the directory containing the ODPI-C library, for
    example:

      export LD_LIBRARY_PATH=/opt/oracle/instantclient:/opt/oracle/odpi/lib

  - Optionally set the environment variables ODPIC_TEST_CONN_USERNAME,
    ODPIC_TEST_CONN_PASSWORD and ODPIC_TEST_CONN_CONNECT_STRING to the
    values for a schema that will be created.

    If you don't set the variables, make sure the schema in the
    Makefile can be dropped and that an empty connect string can be used to 
    connect to your database.

  - Optionally set the environment variable ODPIC_TEST_DIR_NAME to a
    string value.  This is the name used in a CREATE DIRECTORY
    command.

  - Run 'make clean' and 'make' to build the tests

  - Run SQL\*Plus as SYSDBA and create the test suite SQL objects with
    sql/SetupTest.sql.  The syntax is:

      sqlplus / as sysdba @SetupTest <odpicuser> <password> <dirname> <dirpath>

    where the parameters are the names you choose to run the tests.

    The <odpicuser>, <password>, and <dirname> values should match the
    ODPIC_TEST_CONN_USERNAME, ODPIC_TEST_CONN_PASSWORD and
    ODPIC_TEST_DIR_NAME environment variables.  If you did not set
    variables, make sure the values passed to (or defaulting in)
    SetupTest.sql are consistent with the Makefile, and that the
    <dirpath> directory is valid.

    The <dirpath> value is an OS directory that the database server
    can write to.  This is used by TestBFILE.c.

    For example run:

      sqlplus / as sysdba @SetupTest $ODPIC_TEST_CONN_USERNAME $ODPIC_TEST_CONN_PASSWORD $ODPIC_TEST_DIR_NAME /some/shared/directory

  - Change to the 'build' directory and run the TestSuiteRunner executable
    found there. It will run all of the tests in the other executables and
    report on success or failure when it finishes running all of the tests.

  - After running the tests, drop the SQL objects by running the
    script sql/DropTest.sql.  The syntax is:

      sqlplus / as sysdba @DropTest <odpicuser> <dirname>

    For example run:

      sqlplus / as sysdba @DropTest $ODPIC_TEST_CONN_USERNAME $ODPIC_TEST_DIR_NAME
