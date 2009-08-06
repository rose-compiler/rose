#!/bin/sh

JOBS=-j8 # 8 parallel compiler jobs
CCARGS="CXX=g++-4.2.0 CC=gcc-4.2.0"
PREFIX="`pwd`/install"

ENABLE_CLANG="0"
DISABLE_SATIRE="0"
DISABLE_CHECKS="0"

#parse command line options
for i in $@; do
  case $i in
      -help | -h | --help)
          echo "Usage: $0 <INSTALL_PREFIX> [-help] [-enable-clang] [-disable-satire] [-disable-checks] [-gcc-<ver>]" 
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
      -disable-checks) DISABLE_CHECKS="1"
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
    if [ "$DISABLE_CHECKS" ==  "1" ]; then
        (libtoolize && \
            autoreconf -i && \
            ./configure --prefix=$PREFIX --with-rosedir=/usr/local/mstools/rose --with-pagdir=/usr/local/mstools/pag --with-boostdir=/usr/local/mstools/boost CXXFLAGS="-O2 -ggdb" $CCARGS && \
            make $JOBS && \
            make install) \
            || exit 1
    else
        (libtoolize && \
            autoreconf -i && \
            ./configure --prefix=$PREFIX --with-rosedir=/usr/local/mstools/rose --with-pagdir=/usr/local/mstools/pag --with-boostdir=/usr/local/mstools/boost CXXFLAGS="-O2 -ggdb" $CCARGS && \
            make $JOBS && \
            make $JOBS distcheck && \
            make install && \
            make $JOBS installcheck ) \
            || exit 1
    fi
fi

#configure and build clang and llvm
if [ "$ENABLE_CLANG" == "1" ] ; then
    make clang
fi


