#------------------------------------------------------------------------------
# Copyright (c) 2016, 2022, Oracle and/or its affiliates.
#
# This software is dual-licensed to you under the Universal Permissive License
# (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
# 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
# either license.
#
# If you elect to accept the software under the Apache License, Version 2.0,
# the following applies:
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
		DemoBindJSON.c DemoTokenStandalone.c DemoTokenPoolWithCallback.c
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
