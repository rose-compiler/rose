#!/bin/sh -ex

# Liao 5/3/2010, a dedicated script to build EDG binaries

OS_NAME=`uname -s`

if test "$OS_NAME" = "Darwin" ; then 
  echo "Detected the current OS is Darwin, using the default GCC version for now ...."
else
  # For Linux boxes, they should be all using RH 5, with GCC 4.1.2  as a default one
  # default available GCC is 4.1.2
  # special handling for other versions
  #   either call setup.sh or export PATH manually
  if test "x$ROSE_TEST_GCC_VERSION" != "x4.1.2" ; then
     GCCROOT=/usr/apps/gcc/$ROSE_TEST_GCC_VERSION
     if test -e $GCCROOT/setup.sh ; then
        set +ex
        source $GCCROOT/setup.sh
        set -ex
     else
      # This is the case for g++ 3.4.6 (since there is no uniform existence of a setup.sh file).
        echo "Handling case of no $GCCROOT/setup.sh file."
        export PATH="$GCCROOT/bin:$PATH"
        MACHINE=`uname -p`
        # DQ (2/15/2010): For 64-bit this has to be setup to use lib64 explicitly
        if test "$MACHINE" = "i686" ; then
           export LD_LIBRARY_PATH="$GCCROOT/lib:$LD_LIBRARY_PATH"
        else
           export LD_LIBRARY_PATH="$GCCROOT/lib64:$GCCROOT/lib:$LD_LIBRARY_PATH"
        fi
    fi
  fi
fi # end of if test Darwin

echo "ROSE_TEST_GCC_VERSION" = $ROSE_TEST_GCC_VERSION
which gcc
gcc -v

CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall"

# explicit JAVA path for Linux boxes
if test "$OS_NAME" != "Darwin" ; then
  CONFIGURE_FLAGS="${CONFIGURE_FLAGS} --with-java=${ROSE_TEST_JAVA_PATH}"
fi

echo "CONFIGURE_FLAGS = ${CONFIGURE_FLAGS}"

# Skip this for now but uncomment before checkin!
if test "x$ROSE_TEST_BUILD_SKIP_BUILD_SCRIPT" = "xyes" ; then
   echo "Skipping build at part of testing from makefile rule."
else
   echo "Hudson tests on a new branch must run the ROSE build script."
   ./build
fi

# tps (02/04/2010) : Checking the EDG submodule version
git submodule status

if test -e ROSE-install ; then chmod -R u+wx ROSE-install ; fi
rm -rf ROSE-install


if test -e ROSE-build ; then chmod -R u+wx ROSE-build ; fi
rm -rf ROSE-build
mkdir ROSE-build
cd ROSE-build
echo "Starting configure step"
 ../configure ${CONFIGURE_FLAGS}
# we don't care installation path when building only EDG binaries
#../configure --prefix=${CONFIGURE_PREFIX_DIR} ${CONFIGURE_FLAGS}
echo "Done with configure step"

if test "$OS_NAME" = "Darwin" ; then
# Mac box does not have direct access to /nfs/casc/.. now, 
# we use ssh instead
  cd src/frontend/CxxFrontend/
  rm -rf *.tar.gz
  make -j8 binary_edg_tarball
  BIN_FILE=`ls *.tar.gz`
  ssh tux269 "test -e /nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries/$BIN_FILE" 
  if [ $? -ne 0 ]  ; then 
    scp src/frontend/CxxFrontend/*.tar.gz hudson-rose@tux269.llnl.gov:/nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries/.
  fi
else
  make -j8 upload_edg_binary && \
  make -Csrc/frontend/CxxFrontend deploy_tarballs
fi
if [ $? -ne 0 ]  ; then
  echo "fatal error when build edg binaries , aborting..."
  exit 3
else
  echo "Finished normally.."
  exit 0
fi

