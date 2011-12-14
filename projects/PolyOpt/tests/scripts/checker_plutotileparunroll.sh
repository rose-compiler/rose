#!/bin/sh
## checker.sh for pocc in /Users/pouchet
## 
## Made by Louis-Noel Pouchet
## Contact: <louis-noel.pouchet@inria.fr>
## 
## Started on  Sat Jul 18 16:57:09 2009 Louis-Noel Pouchet
## Last update Sat Jul 18 18:27:08 2009 Louis-Noel Pouchet
##

./$CHECKER "PoCC --pluto-tile --pluto-parallel --pluto-unroll" "$TEST_SUITE" tileparunroll
