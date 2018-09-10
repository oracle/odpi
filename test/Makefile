#------------------------------------------------------------------------------
# Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
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
# Look at README.md for information on how to build and run the tests.
#------------------------------------------------------------------------------

BUILD_DIR = build
INCLUDE_DIR = ../include
LIB_DIR = ../lib

CC = gcc
LD = gcc
CFLAGS = -I$(INCLUDE_DIR) -O2 -g -Wall
LIBS = -L$(LIB_DIR) -lodpic
COMMON_OBJS = $(BUILD_DIR)/TestLib.o

SOURCES = TestSuiteRunner.c TestContext.c TestConn.c TestNumbers.c \
          TestPool.c TestPoolProperties.c TestQueries.c \
          TestTransactions.c TestConnProperties.c TestMiscCases.c \
          TestVariables.c TestStatements.c TestDataTypes.c  TestObjectTypes.c \
		  TestObjects.c TestEnqOptions.c TestDeqOptions.c TestMsgProps.c \
		  TestAQ.c TestLOBs.c TestImplicitResults.c TestRowIds.c \
          TestScrollCursors.c TestSubscriptions.c TestBatchErrors.c \
          TestDMLReturning.c TestSodaDb.c TestSodaColl.c TestSodaCollCursor.c \
          TestSodaDoc.c TestSodaDocCursor.c
BINARIES = $(SOURCES:%.c=$(BUILD_DIR)/%)

all: $(BUILD_DIR) $(BINARIES)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c TestLib.h
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(COMMON_OBJS)
	$(LD) $(LDFLAGS) $< -o $@ $(COMMON_OBJS) $(LIBS)

