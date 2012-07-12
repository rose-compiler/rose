#!/bin/sh

# This shell runs the tests with valgrind.

# $Id: run_valgrind_tests.sh,v 1.9 2010/01/26 20:24:18 ed Exp $

set -e
echo ""
echo "Testing programs with valgrind..."

# These are my test programs.
list='tst_h_dimscales tst_h_vl2 tst_h_files '\
'tst_h_atts2 tst_h_dimscales2 tst_h_enums '\
'tst_h_grps tst_h_wrt_cmp tst_h_rd_cmp tst_h_vl tst_h_atts '\
'tst_h_dimscales3 tst_h_files2 tst_h_compounds '\
'tst_h_vars tst_h_opaques tst_h_compounds2 '\
'tst_atts tst_atts2 tst_vl tst_compounds '\
'tst_files tst_utf8 tst_fillbug tst_v2 '\
'tst_vars2 tst_vars3 tst_xplatform tst_dims '\
'tst_dims2 tst_dims3 tst_varms tst_unlim_vars '\
'tst_endian_fill tst_compounds2'

# These don't work yet: tst_h_vars3 
# tst_h_strings tst_h_atts3 tst_h_vars2 tst_vars tst_fills tst_chunks
# tst_coords tst_xplatform2

for tst in $list; do
    echo ""
    echo "Memory testing with $tst:"
    valgrind -q --error-exitcode=2 --leak-check=full ./$tst
done

echo "SUCCESS!!!"

exit 0
