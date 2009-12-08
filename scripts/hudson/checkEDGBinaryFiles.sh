#/bin/bash -vx
# Liao 12/7/2009

# This script will check each tar.gz file 
# from /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/
# to make sure there is no EDG proprietary information there
# Please call it before publish those EDG binary files
rm -rf /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/testplace
mkdir -p /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/testplace
cd /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/testplace
TAR_GZ_FILE=($(find /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries/ -name \*.tar.gz))
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

