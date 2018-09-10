#------------------------------------------------------------------------------
# Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
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
# Sample script for running valgrind.
#
#------------------------------------------------------------------------------

echo "Running valgrind on $1..."

valgrind \
    --num-callers=40 \
    --error-markers=-------------------------,------------------------- \
    --redzone-size=256 \
    --leak-check=yes \
    --track-origins=yes \
    --free-fill=0xaa \
    --leak-check=full \
    --error-limit=no \
    --trace-children=yes \
    --show-leak-kinds=definite,possible \
    --suppressions=$(dirname $0)/suppressions.txt \
    --gen-suppressions=all \
    $@

