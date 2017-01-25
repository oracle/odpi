#------------------------------------------------------------------------------
# Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
# This program is free software: you can modify it and/or redistribute it
# under the terms of:
#
# (i)  the Universal Permissive License v 1.0 or at your option, any
#      later version (http://oss.oracle.com/licenses/upl); and/or
#
# (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
#
#------------------------------------------------------------------------------
#
# Sample Makefile if you wish to build the ODPI-C test executables.
#
# The environment variable OCI_INC_DIR needs to be set to the location
# of the OCI header files.
#
# The DB schema used by the tests can be set with the environment
# variables ODPIC_TEST_CONN_USERNAME, ODPIC_TEST_CONN_PASSWORD and
# ODPIC_TEST_CONN_CONNECT_STRING.  The variable ODPIC_TEST_DIR_NAME
# can also be set.  It is the name used by a CREATE DIRECTORY
# statement.
#
# See README.md for the platforms and compilers known to work.
#
#------------------------------------------------------------------------------

# Database connection attributes for the tests.  These can be changed
# on the command line or set as environment variables
ifndef ODPIC_TEST_CONN_USERNAME
	ODPIC_TEST_CONN_USERNAME=odpic
endif
ifndef ODPIC_TEST_CONN_PASSWORD
	ODPIC_TEST_CONN_PASSWORD=welcome
endif
ifndef ODPIC_TEST_CONN_CONNECT_STRING
	ODPIC_TEST_CONN_CONNECT_STRING=
endif

# Name for a CREATE DIRECTORY command.  The directory is used by TestBFILE.c
ifndef ODPIC_TEST_DIR_NAME
	ODPIC_TEST_DIR_NAME=ODPIC_DIR
endif

# Set location of built executables
BUILD_DIR=build

# Set parameters on Windows
ifdef SYSTEMROOT
	CC=cl
	LD=link
	CFLAGS=-I../include //nologo
	LDFLAGS=//nologo ../lib/odpic.lib
	OBJ_SUFFIX=.obj
	EXE_SUFFIX=.exe
	OBJ_OUT_OPTS=-Fo
	EXE_OUT_OPTS=/OUT:

# Set parameters on all other platforms
else
	CC=gcc
	LD=gcc
	CFLAGS=-I../include -O2 -g -Wall -m64
	LDFLAGS=-L../lib -lodpic
	OBJ_SUFFIX=.o
	EXE_SUFFIX=
	OBJ_OUT_OPTS=-o
	EXE_OUT_OPTS=-o
endif

SOURCES = TestLOB.c TestFetch.c TestInsert.c TestInsertAsArray.c \
		TestCallProc.c TestRefCursors.c TestImplicitResults.c \
		TestFetchObjects.c TestBindObjects.c TestFetchDates.c \
		TestBindArrays.c TestBFILE.c TestAppContext.c TestDistribTrans.c \
		TestAQ.c TestCQN.c TestLongs.c TestLongRaws.c TestDMLReturning.c \
		TestInOutTempLobs.c TestConvertNumbers.c
BINARIES = $(SOURCES:%.c=$(BUILD_DIR)/%$(EXE_SUFFIX)) \
		$(BUILD_DIR)/TestSizes$(EXE_SUFFIX)

all: $(BUILD_DIR) $(BINARIES)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%$(OBJ_SUFFIX): %.c ../include/dpi.h Test.h
	$(CC) -c $(CFLAGS) \
			-DCONN_USERNAME=\"$(ODPIC_TEST_CONN_USERNAME)\" \
			-DCONN_PASSWORD=\"$(ODPIC_TEST_CONN_PASSWORD)\" \
			-DCONN_CONNECT_STRING=\"$(ODPIC_TEST_CONN_CONNECT_STRING)\" \
			-DDIR_NAME=\"$(ODPIC_TEST_DIR_NAME)\" \
			$(OBJ_OUT_OPTS)$@ $<

$(BUILD_DIR)/%$(EXE_SUFFIX): $(BUILD_DIR)/%$(OBJ_SUFFIX)
	$(LD) $(LDFLAGS) $< $(EXE_OUT_OPTS)$@

$(BUILD_DIR)/TestSizes$(OBJ_SUFFIX): TestSizes.c
	$(CC) -c $(CFLAGS) -I$(OCI_INC_DIR) $< $(OBJ_OUT_OPTS)$@

