#!/bin/sh

#./submit.sh build_configs/x86_64_deb_4.0/minimal_default
#./submit.sh build_configs/x86_64_rhas_4/minimal_default
#./submit.sh build_configs/x86_cent_4.2/minimal_default

echo "Submitting All Tests..."

./submit.sh build_configs/x86_64_fc_5/minimal_default

# echo "Submitted All Tests... (exiting prematurely as a test)"
# exit 0

./submit.sh build_configs/x86_suse_10.0/minimal_default
./submit.sh build_configs/x86_rhas_4/minimal_default
./submit.sh build_configs/x86_64_deb_5.0/minimal_default

# DQ (9/5/2009): After removing comments from the build_configs, file then ACLOCAL_INCLUDES 
# was properly passed to the environment.  Then we get an error specific to Suse Linux.
# configure: error: Could not find a binary version of EDG for architecture i686-suse-linux 
# in /home/condor/execute/dir_29767/userdir/trunk/src/frontend/CxxFrontend; it should be 
# named "roseBinaryEDG-i686-suse-linux-8f4da3555b4592d246d4ef2a3473752b.tar.gz".  
# This distribution cannot be used on this platform.
./submit.sh build_configs/x86_sles_9/minimal_default

./submit.sh build_configs/x86_fc_5/minimal_default
./submit.sh build_configs/x86_64_fc_9/minimal_default

# DQ (9/6/2009): This platform does not appear to exist. This all of the prereq 
# are reported to be unavailable (as warnings).
./submit.sh build_configs/x86_deb_3.1/minimal_default

# DQ (9/5/2009): This configuration on this platform can't find libCompass.so (in "make check")
# /home/condor/execute/dir_16350/userdir/build/projects/compass/tools/compassVerifier/.libs/lt-compassVerifier: 
# error while loading shared libraries: libCompass.so.0: cannot open shared object file: No such file or directory
./submit.sh build_configs/x86_deb_5.0/minimal_default

./submit.sh build_configs/x86_slf_3/minimal_default
./submit.sh build_configs/x86_rhap_5/default
./submit.sh build_configs/x86_rhap_5/minimal_default
./submit.sh build_configs/x86_64_rhap_5/minimal_default
./submit.sh build_configs/x86_64_rhas_4/minimal_default

# x86_suse_10.2 works only as well as can be expected:
# configure: error: Could not find a binary version of EDG for architecture i686-suse-linux-gnu 
# in /home/condor/execute/dir_21856/userdir/trunk/src/frontend/CxxFrontend; 
# it should be named "roseBinaryEDG-i686-suse-linux-gnu-3b9f50548a6d38de185142627ef12d99.tar.gz".  
# This distribution cannot be used on this platform.
./submit.sh build_configs/x86_suse_10.2/minimal_default

./submit.sh build_configs/x86_ubuntu_5.10/default
./submit.sh build_configs/x86_ubuntu_5.10/minimal_default
./submit.sh build_configs/x86_sl_4.4/minimal_default
./submit.sh build_configs/x86_64_deb_4.0/minimal_default

# DQ (9/5/2009): when we specify java prereq I think a problem is that we are not getting the jdk.
./submit.sh build_configs/x86_64_rhap_5.2/default

./submit.sh build_configs/x86_64_rhap_5.2/minimal_default
./submit.sh build_configs/x86_64_fc_4/minimal_default
./submit.sh build_configs/x86_cent_4.2/minimal_default

# DQ (9/5/2009): ACLOCAL_INCLUDES does not cause AM_PATH_XML2 to be found.
# ACLOCAL_INCLUDES is not set in the environment (but this works on x86_64_deb_5.0).
./submit.sh build_configs/x86_deb_4.0/minimal_default

./submit.sh build_configs/x86_64_fc_8/default
./submit.sh build_configs/x86_64_fc_8/minimal_default
./submit.sh build_configs/x86_rhas_3/minimal_default

# x86_64_sles_9 works only as well as can be expected:
# configure: error: Could not find a binary version of EDG for architecture x86_64-suse-linux in 
# /home/condor/execute/dir_24902/userdir/trunk/src/frontend/CxxFrontend; it should be 
# named "roseBinaryEDG-x86_64-suse-linux-8f4da3555b4592d246d4ef2a3473752b.tar.gz".  
# This distribution cannot be used on this platform.
./submit.sh build_configs/x86_64_sles_9/minimal_default

echo "Submitted All Tests..."
