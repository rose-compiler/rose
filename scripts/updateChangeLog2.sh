#!/bin/sh -vx
# This script generates a fresh ChangeLog2 under the top test dir of ROSE, 
# which will be copied to a ROSE distribution package later on.
#
# Old version's comments
# This script checks if a new ChangeLog2 file needs to be checked in.
# Please run this script before running the daily regression test scripts
# which updating the external repository. 
# So ChangeLog2 can be always update-to-date

# Liao, 1/26/2008

if [ $# -lt 1 ]
then
  echo This script needs one argument
  echo Usage:$0 ROSE_TOP_TEST_DIR
  echo Exampe:$0 /home/liao6/daily-test-rose/20091117_120001
  exit 1
fi

#set -e # this will stop the script when diff returns non-zero.
SVN=/nfs/apps/subversion/1.5.5/bin/svn
#if test "x$SVN" = "x"; then SVN=svn; fi

# get a fresh checkout for the current ChangeLog2
  # The repository URL
roseRepositoryUrl=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
   # an existing local copy of ROSE, not a very secure choice, but efficient

ROSE_TOP=$1
#ROSE_TOP=/home/liao6/rose
cd ${ROSE_TOP}

#rm -rf ChangeLog2
#${SVN} update -r head ChangeLog2
#if test -f ChangeLog2; then
#  :
#else
#  echo "Fatal Error: ROSE/ChangeLog2 does not exist as expected!" #>&2
#  exit 1
#fi

# generate a fresh one off the head into /tmp/ChangeLog2
rm -rf /tmp/ChangeLog2
svn log file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE --xml --verbose | xsltproc --stringparam include-rev yes --stringparam ignore-message-starting "Automatic updates" /home/liao6/opt/svn2cl-0.11/svn2cl.xsl - > /tmp/ChangeLog2

if test -f /tmp/ChangeLog2; then
  :
else
  echo "Fatal Error: No new /tmp/ChangeLog2 has been generated as expected!" #>&2
  exit 1
fi

# diff them to see if a update is needed
#diff /tmp/ChangeLog2 ChangeLog2
#if [ $? -ne 0 ]; then
#  echo "Updating ChangeLog2...." #>>$mailtext
  cp /tmp/ChangeLog2 ${ROSE_TOP}/ChangeLog2
# Liao, 11/16/2009, 
#  We no longer change ChangeLog2 for the internal svn repository
# But we do update it in the working copy and have it available for the distribution
#  ${SVN} commit -m"Automatic updates of ChangeLog2" ChangeLog2
#else
#  echo "ChangeLog2 is up-to-date. No action is needed" # >>$mailtext
#fi
#
