#!/bin/sh
#
# This script tries to upload a distribution package to the Scidac Outreach center web site
#
# Use:
#  ./thisScript.sh 0.9.4a 2927 /home/liao6/daily-test-rose/release/upload/rose-0.9.4a-source-with-EDG-binary-2927.tar.gz
#
# by Jeremiah, 10/14/2008
# Modified by Liao, 11.25.2008
# Last updated: Liao, 2/8/2010

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

# We now always use curl to login.
# we no longer relies on firefox's session ID to detect login status
# a script trying to log in and redirect output to curl.log 
/home/hudson-rose/releaseScripts/scidac-login.sh

SESSION=`grep 'Set-Cookie' /home/hudson-rose/releaseScripts/curl.log | cut -d' ' -f 3 | cut -d'=' -f 2 | cut -d';' -f 1`

if [ "x$SESSION" = "x" ]; then 
   echo "Fatal Cannot get session for outreach.scidac is found. Aborting..."
   exit 1
else
   echo "SESSION is $SESSION"
fi

# This is no longer critical, we can send any Mozilla string, Liao, 2/8/2010
USERAGENT='Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.0.16) Gecko/2009120307 Red Hat/3.0.16-1.el5_4 Firefox/3.0.16'
RELEASEDATE=`date '+%F %H:%M'`
RELEASENAME="${VERSION_NO}-multiplatform-${REVISION_NO}"
TYPEID=5020 # Source .tar.gz
PROCESSORID=8000 # any processor type

echo $RELEASEDATE
echo $SESSION

# explanations for options
# -A user agent string, can be Mozilla/5.0 or any other string
# -F form "name=content"
# -b cookie "name=data", we use it to send cookie value set by loging script
# -v verbose
# @$filename: @ means the following string is a file name to be read the data from
curl \
  -v \
  -b "session_ser=$SESSION" \
  -F "package_id=21" \
  -F "release_name=$RELEASENAME" \
  -F "release_date=$RELEASEDATE" \
  -F "userfile=@$FULL_PACKAGE_NAME" \
  -F "type_id=$TYPEID" \
  -F "processor_id=$PROCESSORID" \
  -F "release_notes=This is the latest stable ROSE source release without the EDG binaries (which will be automatically downloaded during configuration and building) for multiple platforms, including i686-pc-linux-gnu, x86_64-pc-linux-gnu, and i686-apple-darwin9.4.0." \
  -F "release_changes=" \
  -F "preformatted=0" \
  -F "submit=Release+File" \
  -A "$USERAGENT" \
  'https://outreach.scidac.gov/frs/admin/qrs.php?group_id=24'

