#!/usr/bin/env bash

#
# Set up script for ROSE for network reachable CentOS 6.5 x64
#

prog="$(readlink "$0" 2>&1)"
[ $? -eq 127 -o "$prog" = "" ] && prog="$0"
progdir="$(dirname "$prog")"
source $progdir/setenv-rose.sh


################################################################
# Ensure that JAVA_HOME points to a ROSE compatible version
################################################################
#_JAVA_VERSION=$("$JAVA_HOME/bin/java" -version 2>&1)
#check_jdk "$_JAVA_VERSION"
#if [ $? -ne 0 ]; then
#echo "JAVA_HOME is set but not sure it's compatible with ROSE. Please unset JAVA_HOME and run again."
#exit 1
#fi
[ -z "$_HAS_JDK" ] && exit 1


echo "========================================================================"
echo "  Set up ROSE for network reachable CentOS 6.5 x64                      "
echo "    started on $(date)                                                  "
echo "========================================================================"
#echo "JAVA_HOME=$JAVA_HOME"


################################################################
# Install and update toolchain for ROSE development
################################################################

echo "Installing/updating toolchain requires root privilege and is recommended for the first time when you set up ROSE."
read -t 5 -p "Do you want to install/update toolchain (y/N)? " update_toolchain
if [[ ( $? -eq 0 ) && ( ( "$update_toolchain" = "y" ) || ( "$update_toolchain" = "Y" ) ) ]]; then
echo "========================================================================"
echo "  Installing/updating toolchain...                                      "
echo "========================================================================"
sudo yum -y upgrade
sudo yum -y install git gcc-c++.x86_64 gcc-gfortran.x86_64 automake libtool flex bison byacc.x86_64 ghostscript graphviz subversion
#echo "Removing temporary files..."
#sudo yum -y clean all
echo "========================================================================"
echo "  Installing/updating toolchain is DONE                                 "
echo "========================================================================"
else
echo ""
echo "========================================================================"
echo "  Installing/updating toolchain is SKIPPED                              "
echo "========================================================================"
fi


################################################################
# Build boost
################################################################

#
# The latest supported version of boost is 1.47.0.  Newer version does not work.
#

#
# NOTE: keep the followings consistent
#
_BOOST_VER=1.47.0
_BOOST_DIR=boost_1_47_0
_BOOST_TAR=${_BOOST_DIR}.tar.gz
_BOOST_URL=http://sourceforge.net/projects/boost/files/boost/$_BOOST_VER/$_BOOST_TAR/download

if [ ! -d "$_BOOST_ROOT/$_BOOST_VER" ]; then
cd /tmp
if [ ! -f "$_BOOST_TAR" ]; then
echo "========================================================================"
echo "  Downloading Boost...                                                  "
echo "========================================================================"
wget --no-check-certificate $_BOOST_URL -O $_BOOST_TAR
echo "========================================================================"
echo "  Downloading Boost is DONE                                             "
echo "========================================================================"
fi
echo "========================================================================"
echo "  Building Boost...                                                     "
echo "========================================================================"
rm -rf $_BOOST_DIR
tar xfz $_BOOST_TAR
cd $_BOOST_DIR
mkdir -p $_BOOST_ROOT
./bootstrap.sh --prefix="$_BOOST_ROOT/$_BOOST_VER"
./b2 install --prefix="$_BOOST_ROOT/$_BOOST_VER" -j2
(cd $_BOOST_ROOT; rm -f latest; ln -s $_BOOST_VER latest)
#echo "Removing temporary files..."
#(cd /tmp; rm -rf $_BOOST_DIR $_BOOST_TAR)
echo "========================================================================"
echo "  Building Boost is DONE                                                "
echo "========================================================================"
else
echo "========================================================================"
echo "  Building Boost is SKIPPED                                             "
echo "========================================================================"
fi


################################################################
# Build X10
################################################################

if [ ! -d "$_X10_DIST" ]; then
##### NOTE: checkout from sourceforge
####_X10_URL=https://svn.code.sf.net/p/x10/code/branches/x10-$_X10_VER
####echo "========================================================================"
####echo "  Checking out X10 from SourceForge... (this may take a few minutes)    "
####echo "========================================================================"
####svn checkout -q $_X10_URL "$_X10_DIST/.."
####echo "========================================================================"
####echo "  Checking out X10 from SourceForge is DONE                             "
####echo "========================================================================"
# NOTE: download source tarball
_X10_TAR=x10-${_X10_VER}-src.tar.bz2
_X10_URL=http://downloads.sourceforge.net/project/x10/x10/$_X10_VER/$_X10_TAR
if [ ! -f "/tmp/$_X10_TAR" ]; then
echo "========================================================================"
echo "  Downloading X10 source tarball...                                     "
echo "========================================================================"
wget $_X10_URL -O /tmp/$_X10_TAR
echo "========================================================================"
echo "  Downloading X10 source tarball is DONE                                "
echo "========================================================================"
fi
mkdir -p "$_X10_DIST/.."
(cd "$_X10_DIST/.."; tar xfj /tmp/$_X10_TAR)
[ "$(whoami)" = "root" ] && chown -R root:root "$_X10_DIST"
#echo "Removing temporary files..."
#rm /tmp/$_X10_TAR
echo "========================================================================"
echo "  Building X10... (this may take half an hour)                          "
echo "========================================================================"
(cd "$_X10_DIST"; ant -Doptimize=true clean dist)
echo "========================================================================"
echo "  Building X10 is DONE                                                  "
echo "========================================================================"
else
echo "========================================================================"
echo "  Building X10 is SKIPPED                                               "
echo "========================================================================"
fi


################################################################
# Install ROSE
################################################################

mkdir -p $_ROSE_ROOT
cd $_ROSE_ROOT

unset _ROSE_HASH_CHANGED
_ROSE_REPO=edg4x-rose
if [ -d "$_ROSE_REPO" ]; then
echo "ROSE has already been cloned."
echo "Pulling ROSE repository may require rebuilding ROSE."
read -t 5 -p "Do you want to pull ROSE repository? (y/N)? " pull_rose
if [[ ( $? -eq 0 ) && ( ( "$pull_rose" = "y" ) || ( "$pull_rose" = "Y" ) ) ]]; then
pushd $_ROSE_REPO > /dev/null
_ROSE_HASH_OLD=$(git show --format=%h | head -1)
popd > /dev/null
echo "========================================================================"
echo "  Pulling ROSE repository from GitHub...                                "
echo "========================================================================"
(cd $_ROSE_REPO; git pull)
echo "========================================================================"
echo "  Pulling ROSE repository from GitHub is DONE                           "
echo "========================================================================"
pushd $_ROSE_REPO > /dev/null
_ROSE_HASH_NEW=$(git show --format=%h | head -1)
popd > /dev/null
[ "$_ROSE_HASH_OLD" = "_ROSE_HASH_NEW" ] || _ROSE_HASH_CHANGED=1
else
echo ""
echo "========================================================================"
echo "  Pulling ROSE repository from GitHub is SKIPPED                        "
echo "========================================================================"
fi
else

if [ -n "$GITHUB_USER" ]; then
if [ -z "$GITHUB_OWNER" ]; then
GITHUB_OWNER="$GITHUB_USER"
fi
echo "GITHUB_USER is defined. Use a fork owned by $GITHUB_OWNER as master ROSE repository."
_ROSE_URL="https://${GITHUB_USER}@github.com/${GITHUB_OWNER}/${_ROSE_REPO}.git"
else
echo "GITHUB_USER is not defined. Use rose-compiler as master ROSE repository."
_ROSE_URL="https://github.com/rose-compiler/${_ROSE_REPO}.git"
fi
echo "The URL of ROSE repository is $_ROSE_URL"

echo "========================================================================"
echo "  Cloning ROSE repository from GitHub... (this may take half an hour)   "
echo "========================================================================"
# NOTE: don't pop-up password dialog
#git clone $_ROSE_URL
GIT_ASKPASS=echo git clone $_ROSE_URL
echo "========================================================================"
echo "  Cloning ROSE repository from GitHub is DONE                           "
echo "========================================================================"
_ROSE_HASH_CHANGED=1
fi

pushd $_ROSE_REPO > /dev/null
_ROSE_COMMIT=commit-$(git show --format=%h | head -1)
popd > /dev/null
_ROSE_BUILD=$_ROSE_ROOT/build/$_ROSE_COMMIT
_ROSE_INSTALL=$_ROSE_ROOT/install/$_ROSE_COMMIT

unset _BUILD_ROSE
[[ ( -z "$_ROSE_HASH_CHANGED" ) && ( -d "$_ROSE_BUILD" ) && ( -d "$_ROSE_INSTALL" ) ]] || _BUILD_ROSE=1
if [[ ( -z "$_BUILD_ROSE" ) && ( -z "$_ROSE_HASH_CHANGED" ) ]]; then
echo "ROSE repository isn't changed but you may want to rebuild ROSE because of local changes."
read -t 5 -p "Do you want to rebuild ROSE (y/N)? " rebuild_rose
if [[ ( $? -eq 0 ) && ( ( "$rebuild_rose" = "y" ) || ( "$rebuild_rose" = "Y" ) ) ]]; then
_BUILD_ROSE=1
fi
fi

if [ -n "$_BUILD_ROSE" ]; then
cd $_ROSE_REPO
echo "========================================================================"
echo "  Preparing for building ROSE...                                        "
echo "========================================================================"
./build
echo "========================================================================"
echo "  Preparing for building ROSE is DONE                                   "
echo "========================================================================"
rm -rf "$_ROSE_INSTALL"
mkdir -p "$_ROSE_INSTALL"
rm -rf "$_ROSE_BUILD"
mkdir -p "$_ROSE_BUILD"
cd $_ROSE_BUILD
echo "========================================================================"
echo "  Configuring ROSE...                                                   "
echo "========================================================================"
$_ROSE_ROOT/$_ROSE_REPO/configure --prefix="$_ROSE_INSTALL" --with-boost="$_BOOST_ROOT/latest" --with-x10="$_X10_DIST"
echo "========================================================================"
echo "  Configuring ROSE is DONE                                              "
echo "========================================================================"
echo "========================================================================"
echo "  Building ROSE... (this may take several hours)                        "
echo "========================================================================"
time make -j4 install-core
(cd $_ROSE_INSTALL/..; rm -f latest; ln -s $_ROSE_COMMIT latest)
#echo "Removing temporary files..."
#(cd $_ROSE_BUILD; make clean)
echo "========================================================================"
echo "  Building ROSE is DONE                                                 "
echo "========================================================================"
else
echo ""
echo "========================================================================"
echo "  Building ROSE is SKIPPED                                              "
echo "========================================================================"
fi


################################################################
# Everything is DONE
################################################################

echo "========================================================================"
echo "  Congratulations!                                                      "
echo "  ROSE has been successfully set up!                                    "
echo "    finished on $(date)                                                 "
echo "========================================================================"
