#!/bin/sh
# This shell script which creates the fill.nc file from fill.cdl.
# $Id: create_fills.sh,v 1.2 2009/01/25 14:33:45 ed Exp $

echo
echo "*** Testing creating file with fill values."
set -e
../ncgen/ncgen -b $srcdir/fills.cdl
echo "*** SUCCESS!"
exit 0
