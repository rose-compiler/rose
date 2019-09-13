#!/bin/bash -ex

export SYS_TYPE="toss_3_x86_64_ib"

# Don't error if there are unset variables
set +u

source "/collab/usr/global/tools/rose/bamboo/stable/setup.sh"

rm -rf rose-sh/
git clone --branch charles/dev rose-dev@rosecompiler1.llnl.gov:rose/rose-sh.git
cd rose-sh/
export PATH="$(pwd)/support/bin:${PATH}"

cd applications/ares/
mkdir -p ares_build/build
rm -rf ares/
git clone --branch ares/feature/rose ~/repositories/ares.git

# Want to run in parallel, but how do we capture the output and use bamboo
# if srun exits and runs in the background?
#srun_do run_and_log
./bin/do_make.sh

# important repos (rose shold be already built):
# rose-sh
# ares

# important dirs:
# /collab/usr/global/tools/rose/bamboo/stable/
# ./rose-sh/
#   applications/ares/
#     ares_build/build/
#       ares

