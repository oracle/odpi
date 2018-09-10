/*-----------------------------------------------------------------------------
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 * This program is free software: you can modify it and/or redistribute it
 * under the terms of:
 *
 * (i)  the Universal Permissive License v 1.0 or at your option, any
 *      later version (http://oss.oracle.com/licenses/upl); and/or
 *
 * (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
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

