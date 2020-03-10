.. _installation:

ODPI-C Installation
-------------------

.. contents:: :local:

Overview
========

To use ODPI-C in your own project, download its source from `GitHub
<https://github.com/oracle/odpi>`__.  A sample Makefile is provided if
you wish to build ODPI-C as a shared library.  Otherwise, add the
ODPI-C source code to your project. On Windows, Visual Studio 2008 or
higher is required. On macOS, Xcode 6 or higher is required. On Linux,
GCC 4.4 or higher is required.

Projects using ODPI-C require Oracle Client libraries to be
installed. The libraries provide the necessary network connectivity
allowing applications to access an Oracle Database instance. They also
provide basic and advanced connection management and data features to
ODPI-C applications.

The simplest Oracle Client is the free `Oracle Instant Client
<http://www.oracle.com/technetwork/database/database-technologies/instant-client/overview/index.html>`__.
Only the "Basic" or "Basic Light" package is required. Oracle Client
libraries are also available in any Oracle Database installation or
full Oracle Client installation.

ODPI-C explicitly loads available Oracle Client libraries at
runtime. This allows code using ODPI-C to be built only once, and then
run using available Oracle Client 19, 18, 12, or 11.2 libraries.  If
Oracle Client libraries are not found, the error "DPI-1047: Cannot
locate an Oracle Client library" is raised. If an Oracle Client library is
found but the version cannot be determined, the error "DPI-1072: the Oracle
Client library version is unsupported" is raised. This can be due to the fact
that the Oracle Client library version is too old (prior to 10g) or entirely
unsupported (such as Oracle Client 19 and Windows 7).

On Windows, the Oracle Client libraries are first looked for in the
same directory that the ODPI-C library (or application binary) is
located in.  If they are not found, then the Oracle Client library
directory should be included in the ``PATH`` environment variable.

On non-Windows platforms, if no Oracle Client is located in the
standard operating system search path (e.g. ``$LD_LIBRARY_PATH``), then
``$ORACLE_HOME`` is searched.

The following sections explain how to ensure the Oracle Client is
installed and configured correctly on the various platforms so that
ODPI-C is able to find it.

ODPI-C has been tested on Linux, Windows and macOS.  Other platforms should
also work but have not been tested.


Oracle Client and Oracle Database Interoperability
==================================================

ODPI-C can use Oracle Client 19, 18, 12, or 11.2 libraries.

Oracle's standard client-server network interoperability allows
connections between different versions of Oracle Client and Oracle
Database.  For certified configurations see Oracle Support's `Doc ID
207303.1 <https://support.oracle.com/epmos/faces/DocumentDisplay?id=207303.1>`__.
In summary, Oracle Client 19, 18 and 12.2 can connect to Oracle Database 11.2 or
greater. Oracle Client 12.1 can connect to Oracle Database 10.2 or
greater. Oracle Client 11.2 can connect to Oracle Database 9.2 or
greater.  The technical restrictions on creating connections may be more
flexible.  For example Oracle Client 12.2 can successfully connect to Oracle
Database 10.2.

Since a single ODPI-C binary can use multiple client versions and
access multiple database versions, it is important your application is
tested in your intended release environments.  Newer
Oracle clients support new features, such as the `oraaccess.xml
<https://www.oracle.com/pls/topic/lookup?ctx=dblatest&id=GUID-9D12F489-EC02-46BE-8CD4-5AECED0E2BA2>`__ external configuration
file available with 12.1 or later clients, session pool improvements,
call timeouts with 18 or later clients, and `other enhancements
<https://www.oracle.com/pls/topic/lookup?ctx=dblatest&id=GUID-D60519C3-406F-4588-8DA1-D475D5A3E1F6>`__.

The function :func:`dpiContext_getClientVersion()` can be used to determine
which Oracle Client version is in use and the function
:func:`dpiConn_getServerVersion()` can be used to determine which Oracle
Database version a connection is accessing. These can then be used to adjust
application behavior accordingly. Attempts to use some Oracle features that are
not supported by a particular client/server combination may result in runtime
errors. These include:

    - when attempting to access attributes that are not supported by the
      current Oracle Client library you will get the error "ORA-24315: illegal
      attribute type"

    - when attempting to use implicit results with Oracle Client 11.2
      against Oracle Database 12c you will get the error "ORA-29481:
      Implicit results cannot be returned to client"

    - when attempting to get array DML row counts with Oracle Client
      11.2 you will get the error "DPI-1050: Oracle Client library must be at
      version 12.1 or higher"


Linux
=====

ODPI-C requires Oracle Client libraries, which are found in Oracle
Instant Client, or an Oracle Database installation, or in a full
Oracle Client installation.  The libraries must be either 32-bit or
64-bit, matching your application and ODPI-C library (if one is
created separately).

On Linux, ODPI-C first searches for a library called "libclntsh.so"
using the `standard library search order
<http://man7.org/linux/man-pages/man8/ld.so.8.html>`__. If this is not
found, it will then search for version specific files such as
"libclntsh.so.19.1".  If no library is found, then
``$ORACLE_HOME/lib/libclntsh.so`` is checked.  If no library is found
there, then an error is returned.


Oracle Instant Client Zip
+++++++++++++++++++++++++

To run ODPI-C applications with Oracle Instant Client zip files:

1. Download an Oracle 19, 18, 12, or 11.2 "Basic" or "Basic Light" zip file: `64-bit
   <http://www.oracle.com/technetwork/topics/linuxx86-64soft-092277.html>`__
   or `32-bit
   <http://www.oracle.com/technetwork/topics/linuxsoft-082809.html>`__, matching your
   application 64-bit or 32-bit architecture.

2. Unzip the package into a directory that is accessible to your
   application. For example::

       mkdir -p /opt/oracle
       cd /opt/oracle
       unzip instantclient-basic-linux.x64-19.3.0.0.0dbru.zip

3. Install the operating system ``libaio`` package with sudo or as the root user. For example::

       sudo yum install libaio

   On some Linux distributions this package is called ``libaio1`` instead.

4. If there is no other Oracle software on the machine that will be
   impacted, permanently add Instant Client to the runtime link
   path. For example, with sudo or as the root user::

       sudo sh -c "echo /opt/oracle/instantclient_19_3 > /etc/ld.so.conf.d/oracle-instantclient.conf"
       sudo ldconfig

   Alternatively, set the environment variable ``LD_LIBRARY_PATH`` to
   the appropriate directory for the Instant Client version. For
   example::

       export LD_LIBRARY_PATH=/opt/oracle/instantclient_19_3:$LD_LIBRARY_PATH

5. If you intend to co-locate optional Oracle configuration files such
   as ``tnsnames.ora``, ``sqlnet.ora`` or ``oraaccess.xml`` with
   Instant Client, then create a ``network/admin`` subdirectory, if it
   does not exist.  For example::

       mkdir -p /opt/oracle/instantclient_12_2/network/admin

   This is the default Oracle configuration directory for applications
   linked with this Instant Client.

   Alternatively, Oracle configuration files can be put in another,
   accessible directory.  Then set the environment variable
   ``TNS_ADMIN`` to that directory name.


Oracle Instant Client RPM
+++++++++++++++++++++++++

To run ODPI-C applications with Oracle Instant Client RPMs:

1. Download an Oracle 19, 18, 12, or 11.2 "Basic" or "Basic Light" RPM: `64-bit
   <http://www.oracle.com/technetwork/topics/linuxx86-64soft-092277.html>`__
   or `32-bit
   <http://www.oracle.com/technetwork/topics/linuxsoft-082809.html>`__, matching your
   application architecture.

   Oracle's yum server has `Instant Client RPMs for Oracle Linux 7
   <http://yum.oracle.com/repo/OracleLinux/OL7/oracle/instantclient/x86_64/index.html>`__
   and `Instant Client RPMs for Oracle Linux 6
   <http://yum.oracle.com/repo/OracleLinux/OL6/oracle/instantclient/x86_64/index.html>`__
   that can be downloaded without needing a click-through.

2. Install the downloaded RPM with sudo or as the root user. For example::

       sudo yum install oracle-instantclient19.3-basic-19.3.0.0.0-1.x86_64.rpm

   Yum will automatically install required dependencies, such as ``libaio``.

   Oracle Linux users can alternatively automate download and
   installation.  For example::

       yum -y install oracle-release-el7
       yum-config-manager --enable ol7_oracle_instantclient
       yum -y install oracle-instantclient19.3-basic

3. For Instant Client 19 RPMs, the system library search path is
   automatically configured during installation.

   For older versions, if there is no other Oracle software on the
   machine that will be impacted, permanently add Instant Client to
   the runtime link path. For example, with sudo or as the root user::

       sudo sh -c "echo /usr/lib/oracle/18.5/client64/lib > /etc/ld.so.conf.d/oracle-instantclient.conf"
       sudo ldconfig

   Alternatively, for version 18 and earlier, every shell will need to
   have the environment variable ``LD_LIBRARY_PATH`` set to the
   appropriate directory for the Instant Client version. For example::

       export LD_LIBRARY_PATH=/usr/lib/oracle/18.5/client64/lib:$LD_LIBRARY_PATH

4. If you intend to co-locate optional Oracle configuration files such
   as ``tnsnames.ora``, ``sqlnet.ora`` or ``oraaccess.xml`` with
   Instant Client, then create a ``network/admin`` subdirectory under
   ``lib/``, if it does not exist.  For example::

       sudo mkdir -p /usr/lib/oracle/12.2/client64/lib/network/admin

   This is the default Oracle configuration directory for applications
   linked with this Instant Client.

   Alternatively, Oracle configuration files can be put in another,
   accessible directory.  Then set the environment variable
   ``TNS_ADMIN`` to that directory name.


Local Database or Full Oracle Client
++++++++++++++++++++++++++++++++++++

ODPI-C applications can use Oracle Client 19, 18, 12, or 11.2 libraries
from a local Oracle Database or full Oracle Client installation.

The libraries must be either 32-bit or 64-bit, matching your
application and ODPI-C library (if one is created separately).

1. Set required Oracle environment variables by running the Oracle environment
   script. For example::

       source /usr/local/bin/oraenv

   For Oracle Database XE 11.2, run::

       source /u01/app/oracle/product/11.2.0/xe/bin/oracle_env.sh

2. Optional Oracle configuration files such as ``tnsnames.ora``,
   ``sqlnet.ora`` or ``oraaccess.xml`` can be placed in
   ``$ORACLE_HOME/network/admin``.

   Alternatively, Oracle configuration files can be put in another,
   accessible directory.  Then set the environment variable
   ``TNS_ADMIN`` to that directory name.

Docker Files
++++++++++++

ODPI-C applications can easily be used in Docker by basing your
deployments on the Instant Client Dockerfiles on `GitHub
<https://github.com/oracle/docker-images/tree/master/OracleInstantClient>`__.

To build an Instant Client image, create a Dockerfile, for example::

        FROM oraclelinux:7-slim

        ARG release=19
        ARG update=3

        RUN  yum -y install oracle-release-el7 && \
             yum-config-manager --enable ol7_oracle_instantclient && \
             yum -y install oracle-instantclient${release}.${update}-basic && \
             rm -rf /var/cache/yum

Then run::

        docker build -t oracle/instantclient:19 .

The new image can be used as the basis for your application.

Windows
=======

ODPI-C requires Oracle Client libraries, which are found in Oracle
Instant Client, or an Oracle Database installation, or in a full
Oracle Client installation.  The libraries must be either 32-bit or
64-bit, matching your application and ODPI-C library (if one is
created separately).

On Windows, ODPI-C looks for the Oracle Client library "OCI.dll" first
in the directory containing the ODPI-C library (or application), and
then searches using the `standard library search order
<https://msdn.microsoft.com/en-us/library/windows/desktop/ms682586(v=vs.85).aspx>`__.

Oracle Client libraries require the presence of the correct Visual Studio
redistributable.

    - Oracle 19 needs `VS 2017 <https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads>`__
    - Oracle 18 and 12.2 need `VS 2013 <https://support.microsoft.com/en-us/kb/2977003#bookmark-vs2013>`__
    - Oracle 12.1 needs `VS 2010 <https://support.microsoft.com/en-us/kb/2977003#bookmark-vs2010>`__
    - Oracle 11.2 needs `VS 2005 64-bit <https://www.microsoft.com/en-us/download/details.aspx?id=18471>`__ or `VS 2005 32-bit <https://www.microsoft.com/en-ca/download/details.aspx?id=3387>`__


Oracle Instant Client Zip
+++++++++++++++++++++++++

To run ODPI-C applications with Oracle Instant Client zip files:

1. Download an Oracle 19, 18, 12, or 11.2 "Basic" or "Basic Light" zip
   file: `64-bit
   <http://www.oracle.com/technetwork/topics/winx64soft-089540.html>`__
   or `32-bit
   <http://www.oracle.com/technetwork/topics/winsoft-085727.html>`__, matching your
   application architecture.

   Note that 19c is not supported on Windows 7.

2. Unzip the package into a directory that is accessible to your
   application. For example unzip
   ``instantclient-basic-windows.x64-19.3.0.0.0dbru.zip`` to
   ``C:\oracle\instantclient_19_3``.

3. Add this directory to the ``PATH`` environment variable. For
   example, on Windows 7, update ``PATH`` in Control Panel -> System
   -> Advanced System Settings -> Advanced -> Environment Variables ->
   System Variables -> PATH.  The Instant Client directory must occur
   in ``PATH`` before any other Oracle directories.

   Restart any open command prompt windows.

   To avoid interfering with existing tools that require other Oracle
   Client versions, instead of updating the system-wide ``PATH`` variable, you
   may prefer to write a batch file that sets ``PATH``, for example::

       REM mywrapper.bat
       SET PATH=C:\oracle\instantclient_19_3;%PATH%
       myapp %*

   Invoke this batch file everytime you want to run your application.

   Alternatively use ``SET`` to change your ``PATH`` in each command
   prompt window before you run python.

   Another option is to move the unzipped Instant Client files to the
   same directory as the ODPIC.DLL (or into the directory of the
   application's binary, if ODPI-C is compiled into application).  If
   you do this, then ``PATH`` does not need to be set.

4. If you intend to co-locate optional Oracle configuration files such
   as ``tnsnames.ora``, ``sqlnet.ora`` or ``oraaccess.xml`` with
   Instant Client, then create a ``network\admin`` subdirectory, if it
   does not exist, for example
   ``C:\oracle\instantclient_19_3\network\admin``.

   This is the default Oracle configuration directory for applications
   linked with this Instant Client.

   Alternatively, Oracle configuration files can be put in another,
   accessible directory.  Then set the environment variable
   ``TNS_ADMIN`` to that directory name.

If you wish to package Instant Client with your application, you can
move the Instant Client libraries to the same directory as the ODPI-C
library (or application). Refer to the `Instant Client documentation
<https://www.oracle.com/pls/topic/lookup?ctx=dblatest&id=GUID-AAB0378F-2C7B-41EB-ACAC-18DD5D052B01>`__
for the minimal set of Instant Client files required. There is no need
to set ``PATH``. This only works on Windows.

Local Database or Full Oracle Client
++++++++++++++++++++++++++++++++++++

The Oracle libraries must be either 32-bit or 64-bit, matching your
application and ODPI-C library (if one is created separately).

To run ODPI-C applications using client libraries from a local Oracle Database (or full Oracle Client) 19, 18, 12, or 11.2 installation:

1. Set the environment variable ``PATH`` to include the path that contains
   OCI.dll, if it is not already set. For example, on Windows 7, update
   ``PATH`` in Control Panel -> System -> Advanced System Settings ->
   Advanced -> Environment Variables -> System Variables -> PATH.

   Restart any open command prompt windows.

2. Optional Oracle configuration files such as ``tnsnames.ora``,
   ``sqlnet.ora`` or ``oraaccess.xml`` can be placed in the
   ``network/admin`` subdirectory of the Oracle software.

   Alternatively, Oracle configuration files can be put in another,
   accessible directory.  Then set the environment variable
   ``TNS_ADMIN`` to that directory name.


macOS
=====

ODPI-C requires Oracle Client libraries, which are found in Oracle
Instant Client for macOS.

On macOS, ODPI-C first searches for a library called "libclntsh.dylib"
using the `standard library search order
<https://developer.apple.com/library/content/documentation/DeveloperTools/Conceptual/DynamicLibraries/100-Articles/DynamicLibraryUsageGuidelines.html>`__. If
this is not found, it will then search for "libclntsh.dylib.18.1",
"libclntsh.dylib.12.1" and then for "libclntsh.dylib.11.1" before
returning an error.


Oracle Instant Client Zip
+++++++++++++++++++++++++

To run ODPI-C applications with Oracle Instant Client zip files:

1. Download the 19, 18, 12, or 11.2 "Basic" or "Basic Light" zip file from `here
   <https://www.oracle.com/database/technologies/instant-client/macos-intel-x86-downloads.html>`__.
   Choose either a 64-bit or 32-bit package, matching your
   application architecture.  Most applications use 64-bit.

2. Unzip the package into a single directory that is accessible to your
   application. For example, in Terminal you could unzip in your home directory::

       cd ~
       unzip instantclient-basic-macos.x64-19.3.0.0.0dbru.zip

3. Add a link to ``$HOME/lib`` or ``/usr/local/lib`` to enable applications to
   find Instant Client. If the ``lib`` sub-directory does not exist, you can
   create it. For example::

       mkdir ~/lib
       ln -s ~/instantclient_19_3/libclntsh.dylib ~/lib/

   If you now run ``ls -l ~/lib/libclntsh.dylib`` you will see something like::

       lrwxr-xr-x  1 yourname  staff  48 12 Nov 15:04 /Users/yourname/lib/libclntsh.dylib -> /Users/yourname/instantclient_19_3/libclntsh.dylib

   Alternatively, copy the required OCI libraries. For example::

        mkdir ~/lib
        cp ~/instantclient_19_3/{libclntsh.dylib.19.1,libclntshcore.dylib.19.1,libons.dylib,libnnz12.dylib,libociei.dylib} ~/lib/

   For Instant Client 11.2, the OCI libraries must be copied. For example::

        mkdir ~/lib
        cp ~/instantclient_11_2/{libclntsh.dylib.11.1,libnnz11.dylib,libociei.dylib} ~/lib/

4. If you intend to co-locate optional Oracle configuration files such
   as ``tnsnames.ora``, ``sqlnet.ora`` or ``oraaccess.xml`` with
   Instant Client, then create a ``network/admin`` subdirectory, if it
   does not exist.  For example::

       mkdir -p ~/instantclient_12_2/network/admin

   This is the default Oracle configuration directory for applications
   linked with this Instant Client.

   Alternatively, Oracle configuration files can be put in another,
   accessible directory.  Then set the environment variable
   ``TNS_ADMIN`` to that directory name.

Other Platforms
===============

To run ODPI-C applications on other platforms (such as Solaris and AIX), follow the same
general directions as for Linux Instant Client zip files or Local Database.  Add the
Oracle libraries to the appropriate library path variable, such as ``LD_LIBRARY_PATH``
on Solaris, or ``LIBPATH`` on AIX.
