#!/bin/sh

if [ x$1 = x ]
then
    echo "Usage: $0 <INSTALL_PREFIX>"
    echo "  autoreconf, configure, test and install SATIrE to <INSTALL_PREFIX>"
    echo "  Paths are hardcoded for c8.complang.tuwien.ac.at"
    exit 1
fi

JOBS=-j8 # 8 parallel compiler jobs

autoreconf -i && \
  ./configure --prefix=$1 --with-rosedir=/usr/local/mstools/rose --with-pagdir=/usr/local/mstools/pag --with-boostdir=/usr/local/mstools/boost CXXFLAGS="-O2 -ggdb -Werror" CXX=g++-4.2.0 && \
  make $JOBS && \
  make $JOBS distcheck && \
  make install && \
  make $JOBS installcheck
