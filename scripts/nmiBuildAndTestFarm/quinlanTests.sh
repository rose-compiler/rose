#!/bin/sh

echo "Submitting All Tests..."

echo "***************************"
echo "* Passing tests (11 tests) *"
echo "***************************"

# DQ (9/9/2009): Fedora 8 was released Nov 2007 (completes, but takes a long time to run).
# ./submit.sh build_configs/x86_64_fc_8/minimal_default (renamed)
./submit.sh build_configs/x86_64_fedora_8/minimal_default

./submit.sh build_configs/x86_64_fc_5/minimal_default
./submit.sh build_configs/x86_64_rhap_5/minimal_default
./submit.sh build_configs/x86_64_rhas_4/minimal_default
./submit.sh build_configs/x86_64_rhap_5.2/minimal_default
./submit.sh build_configs/x86_suse_10.0/minimal_default
./submit.sh build_configs/x86_suse_10.2/minimal_default
./submit.sh build_configs/x86_fc_5/minimal_default
./submit.sh build_configs/x86_rhas_4/minimal_default
./submit.sh build_configs/x86_rhap_5/minimal_default
./submit.sh build_configs/x86_sl_4.4/minimal_default

echo "******************************************"
echo "* Failing tests: libCompass.so not found *"
echo "******************************************"

# DQ (9/9/2009): This configuration on this platform can't find libCompass.so (in "make check")
# /home/condor/execute/dir_16350/userdir/build/projects/compass/tools/compassVerifier/.libs/lt-compassVerifier: 
# error while loading shared libraries: libCompass.so.0: cannot open shared object file: No such file or directory
./submit.sh build_configs/x86_deb_5.0/minimal_default

# DQ (9/12/2009): lsb_release is not available, so we run tests that are known to fail on Debian (only test NMI deb_5.0 platforms).
# DQ (9/9/2009): This configuration on this platform can't find libCompass.so (in "make check")
# /home/condor/execute/dir_24405/userdir/build/projects/compass/tools/compassVerifier/.libs/lt-compassVerifier: 
# error while loading shared libraries: libCompass.so.0: cannot open shared object file: No such file or directory
# DQ (9/7/2009): AM_PATH_XML2 not found (still)!
# ./submit.sh build_configs/x86_64_deb_4.0/minimal_default

# DQ (9/9/2009): Compass lib not found in "make check"
./submit.sh build_configs/x86_64_deb_5.0/minimal_default

echo "******************************************************************************************************************"
echo "* Failing tests: (different reasons: RTED exit not define, Compass segfault, and testCallGraphAnalysis segfault) *"
echo "******************************************************************************************************************"

# DQ (9/10/2009): NMI changed the name of this test to x86_64_fedora_9 (so our test now fails).
# DQ (9/10/2009): Fixed problem with gnu 4.3 and one of our tests that used main() with a single argument (copyAST_tests).
# DQ (9/7/2009): This fails in the RTED work...("make check") (NOW FIXED)
# Error: rose_example.C: In function 'void check_var(void*, char*)':
#        rose_example.C:34: error: 'exit' was not declared in this scope
#        Leaving directory `/home/condor/execute/dir_13881/userdir/build/projects/runtimeErrorCheck'
# ./submit.sh build_configs/x86_64_fc_9/minimal_default
./submit.sh build_configs/x86_64_fedora_9/minimal_default

# DQ (9/11/2009): Segfault in Compass tests ("make check")
# DQ (9/9/2009): This platform will be removed (old: released June 2005)
# DQ (9/7/2009): Segfault in Compass tests ("make check")
# make[9]: *** [test] Segmentation fault
# make[9]: Leaving directory `/home/condor/execute/dir_22074/userdir/build/projects/compass/tools/compass'
./submit.sh build_configs/x86_64_fc_4/minimal_default

# DQ (9/9/2009): This platform will be removed, redundant, please consider just building on x86_rhas_4
# DQ (9/7/2009): Error in testCallGraphAnalysis
# make[6]: *** [test2006_02.C.dot] Segmentation fault
# make[6]: Leaving directory `/home/condor/execute/dir_32311/userdir/build/tests/roseTests/programAnalysisTests/testCallGraphAnalysis'
./submit.sh build_configs/x86_cent_4.2/minimal_default

echo "***********************************************"
echo "* Failing test on Suse Linux version 9 (link problem multiple definition of __i686.get_pc_thunk.bx) *"
echo "***********************************************"
# DQ (9/11/2009): Same error!
# DQ (9/9/2009): New error: 
# src/frontend/CxxFrontend/EDG_3.3/src/preproc.c:99: multiple definition of `__i686.get_pc_thunk.bx'
# frontend/CxxFrontend/.libs/libroseEDG.a(preproc.o)(.text.__i686.get_pc_thunk.cx+0x0): In function `identify_dir_keyword':
# ../../../../../../sourcetree/src/frontend/CxxFrontend/EDG_3.3/src/preproc.c:99: multiple definition of `__i686.get_pc_thunk.cx'
# ../src/backend/unparser/.libs/libunparse.a(unparser.o)(.gnu.linkonce.t.__i686.get_pc_thunk.cx+0x0): first defined here
# collect2: ld returned 1 exit status
# make[3]: *** [librose.la] Error 1
# make[3]: Leaving directory `/home/condor/execute/dir_16335/userdir/build/src'
./submit.sh build_configs/x86_sles_9/minimal_default

echo "**************************************************************"
echo "* Failing test on Suse Linux version 9 (link problem wrong version of libstdc++.so) *"
echo "**************************************************************"
# DQ (9/9/2009): New error: 
./submit.sh build_configs/x86_64_sles_9/minimal_default


echo "******************************************************"
echo "* Still running... (these appear to never terminate: heavily loaded machines) *"
echo "******************************************************"

# DQ (9/9/2009): error message from 8/7/2009) -- config.status: error: cannot find input file: Makefile.in
# I think that this platform is too old to be used for ROSE (Rhat version 3 released Oct 2003).
# ./submit.sh build_configs/x86_rhas_3/minimal_default

# DQ (9/9/2009): This platform will be removed (old machine)
# ./submit.sh build_configs/x86_slf_3/minimal_default

# DQ (9/9/2009): This platform will be removed, consider supporting one of the two current LTS releases (see email)
# DQ (9/7/2009): AM_PATH_XML2 not found (fixed and waiting for new test results)
./submit.sh build_configs/x86_ubuntu_5.10/minimal_default

# DQ (9/9/2009): This platform will be removed (skip this test)
# DQ (9/6/2009): This platform does not appear to exist. This all of the prereq 
# are reported to be unavailable (as warnings). 
# DQ (9/9/2009): configure.in:21: error: Autoconf version 2.60 or higher is required (rerunning test)
# ./submit.sh build_configs/x86_deb_3.1/minimal_default

# DQ (9/12/2009): lsb_release is not available, so we run tests that are known to fail on Debian (only test NMI deb_5.0 platforms).
# DQ (9/9/2009): This is a heavily loaded machine (tests accumulate)
# DQ (9/7/2009): ACLOCAL_INCLUDES does not cause AM_PATH_XML2 to be found.
# ACLOCAL_INCLUDES is not set in the environment (but this works on x86_64_deb_5.0).
# ./submit.sh build_configs/x86_deb_4.0/minimal_default



echo "*************************************************************************"
echo "* Expected JAVA Failures (can't find libjvm.so when testing librose.so) *"
echo "*************************************************************************"
# DQ (9/9/2009): Skip this test until we have the Java support debugged...
# DQ (9/7/2009): Java tests fail to find libjvm.so when testing librose.so (fails in "make")
# The test without Java completes.
#./submit.sh build_configs/x86_rhap_5/default

# DQ (9/9/2009): Skip this test until we have the Java support debugged...
# DQ (9/5/2009): when we specify java prereq I think a problem is that we are not getting the jdk (fails in "make check")
# The test without Java completes.
#./submit.sh build_configs/x86_64_rhap_5.2/default

# DQ (9/9/2009): Skip this test until we have the Java support debugged...
# The test without Java completes, but it takes several days.
# ./submit.sh build_configs/x86_64_fc_8/default

# DQ (9/9/2009): Skip this test until we have the Java support debugged...
# ./submit.sh build_configs/x86_ubuntu_5.10/default

echo "************************"
echo "* New Mac OSX tests... *"
echo "************************"
./submit.sh build_configs/x86_64_macos_10.5/minimal_default
./submit.sh build_configs/x86_macos_10.4/minimal_default

echo "********************************************************"
echo "* New possible tests... (not available as of 9/9/2009) *"
echo "********************************************************"
# ./submit.sh build_configs/x86_64_fc_11/minimal_default
# ./submit.sh build_configs/x86_64_ubuntu_8.04/minimal_default
# ./submit.sh build_configs/x86_64_ubuntu_6.06/minimal_default
# ./submit.sh build_configs/x86_sles_11/minimal_default
# ./submit.sh build_configs/ia64_rhap_5.3/minimal_default

echo "*****************************************"
echo "* New GNU Compiler Portability tests... *"
echo "*****************************************"

# DQ (9/12/2009): g++ 4.3.2 prereq is not available on x86_64_rhap_5.2
# ./submit.sh build_configs/x86_64_rhap_5.2/compilerTest_gnu_4.3
./submit.sh build_configs/x86_64_rhap_5/compilerTest_gnu_4.3

# DQ (9/12/2009): Added g++ 4.1.2 test
./submit.sh build_configs/x86_64_rhap_5.2/compilerTest_gnu_4.1

# DQ (9/12/2009): Added g++ 4.0.4 test
./submit.sh build_configs/x86_64_rhap_5/compilerTest_gnu_4.0

# DQ (9/12/2009): Added g++ 3.4.3 test
./submit.sh build_configs/x86_rhas_4/compilerTest_gnu_3.4


echo "Submitted All Tests..."
