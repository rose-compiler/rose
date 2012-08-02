#!/bin/sh

################################################################################
#
# pluto-compare.sh
#
# Runs benchmarks through pluto and plutorose, collects translated programs into 
# an output directory
################################################################################

# Change these to fit your environment
PLUTOROSE_DIR="$HOME/research/plutorose"
PLUTO_DIR="$HOME/research/pluto-0.5.0-pre"
OUTPUT_DIR="$PLUTOROSE_DIR/benchmarks/pluto-compare-results"

# Location of the benchmarks
PLUTO_BENCHMARKS_DIR="$PLUTOROSE_DIR/benchmarks/pluto"

# Command lines for polycc and plutorose
POLYCC="$PLUTO_DIR/polycc --nofuse"
PLUTOROSE="$PLUTOROSE_DIR/plutorose"

# Check for pluto and plutorose directories, exit if they're not there
if [ ! -d "$PLUTO_DIR" ] ; then
  echo "$PLUTO_DIR not found, exiting..."
  exit 1
fi
if [ ! -d "$PLUTOROSE_DIR" ] ; then
  echo "$PLUTOROSE_DIR not found, exiting..."
  exit 1
fi

# Make the output dir if it doesn't exist
if [ ! -d "$OUTPUT_DIR" ] ; then
  mkdir -p "$OUTPUT_DIR"
fi

# List of benchmarks to translate
PLUTO_BENCHMARKS="adi 
                  doitgen
                  fdtd-2d 
                  gemver 
                  jacobi-1d-imper 
                  jacobi-2d-imper 
                  lu 
                  matmul 
                  matmul-init 
                  mvt 
                  seidel 
                  stencil3d
                  template"

# Run all benchmarks through pluto and plutorose
for BENCHMARK in $PLUTO_BENCHMARKS ; do
  cd $PLUTO_BENCHMARKS_DIR/$BENCHMARK
  echo "***********************************************************************"
  echo "pluto transform of $BENCHMARK"
  echo "***********************************************************************"
  $POLYCC $BENCHMARK.c
  cp $BENCHMARK.opt.c $OUTPUT_DIR
  echo "***********************************************************************"
  echo "plutorose transform of $BENCHMARK"
  echo "***********************************************************************"
  $PLUTOROSE $BENCHMARK.c
  cp rose_$BENCHMARK.c $OUTPUT_DIR
done
