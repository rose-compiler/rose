#!/bin/bash
# Wrapper script for loopbounds.pl

SATIRE_BASE=`which newanalysis | sed -e 's|/bin/newanalysis$||'`
TERMITE_LIB=${SATIRE_BASE}/share/termite

export TERMITE_LIB

if pl --version |grep -q SWI-Prolog; then 
   PL=pl    # Linux
else 
   PL=swipl # OS X, GNUstep
fi

../interval/interval $1 --output-term=$1.term ||exit 1
$PL -q -O -t main -f loopbounds.pl < $1.term > $1.loops.term ||exit 1