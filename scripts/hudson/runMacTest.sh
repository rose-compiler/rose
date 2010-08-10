#!/bin/sh -ex

# Liao 7/2/2010, a dedicated script to build Mac test

echo "ROSE_TEST_GCC_VERSION" = $ROSE_TEST_GCC_VERSION
which gcc
gcc -v

# Note that Boost 1.39 and greater require using "--with-boost-libdir"
# CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib --with-java=${ROSE_TEST_JAVA_PATH} --with-ROSE_LONG_MAKE_CHECK_RULE=yes --enable-dq-developer-tests"
# CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib --with-java=${ROSE_TEST_JAVA_PATH} --with-ROSE_LONG_MAKE_CHECK_RULE=yes"
CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall"

echo "CONFIGURE_FLAGS = ${CONFIGURE_FLAGS}"

# DQ (1/14/2010): Support for dumping the enviroment so that it 
# can be easily sourced to permit Hudson tests to be reproduced.
ROSE_DUMP_HUDSON_ENV=yes
#if test "x$ROSE_DUMP_HUDSON_ENV" = "xyes" ; then
   echo "Dumping the Hudson environment...."
#   rm -f HudsonEnvironment
   echo "# Required environment variables for reproducing Hudson tests" > HudsonEnvironment
   echo "export PATH=$PATH" >> HudsonEnvironment
   echo "export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_JAVA_PATH=$ROSE_TEST_JAVA_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_BOOST_PATH=$ROSE_TEST_BOOST_PATH" >> HudsonEnvironment
   echo "export ROSE_CONFIGURE_FLAGS=\"$CONFIGURE_FLAGS\"" >> HudsonEnvironment
   echo "export ROSE_TEST_QT_PATH=$ROSE_TEST_QT_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_OPENGL_PATH=$ROSE_TEST_OPENGL_PATH" >> HudsonEnvironment
   echo "Finished dumping the Hudson environment into: $PWD/HudsonEnvironment"
   cat $PWD/HudsonEnvironment

#fi

# Skip this for now but uncomment before checkin!
if test "x$ROSE_TEST_BUILD_SKIP_BUILD_SCRIPT" = "xyes" ; then
   echo "Skipping build at part of testing from makefile rule."
else
   echo "Hudson tests on a new branch must run the ROSE build script."
   ./build
fi

# tps (02/04/2010) : Checking the EDG submodule version
git submodule status

if test -e ROSE-install ; then chmod -R u+w ROSE-install ; fi
rm -rf ROSE-install


if test -e ROSE-build ; then chmod -R u+w ROSE-build ; fi
rm -rf ROSE-build
mkdir ROSE-build
cd ROSE-build
echo "Starting configure step"
 ../configure ${CONFIGURE_FLAGS}
# we don't care installation path when building only EDG binaries
#../configure --prefix=${CONFIGURE_PREFIX_DIR} ${CONFIGURE_FLAGS}
echo "Done with configure step"


time make -j16 && \
time make check -j16 

if [ $? -ne 0 ]  ; then
  echo "fatal error when build edg binaries , aborting..."
  exit 3
else
  echo "Finished normally.."
  exit 0
fi

