This directory contains tests for ODPI-C. All of the test executables
can be built using the supplied Makefile. The test executables will be
placed in the subdirectory "build" and can be run from there.

To run the tests:

  - Set the environment variable OCI_INC_DIR to the location of the
    OCI header files

  - Optionally set the environment variables ODPIC_TEST_CONN_USERNAME,
    ODPIC_TEST_CONN_PASSWORD and ODPIC_TEST_CONN_CONNECT_STRING to the
    values for the schema used by the tests

  - Optionally set the environment variable ODPIC_TEST_DIR_NAME to the
    a valid OS directory that the database server can write to.  This
    is used by TestBFILE.c.  Note: TestBFILE.c needs to write to the
    physical directory that the database can read from.  The test will
    fail unless it is run on the same machine as the database.

  - Run 'make clean' and 'make' to build the tests

  - Run SQL*Plus as SYSDBA and create the test suite SQL objects with
    sql/SetupTest.sql.  The syntax is:

      sqlplus / as sysdba @SetupTest <odpicuser> <odpicproxyuser> <password> <dirname> <dirpath>

    where the parameters are the names you choose to run the tests.

    The <odpicuser> and <password> values should match the
    ODPIC_TEST_CONN_USERNAME and ODPIC_TEST_CONN_PASSWORD environment
    variables.  The <dirname> value should match the
    ODPIC_TEST_DIR_NAME variable. I.e. run:

      sqlplus / as sysdba @SetupTest $ODPIC_TEST_CONN_USERNAME <odpicproxyuser> $ODPIC_TEST_CONN_PASSWORD $ODPIC_TEST_DIR_NAME <dirpath>


  - On Linux, add the directory containing the ODPI-C library to
    LD_LIBRARY_PATH

  - Change to the 'build' directory and run each test individually

  - After running the tests, drop the SQL objects by running the
    script sql/DropTest.sql.
