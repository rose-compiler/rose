#!/bin/sh
# This script checks if a new ChangeLog2 file needs to be checked in.
# Please run this script before running the daily regression test scripts
# which updating the external repository. 
# So ChangeLog2 can be always update-to-date

# Liao, 1/26/2008

#set -e # this will stop the script when diff returns non-zero.
SVN=/nfs/apps/subversion/1.5.5/bin/svn
#if test "x$SVN" = "x"; then SVN=svn; fi

# get a fresh checkout for the current ChangeLog2
  # The repository URL
roseRepositoryUrl=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
   # an existing local copy of ROSE, not a very secure choice, but efficient
ROSE_TOP=/home/liao6/rose
cd ${ROSE_TOP}
rm ChangeLog2
${SVN} update -r head ChangeLog2
if test -f ChangeLog2; then
  :
else
  echo "Fatal Error: ROSE/ChangeLog2 does not exist as expected!" #>&2
  exit 1
fi

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
diff /tmp/ChangeLog2 ChangeLog2
if [ $? -ne 0 ]; then
  echo "Updating ChangeLog2...." #>>$mailtext
  cp /tmp/ChangeLog2 ChangeLog2
  ${SVN} commit -m"Automatic updates of ChangeLog2" ChangeLog2
else
  echo "ChangeLog2 is up-to-date. No action is needed" # >>$mailtext
fi

