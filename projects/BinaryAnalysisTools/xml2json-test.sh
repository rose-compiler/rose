#!/bin/bash
set -e

ROSE_TRANSLATOR=./xml2json
CHEEDOONG_TRANSLATOR=$HOME/GS-CAD/ROSE/sources/xml2json-cheedoong/xml2json
HAY_TRANSLATOR=$HOME/GS-CAD/ROSE/sources/dhs-firmware/src/matzke/xml2json/xml2json.py

xml="$1"; shift
output1=/tmp/x.json

#--------------------------------------------------------------------------------
figlet ROSE
echo "ROSE translator (C++)..."
rm -f "$output1" x-rose.json
"$ROSE_TRANSLATOR" --version
/usr/bin/time -v "$ROSE_TRANSLATOR" "$xml" "$output1"
jq --sort-keys . <"$output1" >x-rose.json

#--------------------------------------------------------------------------------
figlet Cheedoong
echo "Cheedoong translator (C++)..."
rm -f "$output1" x-cheedoong.json
if [ -x "$CHEEDOONG_TRANSLATOR" ]; then
    /usr/bin/time -v "$CHEEDOONG_TRANSLATOR" "$xml" >"$output1"
    jq --sort-keys . <"$output1" >x-cheedoong.json
else
    echo "skipped"
fi

#--------------------------------------------------------------------------------
figlet Hay
echo "Hay translator (Python)..."
rm -rf "$output1" x-hay.json
if [ -x "$HAY_TRANSLATOR" ]; then
    /usr/bin/time -v "$HAY_TRANSLATOR" --strip_text <"$xml" >"$output1"
    jq --sort-keys . <"$output1" >x-hay.json
else
    echo "skipped"
fi

#--------------------------------------------------------------------------------
figlet Compare

if [ -r x-cheedoong.json ]; then
    echo "Differences from Cheedoong to ROSE:"
    diff -u x-cheedoong.json x-rose.json || true
fi

if [ -r x-hay.json ]; then
    echo "Differences from Hay to ROSE:"
    diff -u x-hay.json x-rose.json || true
fi
