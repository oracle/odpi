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
 * TestEnv.sql
 *   Sets up configuration for the SetupTest.sql and DropTest.sql scripts.
 * Change the values below if you would like to use something other than the
 * default values. Note that the environment variables noted below will also
 * need to be set, if non-default values are used.
 *   The directory path must be one that both the server and the client can
 * read simultaneously.
 *---------------------------------------------------------------------------*/

set echo off termout on feedback off verify off

define main_user = "odpic"              -- $ODPIC_TEST_MAIN_USER
define main_password = "welcome"        -- $ODPIC_TEST_MAIN_PASSWORD
define proxy_user = "odpic_proxy"       -- $ODPIC_TEST_PROXY_USER
define proxy_password = "welcome"       -- $ODPIC_TEST_PROXY_PASSWORD
define edition_user = "odpic_edition"   -- $ODPIC_TEST_EDITION_USER
define edition_password = "welcome"     -- $ODPIC_TEST_EDITION_PASSWORD
define edition_name = "odpic_e1"        -- $ODPIC_TEST_EDITION_NAME
define dir_name = "odpic_dir"           -- $ODPIC_TEST_DIR_NAME
define dir_path = "/tmp"

prompt ************************************************************************
prompt                               CONFIGURATION
prompt ************************************************************************
prompt Main Schema: &main_user
prompt Proxy Schema: &proxy_user
prompt Edition Schema: &edition_user
prompt Edition Name: &edition_name
prompt Directory Name: &dir_name
prompt Directory Path: &dir_path
prompt

set echo on verify on feedback on
