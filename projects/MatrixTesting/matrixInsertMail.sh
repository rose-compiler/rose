#!/bin/bash

# Insert a new database record by parsing an email message

TEMP_FILE=$(mktemp)
>"$TEMP_FILE"

DATABASE="postgresql://rose:fcdc7b4207660a1372d0cd5491ad856e@localhost/rose_matrix"

ERRORS_TO="matzke@hoosierfocus.com"

if (sed -n '1,/^$/p' <"$TEMP_FILE" |sed -n 's/^Subject: //p' |grep '^matrix test result$'); then
    vars=($(sed -n '/^$/,$p' <"$TEMP_FILE" |sed -n '/^[a-zA-Z_][-a-zA-Z_]*=[-a-zA-Z_0-9. :\/()\[\]]/p'))
    echo >>"$TEMP_FILE"
    echo "Filter output:"  >>"$TEMP_FILE"
    (set -x; matrixTestResult --log 'tool(all)' -d "$DATABASE" "${vars[@]}") 2>&1 >>"$TEMP_FILE"
    exit 0
fi

# Might only work if $ERRORS_TO are local addresses
mail -s "matrixInsertMail.sh failed to insert" "$ERRORS_TO" <"$TEMP_FILE"
