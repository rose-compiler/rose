#!/bin/sh

if [ x$1 = x ]
then
    echo "Usage: $0 <INSTALL_PREFIX>"
    echo "  autoreconf, configure, test and install SATIrE to <INSTALL_PREFIX>"
    echo "  Paths are hardcoded for c8.complang.tuwien.ac.at"
    exit 1
fi


autoreconf -i && \
  ./configure --prefix=$1 --with-rosedir=/usr/local/mstools/rose --with-pagdir=/usr/local/mstools/pag CXXFLAGS="-O0 -ggdb -Werror" && \
  make && \
  make distcheck && \
  make install && \
  make installcheck
