#!/bin/sh

if test x"$1" = x ; then
  echo \
"
usage: $0 <lib1.a> [<lib2.a> [<lib3.a> ...]]

Merges the static libraries <lib2.a>, <lib3.a>,
and so on, into <lib1.a>. If <lib1.a> does not
exist, it is created. Otherwise, its contents
are preserved during the merge.

To override the library archive utility or
the 'ranlib' utility, define the environment
variables AR and RANLIB, respectively.
"
  exit 1
fi

AR=${AR-ar}
RANLIB=${RANLIB-ranlib}
TMPDIR=${TMPDIR-/tmp}

CHILD_LIB_BASE=x_child_lib
CHILD_LIB_DIR=${TMPDIR}/${CHILD_LIB_BASE}
rm -rf ${CHILD_LIB_DIR}
mkdir -p ${CHILD_LIB_DIR}

LIB_1="$1" ; shift
LIB_1_BASE=`basename "${LIB_1}"`

if test -f ${LIB_1} ; then
  if ! cp ${LIB_1} ${TMPDIR}/${LIB_1_BASE} ; then
    echo "*** Out of space ***" && exit 1
  fi
else
  rm -f ${TMPDIR}/${LIB_1_BASE}
fi

SAVE_DIR=`pwd`
while test x"$1" != x ; do
  LIB_i="$1"
  shift
  LIB_i_BASE=`basename "${LIB_i}"`

  rm -rf ${CHILD_LIB_DIR}/*
  if ! cp ${LIB_i} ${TMPDIR}/. ; then
    echo "*** Out of space copying ${LIB_i} ***" && exit 1
  fi
  cd ${CHILD_LIB_DIR}
  if ! ar x ../${LIB_i_BASE} ; then
    echo "*** Can't extract files from ${LIB_i} ***"
  fi
  cd ..
  if ! ar r ${LIB_1_BASE} ${CHILD_LIB_BASE}/* ; then
    echo "*** Out of space while merging ${LIB_i} ***" && exit 1
  fi
  cd ${SAVE_DIR}
done

# Everything should be OK so far...
if mv ${TMPDIR}/${LIB_1_BASE} ${LIB_1}-chk_copy ; then
  rm -f ${LIB_1}
  mv ${LIB_1}-chk_copy ${LIB_1}
  echo "Merge was successful."
  RETURN_CODE=0
else # Couldn't copy library
  echo "*** Ran out of space; leaving libraries untouched ***"
  RETURN_CODE=1
fi

# clean-up and return
rm -rf ${TMPDIR}/${LIB_1_BASE} ${CHILD_LIB_DIR}
exit ${RETURN_CODE}

# eof
