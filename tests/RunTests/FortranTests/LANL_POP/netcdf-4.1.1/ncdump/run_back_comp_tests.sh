#!/bin/sh
# This shell script runs the backward compatibility tests.

# $Id: run_back_comp_tests.sh,v 1.1 2010/03/09 00:23:44 ed Exp $

set -e
echo ""
echo "*** Testing backward compatibility."
echo "*** checking ref_nc_test_netcdf4_4_0.nc..."
./ncdump $srcdir/ref_nc_test_netcdf4_4_0.nc > tst_nc_test_netcdf4_4_0.cdl
tail -n +2 $srcdir/ref_nc_test_netcdf4.cdl > tmp.cdl
tail -n +2 tst_nc_test_netcdf4_4_0.cdl > tmp_4_0.cdl
diff -w tmp.cdl tmp_4_0.cdl

echo "*** All backward compatibility tests passed!"
exit 0
