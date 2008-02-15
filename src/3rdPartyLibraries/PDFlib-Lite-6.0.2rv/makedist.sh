#!/bin/sh
#
# This directory creates a distribution source tree containing the
# files and directory structure given by 'distdirs.txt' and
# 'distfiles.txt'.
#
# The source tree to copy is always assumed to be rooted at the
# directory containing this script.
#

SRCDIR=`dirname $0`; export SRCDIR
DESTDIR="$1"

test -d "${DESTDIR}" || exit 1
test -f "${SRCDIR}/distdirs.txt" || exit 1
test -f "${SRCDIR}/distfiles.txt" || exit 1

for d in `cat "${SRCDIR}/distdirs.txt"` ; do
	mkdir -p "${DESTDIR}/${d}" || exit 1
done

ABS_SRCDIR="`cd ${SRCDIR}; pwd`"

for f in `cat "${SRCDIR}/distfiles.txt"` ; do
	ln -sf "${ABS_SRCDIR}/${f}" "${DESTDIR}/${f}" || exit 1
done

# eof
