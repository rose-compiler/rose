#!/bin/sh

# This shell just tests the tst_chunks program by running it a few
# times to generate a simple test file. Then it uses ncdump -s to
# check that the output is what it should be.

# $Id: run_tst_chunks.sh,v 1.2 2009/02/24 01:49:12 russ Exp $

set -e
echo ""

echo "*** Running benchmarking program tst_chunks for tiny test file"
compress_level=1
dim1=6
chunk1=2
dim2=12
chunk2=3
dim3=4
chunk3=1
./tst_chunks $compress_level $dim1 $chunk1 $dim2 $chunk2 $dim3 $chunk3
../ncdump/ncdump -s tst_chunks.nc > tst_chunks.cdl
diff tst_chunks.cdl ref_chunks1.cdl
echo '*** SUCCESS!!!'

echo ""
echo "*** Testing the benchmarking program tst_chunks for larger variables ..."
compress_level=1
dim1=32
chunk1=8
dim2=90
chunk2=10
dim3=91
chunk3=13
cachesize=10000000
cachehash=10000
cachepre=0.0
./tst_chunks $compress_level $dim1 $chunk1 $dim2 $chunk2 $dim3 $chunk3 $cachesize $cachehash $cachepre
../ncdump/ncdump -s -h tst_chunks.nc > tst_chunks.cdl
diff tst_chunks.cdl ref_chunks2.cdl
echo '*** SUCCESS!!!'

exit 0