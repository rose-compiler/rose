#!/bin/bash
# Collect the names of all the source files compiled by do_make.sh

# log, strict, etc.:
source `which utility_functions.sh`

out_file=source_files_2018_08_28.txt
log_file=do_make.sh__2018-08-29_17.13.46.log

rm -f ${out_file}
rm -f ${out_file}.*
grep 'Making cc' ${log_file} > ${out_file}.all
grep 'Making cxx' ${log_file} >> ${out_file}.all
#grep 'Making cc' do_make.sh*.log > ${out_file}.all
#grep 'Making cxx' do_make.sh*.log >> ${out_file}.all
log "`wc -l ${out_file}.all`"
#  Trim off left:
# Making cc  ../../../ares/
# Making cxx  ../../../ares/
# [1m[2m[32mMaking cc  ../../../ares/
# [1m[2m[32mMaking cxx  ../../../ares/
#  Trim off right:
# [0m
# Sort and unique:
sed -e  's#^.*Making cc  \.\./\.\./\.\./ares/\(.*$\)#\1#g' ${out_file}.all | \
sed -e 's#^.*Making cxx  \.\./\.\./\.\./ares/\(.*$\)#\1#g' | \
sed -e 's/\(^.*\)\x1b\[0m$/\1/g' | \
sort -u > ${out_file}
rm ${out_file}.all
log "`wc -l ${out_file}	`"

