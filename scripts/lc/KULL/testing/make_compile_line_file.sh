#!/bin/bash
# Make a file of compilation lines

pattern="/usr/dnta/kull/developers/tools/compilers/mvapich2-2.2/gcc-4.9.3p/mpicxx "
command_file="compile_line_file.txt"
rm -f ${command_file}
log_file="/g/g17/charles/code/ROSE/ROSE-18/kull_testing/do_make.sh__2018-02-26_11.31.17.log"
grep "${pattern}" ${log_file} >> ${command_file}
log_file="/g/g17/charles/code/ROSE/ROSE-18/kull_testing/do_make.sh__2018-02-26_11.36.55.log"
grep "${pattern}" ${log_file} >> ${command_file}

sed -e 's/^0: //' ${command_file} > ${command_file}.sed
sort ${command_file}.sed > ${command_file}.sed.sorted
sort -u ${command_file}.sed.sorted > ${command_file}.sed.unique

