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
# Look at README.md for information on how to build and run the samples.
#------------------------------------------------------------------------------

# Set location of built executables
BUILD_DIR=build

# Set parameters on Windows
ifdef SYSTEMROOT
	CC=cl
	LD=link
	CFLAGS=-I../include //nologo
	LDFLAGS=//nologo
	LIBS=../lib/odpic.lib
	OBJ_SUFFIX=.obj
	EXE_SUFFIX=.exe
	OBJ_OUT_OPTS=-Fo
	EXE_OUT_OPTS=/OUT:

# Set parameters on all other platforms
else
	CC=gcc
	LD=gcc
	CFLAGS=-I../include -O2 -g -Wall
	LIBS=-L../lib -lodpic -ldl
	OBJ_SUFFIX=.o
	EXE_SUFFIX=
	OBJ_OUT_OPTS=-o
	EXE_OUT_OPTS=-o
endif

SOURCES = TestBLOB.c TestCLOB.c TestFetch.c TestInsert.c TestInsertAsArray.c \
		TestCallProc.c TestRefCursors.c TestImplicitResults.c \
		TestFetchObjects.c TestBindObjects.c TestFetchDates.c \
		TestBindArrays.c TestBFILE.c TestAppContext.c TestDistribTrans.c \
		TestAQ.c TestCQN.c TestLongs.c TestLongRaws.c TestDMLReturning.c \
		TestInOutTempLobs.c TestConvertNumbers.c
BINARIES = $(SOURCES:%.c=$(BUILD_DIR)/%$(EXE_SUFFIX))

all: $(BUILD_DIR) $(BINARIES)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%$(OBJ_SUFFIX): %.c ../include/dpi.h SampleLib.h
	$(CC) -c $(CFLAGS) $(OBJ_OUT_OPTS)$@ $<

$(BUILD_DIR)/%$(EXE_SUFFIX): $(BUILD_DIR)/%$(OBJ_SUFFIX) \
		$(BUILD_DIR)/SampleLib$(OBJ_SUFFIX)
	$(LD) $(LDFLAGS) $< $(EXE_OUT_OPTS)$@ $(BUILD_DIR)/SampleLib$(OBJ_SUFFIX) \
			$(LIBS)

