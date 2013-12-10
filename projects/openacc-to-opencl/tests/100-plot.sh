#!/bin/bash

source $1/test-vars.rc

rm $1/graphs
mkdir -p $1/graphs



echo "" > gunplot.cmd

echo -n "plot" >> gunplot.cmd

executable-103-tile_0.dat

for gang in 1 2 4 8 16 32
do
  for worker in 1 2 4 8 16 32 64 128
  do
    paste ref.log acc1-$gang-$worker.log | awk '{ print $1, $4, $5, $2/$6, $2/$7 }' > acc1-$gang-$worker.dat
    echo -n " \"acc1-$gang-$worker.log\" using 1:4 title \"T0=? G=$gang T1=1 W=$worker T2=1\"," >> gunplot.cmd

    paste ref.log acc2-$gang-$worker.log | awk '{ print $1, $4, $5, $2/$6, $2/$7 }' > acc2-$gang-$worker.dat
    echo -n " \"acc2-$gang-$worker.log\" using 1:4 title \"T0=1 G=$gang T1=? W=$worker T2=1\"," >> gunplot.cmd

    paste ref.log acc3-$gang-$worker.log | awk '{ print $1, $4, $5, $2/$6, $2/$7 }' > acc3-$gang-$worker.dat
    echo -n " \"acc3-$gang-$worker.log\" using 1:4 title \"T0=1 G=$gang T1=1 W=$worker T2=?\"," >> gunplot.cmd
  done
done

gnuplot plotter.gplot

