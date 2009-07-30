#!/bin/sh -x
#
# This script tries to upload a distribution package to the Scidac Outreach center web site
# The assumption is that the machine running this script has a firefox browser with 
# saved session which has already logged into the website. 
# So we don't need put user account/password into this script
# 
# It will be called by another script: makeAndReleaseROSE.sh
#
# Use:
#  ./makeRelease.sh 0.9.4a 2927 /home/liao6/daily-test-rose/release/upload/rose-0.9.4a-source-with-EDG-binary-2927.tar.gz
#
# by Jeremiah, 10/14/2008
# Modified by Liao, 11.25.2008
if [ $# -lt 3 ]
then
  echo This script needs three arguments
  echo Usage:$0 VERSION_NO REVISION_NO FULL_PACKAGE_NAME
  echo Exampe:$0 0.9.3a 2975 /file/rose-0.9.4a-source-with-EDG-binary-2759.tar.gz
  exit
fi

VERSION_NO=$1
REVISION_NO=$2
FULL_PACKAGE_NAME=$3

# ls ~/.mozilla/firefox/ to find out your firefox id
FIREFOXID=58zvv6td.default

# try to grab existing session id
SESSION=`sed -n '/^outreach\.scidac\.gov\t.*\tsession_ser\t/p' < $HOME/.mozilla/firefox/$FIREFOXID/cookies.txt | cut -f 7`

# if failed, try to login automatically
# and add the session into cookies
# Changed to use auto-login no matter what. 
#if [ "x$SESSION" = "x" ]; then 
   echo "No session for outreach.scidac is found. Trying to log in..."
   # a script trying to log in and redirect output to $HOME/curl.log 
   $HOME/release/scidac-login.sh
   SESSION=`grep 'Set-Cookie' $HOME/curl.log | cut -d' ' -f 3 | cut -d'=' -f 2 | cut -d';' -f 1`
   echo "outreach.scidac.gov	FALSE	/	FALSE	1228246200	session_ser	$SESSION">>$HOME/.mozilla/firefox/$FIREFOXID/cookies.txt
#fi

# debug here
# exit

if [ "x$SESSION" = "x" ]; then 
   echo "Fatal Cannot get session for outreach.scidac is found. Aborting..."
   exit 1
fi

# This needs to match the browser you used to log in to Outreach; find it from http://browserspy.dk/useragent.php
# this is also set in scidac-login.sh now, 7/30/2009, Liao
USERAGENT='Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.12) Gecko/2009070610 Firefox/3.0.12'
#USERAGENT='Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.18) Gecko/20081029 Firefox/2.0.0.18'
# The actual content to be filled into the web form

#RELEASENAME="0.9.3a-multiplatform-2484"

RELEASENAME="${VERSION_NO}-multiplatform-${REVISION_NO}"
RELEASEDATE=`date '+%F %H:%M'`

#FILENAME="/home/liao6/daily-test-rose/20081014_120001/build/rose-0.9.3a-source-with-EDG-binary-2484.tar.gz"
TYPEID=5020 # Source .tar.gz
PROCESSORID=8000 # any processor type
# PROCESSORID=9999 # other

echo $RELEASEDATE
echo $SESSION

#exit 0

# explanations for options
# -A user agent string
# -F form "name=content"
# -b cookie "name=data"
# -v verbose
curl \
  -v \
  -b "session_ser=$SESSION" \
  -F "package_id=21" \
  -F "release_name=$RELEASENAME" \
  -F "release_date=$RELEASEDATE" \
  -F "userfile=@$FULL_PACKAGE_NAME" \
  -F "type_id=$TYPEID" \
  -F "processor_id=$PROCESSORID" \
  -F "release_notes=This is the latest stable ROSE source release with the EDG binaries for multiple platforms, including i686-pc-linux-gnu, x86_64-pc-linux-gnu, and i686-apple-darwin9.4.0." \
  -F "release_changes=" \
  -F "preformatted=0" \
  -F "submit=Release+File" \
  -A "$USERAGENT" \
  'https://outreach.scidac.gov/frs/admin/qrs.php?group_id=24'
