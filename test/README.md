This directory contains tests for ODPI-C. All of the test executables are built
using the supplied Makefile (Makefile.win32 for use with nmake on Windows). The
test executables will be placed in the subdirectory "build".

See the top level [README](../README.md) for the platforms and compilers that
have been tested and are known to work.

To run the tests:

  - Ensure that installation of the ODPI-C library has been completed as
    explained [here](https://oracle.github.io/odpi/doc/installation.html).

  - Optionally edit the file sql/TestEnv.sql and edit the parameters defined
    there. If you don't change any of the parameters, make the sure the schemas
    odpic and odpic_proxy can be dropped. If you do edit the parameters defined
    there, also set the corresponding environment variables indicated on the
    right of the defined variable.

  - Optionally set the environment variable ODPIC_TEST_CONNECT_STRING to point
    to the database in which you plan to run the tests. If this environment
    variable is not set, the connect string will be assumed to be the
    EZ connect string "localhost/orclpdb".

  - Run 'make clean' and 'make' to build the tests

  - Run SQL\*Plus as SYSDBA and create the test suite SQL objects with
    sql/SetupTest.sql.  The syntax is:

        sqlplus sys/syspassword@hostname/servicename as sysdba @SetupTest

  - Change to the 'build' directory and run the TestSuiteRunner executable
    found there. It will run all of the tests in the other executables and
    report on success or failure when it finishes running all of the tests.
    On Linux you will need to set LD_LIBRARY_PATH to point to the location of
    the library, as in:

        LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../lib ./TestSuiteRunner

  - After running the tests, drop the SQL objects by running the
    script sql/DropTest.sql.  The syntax is:

        sqlplus sys/syspassword@hostname/servicename as sysdba @DropTest

NOTES:

  - if you are using the BEQ connection method (setting the environment
    variable ORACLE_SID and using an empty connection string) then you will
    need to add the configuration bequeath_detach=yes to your sqlnet.ora file

