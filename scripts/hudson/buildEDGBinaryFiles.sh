#!/bin/sh -ex

# Liao 5/3/2010, a dedicated script to build EDG binaries

# We need to be using compiler specific versions of Boost.
# Set the path to the compiler specific version of Boost.
# export ROSE_TEST_BOOST_PATH=${ROSE_TEST_BOOST_PATH}-gnu_${ROSE_TEST_GCC_VERSION}/lib

# tps : 04/26/2010 - Added support for OpenGL for 64bit
export LD_LIBRARY_PATH="${ROSE_TEST_BOOST_PATH}/lib:${ROSE_TEST_OPENGL_PATH}/lib:$LD_LIBRARY_PATH"

echo "*******************************************************************"
echo "         Output variables required by Hudson runTest script."
echo "*******************************************************************"
echo "LD_LIBRARY_PATH                   = $LD_LIBRARY_PATH"
echo "ROSE_TEST_BUILD_SKIP_BUILD_SCRIPT = ${ROSE_TEST_BUILD_SKIP_BUILD_SCRIPT}"
echo "ROSE_TEST_BUILD_STYLE             = ${ROSE_TEST_BUILD_STYLE}"
echo "ROSE_TEST_BOOST_PATH              = ${ROSE_TEST_BOOST_PATH}"
echo "ROSE_TEST_JAVA_PATH               = ${ROSE_TEST_JAVA_PATH}"

echo "Environment variables required for ROSE_TEST_BUILD_STYLE = full"
echo "ROSE_TEST_QT_PATH                 = ${ROSE_TEST_QT_PATH}"
echo "ROSE_TEST_SQLITE_PATH             = ${ROSE_TEST_SQLITE_PATH}"
echo "ROSE_TEST_RTED_PATH               = ${ROSE_TEST_RTED_PATH}"
echo "ROSE_TEST_OPENGL_PATH             = ${ROSE_TEST_OPENGL_PATH}"
echo "*******************************************************************"

# Output the name of the machine where the build is being run (helpful for debugging).
machine_name=`uname -n`
start_time=`date`
start_time_seconds="$(date +%s)"
echo ""
echo "*****************************************************************************"
echo "Executing buildEDGBinaries.sh for ROSE Project on: $machine_name start_time = $start_time"
echo "*****************************************************************************"

if test "x$ROSE_TEST_GCC_VERSION" != "x4.1.2" ; then
	GCCROOT=/usr/apps/gcc/$ROSE_TEST_GCC_VERSION
	if test -e $GCCROOT/setup.sh ; then
		set +ex
		. $GCCROOT/setup.sh
		set -ex
	else
    # This is the case for g++ 3.4.6 (since there is no uniform existence of a setup.sh file).
      echo "Handling case of no $GCCROOT/setup.sh file."
		export PATH="$GCCROOT/bin:$PATH"

    # DQ (2/15/2010): For 64-bit this has to be setup to use lib64 explicitly
    # export LD_LIBRARY_PATH="$GCCROOT/lib:$LD_LIBRARY_PATH"
      if test "$MACHINE" = "i686" ; then
         export LD_LIBRARY_PATH="$GCCROOT/lib:$LD_LIBRARY_PATH"
      else
         export LD_LIBRARY_PATH="$GCCROOT/lib64:$GCCROOT/lib:$LD_LIBRARY_PATH"
      fi
	fi
fi

# Note that Boost 1.39 and greater require using "--with-boost-libdir"
# CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib --with-java=${ROSE_TEST_JAVA_PATH} --with-ROSE_LONG_MAKE_CHECK_RULE=yes --enable-dq-developer-tests"
# CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib --with-java=${ROSE_TEST_JAVA_PATH} --with-ROSE_LONG_MAKE_CHECK_RULE=yes"
CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib --with-java=${ROSE_TEST_JAVA_PATH} --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall"

# Setup the prefix directory so that an install tree can be generated 
# for use with secondary testing of other projects using ROSE.
# We want the install tree for all the different versions of ROSE in the 
# Hudson home directory so that any secondary tests will have it avaiable
# for testing any platform.
# Note that running make install is very quick so not a problem for a 
# remotely mounted directory.
MACHINE=`uname -m`
if test "$MACHINE" = "i686" ; then
  OS_MACHINE_LABEL=i686-linux
else
  OS_MACHINE_LABEL=amd64-linux
fi

if test "x$ROSE_TEST_BUILD_STYLE" = "xfull" ; then

 # DQ (1/14/2010): I am debugging the Hudson tests of failures on tux269.
 # echo "This test is forced to exit with an error as part of debugging a subset of the Hudson tests."
 # exit 1

	CONFIGURE_FLAGS="${CONFIGURE_FLAGS} --with-qt=${ROSE_TEST_QT_PATH} --with-roseQt --with-sqlite3=${ROSE_TEST_SQLITE_PATH} --with-rted=${ROSE_TEST_RTED_PATH} LDFLAGS=-L${ROSE_TEST_OPENGL_PATH}/lib CPPFLAGS=-I${ROSE_TEST_OPENGL_PATH}/include"
	if test "x$ROSE_TEST_GCC_VERSION" = "x4.1.2" ; then
		CONFIGURE_FLAGS="${CONFIGURE_FLAGS} --with-pch"
	fi
fi

echo "CONFIGURE_FLAGS = ${CONFIGURE_FLAGS}"

# DQ (1/14/2010): Support for dumping the enviroment so that it 
# can be easily sourced to permit Hudson tests to be reproduced.
ROSE_DUMP_HUDSON_ENV=yes
if test "x$ROSE_DUMP_HUDSON_ENV" = "xyes" ; then
   echo "Dumping the Hudson environment...."
#   rm -f HudsonEnvironment
   echo "# Required environment variables for reproducing Hudson tests" > HudsonEnvironment
   echo "export PATH=$PATH" >> HudsonEnvironment
   echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_JAVA_PATH=$ROSE_TEST_JAVA_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_BOOST_PATH=$ROSE_TEST_BOOST_PATH" >> HudsonEnvironment
   echo "export ROSE_CONFIGURE_FLAGS=\"$CONFIGURE_FLAGS\"" >> HudsonEnvironment
   echo "export ROSE_TEST_SQLITE_PATH=$ROSE_TEST_SQLITE_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_RTED_PATH=$ROSE_TEST_RTED_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_QT_PATH=$ROSE_TEST_QT_PATH" >> HudsonEnvironment
   echo "export ROSE_TEST_OPENGL_PATH=$ROSE_TEST_OPENGL_PATH" >> HudsonEnvironment

   echo "Finished dumping the Hudson environment into: $PWD/HudsonEnvironment"
   cat $PWD/HudsonEnvironment

 # echo "Exiting after dumping the environment..."
 # exit 1
fi

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

#if test $ROSE_TEST_BUILD_STYLE = edg ; then
	make -j8 upload_edg_binary && \
	make -Csrc/frontend/CxxFrontend deploy_tarballs
      if [ $? -ne 0 ]  ; then
        echo "fatal error when build edg binaries , aborting..."
        exit 3
      fi
	exit 0
#fi

