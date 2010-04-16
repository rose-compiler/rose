#!/bin/bash -x

set -e

if test $# -ne 1; then
  echo "Usage: $0 run_build" >&2
  exit 1
fi

run_build=$1

# DQ (9/26/2009): If we are running the build script then we will need to 
# have access to automake 1.9.6 (default is 1.9.2 and will be an error).
export PATH="/usr/apps/automake/1.9.6/bin:$PATH"

# DQ (9/26/2009): The default svn on my machine is 1.1.4, too old to use 
# the "svn log -l 100" or "svn log --limit 100" optional parameters.
export PATH="/nfs/apps/subversion/1.5.1/bin:$PATH"

# BUILD BINARIES (first step is to update the readonly repository)
if test $run_build = "build"; then
  echo "Running SVN UPDATE and BUILD script before building libraries..."
  cd /home/dquinlan/ROSE/svn-rose/scripts && ./update_svn-readonly-rose_repository-Linux-Dan.sh
# sleep 15m
else
  echo "Skipping SVN UPDATE and BUILD script before building libraries..."
fi

# BUILD BINARIES (for all versions of compilers)
# DQ (9/15/2009): Test different versions of compilers.
cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-4.4.sh --without-java &

if true; then
  sleep 1m
  cd /home/dquinlan/ROSE/svn-rose/scripts && ./roseFreshTest ./roseFreshTestStub-Dan-gnu-4.3.sh --without-java &
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
