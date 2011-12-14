#!/bin/sh
## checker.sh for pocc in /Users/pouchet
## 
## Made by Louis-Noel Pouchet
## Contact: <louis-noel.pouchet@inria.fr>
## 
## Started on  Sat Jul 18 16:57:09 2009 Louis-Noel Pouchet
## Last update Mon Jul 20 17:04:04 2009 Louis-Noel Pouchet
##

./$CHECKER "PoCC --pluto-tile --pluto-parallel --pluto-unroll --pluto-prevector" "$TEST_SUITE" tileparunrollprev
