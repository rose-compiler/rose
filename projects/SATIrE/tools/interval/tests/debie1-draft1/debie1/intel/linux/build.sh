#!/bin/sh
#
# Script to compile debie1 on the host Intel/Linux system.
#
# Any arguments go into the CCOPT (eg. -Wpadded).


# Set tpd to the Test Program Directory:
tpd=../..

# Native gcc and options:

export CC="gcc"
export CCOPT="-g -O2 -I. -Wall $*"
export LD="gcc"
export LDOPT=

${CC} ${CCOPT} -c ${tpd}/class.c
${CC} ${CCOPT} -c ${tpd}/classtab.c
${CC} ${CCOPT} -c ${tpd}/debie.c
${CC} ${CCOPT} -c ${tpd}/harness.c -DTRACE_HARNESS
${CC} ${CCOPT} -c ${tpd}/health.c
${CC} ${CCOPT} -c ${tpd}/hw_if.c
${CC} ${CCOPT} -c ${tpd}/measure.c
${CC} ${CCOPT} -c ${tpd}/tc_hand.c
${CC} ${CCOPT} -c ${tpd}/telem.c


${CC} ${LDOPT}           \
    -o debie1            \
    class.o              \
    classtab.o           \
    debie.o              \
    harness.o            \
    health.o             \
    hw_if.o              \
    measure.o            \
    tc_hand.o            \
    telem.o

