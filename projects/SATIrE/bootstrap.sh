#!/bin/sh

# Run this on g0.complang.tuwien.ac.at

autoreconf -i && ./configure --prefix=$1 --with-rosedir=/usr/local/mstools/rose/ --with-pagdir=/usr/local/mstools/pag CXXFLAGS="-O0 -ggdb" && make && make distcheck && make install && make installcheck
