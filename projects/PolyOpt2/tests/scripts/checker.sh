#!/bin/sh
## checker.sh for pocc in /Users/pouchet
##
## Made by Louis-Noel Pouchet
## Contact: <louis-noel.pouchet@inria.fr>
##
## Started on  Sat Jul 18 16:57:09 2009 Louis-Noel Pouchet
## Last update Mon Jul  3 12:04:32 2017 Louis-Noel Pouchet
##

output=0
TEST_FILES="$2";
echo "[CHECK] $1";
mode="$3";
TESTS_PREFIX_SRC="$top_srcdir/tests/unitary";
TESTS_PREFIX_BLD="$top_builddir/tests";
if ! [ -d "$TESTS_PREFIX_BLD" ]; then mkdir -p "$TESTS_PREFIX_BLD"; fi;
for i in $TEST_FILES; do
    outtemp=0;
    echo "[TEST] == $i ==";
    filename=${i%.c};
    filename=`basename "$filename"`;
    case "$mode" in
	passthru) outfile="$filename.passthru"; options="";;
	tile) outfile="$filename.plutotile"; options="--polyopt-pluto-tile";;
	tilepar) outfile="$filename.plutotilepar"; options="--polyopt-pluto-tile --polyopt-pluto-parallel";;
	tilemf) outfile="$filename.plutotilemf"; options="--polyopt-pluto-tile --polyopt-pluto-fuse maxfuse";;
	tileparmf) outfile="$filename.plutotileparmf"; options="--polyopt-pluto-tile --polyopt-pluto-parallel  --polyopt-pluto-fuse maxfuse";;
	tileparunroll) outfile="$filename.plutotileparunroll"; options="--polyopt-pluto-tile --polyopt-pluto-parallel --polyopt-pluto-unroll";;
	tileparunrollprev) outfile="$filename.plutotileparunrollprev"; options="--polyopt-pluto-tile --polyopt-pluto-parallel --polyopt-pluto-unroll --polyopt-pluto-prevector";;
    esac;
    ## (1) Check that generated files are the same.
#    $top_builddir/src/PolyRose $options $TESTS_PREFIX_SRC/$i -rose:o $TESTS_PREFIX_BLD/$outfile.test.c 2>/tmp/poccout >/dev/null
    #   z=`diff --ignore-matching-lines='CLooG' --ignore-matching-lines='rose\[WARN' --ignore-matching-lines='int c' --ignore-blank-lines $TESTS_PREFIX_BLD/$outfile.test.c $TESTS_PREFIX_SRC/$outfile.c 2>&1`
    prototype_name="$outfile";
    outfile="$TESTS_PREFIX_BLD/$prototype_name";
    reffile="$TESTS_PREFIX_SRC/$prototype_name";
    $top_builddir/src/PolyRose $options $i -rose:o $outfile.test.c 2>/tmp/poccout >/dev/null
    z=`diff --ignore-matching-lines='CLooG' --ignore-matching-lines='rose\[WARN' --ignore-matching-lines='int c' --ignore-blank-lines --ignore-space-change --ignore-all-space $outfile.test.c $reffile.c 2>&1`
    err=`cat /tmp/poccout | grep -v "\[CLooG\] INFO:"`;
    if ! [ -z "$z" ]; then
	echo "DEBUG: diff output:[BEGIN] $z [END]";
	echo "\033[31m[FAIL] PoCC -> generated codes are different\033[0m";
	outtemp=1;
	output=1;
    elif ! [ -z "$err" ]; then
	echo "\033[31m[WARNING] PoCC -> stderr output: $err\033[0m";
    fi;
    if [ "$outtemp" -eq 0 ]; then
	echo "[INFO] Generated file is conform";
    fi;
    ## (2) Check that the generated files do compile
#    rm -f /tmp/poccout;
    z=`gcc $outfile.test.c -o $outfile.test.bin 2>&1`;
    ret="$?";
    gcchasnoomp=`echo "$z" | grep "error: omp.h: No such file"`;
    numlines=`echo "$z" | wc -l`;
    if [ "$ret" -ne 0 ] && [ "$numlines" -eq 1 ] && ! [ -z "$gcchasnoomp" ]; then
	echo "[INFO] GCC version used does not support OpenMP";
	ret=0;
    fi;
    if ! [ "$?" -eq 0 ]; then
	echo  "\033[31m[FAIL] PoCC -> generated file does not compile\033[0m";
	outtemp=1;
	output=1;
    else
	echo "[INFO] Generated file does compile correctly";
    fi;
    if [ "$outtemp" -eq 0 ]; then
	echo "[PASS] $i";
    fi;
    rm -f $outfile.test.bin;
done
if [ "$output" -eq 1 ]; then
    echo "\033[31m[FAIL] $1\033[0m";
else
    echo "[PASS] $1";
fi
exit $output
