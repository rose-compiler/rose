#!/bin/sh

JOBS=-j8 # 8 parallel compiler jobs
CCARGS="CXX=g++-4.2.0 CC=gcc-4.2.0"
PREFIX="`pwd`/install"

ENABLE_CLANG="0"
DISABLE_SATIRE="0"

#parse command line options
for i in $@; do
  case $i in
      -help | -h | --help)
          echo "Usage: $0 <INSTALL_PREFIX> [-help] [-enable-clang] [-disable-satire] [-gcc-<ver>]" 
          echo "  autoreconf, configure, test and install SATIrE to <INSTALL_PREFIX>"
          echo "  Paths are hardcoded for c8.complang.tuwien.ac.at"
          exit 1
          ;;
      -gcc-*)
          CC="gcc-${i:5}"
          CXX="g++-${i:5}"
          CCARGS="CC=$CC CXX=$CXX"
          export CC=$CC
          export CXX=$CXX
          ;;
      -enable-clang) ENABLE_CLANG="1"
          ;;
      -disable-satire) DISABLE_SATIRE="1" # build only CLANG frontend
          ;;
      -*) echo "invalid option $i"
          exit 1
          ;;
      *) PREFIX="$i";;
  esac
done;

echo "using prefix $PREFIX"

#configure and build satire
if [ "$DISABLE_SATIRE" == "0" ] ; then
    echo "building satire"
    libtoolize && \
    autoreconf -i && \
        ./configure --prefix=$PREFIX --with-rosedir=/usr/local/mstools/rose --with-pagdir=/usr/local/mstools/pag --with-boostdir=/usr/local/mstools/boost CXXFLAGS="-O2 -ggdb -Werror" $CCARGS && \
        make $JOBS && \
        make $JOBS distcheck && \
        make install && \
        make $JOBS installcheck
fi

#configure and build clang and llvm
if [ "$ENABLE_CLANG" == "1" ] ; then
    function exit_failure {
        echo "BUILD FAILED: $1";
        exit 1;
    }
    
    function update_or_checkout_svn {
        DIR="$1"
        URL="$2"
        VER="$3"
        pushd `dirname $DIR`
        echo "svn-checkout for $2 (version $3) into $1"
        #do nothing if the directory is already present to preserve local changes
        if [ ! -d $DIR ] ; then
            svn co $URL `basename $DIR` -r $VER || exit_failure "svn checkout failed"
        else
            echo "already done before"
        fi
        popd
    }

    echo "building clang frontend";
    BASEDIR="`pwd`/clang";
    LLVMDIR="$BASEDIR/llvm";
    CLANGDIR="$BASEDIR/llvm/tools/clang";
    BUILDDIR="$BASEDIR/build";
    SRCDIR="`pwd`/src/termite/clang";

    LLVM_SVN="`cat $SRCDIR/VERSIONS | grep LLVM | cut  -f 2`";
    LLVM_VER="`cat $SRCDIR/VERSIONS | grep LLVM | cut  -f 3`";
    CLANG_SVN="`cat $SRCDIR/VERSIONS | grep CLANG | cut  -f 2`";
    CLANG_VER="`cat $SRCDIR/VERSIONS | grep CLANG | cut  -f 3`";

    #make sure we have a sane environment
    which svn > /dev/null && which darcs > /dev/null && which patch > /dev/null
    if [ ! "$?x" ==  "0x" ] ; then
        echo "can't find required tools"
        exit 1
    fi

    #create directories if necessary
    mkdir -p $BUILDDIR

    #update llvm and clang from svn
    update_or_checkout_svn $LLVMDIR $LLVM_SVN $LLVM_VER
    update_or_checkout_svn $CLANGDIR $CLANG_SVN $CLANG_VER

    #apply the patch if necessary
    if [ -f $CLANGDIR/Driver/TermDumper.cpp ] ; then
        echo "clang patch already applied, reverting"
        pushd $CLANGDIR
        svn revert . -R || exit_failure "svn revert failed"
        #we have to manually remove TermDumper.cpp as it's not part of the official distribution
        rm -rf $CLANGDIR/Driver/TermDumper.cpp
        popd
    fi

    echo "applying clang patch"
    pushd $CLANGDIR
    patch -p0 < $SRCDIR/clang-termite.patch || exit_failure "failed ot apply clang patch"
    popd
   
    #configure and build clang and llvm
    echo "building clang+llvm"
    pushd $BUILDDIR
    if [ ! -f ./Makefile ] ; then
        $LLVMDIR/configure --prefix=$PREFIX $LLVM_CONFIG $CCARGS || exit_failure "configuring LLVM failed"
    fi;

    make ENABLE_OPTIMIZED=0 $CCARGS $JOBS || exit_failure "building llvm failed"
    make install || exit_failure "installing llvm failed"            
    popd
    echo "DONE"
fi


