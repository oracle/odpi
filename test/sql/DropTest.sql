/*-----------------------------------------------------------------------------
 * Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
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
 *   sqlplus / as sysdba @DropTest <odpicuser> <dirname>
 *---------------------------------------------------------------------------*/

set echo off termout on feedback off verify off
whenever sqlerror exit failure

-- Set default schema values if not passed in as parameters
column 1 new_value 1 noprint
column 2 new_value 2 noprint
select '' "1", '' "2" from dual where 1 = 0;
define username = &1 "ODPIC"
define dirname = &2 "ODPIC_DIR"

-- Convert names to uppercase
col username new_value username noprint
col dirname new_value dirname noprint
select upper('&username') username, upper('&dirname') dirname from dual;

-- Set Proxy username
define usernameprx = &username._PROXY

set echo on verify on feedback on

-- Drop existing users, if present
begin
  for r in
      ( select username
        from dba_users
        where username in ('&username', '&usernameprx')
      ) loop
    execute immediate 'drop user ' || r.username || ' cascade';
  end loop;
end;
/

-- Drop directory
begin
  for r in
      ( select directory_name
        from dba_directories
        where directory_name = '&dirname'
      ) loop
    execute immediate 'drop directory ' || r.directory_name;
  end loop;
end;
/

exit
