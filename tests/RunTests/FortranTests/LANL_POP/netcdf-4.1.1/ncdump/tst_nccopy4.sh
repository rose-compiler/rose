#!/bin/sh
# For a netCDF-4 build, test nccopy on netCDF files in this directory

set -e
echo ""

# These files are actually in $srcdir in distcheck builds, so they
# need to be handled differently.
# ref_tst_compounds2 ref_tst_compounds3 ref_tst_compounds4 
TESTFILES='c0 c0tmp ctest0 ctest0_64 small small2 test0 test1
  tst_calendars tst_comp tst_comp2 tst_enum_data tst_fillbug
  tst_group_data tst_mslp tst_mslp_64 tst_nans tst_ncml
  tst_opaque_data tst_small tst_solar_1 tst_solar_2 tst_solar_cmp
  tst_special_atts tst_string_data tst_unicode tst_utf8 tst_vlen_data
  utf8'

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
