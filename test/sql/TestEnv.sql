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

