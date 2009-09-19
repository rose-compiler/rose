#!/bin/bash -x

set -e

if test $# -ne 1; then
  echo "Usage: $0 run_build" >&2
  exit 1
fi

run_build=$1

# BUILD BINARIES (first step is to update the readonly repository)
if test $run_build == "build"; then
  cd /home/dquinlan/ROSE/svn-rose/scripts && ./update_svn-readonly-rose_repository-Linux-Dan.sh
# sleep 15m
fi

# BUILD BINARIES (for all versions of compilers)
# DQ (9/15/2009): Test different versions of compilers.
cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-4.4.sh --without-java &
sleep 1m
cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-4.3.sh --without-java &

if test false; then
  sleep 1m
  cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-4.2.sh --without-java &
  sleep 1m
  cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-4.1.sh --without-java &
  sleep 1m
  cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-4.0.sh --without-java &
  sleep 1m

# DQ (9/15/2009): There is no gfortran available for gcc 3.4 on tux270
# alternative way to use -fPIC: CFLAGS=-fPIC CXXFLAGS=-fPIC
# flags to debug STL: CPPFLAGS="-D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG"
  cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-3.4.sh --without-java CFLAGS=-fPIC CXXFLAGS=-fPIC &

fi

# set -o pipefail
