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

#
# Sample ODPI-C Makefile if you wish to build DPI as a shared library
#

vpath %.c src
vpath %.h include src

BUILD_DIR=build
LIB_DIR=lib
EXTRA_CFLAGS=
ifdef SYSTEMROOT
	CC=cl
	LD=link
	CFLAGS=-Iinclude -I$(OCI_INC_DIR) //nologo
	LDFLAGS=//DLL //nologo //LIBPATH:$(OCI_LIB_DIR) oci.lib
	LIBNAME=$(LIB_DIR)/dpi.dll
	OBJSUFFIX=.obj
	LIBOUTOPT=/OUT:
	OBJOUTOPT=-Fo
	IMPLIBNAME=$(LIB_DIR)/dpi.lib
else
	CC=gcc
	CFLAGS=-Iinclude -I$(OCI_INC_DIR) -O2 -g -Wall -m64 -fPIC
	LDFLAGS=-L$(OCI_LIB_DIR) -lclntsh -shared
	LIBNAME=$(LIB_DIR)/libdpi.so
	OBJSUFFIX=.o
	LIBOUTOPT=-o
	OBJOUTOPT=-o
	IMPLIBNAME=
endif

ifdef DPI_TRACE_REFS
	EXTRA_CFLAGS=-DDPI_TRACE_REFS
endif

SRCS = dpiConn.c dpiContext.c dpiData.c dpiEnv.c dpiError.c dpiGen.c \
       dpiGlobal.c dpiLob.c dpiObject.c dpiObjectAttr.c dpiObjectType.c \
       dpiPool.c dpiStmt.c dpiUtils.c dpiVar.c dpiOracleType.c dpiSubscr.c \
       dpiDeqOptions.c dpiEnqOptions.c dpiMsgProps.c dpiRowid.c
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%$(OBJSUFFIX))

all: $(BUILD_DIR) $(LIB_DIR) $(LIBNAME) $(IMPLIBNAME)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LIB_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(LIB_DIR):
	mkdir $(LIB_DIR)

$(BUILD_DIR)/%$(OBJSUFFIX): %.c dpi.h dpiImpl.h dpiErrorMessages.h
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) $< $(OBJOUTOPT)$@

$(LIBNAME): $(OBJS)
	$(LD) $(LDFLAGS) $(LIBOUTOPT)$(LIBNAME) $(OBJS)

ifdef IMPLIBNAME
$(IMPLIBNAME): $(OBJS)
	lib $(OBJS) //LIBPATH:$(OCI_LIB_DIR) oci.lib /OUT:$@
endif

