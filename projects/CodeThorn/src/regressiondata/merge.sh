#!/bin/sh

for i in `seq 14 19`; do
    echo "merge QC and LIVE results: $i"
    cat Problem${i}-qc.csv CodeThorn_Problem${i}_assert_live_csv.txt \
	| sort -n | uniq >tmp.csv 
    mv tmp.csv Problem${i}-merged.csv 
done
