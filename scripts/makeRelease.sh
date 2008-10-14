#!/bin/sh
# by Jeremiah, 10/14/2008

# ls ~/.mozilla/firefox/ to find out your firefox id
FIREFOXID=58zvv6td.default

SESSION=`sed -n '/^outreach\.scidac\.gov\t.*\tsession_ser\t/p' < $HOME/.mozilla/firefox/$FIREFOXID/cookies.txt | cut -f 7`

# This needs to match the browser you used to log in to Outreach; find it from http://browserspy.dk/useragent.php
USERAGENT='Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.17) Gecko/20080829 Firefox/2.0.0.17'

# The actual content to be filled into the web form
RELEASENAME="0.9.3a-multiplatform-2484"
RELEASEDATE=`date '+%F %H:%M'`
FILENAME="/home/liao6/daily-test-rose/20081014_120001/build/rose-0.9.3a-source-with-EDG-binary-2484.tar.gz"
TYPEID=5020 # Source .tar.gz
PROCESSORID=8000 # any processor type
# PROCESSORID=9999 # other

echo $RELEASEDATE
echo $SESSION

#exit 0

curl \
  -v \
  -b "session_ser=$SESSION" \
  -F "package_id=21" \
  -F "release_name=$RELEASENAME" \
  -F "release_date=$RELEASEDATE" \
  -F "userfile=@$FILENAME" \
  -F "type_id=$TYPEID" \
  -F "processor_id=$PROCESSORID" \
  -F "release_notes=This is the latest stable ROSE source release with the EDG binaries for multiple platforms, including i686-pc-linux-gnu, x86_64-pc-linux-gnu, and i686-apple-darwin9.4.0." \
  -F "release_changes=" \
  -F "preformatted=0" \
  -F "submit=Release+File" \
  -A "$USERAGENT" \
  'https://outreach.scidac.gov/frs/admin/qrs.php?group_id=24'
