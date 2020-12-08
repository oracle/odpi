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

SOURCES = DemoBLOB.c DemoCLOB.c DemoFetch.c DemoInsert.c DemoInsertAsArray.c \
		DemoCallProc.c DemoRefCursors.c DemoImplicitResults.c \
		DemoFetchObjects.c DemoBindObjects.c DemoFetchDates.c \
		DemoBindArrays.c DemoBFILE.c DemoAppContext.c DemoDistribTrans.c \
		DemoObjectAQ.c DemoRawAQ.c DemoBulkAQ.c DemoCQN.c DemoLongs.c \
		DemoLongRaws.c DemoDMLReturning.c DemoInOutTempLobs.c \
		DemoConvertNumbers.c DemoCreateSodaColl.c DemoIterSodaColls.c \
		DemoDropSodaColl.c DemoInsertSodaColl.c DemoGetSodaDoc.c \
		DemoRemoveSodaDoc.c DemoReplaceSodaDoc.c DemoGetAllSodaDocs.c \
		DemoGetSodaCollNames.c DemoCLOBsAsStrings.c DemoBLOBsAsBytes.c \
		DemoInsertManySodaColl.c DemoShardingNumberKey.c DemoFetchJSON.c \
		DemoBindJSON.c
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
