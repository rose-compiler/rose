#!/bin/sh
# This shell script runs the ncdump tests.
# $Id: run_nc4_tests.sh,v 1.2 2006/08/29 15:26:04 russ Exp $

echo "*** Testing ncgen for netCDF-4."
set -e
echo "*** creating netCDF-4 file c0_4.nc from c0.cdl..."
./ncgen -k3 -b -o c0_4.nc $srcdir/c0.cdl
echo "*** creating netCDF-4 classic model file c0_4c.nc from c0.cdl..."
./ncgen -k4 -b -o c0_4c.nc $srcdir/c0.cdl

echo "*** Test successful!"
exit 0
