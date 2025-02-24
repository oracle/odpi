#------------------------------------------------------------------------------
# Copyright (c) 2016, 2025, Oracle and/or its affiliates.
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

SOURCES = TestSuiteRunner.c \
		  test_1000_context.c \
		  test_1100_numbers.c \
		  test_1200_conn.c \
		  test_1300_conn_properties.c \
		  test_1400_pool.c \
		  test_1500_pool_properties.c \
		  test_1600_queries.c \
		  test_1700_transactions.c \
		  test_1800_misc.c \
		  test_1900_variables.c \
		  test_2000_statements.c \
		  test_2100_data_types.c \
		  test_2200_object_types.c \
		  test_2300_objects.c \
		  test_2400_enq_options.c \
		  test_2500_deq_options.c \
		  test_2600_msg_props.c \
		  test_2700_aq.c \
		  test_2800_lobs.c \
		  test_2900_implicit_results.c \
		  test_3000_scroll_cursors.c \
		  test_3100_subscriptions.c \
		  test_3200_batch_errors.c \
		  test_3300_dml_returning.c \
		  test_3400_soda_db.c \
		  test_3500_soda_coll.c \
		  test_3600_soda_coll_cursor.c \
		  test_3700_soda_doc.c \
		  test_3800_soda_doc_cursor.c \
		  test_3900_sess_tags.c \
		  test_4000_queue.c \
		  test_4100_binds.c \
		  test_4200_rowids.c \
		  test_4300_json.c \
		  test_4400_vector.c
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
