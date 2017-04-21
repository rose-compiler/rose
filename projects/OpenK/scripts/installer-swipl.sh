#!/bin/bash
#/usr/bin/env bash
# This is a bash script to download and install SWIPL
# To use this script, cd yourhome/opt 
# type ./installer-swipl.sh 
# SWI Prolog will be installed into ~/opt/swipl-7.4.2

WGET=wget
CURRENT_DIR=`pwd`
INSTALL_DIR=`pwd`/swipl

#VERSION=7.2.1
VERSION=7.4.2

# download and extract the source folder 
if [ "$(uname)" == "Darwin" ]; then
    WGET='curl -O '
    TARGET='--target=x86_64'
fi

if [ -d swipl-$VERSION ]; then
    echo Found swipl-$VERSION! Not downloading source again.
else
    $WGET http://www.swi-prolog.org/download/stable/src/swipl-$VERSION.tar.gz
    tar -xzf swipl-$VERSION.tar.gz
fi

if [ -d swipl-$VERSION ]; then
    echo Everything looks normal so far.
    BUILD_DIR=`pwd`/swipl-$VERSION
else
    echo Install had problems. Quitting.
    exit
fi

# configure and build swipl-$VERSION/src
if [ -d $INSTALL_DIR ]; then
    cd $INSTALL_DIR
    echo Found $INSTALL_DIR. Please remove to reinstall.
    exit
else
    mkdir -p $INSTALL_DIR
    cd $BUILD_DIR/src
    ./configure --prefix=$INSTALL_DIR --enable-shared
fi

if [ -x $INSTALL_DIR/bin/swipl ]; then
    echo swipl successfully installed...
else
    echo Building swipl core...
    make && make install 

    if [ -x $INSTALL_DIR/bin/swipl ]; then
        true
    else
        echo swipl not installed properly.
        exit 0
    fi
fi

# build swipl-$VERSION/packages
if [ -d $BUILD_DIR/packages ]; then
    cd $BUILD_DIR/packages
    ./configure
    echo Building swipl packages...
    make && make install
else
    echo Not find packages folder. Please check the downloaded source
    exit 1
fi

# set up PATH
cd $CURRENT_DIR
touch prolog-bashrc
echo PATH=$INSTALL_DIR/bin:\$PATH > set.swipl
echo "export PATH" >> set.swipl

export PATH=$PATH:$INSTALL_DIR/bin

echo "Install completed. Please add set.swipl to your .bashrc"
echo The executables are stored in $INSTALL_DIR/bin/
echo ..the library in $INSTALL_DIR/lib/pl-%plversion/ 
echo ..and the include files in $INSTALL_DIR/include.
