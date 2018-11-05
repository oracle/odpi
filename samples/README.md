This directory contains samples for ODPI-C. All of the executables can be built
using the supplied Makefile (Makefile.win32 for use with nmake on Windows). The
executables will be placed in the subdirectory "build" and can be run from
there.

See the top level [README](../README.md) for the platforms and compilers that
have been tested and are known to work.

Some samples require the latest Database features.  TestBFILE needs to
write to the physical directory that the database can read from, so it
will generally fail unless it is run on the same machine as the database.

To run the samples:

  - Ensure that installation of the ODPI-C library has been completed as
    explained [here](https://oracle.github.io/odpi/doc/installation.html).

  - Optionally edit the file sql/SampleEnv.sql and edit the parameters defined
    there. If you don't change any of the parameters, make the sure the schemas
    odpicdemo and odpicdemo_proxy can be dropped. If you do edit the parameters
    defined there, also set the corresponding environment variables indicated
    on the right of the defined variable.

  - Optionally set the environment variable ODPIC_SAMPLES_CONNECT_STRING to
    point to the database in which you plan to run the tests. If this
    environment variable is not set, the connect string will be assumed to be
    the EZ connect string "localhost/orclpdb".

  - Run 'make clean' and 'make' to build the samples

  - Run SQL\*Plus as SYSDBA and create the sample SQL objects with
    sql/SetupSamples.sql.  The syntax is:

      sqlplus sys/syspassword@hostname/servicename as sysdba @SetupSamples

  - Change to the 'build' directory and run each sample individually.
    On Linux you will need to set LD_LIBRARY_PATH to point to the location of
    the library, for example:

        LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../../lib ./TestAppContext


  - After running the samples, drop the SQL objects by running the
    script sql/DropSamples.sql.  The syntax is:

      sqlplus sys/syspassword@hostname/servicename as sysdba @DropSamples

