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
# Sample Makefile if you wish to build the ODPI-C sample executables.
#
# Look at README.md for information on how to build and run the samples.
#------------------------------------------------------------------------------

BUILD_DIR = build
INCLUDE_DIR = ../include
LIB_DIR = ../lib

CC=gcc
LD=gcc
CFLAGS=-I$(INCLUDE_DIR) -O2 -g -Wall
LIBS=-L$(LIB_DIR) -lodpic
COMMON_OBJS = $(BUILD_DIR)/SampleLib.o

SOURCES = TestBLOB.c TestCLOB.c TestFetch.c TestInsert.c TestInsertAsArray.c \
		TestCallProc.c TestRefCursors.c TestImplicitResults.c \
		TestFetchObjects.c TestBindObjects.c TestFetchDates.c \
		TestBindArrays.c TestBFILE.c TestAppContext.c TestDistribTrans.c \
		TestAQ.c TestCQN.c TestLongs.c TestLongRaws.c TestDMLReturning.c \
		TestInOutTempLobs.c TestConvertNumbers.c TestCreateSodaColl.c \
		TestIterSodaColls.c TestDropSodaColl.c TestInsertSodaColl.c \
		TestGetSodaDoc.c TestRemoveSodaDoc.c TestReplaceSodaDoc.c \
		TestGetAllSodaDocs.c TestGetSodaCollNames.c
BINARIES = $(SOURCES:%.c=$(BUILD_DIR)/%)

all: $(BUILD_DIR) $(BINARIES)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c SampleLib.h
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(COMMON_OBJS)
	$(LD) $(LDFLAGS) $< -o $@ $(COMMON_OBJS) $(LIBS)

