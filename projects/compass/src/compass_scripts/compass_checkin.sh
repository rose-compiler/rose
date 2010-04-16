#!/bin/bash
###############################################################################
#
# Authors: Gary Yuan & Ramakrishna Upadrasta
# File: compass_checkin.sh
# Date: July 18th 2007
# Help: Contact Gary Yuan
#
###############################################################################

FILE=$1
CHECKIN_DIRECTORY=$2
declare -i MAX_FILE_SIZE=10240

if(( $# != 2 )); then
    echo "usage: $0 <Tar GZIP checker> <Checkin Directory>"
    exit 1
fi

if [[ -f $FILE ]]; then
    tar -ztf ${FILE} >& /dev/null

    if(( $? != 0 )); then
        echo "Error: Cannot submit non tar GZIP archive ${FILE}"
        exit 1
    fi

    declare -i FILE_SIZE=`du -sk ${FILE} | awk '{print $1}'`
    if(( $FILE_SIZE > $MAX_FILE_SIZE )); then
        echo "Error: File size of ${FILE} exceeds the maximum file size ${MAX_FILE_SIZE} KB."
        exit 1
    fi
    
    echo "checking in $FILE to ${CHECKIN_DIRECTORY}..."
    cp $FILE $CHECKIN_DIRECTORY
    
    chgrp rose ${CHECKIN_DIRECTORY}/`basename ${FILE}`

    #   We do not want to do a chown because the user may want to do another submission.  
    #    chown dquinlan ${CHECKIN_DIRECTORY}/`basename ${FILE}`

    chmod g+r ${CHECKIN_DIRECTORY}/`basename ${FILE}`
    echo "Done...Thank you for submitting $FILE!"
else
    echo "Error: Cannot submit ${FILE}. Can only submit regular files."
    exit 1
fi

exit 0
