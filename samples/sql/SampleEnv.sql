/*-----------------------------------------------------------------------------
 * Copyright (c) 2017, 2022, Oracle and/or its affiliates.
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
 * SampleEnv.sql
 *   Sets up configuration for the SetupSamples.sql and DropSamples.sql
 * scripts.  Change the values below if you would like to use something other
 * than the default values. Note that the environment variables noted below
 * will also need to be set, if non-default values are used.
 *   The directory path must be one that both the server and the client can
 * read simultaneously.
 *---------------------------------------------------------------------------*/

set echo off termout on feedback off verify off

define main_user = "odpicdemo"          -- $ODPIC_SAMPLES_MAIN_USER
define main_password = "welcome"        -- $ODPIC_SAMPLES_MAIN_PASSWORD
define proxy_user = "odpicdemo_proxy"   -- $ODPIC_SAMPLES_PROXY_USER
define proxy_password = "welcome"       -- $ODPIC_SAMPLES_PROXY_PASSWORD
define dir_name = "odpicdemo_dir"       -- $ODPIC_SAMPLES_DIR_NAME
define dir_path = "/tmp"

prompt ************************************************************************
prompt                               CONFIGURATION
prompt ************************************************************************
prompt Main Schema: &main_user
prompt Proxy Schema: &proxy_user
prompt Directory Name: &dir_name
prompt Directory Path: &dir_path
prompt

set echo on verify on feedback on
