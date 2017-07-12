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
# The DB schema used by the tests can be set with the environment
# variables ODPIC_TEST_CONN_USERNAME, ODPIC_TEST_CONN_PASSWORD and
# ODPIC_TEST_CONN_CONNECT_STRING.  The variable ODPIC_TEST_DIR_NAME
# can also be set.  It is the name used by a CREATE DIRECTORY
# statement.
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
	CFLAGS=-I../include -O2 -g -Wall
	LDFLAGS=-L../lib -lodpic -ldl
	OBJ_SUFFIX=.o
	EXE_SUFFIX=
	OBJ_OUT_OPTS=-o
	EXE_OUT_OPTS=-o
endif

SOURCES = TestContext.c TestConn.c TestNumbers.c \
          TestPool.c TestPoolProperties.c TestQueries.c \
          TestTransactions.c TestConnProperties.c TestMiscCases.c \
          TestVariables.c TestStatements.c TestDataTypes.c  TestObjectTypes.c \
		  TestObjects.c

BINARIES = $(SOURCES:%.c=$(BUILD_DIR)/%$(EXE_SUFFIX))

all: $(BUILD_DIR) $(BINARIES) $(BUILD_DIR)/TestSuiteRunner$(EXE_SUFFIX)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/TestLib$(OBJ_SUFFIX): TestLib.c TestLib.h ../include/dpi.h
	$(CC) -c $(CFLAGS) \
			-DCONN_USERNAME=\"$(ODPIC_TEST_CONN_USERNAME)\" \
			-DCONN_PASSWORD=\"$(ODPIC_TEST_CONN_PASSWORD)\" \
			-DCONN_CONNECT_STRING=\"$(ODPIC_TEST_CONN_CONNECT_STRING)\" \
			-DDIR_NAME=\"$(ODPIC_TEST_DIR_NAME)\" \
			$(OBJ_OUT_OPTS)$@ $<

$(BUILD_DIR)/TestSuiteRunner$(OBJ_SUFFIX): TestSuiteRunner.c
	$(CC) -c $(CFLAGS) $(OBJ_OUT_OPTS)$@ $<

$(BUILD_DIR)/%$(OBJ_SUFFIX): %.c ../include/dpi.h TestLib.h
	$(CC) -c $(CFLAGS) $(OBJ_OUT_OPTS)$@ $<

$(BUILD_DIR)/%$(EXE_SUFFIX): $(BUILD_DIR)/%$(OBJ_SUFFIX) \
		$(BUILD_DIR)/TestLib$(OBJ_SUFFIX)
	$(LD) $(LDFLAGS) $< $(EXE_OUT_OPTS)$@ $(BUILD_DIR)/TestLib$(OBJ_SUFFIX)

$(BUILD_DIR)/TestSuiteRunner$(EXE_SUFFIX): \
		$(BUILD_DIR)/TestSuiteRunner$(OBJ_SUFFIX) $(BINARIES)
	$(LD) $< $(EXE_OUT_OPTS)$@

