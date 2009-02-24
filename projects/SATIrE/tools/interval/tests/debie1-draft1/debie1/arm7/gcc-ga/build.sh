#!/bin/sh
#
# Script to compile debie1.exe using the gnuarm.com ARM7 GCC compiler.
#


# Set tpd to the Test Program Directory:
tpd=../..

source ./setup.sh

export CXOPT="${CCOPT} -I."

${CC} ${CXOPT} -c ${tpd}/class.c     >class.lst
${CC} ${CXOPT} -c ${tpd}/classtab.c  >classtab.lst
${CC} ${CXOPT} -c ${tpd}/debie.c     >debie.lst
${CC} ${CXOPT} -c ${tpd}/harness.c   >harness.lst
${CC} ${CXOPT} -c ${tpd}/health.c    >health.lst
${CC} ${CXOPT} -c ${tpd}/hw_if.c     >hw_if.lst
${CC} ${CXOPT} -c ${tpd}/measure.c   >measure.lst
${CC} ${CXOPT} -c ${tpd}/tc_hand.c   >tc_hand.lst
${CC} ${CXOPT} -c ${tpd}/telem.c     >telem.lst


${CC} ${LDOPT}           \
    -o debie1.exe        \
    class.o              \
    classtab.o           \
    debie.o              \
    harness.o            \
    health.o             \
    hw_if.o              \
    measure.o            \
    tc_hand.o            \
    telem.o              \
    >ld.lst 2>&1

