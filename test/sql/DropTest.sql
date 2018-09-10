/*-----------------------------------------------------------------------------
 * Copyright (c) 2016, 2017, Oracle and/or its affiliates. All rights reserved.
 * This program is free software: you can modify it and/or redistribute it
 * under the terms of:
 *
 * (i)  the Universal Permissive License v 1.0 or at your option, any
 *      later version (http://oss.oracle.com/licenses/upl); and/or
 *
 * (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * DropTest.sql
 *   Drops database objects used for testing
 *
 * Run this like:
 *   sqlplus / as sysdba @DropTest
 *
 * Note that the script TestEnv.sql should be modified if you would like to use
 * something other than the default configuration.
 *---------------------------------------------------------------------------*/

whenever sqlerror exit failure

-- setup environment
@@TestEnv.sql

-- drop schemas
begin

    for r in
            ( select username
              from dba_users
              where username in (upper('&main_user'), upper('&proxy_user'),
                      upper('&edition_user'))
            ) loop
        execute immediate 'drop user ' || r.username || ' cascade';
    end loop;

end;
/

-- drop directory
begin
    for r in
            ( select directory_name
              from dba_directories
              where directory_name = upper('&dir_name')
            ) loop
        execute immediate 'drop directory ' || r.directory_name;
    end loop;
end;
/

-- drop edition
begin
    for r in
            ( select edition_name
              from dba_editions
              where edition_name in (upper('&edition_name'))
            ) loop
        execute immediate 'drop edition ' || r.edition_name || ' cascade';
    end loop;
end;
/

