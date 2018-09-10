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
 * DropSamples.sql
 *   Drops database objects used for samples.
 *
 * Run this like:
 *   sqlplus / as sysdba @DropSamples
 *
 * Note that the script SampleEnv.sql should be modified if you would like to
 * use something other than the default configuration.
 *---------------------------------------------------------------------------*/

whenever sqlerror exit failure

-- setup environment
@@SampleEnv.sql

-- drop schemas
begin

    for r in
            ( select username
              from dba_users
              where username in (upper('&main_user'), upper('&proxy_user'))
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

