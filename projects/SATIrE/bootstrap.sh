#!/bin/sh

if [ x$1 = x ]
then
    echo "Usage: $0 <INSTALL_PREFIX>"
    echo "  autoreconf, configure, test and install SATIrE to <INSTALL_PREFIX>"
    echo "  Paths are hardcoded for c8.complang.tuwien.ac.at"
    exit 1
fi


autoreconf -i && \
  ./configure --prefix=$1 --with-rosedir=/usr/local/mstools/rose-0.9.0b --with-pagdir=/usr/local/mstools/pag-7.3.8.55 CXXFLAGS="-O0 -ggdb" && \
  make && \
  make distcheck && \
  make install && \
  make installcheck
