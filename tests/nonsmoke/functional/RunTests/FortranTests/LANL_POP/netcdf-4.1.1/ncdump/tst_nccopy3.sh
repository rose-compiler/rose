#!/bin/sh
# For a netCDF-3 build, test nccopy on netCDF files in this directory

set -e
echo ""

TESTFILES='c0 c0tmp ctest0 ctest0_64 small small2 test0 test1
 tst_calendars tst_mslp tst_mslp_64 tst_ncml tst_small tst_utf8 utf8'

echo "*** Testing nccopy on ncdump/*.nc files"
for i in $TESTFILES ; do
    echo "*** copy $i.nc to copy_of_$i.nc ..."
    ./nccopy $i.nc copy_of_$i.nc
    ./ncdump -n copy_of_$i $i.nc > tmp.cdl
    ./ncdump copy_of_$i.nc > copy_of_$i.cdl
    echo "*** compare " with copy_of_$i.cdl
    diff copy_of_$i.cdl tmp.cdl
    rm copy_of_$i.nc copy_of_$i.cdl tmp.cdl
done
echo
echo "*** All nccopy tests passed!"
exit 0
