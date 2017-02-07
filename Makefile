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

#------------------------------------------------------------------------------
# Sample Makefile if you wish to build ODPI-C as a shared library.
#
# Environment variable OCI_INC_DIR needs to be set to the location of the OCI
# header files and environment variable OCI_LIB_DIR needs to be set to the
# location of the OCI libraries.
#
# See README.md for the platforms and compilers known to work.
#------------------------------------------------------------------------------

vpath %.c src
vpath %.h include src

# verify that OCI_INC_DIR and OCI_LIB_DIR have been set
ifndef OCI_LIB_DIR
    $(error Set environment variable OCI_LIB_DIR before running make)
endif
ifndef OCI_INC_DIR
    $(error Set environment variable OCI_INC_DIR before running make)
endif

# define location for library target and intermediate files
BUILD_DIR=build
LIB_DIR=lib

# set parameters on Windows
ifdef SYSTEMROOT
	CC=cl
	LD=link
	CFLAGS=-Iinclude -I$(OCI_INC_DIR) //nologo
	LDFLAGS=//DLL //nologo
	LIBS=//LIBPATH:$(OCI_LIB_DIR) oci.lib
	LIB_NAME=odpic.dll
	OBJ_SUFFIX=.obj
	LIB_OUT_OPTS=/OUT:$(LIB_DIR)/$(LIB_NAME)
	OBJ_OUT_OPTS=-Fo
	IMPLIB_NAME=$(LIB_DIR)/odpic.lib

# set parameters on all other platforms
else
	CC=gcc
	LD=gcc
	CFLAGS=-Iinclude -I$(OCI_INC_DIR) -O2 -g -Wall -fPIC
	LDFLAGS=-shared
	LIBS=-L$(OCI_LIB_DIR) -lclntsh
	OBJ_SUFFIX=.o
	OBJ_OUT_OPTS=-o
	IMPLIB_NAME=
	ifeq ($(shell uname -s), Darwin)
		LIB_NAME=libodpic.dylib
		LIB_OUT_OPTS=-dynamiclib \
			-install_name $(shell pwd)/$(LIB_DIR)/$(LIB_NAME) \
			-Wl,-rpath,$(OCI_LIB_DIR) -o $(LIB_DIR)/$(LIB_NAME)
	else
		LIB_NAME=libodpic.so
		LIB_OUT_OPTS=-o $(LIB_DIR)/$(LIB_NAME)
	endif
endif

# set flag DPI_TRACE_REFS if environment variable set
ifdef DPI_TRACE_REFS
	CFLAGS+=-DDPI_TRACE_REFS
endif

SRCS = dpiConn.c dpiContext.c dpiData.c dpiEnv.c dpiError.c dpiGen.c \
       dpiGlobal.c dpiLob.c dpiObject.c dpiObjectAttr.c dpiObjectType.c \
       dpiPool.c dpiStmt.c dpiUtils.c dpiVar.c dpiOracleType.c dpiSubscr.c \
       dpiDeqOptions.c dpiEnqOptions.c dpiMsgProps.c dpiRowid.c
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%$(OBJ_SUFFIX))

all: $(BUILD_DIR) $(LIB_DIR) $(LIB_DIR)/$(LIB_NAME) $(IMPLIB_NAME)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LIB_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(LIB_DIR):
	mkdir $(LIB_DIR)

$(BUILD_DIR)/%$(OBJ_SUFFIX): %.c dpi.h dpiImpl.h dpiErrorMessages.h
	$(CC) -c $(CFLAGS) $< $(OBJ_OUT_OPTS)$@

$(LIB_DIR)/$(LIB_NAME): $(OBJS)
	$(LD) $(LDFLAGS) $(LIB_OUT_OPTS) $(OBJS) $(LIBS)

# import library is specific to Windows
ifdef IMPLIB_NAME
$(IMPLIB_NAME): $(OBJS)
	lib $(OBJS) //LIBPATH:$(OCI_LIB_DIR) oci.lib /OUT:$@
endif

