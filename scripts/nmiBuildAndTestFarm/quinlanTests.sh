#!/bin/sh

echo "Submitting All Tests..."

echo "****************************"
echo "* Passing tests (18 tests) *"
echo "****************************"

# DQ (9/26/2009): NMI reports that fedora_8 and fedora_9 are the new names.
# DQ (9/9/2009): Fedora 8 was released Nov 2007 (completes, but takes a long time to run).
# ./submit.sh build_configs/x86_64_fc_8/minimal_default (renamed)
# ./submit.sh build_configs/x86_64_fedora_8/minimal_default (renamed again)
# ./submit.sh build_configs/x86_64_fc_8/minimal_default
./submit.sh build_configs/x86_64_fedora_8/minimal_default &
./submit.sh build_configs/x86_64_fc_5/minimal_default &
./submit.sh build_configs/x86_64_rhap_5/minimal_default &
./submit.sh build_configs/x86_64_rhas_4/minimal_default &
./submit.sh build_configs/x86_64_rhap_5.2/minimal_default &
./submit.sh build_configs/x86_suse_10.0/minimal_default &
./submit.sh build_configs/x86_suse_10.2/minimal_default &
./submit.sh build_configs/x86_fc_5/minimal_default &
./submit.sh build_configs/x86_rhas_4/minimal_default &
./submit.sh build_configs/x86_rhap_5/minimal_default &
./submit.sh build_configs/x86_sl_4.4/minimal_default &
./submit.sh build_configs/x86_64_macos_10.5/minimal_default &

# Passing compiler tests
./submit.sh build_configs/x86_64_rhap_5.2/compilerTest_gnu_4.1 &
./submit.sh build_configs/x86_rhap_5/compilerTest_gnu_4.1 &

./submit.sh build_configs/x86_rhap_5/default &
./submit.sh build_configs/x86_64_macos_10.5/default &

# DQ (10/2/2009): Testing the default compiler (4.0.4)
./submit.sh build_configs/x86_64_rhap_5/default &

./submit.sh build_configs/x86_64_fedora_9/default &
./submit.sh build_configs/x86_64_fedora_9/default_compiler &

./submit.sh build_configs/x86_suse_10.2/default &
./submit.sh build_configs/x86_suse_10.2/default_compiler &

./submit.sh build_configs/x86_ubuntu_5.10/default &
./submit.sh build_configs/x86_ubuntu_5.10/default_compiler &

./submit.sh build_configs/x86_sl_4.4/default &
./submit.sh build_configs/x86_sl_4.4/default_compiler &

echo "******************************************"
echo "* Failing tests: libCompass.so not found *"
echo "******************************************"

# DQ (9/15/2009): Passing with skipping Debian tests.
# DQ (9/9/2009): This configuration on this platform can't find libCompass.so (in "make check")
# /home/condor/execute/dir_16350/userdir/build/projects/compass/tools/compassVerifier/.libs/lt-compassVerifier: 
# error while loading shared libraries: libCompass.so.0: cannot open shared object file: No such file or directory
./submit.sh build_configs/x86_deb_5.0/minimal_default &
./submit.sh build_configs/x86_deb_5.0/default &

# DQ (9/15/2009): Passing with skipping Debian tests.
# DQ (9/14/2009): NMI added lsb_release
# DQ (9/12/2009): lsb_release is not available, so we run tests that are known to fail on Debian (only test NMI deb_5.0 platforms).
# DQ (9/9/2009): This configuration on this platform can't find libCompass.so (in "make check")
# /home/condor/execute/dir_24405/userdir/build/projects/compass/tools/compassVerifier/.libs/lt-compassVerifier: 
# error while loading shared libraries: libCompass.so.0: cannot open shared object file: No such file or directory
# DQ (9/7/2009): AM_PATH_XML2 not found (still)!
./submit.sh build_configs/x86_64_deb_4.0/minimal_default &

# DQ (9/15/2009): Passing with skipping Debian tests.
# DQ (9/9/2009): Compass lib not found in "make check"
./submit.sh build_configs/x86_64_deb_5.0/minimal_default &
./submit.sh build_configs/x86_64_deb_5.0/default &

# DQ (9/15/2009): Passing with skipping Debian tests.
# DQ (9/14/2009): NMI added lsb_release
# DQ (9/12/2009): lsb_release is not available, so we run tests that are known to fail on Debian (only test NMI deb_5.0 platforms).
# DQ (9/9/2009): This is a heavily loaded machine (tests accumulate)
# DQ (9/7/2009): ACLOCAL_INCLUDES does not cause AM_PATH_XML2 to be found.
# ACLOCAL_INCLUDES is not set in the environment (but this works on x86_64_deb_5.0).
./submit.sh build_configs/x86_deb_4.0/minimal_default &

echo "*************************************************************************************************"
echo "* Failing tests: (different reasons: e.g. Compass segfault, and testCallGraphAnalysis segfault) *"
echo "*************************************************************************************************"

# DQ (9/10/2009): NMI changed the name of this test to x86_64_fedora_9 (so our test now fails).
# DQ (9/10/2009): Fixed problem with gnu 4.3 and one of our tests that used main() with a single argument (copyAST_tests).
# DQ (9/7/2009): This fails in the RTED work...("make check") (NOW FIXED)
# Error: rose_example.C: In function 'void check_var(void*, char*)':
#        rose_example.C:34: error: 'exit' was not declared in this scope
#        Leaving directory `/home/condor/execute/dir_13881/userdir/build/projects/runtimeErrorCheck'
# ./submit.sh build_configs/x86_64_fc_9/minimal_default
# ./submit.sh build_configs/x86_64_fedora_9/minimal_default
# ./submit.sh build_configs/x86_64_fc_9/minimal_default
./submit.sh build_configs/x86_64_fedora_9/minimal_default &

# (DQ (10/3/2009): Machine not available yet!
# DQ (9/30/2009): Added new test (default compiler is 4.3.?, no other compiler is available)
# ./submit.sh build_configs/x86_64_fedora_11/minimal_default &
./submit.sh build_configs/x86_64_fedora_11/default_compiler &

# DQ (10/3/2009): This platform has been removed...
# DQ (9/11/2009): Segfault in Compass tests ("make check")
# DQ (9/9/2009): This platform will be removed (old: released June 2005)
# DQ (9/7/2009): Segfault in Compass tests ("make check")
# make[9]: *** [test] Segmentation fault
# make[9]: Leaving directory `/home/condor/execute/dir_22074/userdir/build/projects/compass/tools/compass'
# ./submit.sh build_configs/x86_64_fc_4/minimal_default &

# DQ (10/1/2009): This platform is not available any more.
# DQ (9/9/2009): This platform will be removed, redundant, please consider just building on x86_rhas_4
# DQ (9/7/2009): Error in testCallGraphAnalysis
# make[6]: *** [test2006_02.C.dot] Segmentation fault
# make[6]: Leaving directory `/home/condor/execute/dir_32311/userdir/build/tests/roseTests/programAnalysisTests/testCallGraphAnalysis'
# ./submit.sh build_configs/x86_cent_4.2/minimal_default &

echo "***********************************************"
echo "* Failing test on Suse Linux version 9 (link problem multiple definition of __i686.get_pc_thunk.bx) *"
echo "***********************************************"

# DQ (10/1/2009): Reconfirmed 9/9/2000 error!
# DQ (9/15/2009): Progress! New error -- tclsh: command not found in: src/frontend/BinaryDisassembly
# DQ (9/11/2009): Same error!
# DQ (9/9/2009): New error: 
# src/frontend/CxxFrontend/EDG_3.3/src/preproc.c:99: multiple definition of `__i686.get_pc_thunk.bx'
# frontend/CxxFrontend/.libs/libroseEDG.a(preproc.o)(.text.__i686.get_pc_thunk.cx+0x0): In function `identify_dir_keyword':
# ../../../../../../sourcetree/src/frontend/CxxFrontend/EDG_3.3/src/preproc.c:99: multiple definition of `__i686.get_pc_thunk.cx'
# ../src/backend/unparser/.libs/libunparse.a(unparser.o)(.gnu.linkonce.t.__i686.get_pc_thunk.cx+0x0): first defined here
# collect2: ld returned 1 exit status
# make[3]: *** [librose.la] Error 1
# make[3]: Leaving directory `/home/condor/execute/dir_16335/userdir/build/src'
# SUSE LINUX Enterprise Server 9 (x86_64)
./submit.sh build_configs/x86_sles_9/minimal_default &

# The default compiler is GNU 3.3, and we don't build a binary that old!
# ./submit.sh build_configs/x86_sles_9/default_compiler &

echo "**************************************************************"
echo "* Failing test on Suse Linux version 9 (link problem wrong version of libstdc++.so) *"
echo "**************************************************************"
# DQ (9/9/2009): Error: 
#    /prereq/gcc-4.2.4/lib/../lib64/libstdc++.so: undefined reference to `_Unwind_GetIPInfo@GCC_4.2.0'
./submit.sh build_configs/x86_64_sles_9/minimal_default &

# The default compiler is GNU 3.3, and we don't build a binary that old!
# ./submit.sh build_configs/x86_64_sles_9/default_compiler &


echo "******************************************************"
echo "* Still running... (these appear to never terminate: heavily loaded machines) *"
echo "******************************************************"

# DQ (10/3/2009): There platforms a current problem: 
#    x86_64_fc_4, x86_rhap_5.3, x86_ubuntu_8.04.3, x86_cent_4.2, x86_slf_3, x86_64_fedora_11
#    These should be available soon: x86_rhap_5.3, x86_ubuntu_8.04.3, x86_64_fedora_11
#    These have been removed: x86_64_fc_4, x86_cent_4.2, x86_slf_3

# DQ (9/9/2009): error message from 8/7/2009) -- config.status: error: cannot find input file: Makefile.in
# I think that this platform is too old to be used for ROSE (Rhat version 3 released Oct 2003).
# ./submit.sh build_configs/x86_rhas_3/minimal_default

# DQ (9/9/2009): This platform will be removed (old machine)
# ./submit.sh build_configs/x86_slf_3/minimal_default

# DQ (9/30/2009): The platform demonstrates a segfault error when the "src/testSharedRoseLib" executable is run.
# DQ (9/9/2009): This platform will be removed, consider supporting one of the two current LTS releases (see email)
# DQ (9/7/2009): AM_PATH_XML2 not found (fixed and waiting for new test results)
./submit.sh build_configs/x86_ubuntu_5.10/minimal_default &

# (DQ (10/3/2009): Machine not available yet!
# DQ (9/29/2009): Suggested new machine (by SNL)
# ./submit.sh build_configs/x86_64_ubuntu_8.04.3/minimal_default &
./submit.sh build_configs/x86_64_ubuntu_8.04.3/default_compiler &

# DQ (9/9/2009): This platform will be removed (skip this test)
# DQ (9/6/2009): This platform does not appear to exist. This all of the prereq 
# are reported to be unavailable (as warnings). 
# DQ (9/9/2009): configure.in:21: error: Autoconf version 2.60 or higher is required (rerunning test)
# ./submit.sh build_configs/x86_deb_3.1/minimal_default



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
./submit.sh build_configs/x86_64_rhap_5.2/default &
# ./submit.sh build_configs/x86_64_rhap_5.2/default_compiler &
./submit.sh build_configs/x86_64_rhap_5.2/default_compiler_boost_1.36 &

# DQ (9/9/2009): Skip this test until we have the Java support debugged...
# The test without Java completes, but it takes several days.
# ./submit.sh build_configs/x86_64_fc_8/default

# DQ (9/9/2009): Skip this test until we have the Java support debugged...
# ./submit.sh build_configs/x86_ubuntu_5.10/default

echo "****************************"
echo "* Failing Mac OSX tests... *"
echo "****************************"
# DQ (10/1/2009): Failes to link to boost library in configure
#    checking whether the Boost::Thread library is available... yes
#    checking for toupper in -lboost_thread-mt-1_36... yes
#    checking whether the Boost::Date_Time library is available... yes
#    checking for toupper in -lboost_date_time-mt-1_36... yes
#    checking whether the Boost::Regex library is available... yes
#    configure: error: Could not link against boost_date_time-mt-1_36 !
./submit.sh build_configs/x86_macos_10.4/minimal_default &

echo "********************************************************"
echo "* New possible tests... (not available as of 9/9/2009) *"
echo "********************************************************"
# ./submit.sh build_configs/x86_64_fc_11/minimal_default
# ./submit.sh build_configs/x86_64_ubuntu_8.04/minimal_default
# ./submit.sh build_configs/x86_64_ubuntu_6.06/minimal_default
# ./submit.sh build_configs/x86_sles_11/minimal_default

# (DQ (10/3/2009): Machine not available yet!
# DQ (9/29/2009): Added Red Hat 5.3 test (no results yet)
# ./submit.sh build_configs/x86_64_rhap_5.3/minimal_default &
./submit.sh build_configs/x86_64_rhap_5.3/default_compiler &


echo "************************************************************************"
echo "* New GNU Compiler Portability tests... (default testing uses GNU 4.2) *"
echo "************************************************************************"

# DQ (10/1/2009): Link Error on x86_64_rhap_5 using GNU 4.3 compiler.
#    LINK    librose.la
#    /usr/bin/ld: /prereq/gcc-4.3.2/lib/../lib64/libstdc++.a(compatibility.o): relocation R_X86_64_32 against `typeinfo for __cxxabiv1::__forced_unwind' can not be used when making a shared object; recompile with -fPIC
#    /prereq/gcc-4.3.2/lib/../lib64/libstdc++.a: could not read symbols: Bad value
#    collect2: ld returned 1 exit status
# DQ (9/12/2009): g++ 4.3.2 prereq is not available on x86_64_rhap_5.2
# ./submit.sh build_configs/x86_64_rhap_5.2/compilerTest_gnu_4.3
./submit.sh build_configs/x86_64_rhap_5/compilerTest_gnu_4.3 &

# DQ (10/1/2009): Error with incompatable declarations of builtin functions. (should be fixed in new release!)
#    "/home/condor/execute/dir_24917/userdir/build/include-staging/gcc_HEADERS/rose_edg_required_macros_and_functions.h", line 462: error: 
#    declaration is incompatible with "void *__builtin_memcpy(void *, const void *, unsigned int)"
#    void* __builtin_memcpy (void * destination, const void * source, unsigned long long num );
#    "/home/condor/execute/dir_24917/userdir/build/include-staging/gcc_HEADERS/rose_edg_required_macros_and_functions.h", line 463: error: 
#    declaration is incompatible with "int __builtin_memcmp(const void *, const void *, unsigned int)"
#    int __builtin_memcmp ( const void * ptr1, const void * ptr2, unsigned long long num );
#    terminate called after throwing an instance of 'rose_exception'
# DQ (9/29/2009): Added g++ 4.3.2 test
./submit.sh build_configs/x86_rhas_4/compilerTest_gnu_4.3 &

# DQ (10/1/2009): Compile Error on x86_64_rhap_5 using GNU 4.0 compiler.
# COMPILE dummyCppFileForLibrose.lo
#    /usr/bin/ld: /prereq/gcc-4.0.4/lib/../lib64/libstdc++.a(ctype.o): relocation R_X86_64_32 against `std::ctype<char>::id' can not be used when making a shared object; recompile with -fPIC
#    /prereq/gcc-4.0.4/lib/../lib64/libstdc++.a: could not read symbols: Bad value
#    collect2: ld returned 1 exit status
# DQ (9/12/2009): Added g++ 4.0.4 test
./submit.sh build_configs/x86_64_rhap_5/compilerTest_gnu_4.0 &

# DQ (10/1/2009): This causes an internal compiler error in: projects/interpreter (new version skips this code when using GNU 3.4)
#    error: internal compiler error: in make_decl_rtl, at varasm.c:752
# DQ (9/12/2009): Added g++ 3.4.3 test
# Error: can't find the correct binary (32-bit GNU 3.4; but we should have the renaming such that the 4.1 version binary is used.
./submit.sh build_configs/x86_rhas_4/compilerTest_gnu_3.4 &

# DQ (10/1/2009): It appears libtool finds the wrong libstdc++ library.
# The solution may be related to: http://www.mail-archive.com/libtool@gnu.org/msg07653.html
# The solution might be to configure withouth libxml.
# DQ (9/29/2009): Added g++ 3.4.3 test
./submit.sh build_configs/x86_rhap_5/compilerTest_gnu_3.4 &

echo "Submitted All Tests..."
