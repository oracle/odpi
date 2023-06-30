.. _installation:

*****************
Installing ODPI-C
*****************

.. _overview:

Overview
========

To use ODPI-C in your own project, download its source from `GitHub
<https://github.com/oracle/odpi>`__. A sample Makefile is provided if
you wish to build ODPI-C as a shared library. Otherwise, add the
ODPI-C source code to your project. On Windows, Visual Studio 2008 or
higher is required. On macOS, Xcode 6 or higher is required. On Linux,
GCC 4.4 or higher is required.

ODPI-C has been tested on Linux, Windows and macOS. Other platforms should
also work but have not been tested.

Projects using ODPI-C require Oracle Client libraries to be
installed. The libraries provide the necessary network connectivity
allowing applications to access an Oracle Database instance. They also
provide basic and advanced connection management and data features to
ODPI-C applications.

.. _clientlibloading:

Oracle Client Library Loading
-----------------------------

The simplest Oracle Client is the free `Oracle Instant Client
<https://www.oracle.com/database/technologies/instant-client.html>`__.
Only the "Basic" or "Basic Light" package is required. Oracle Client
libraries are also available in any Oracle Database installation or
full Oracle Client installation.

ODPI-C dynamically loads available Oracle Client libraries at
runtime. This allows code using ODPI-C to be built only once, and then
run using any available Oracle Client 21, 19, 18, 12, or 11.2 libraries. If
Oracle Client libraries are not found, the error "DPI-1047: Cannot
locate an Oracle Client library" is raised. If an Oracle Client library is
found but the version cannot be determined, the error "DPI-1072: the Oracle
Client library version is unsupported" is raised. For example, this could be
because the Oracle Client library version is too old (prior to 10g) or entirely
unsupported (such as Oracle Client 19 on Windows 7).

If the member :member:`dpiContextCreateParams.oracleClientLibDir` is specified
when calling :func:`dpiContext_createWithParams()`, then only that directory is
searched for the Oracle Client libraries.

If no directory is specified, then the Oracle Client libraries are looked for in
the same directory that the ODPI-C library (or application binary) is located
in. If they are not found, then they are searched for in the standard operating
system search path, for example ``PATH`` on Windows or ``LD_LIBRARY_PATH`` on
Linux. See `standard Windows library search order
<https://msdn.microsoft.com/en-us/library/windows/desktop/ms682586(v=vs.85).aspx>`__
or `standard Linux library search order
<http://man7.org/linux/man-pages/man8/ld.so.8.html>`__. Finally, on platforms
other than Windows, ``$ORACLE_HOME/lib`` is also searched.

.. note::

    - When using Instant Client on Linux and related operating systems, then
      that directory must always be in the system library search path.

    - When using the library directory from a full client or database
      installation, then you additionally need to set the application
      environment to use this software, for example by setting the
      ``ORACLE_HOME`` environment variable. This should be done prior to ODPI-C
      initialization.

Make sure the application has directory and file access permissions for the
Oracle Client libraries. On Linux ensure ``libclntsh.so`` exists. On macOS
ensure ``libclntsh.dylib`` exists. ODPI-C will not directly load
``libclntsh.*.XX.1`` files in
:member:`dpiContextCreateParams.oracleClientLibDir` or from the directory where
the ODPI-C library (or application binary) is. Note other files used by
``libclntsh*`` are also required.

To trace the loading of Oracle Client libraries, the environment variable
``DPI_DEBUG_LEVEL`` can be set to 64 before starting your application. See
:ref:`debugging`.

The following sections explain how to ensure the Oracle Client is
installed and configured correctly on the various platforms so that
ODPI-C is able to find it.


Oracle Client and Oracle Database Interoperability
--------------------------------------------------

Oracle's standard client-server network interoperability allows connections
between different versions of Oracle Client and Oracle Database. For certified
configurations see Oracle Support's `Doc ID 207303.1
<https://support.oracle.com/epmos/faces/DocumentDisplay?id=207303.1>`__. In
summary, Oracle Client 21 can connect to Oracle Database 12.1 or later. Oracle
Client 19, 18 and 12.2 can connect to Oracle Database 11.2 or greater. Oracle
Client 12.1 can connect to Oracle Database 10.2 or greater. Oracle Client 11.2
can connect to Oracle Database 9.2 or greater. The technical restrictions on
creating connections may be more flexible. For example Oracle Client 12.2 can
successfully connect to Oracle Database 10.2.

Since a single ODPI-C binary can use multiple client versions and
access multiple database versions, it is important your application is
tested in your intended release environments. Newer
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


.. _linuxinstall:

Linux
=====

ODPI-C requires Oracle Client libraries, which are found in Oracle Instant
Client, or an Oracle Database installation, or in a full Oracle Client
installation, see :ref:`clientlibloading`. The libraries must be either 32-bit
or 64-bit, matching your application and ODPI-C library (if one is created
separately).

.. _instzip:

Oracle Instant Client Zip Files
-------------------------------

To run ODPI-C applications with Oracle Instant Client zip files:

1. Download an Oracle 21, 19, 18, 12, or 11.2 "Basic" or "Basic Light" zip file
   matching your application 64-bit or 32-bit architecture:

   - `x86-64 64-bit <https://www.oracle.com/database/technologies/instant-client/linux-x86-64-downloads.html>`__
   - `x86 32-bit <https://www.oracle.com/database/technologies/instant-client/linux-x86-32-downloads.html>`__
   - `ARM (aarch64) 64-bit <https://www.oracle.com/database/technologies/instant-client/linux-arm-aarch64-downloads.html>`__

2. Unzip the package into a directory that is accessible to your
   application. For example::

       mkdir -p /opt/oracle
       cd /opt/oracle
       unzip instantclient-basic-linux.x64-21.1.0.0.0.zip

3. Install the operating system ``libaio`` package with sudo or as the root user. For example::

       sudo yum install libaio

   On some Linux distributions this package is called ``libaio1`` instead.

   On recent Linux versions, such as Oracle Linux 8, you may also need to
   install the ``libnsl`` package when using Oracle Instant Client 19.

4. If there is no other Oracle software on the machine that will be
   impacted, permanently add Instant Client to the runtime link
   path. For example, with sudo or as the root user::

       sudo sh -c "echo /opt/oracle/instantclient_21_1 > /etc/ld.so.conf.d/oracle-instantclient.conf"
       sudo ldconfig

   Alternatively, set the environment variable ``LD_LIBRARY_PATH`` to
   the appropriate directory for the Instant Client version. For
   example::

       export LD_LIBRARY_PATH=/opt/oracle/instantclient_21_1:$LD_LIBRARY_PATH

5. If you use optional Oracle configuration files such as ``tnsnames.ora``,
   ``sqlnet.ora`` or ``oraaccess.xml``, put the files in an accessible
   directory. Then set the member
   :member:`dpiContextCreateParams.oracleClientConfigDir` when calling
   :func:`dpiContext_createWithParams()`.

   Alternatively, set the environment variable ``TNS_ADMIN`` to that directory
   name.

   Alternatively, create a ``network/admin`` subdirectory of Instant Client, if
   it does not exist. For example, with Instant Client 12.2::

       mkdir -p /opt/oracle/instantclient_12_2/network/admin

   Put the files in this directory. This is the default Oracle configuration
   directory for applications linked with this Instant Client.

.. _instrpm:

Oracle Instant Client RPM Files
-------------------------------

To run ODPI-C applications with Oracle Instant Client RPMs:

1. Download an Oracle 21,19, 18, 12, or 11.2 "Basic" or "Basic Light" RPM matching your application architecture:

   - `x86-64 64-bit <https://www.oracle.com/database/technologies/instant-client/linux-x86-64-downloads.html>`__
   - `x86 32-bit <https://www.oracle.com/database/technologies/instant-client/linux-x86-32-downloads.html>`__
   - `ARM (aarch64) 64-bit <https://www.oracle.com/database/technologies/instant-client/linux-arm-aarch64-downloads.html>`__

   Oracle's yum server has convenient repositories:

   - `Instant Client 21 RPMs for Oracle Linux x86-64 8 <https://yum.oracle.com/repo/OracleLinux/OL8/oracle/instantclient21/x86_64/index.html>`__, `Older Instant Client RPMs for Oracle Linux x86-64 8 <https://yum.oracle.com/repo/OracleLinux/OL8/oracle/instantclient/x86_64/index.html>`__
   - `Instant Client 21 RPMs for Oracle Linux x86-64 7 <https://yum.oracle.com/repo/OracleLinux/OL7/oracle/instantclient21/x86_64/index.html>`__, `Older Instant Client RPMs for Oracle Linux x86-64 7 <https://yum.oracle.com/repo/OracleLinux/OL7/oracle/instantclient/x86_64/index.html>`__
   - `Instant Client RPMs for Oracle Linux x86-64 6 <https://yum.oracle.com/repo/OracleLinux/OL6/oracle/instantclient/x86_64/index.html>`__
   - `Instant Client RPMs for Oracle Linux ARM (aarch64) 8 <https://yum.oracle.com/repo/OracleLinux/OL8/oracle/instantclient/aarch64/index.html>`__
   - `Instant Client RPMs for Oracle Linux ARM (aarch64) 7 <https://yum.oracle.com/repo/OracleLinux/OL7/oracle/instantclient/aarch64/index.html>`__

2. Install the downloaded RPM with sudo or as the root user. For example::

       sudo yum install oracle-instantclient-basic-21.1.0.0.0-1.x86_64.rpm

   On recent Linux versions, such as Oracle Linux 8, you may also need to
   manually install the ``libnsl`` package package when using Oracle Instant
   Client 19. Yum will automatically install other required dependencies, such
   as ``libaio``.

   Oracle Linux users can alternatively automate download and
   installation. For example::

       yum install oracle-instantclient-release-el7
       yum install oracle-instantclient-basic

   Refer to `Instant Client for Oracle Linux
   <https://yum.oracle.com/oracle-instant-client.html>`__ for details.

3. For Instant Client 19, or later, RPMs, the system library search path is
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

4. If you use optional Oracle configuration files such as ``tnsnames.ora``,
   ``sqlnet.ora`` or ``oraaccess.xml``, put the files in an accessible
   directory. Then set the member
   :member:`dpiContextCreateParams.oracleClientConfigDir` when calling
   :func:`dpiContext_createWithParams()`.

   Alternatively, set the environment variable ``TNS_ADMIN`` to that directory
   name.

   Alternatively, create a ``network/admin`` subdirectory of Instant Client, if
   it does not exist. For example, with Instant Client 12.2::

       sudo mkdir -p /usr/lib/oracle/12.2/client64/lib/network/admin

   Put the files in this directory. This is the default Oracle configuration
   directory for applications linked with this Instant Client.

.. _instoh:

Local Database or Full Oracle Client
------------------------------------

ODPI-C applications can use Oracle Client 21, 19, 18, 12, or 11.2 libraries
from a local Oracle Database or full Oracle Client installation.

The libraries must be either 32-bit or 64-bit, matching your
application and ODPI-C library (if one is created separately).

1. Set required Oracle environment variables by running the Oracle environment
   script. For example::

       source /usr/local/bin/oraenv

   For Oracle Database XE 11.2, run::

       source /u01/app/oracle/product/11.2.0/xe/bin/oracle_env.sh

2. Optional Oracle configuration files such as ``tnsnames.ora``, ``sqlnet.ora``
   or ``oraaccess.xml`` can be placed in ``$ORACLE_HOME/network/admin``.

   Alternatively, Oracle configuration files can be put in another, accessible
   directory. Then set the member
   :member:`dpiContextCreateParams.oracleClientConfigDir` when calling
   :func:`dpiContext_createWithParams()`, or set the environment variable
   ``TNS_ADMIN`` to that directory name.

.. _containers:

Containers
----------

Dockerfiles
+++++++++++

ODPI-C applications can easily be used in containers by basing your deployments
on the Instant Client Dockerfiles on `GitHub
<https://github.com/oracle/docker-images/tree/main/OracleInstantClient>`__.

To build an Instant Client image, create a Dockerfile, for example::

        FROM oraclelinux:8-slim

        RUN  microdnf install oracle-instantclient-release-el8 && \
             microdnf install oracle-instantclient-basic oracle-instantclient-devel oracle-instantclient-sqlplus && \
             microdnf clean all

Then run::

        docker build -t oracle/instantclient:21 .

The new image can be used as the basis for your application.

Pre-built Containers
++++++++++++++++++++

Pre-built images for Instant Client are in the GitHub Container Registry:

    - https://github.com/orgs/oracle/packages/container/package/oraclelinux8-instantclient
    - https://github.com/orgs/oracle/packages/container/package/oraclelinux7-instantclient

These are easily used. For example, to pull an Oracle Linux 8 image with
Oracle Instant Client 21c already installed, execute::

    docker pull ghcr.io/oracle/oraclelinux8-instantclient:21

.. _windowsinstallation:

Windows
=======

ODPI-C requires Oracle Client libraries, which are found in Oracle Instant
Client, or an Oracle Database installation, or in a full Oracle Client
installation, see :ref:`clientlibloading`. The libraries must be either 32-bit
or 64-bit, matching your application and ODPI-C library (if one is created
separately).

Oracle Client libraries require the presence of the correct Visual Studio
redistributable:

- For Instant Client 21 install `VS 2019 <https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170>`__ or later.
- For Instant Client 19 install `VS 2017 <https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170>`__.
- For Instant Client 18 or 12.2 install `VS 2013 <https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2013-vc-120>`__
- For Instant Client 12.1 install `VS 2010 <https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2010-vc-100-sp1-no-longer-supported>`__
- For Instant Client 11.2 install `VS 2005 64-bit <https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2005-vc-80-sp1-no-longer-supported>`__

.. _instwin:

Oracle Instant Client Zip Files
-------------------------------

To run ODPI-C applications with Oracle Instant Client zip files:

1. Download an Oracle 19, 18, 12, or 11.2 "Basic" or "Basic Light" zip
   file: `64-bit
   <https://www.oracle.com/database/technologies/instant-client/winx64-64-downloads.html>`__
   or `32-bit
   <https://www.oracle.com/database/technologies/instant-client/microsoft-windows-32-downloads.html>`__, matching your
   application architecture.

   Note that 19c is not supported on Windows 7.

2. Unzip the package into a directory that is accessible to your
   application. For example unzip
   ``instantclient-basic-windows.x64-19.11.0.0.0dbru.zip`` to
   ``C:\oracle\instantclient_19_11``.

Configure Oracle Instant Client
+++++++++++++++++++++++++++++++

1. There are several alternative ways to tell your application where your Oracle
   Instant Client libraries are.

   * Set this directory in the member
     :member:`dpiContextCreateParams.oracleClientLibDir` when calling
     :func:`dpiContext_createWithParams()`.

   * Alternatively, move the unzipped Instant Client files to the
     directory containing ODPIC.DLL (or into the directory of the application's
     binary, if ODPI-C is compiled into the application).

   * Alternatively, add the Instant Client directory to the ``PATH``
     environment variable. For example, on Windows 7, update ``PATH`` in
     Control Panel -> System -> Advanced System Settings -> Advanced ->
     Environment Variables -> System Variables -> PATH. The Instant Client
     directory must occur in ``PATH`` before any other Oracle directories.

     Restart any open command prompt windows.

     To avoid interfering with existing tools that require other Oracle Client
     versions, instead of updating the system-wide ``PATH`` variable, you may
     prefer to write a batch file that sets ``PATH``, for example::

         REM mywrapper.bat
         SET PATH=C:\oracle\instantclient_19_11;%PATH%
         myapp %*

     Invoke this batch file every time you want to run your application.

     Or simply use ``SET`` to change your ``PATH`` in each command prompt window
     before you run your application.

2. If you use optional Oracle configuration files such as ``tnsnames.ora``,
   ``sqlnet.ora`` or ``oraaccess.xml``, put the files in an accessible
   directory. Then set the member
   :member:`dpiContextCreateParams.oracleClientConfigDir` when calling
   :func:`dpiContext_createWithParams()`.

   Alternatively, set the environment variable ``TNS_ADMIN`` to that directory
   name.

   Alternatively, create a ``network\admin`` subdirectory of Instant Client, if
   it does not exist. For example ``C:\oracle\instantclient_19_11\network\admin``.

   Put the files in this directory. This is the default Oracle configuration
   directory for applications linked with this Instant Client.

If you wish to package Instant Client with your application, you can move the
Instant Client libraries to the same directory as the ODPI-C library (or
application). Refer to the `Instant Client documentation
<https://www.oracle.com/pls/topic/lookup?ctx=dblatest&id=GUID-AAB0378F-2C7B-41EB-ACAC-18DD5D052B01>`__
for the minimal set of Instant Client files required. There is no need to set
:member:`dpiContextCreateParams.oracleClientConfigDir` or to set ``PATH``.

.. _instwinoh:

Local Database or Full Oracle Client
------------------------------------

ODPI-C applications can use Oracle Client 19, 18, 12, or 11.2 libraries from a
local Oracle Database or full Oracle Client installation.

The Oracle libraries must be either 32-bit or 64-bit, matching your
application and ODPI-C library (if one is created separately).

To run ODPI-C applications using client libraries from a local Oracle Database
(or full Oracle Client) 19, 18, 12, or 11.2 installation:

1. Set the environment variable ``PATH`` to include the path that contains
   OCI.dll, if it is not already set. For example, on Windows 7, update
   ``PATH`` in Control Panel -> System -> Advanced System Settings ->
   Advanced -> Environment Variables -> System Variables -> PATH.

   Restart any open command prompt windows.

2. Optional Oracle configuration files such as ``tnsnames.ora``,
   ``sqlnet.ora`` or ``oraaccess.xml`` can be placed in the
   ``network/admin`` subdirectory of the Oracle software.

   Alternatively, Oracle configuration files can be put in another, accessible
   directory. Then set the member
   :member:`dpiContextCreateParams.oracleClientConfigDir` when calling
   :func:`dpiContext_createWithParams()`, or set the environment variable
   ``TNS_ADMIN`` to that directory name.

.. _instmacos:

macOS
=====

ODPI-C requires Oracle Client libraries, which are found in Oracle
Instant Client for macOS.

The Instant Client DMG packages are notarized and are recommended for all newer
macOS versions. In some future Oracle release, only DMG packages will be
available.

.. _dmg:

Oracle Instant Client DMG Files
-------------------------------

Manual or scripted installation of Oracle Instant Client DMG files can be
performed.

Manual Installation
+++++++++++++++++++

* Download the **Basic** or **Basic Light** 64-bit DMG from `Oracle
  <https://www.oracle.com/database/technologies/instant-client/macos-intel-x86-downloads.html>`__.

* In Finder, double click on the DMG to mount it.

* Open a terminal window and run the install script in the mounted package, for example:

  .. code-block:: shell

         /Volumes/instantclient-basic-macos.x64-19.8.0.0.0dbru/install_ic.sh

  This copies the contents to ``$HOME/Downloads/instantclient_19_8``.
  Applications may not have access to the ``Downloads`` directory, so you
  should move Instant Client somewhere convenient.

* In Finder, eject the mounted Instant Client package.

If you have multiple Instant Client DMG packages mounted, you only need to run
``install_ic.sh`` once. It will copy all mounted Instant Client DMG packages at
the same time.

Scripted Installation
+++++++++++++++++++++

Instant Client installation can alternatively be scripted, for example:

.. code-block:: shell

    cd $HOME/Downloads
    curl -O https://download.oracle.com/otn_software/mac/instantclient/198000/instantclient-basic-macos.x64-19.8.0.0.0dbru.dmg
    hdiutil mount instantclient-basic-macos.x64-19.8.0.0.0dbru.dmg
    /Volumes/instantclient-basic-macos.x64-19.8.0.0.0dbru/install_ic.sh
    hdiutil unmount /Volumes/instantclient-basic-macos.x64-19.8.0.0.0dbru

The Instant Client directory will be ``$HOME/Downloads/instantclient_19_8``.
Applications may not have access to the ``Downloads`` directory, so you should
move Instant Client somewhere convenient.

.. _macoszip:

Oracle Instant Client Zip Files
-------------------------------

To install Oracle Instant Client zip files:

1. Download the 19, 18, 12, or 11.2 "Basic" or "Basic Light" zip file from `Oracle
   <https://www.oracle.com/database/technologies/instant-client/macos-intel-x86-downloads.html>`__.
   Choose either a 64-bit or 32-bit package, matching your
   application architecture. Most applications use 64-bit.

2. Unzip the package into a single directory that is accessible to your
   application. For example, in Terminal you could unzip:

   .. code-block:: shell

       mkdir /opt/oracle
       cd /opt/oracle
       unzip /your/path/to/instantclient-basic-macos.x64-19.8.0.0.0dbru.zip

Configure Oracle Instant Client
-------------------------------

1. There are several alternative ways to tell your application where your Oracle
   Instant Client libraries are.

   * Use the extracted directory for the member
     :member:`dpiContextCreateParams.oracleClientLibDir` in a call to
     :func:`dpiContext_createWithParams()`

   * Alternatively, copy Oracle Instant Client to the directory containing the
     ODPI-C module binary. For example, if ``libodpic.dylib`` (or your binary
     containing the ODPI-C code) is in ``~/myprograms`` you can then run ``ln -s
     ~/Downloads/instantclient_19_8/libclntsh.dylib ~/myprograms``. You can also copy the
     Instant Client libraries to that directory.

   * Alternatively, set ``DYLD_LIBRARY_PATH`` to the Instant Client directory. Note this
     variable does not propagate to sub-shells.

   * Alternatively, you might decide to compile the ODPI-C library with an RPATH
     option like ``-Wl,-rpath,/usr/local/lib``. Then you can link Oracle
     Instant Client to this directory, for example::

         ln -s /opt/oracle/instantclient_19_8/libclntsh.dylib /usr/local/lib/

     Or, instead of a link you can copy the required OCI libraries. For example::

         cp /opt/oracle/instantclient_19_8/{libclntsh.dylib.19.1,libclntshcore.dylib.19.1,libons.dylib,libnnz12.dylib,libociei.dylib} /usr/local/lib/

   * Alternatively, on older versions of macOS, you could add a link to
     ``$HOME/lib`` or ``/usr/local/lib`` to enable applications to find Instant
     Client. If the ``lib`` sub-directory does not exist, you can create
     it. For example:

     .. code-block:: shell

         mkdir ~/lib
         ln -s ~/Downloads/instantclient_19_8/libclntsh.dylib ~/lib/

     Instead of linking, you can copy the required OCI libraries. For example:

     .. code-block:: shell

          mkdir ~/lib
          cp ~/Downloads/instantclient_19_8/{libclntsh.dylib.19.1,libclntshcore.dylib.19.1,libnnz19.dylib,libociei.dylib} ~/lib/

     For Instant Client 11.2, the OCI libraries must be copied. For example:

     .. code-block:: shell

          mkdir ~/lib
          cp ~/Downloads/instantclient_11_2/{libclntsh.dylib.11.1,libnnz11.dylib,libociei.dylib} ~/lib/

2. If you use optional Oracle configuration files such as ``tnsnames.ora``,
   ``sqlnet.ora`` or ``oraaccess.xml``, put the files in an accessible
   directory. Then set the member
   :member:`dpiContextCreateParams.oracleClientConfigDir` when calling
   :func:`dpiContext_createWithParams()`.

   Alternatively, set the environment variable ``TNS_ADMIN`` to that directory
   name.

   Alternatively, create a ``network/admin`` subdirectory of Instant Client, if
   it does not exist. For example::

       mkdir -p ~/Downloads/instantclient_19_8/network/admin

   Put the files in this directory. This is the default Oracle configuration
   directory for applications linked with this Instant Client.

.. _other:

Other Platforms
===============

To run ODPI-C applications on other platforms (such as Solaris and AIX), follow the same
general directions as for Linux Instant Client zip files or Local Database. Add the
Oracle libraries to the appropriate library path variable, such as ``LD_LIBRARY_PATH``
on Solaris, or ``LIBPATH`` on AIX.
