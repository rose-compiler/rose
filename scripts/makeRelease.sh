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
# Modified by Liao, 10.29.2008
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
SESSION=`sed -n '/^outreach\.scidac\.gov\t.*\tsession_ser\t/p' < $HOME/.mozilla/firefox/$FIREFOXID/cookies.txt | cut -f 7`
if [ "x$SESSION" = "x" ]; then 
  echo "Fatal error: No session for outreach.scidac is found! Not logged in!!"
  exit 1
fi
# This needs to match the browser you used to log in to Outreach; find it from http://browserspy.dk/useragent.php
USERAGENT='Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.18) Gecko/20081029 Firefox/2.0.0.18'
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
