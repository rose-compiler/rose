#!/bin/sh
# Copyright 2006, Unidata/UCAR. See COPYRIGHT file.

# This is part of the libcf library.

# This shell script runs the cfcheck tests.

# Ed Hartnett, 11/23/06

# $Id: tst_cfcheck.sh,v 1.1.1.1 2009/07/06 15:06:29 ed Exp $

set -e
echo ""
echo "*** Testing cfcheck."

echo "*** testing that useage works..."
./cfcheck &> useage.txt
#diff -w usage.txt ref_usage.txt

echo "*** All tests of cfcheck passed!"
exit 0
