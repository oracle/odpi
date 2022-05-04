/*-----------------------------------------------------------------------------
 * Copyright (c) 2016, 2022, Oracle and/or its affiliates.
 *
 * This software is dual-licensed to you under the Universal Permissive License
 * (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
 * 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
 * either license.*
 *
 * If you elect to accept the software under the Apache License, Version 2.0,
 * the following applies:
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
