#!/bin/bash

# this script takes as input a CSV file with lines:
# NUMBER,FILENAME
# NUMBER,FILENAME
# ...
# and renames all files according to this list as DRD<NUMBER>-<FILENAME>
# the number is format to 3 digits with leading 0s.

while IFS='' read -r line || [[ -n "$line" ]]; do
    IFS=',' read -ra ENTRY <<< "$line"
    ID=${ENTRY[0]}
    FILENAME=${ENTRY[1]}
#    printf "DRB%03d-%s\n" $ID $FILENAME
    COMMAND=$(printf "git mv %s DRB%03d-%s\n" $FILENAME $ID $FILENAME)
    echo "DRYRUN: $COMMAND"
#    $COMMAND
#    echo "ID:$ID FILENAME:$FILENAME"
done < "$1"
