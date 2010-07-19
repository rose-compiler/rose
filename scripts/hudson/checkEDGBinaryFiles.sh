#/bin/bash -vx
# Liao 12/7/2009

if [ $# -ne 0 ]
  then
    echo This script does not need targument to run.
   exit 1
fi

# Check if any tar.gz files for Mac OS exist
# The reason is that the 32-bit and 64-bit binaries are controlled by Hudson and they can be reliably generated
# we use the signature of the latest 64-bit binary to check Mac OS binary
cd /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/
# fix any possible file permission problem
chmod a+r *.tar.gz || echo "don't care"

ls -ct roseBinaryEDG*-x86_64-pc-linux-gnu-GNU-4.4-*.tar.gz &>/tmp/.tttedg123
# we have two more fields for EDG major and minor version
EDG_SIGNATURE_SUFFIX=`head -1 /tmp/.tttedg123 | cut -f 10 -d'-'`
rm -rf /tmp/.tttedg123

ls roseBinaryEDG*-i686-apple-darwin-GNU-*$EDG_SIGNATURE_SUFFIX

if [ $? -ne 0  ]; then
  echo "Cannot find the matching EDG binary for Mac OS! for signature "
  echo $EDG_SIGNATURE_SUFFIX
  exit 2
fi

# This script will check each tar.gz file 
# from /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/
# to make sure there is no EDG proprietary information there
# Please call it before publish those EDG binary files
#
# this is too expensive as the number of binary files increase!!!
# So we only check the files modified within the last 7 days.
rm -rf /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/testplace
mkdir -p /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/testplace
cd /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/testplace
TAR_GZ_FILE=($(find /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/ -ctime -7 -name \*.tar.gz))
FILE_COUNT=${#TAR_GZ_FILE[@]}
for (( i=0; i<${FILE_COUNT}; i++ ));
do
    echo "checking file ${TAR_GZ_FILE[$i]} ..."
    tar xzvf ${TAR_GZ_FILE[$i]}
    temp=`basename ${#TAR_GZ_FILE[$i]} .tar.gz`
    EDG_COPYRIGHT_STRINGS=($(grep -r 'Proprietary information of Edison Design Group ' .))
    if [ ${EDG_COPYRIGHT_STRINGS[0]} ]; then
       echo Fatal Error: Found copyrighted EDG text in tarball:${TAR_GZ_FILE[$i]}
       exit 2
    fi
done

