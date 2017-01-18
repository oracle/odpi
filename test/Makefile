#------------------------------------------------------------------------------
# Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
# This program is free software: you can modify it and/or redistribute it
# under the terms of:
#
# (i)  the Universal Permissive License v 1.0 or at your option, any
#      later version (http://oss.oracle.com/licenses/upl); and/or
#
# (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
#------------------------------------------------------------------------------

ifdef SYSTEMROOT
	CC=cl
	LD=link
	CFLAGS=-I../include //nologo
	LDFLAGS=//nologo ../lib/dpi.lib
	OBJSUFFIX=.obj
	EXESUFFIX=.exe
	OBJOUTOPT=-Fo
	EXEOUTOPT=/OUT:
else
	CC=gcc
	LD=gcc
	CFLAGS=-I../include -O2 -g -Wall -m64
	LDFLAGS=-L../lib -ldpi
	OBJSUFFIX=.o
	EXESUFFIX=
	OBJOUTOPT=-o
	EXEOUTOPT=-o
endif

BUILD_DIR=build

SOURCES = TestLOB.c TestFetch.c TestInsert.c TestInsertAsArray.c \
		TestCallProc.c TestRefCursors.c TestImplicitResults.c \
		TestFetchObjects.c TestBindObjects.c TestFetchDates.c \
		TestBindArrays.c TestBFILE.c TestAppContext.c TestDistribTrans.c \
		TestAQ.c TestCQN.c TestLongs.c TestLongRaws.c TestDMLReturning.c \
		TestInOutTempLobs.c TestConvertNumbers.c
BINARIES = $(SOURCES:%.c=$(BUILD_DIR)/%$(EXESUFFIX)) \
		$(BUILD_DIR)/TestSizes$(EXESUFFIX)

all: $(BUILD_DIR) $(BINARIES)

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%$(OBJSUFFIX): %.c ../include/dpi.h Test.h
	$(CC) -c $(CFLAGS) $(OBJOUTOPT)$@ $<

$(BUILD_DIR)/%$(EXESUFFIX): $(BUILD_DIR)/%$(OBJSUFFIX)
	$(LD) $(LDFLAGS) $< $(EXEOUTOPT)$@

$(BUILD_DIR)/TestSizes$(OBJSUFFIX): TestSizes.c
	$(CC) -c $(CFLAGS) -I$(OCI_INC_DIR) $< $(OBJOUTOPT)$@

