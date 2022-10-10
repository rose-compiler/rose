#! /bin/bash
executable="$1"
specimen="$2"
build_dir="$3"
specimenDir=`dirname ${specimen}`
specimenFilename=`basename ${specimen}`
result1="${specimenFilename}_v1"
result2="${specimenFilename}_v2"
log_file="${specimenFilename}.log"
passed_file="${specimenFilename}.passed"

log () {
  touch $log_file
  echo $@ >> $log_file
}

exec_and_log () {
  log $@
  "$@" >> $log_file 2>&1
}

rm -f "${result1}" "${result2}" "${log_file}"
log `date`

exec_and_log "${executable}" -c "${specimen}" > "${result1}"
status=$?
if [ $status -ne 0 ]; then
    echo "first run failed with return code $status" >> $log_file
    exit 1
fi

#log "$executable -c $specimen $result2"
exec_and_log $executable -c "${specimen}" >& "${result2}"
status=$?
if [ $status -ne 0 ]; then
    echo "second run failed with return code $status" >> $log_file
    exit 2
fi
exec_and_log diff "${result1}" "${result2}"
status=$?
if [ $status -ne 0 ]; then
    echo "result files differ, test failed" >> $log_file
    exit 3
fi

touch "${passed_file}"
