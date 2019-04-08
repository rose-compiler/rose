#!/bin/bash
# Analyze a log file from e.g. 
#   run_and_log do_make.sh
# Takes one parameter - the name of the log file
# Expects PASSED and FAILED lines

# Sets strict, defines log, log_then_run, etc.:
source `which utility_functions.sh`

# TODO: Fix errors when nothing found.
# Temp: keep script from exiting on error:
set +e

log_file=$1

#log_file_count=`(set -o pipefail; ls ${log_file}.* 2> /dev/null | wc -w)`
#status=$?
#echo ${status}
#echo ${log_file_count}
#ls ${log_file}.* | wc
log "Removing any old ${log_file}_*.* files"
rm -f ${log_file}_*.*

error_lines_file="${log_file}_error-lines.txt"
error_lines_unique_file="${log_file}_error-lines_unique.txt"
error_messages_not_backend="${log_file}_error-messages_not-backend.txt"
error_messages_not_backend_unique="${log_file}_error-messages_not-backend_unique.txt"
error_messages_backend="${log_file}_error-messages_backend.txt"
error_messages_backend_unique="${log_file}_error-messages_backend_unique.txt"
error_files_backend_unique_file="${log_file}_error-files_backend_unique.txt"

log "Putting error lines in ${error_lines_file}"
grep "[Ee]rror: " ${log_file} > ${error_lines_file}

log "Putting sorted, uniqued error lines in ${error_lines_unique_file}"
sort -u ${error_lines_file} > ${error_lines_unique_file}

log "Putting sorted, not-backend messages in ${error_messages_not_backend}"
sed -e 's/^\(rose_.*\.c[cx]*\)\(.*$\)/\1/' ${error_lines_file} | \
grep -v "^rose_" | \
sort > ${error_messages_not_backend}

log "Putting sorted, uniqued not-backend messages in ${error_messages_not_backend_unique}"
sort -u ${error_messages_not_backend} > ${error_messages_not_backend_unique}

log "Putting sorted, uniqued backend error messages in ${error_messages_backend_unique}"
sed -e 's/^.*\([Ee]rror: .*\)/\1/' ${error_lines_file}| \
sort > ${error_messages_backend}
log "Putting sorted, uniqued backend error messages in ${error_messages_backend_unique}"
sort -u ${error_messages_backend} > ${error_messages_backend_unique}

log "Putting sorted, uniqued backend error files in ${error_files_backend_unique_file}"
sed -e 's/^\(rose_.*\.c[cx]*\)\(.*$\)/\1/' ${error_lines_unique_file} | \
grep "^rose_" | \
sort -u > ${error_files_backend_unique_file}

# Count things:
# Old support for do_one.sh with "(status 0)" etc. messages:
do_script_prefix="do_one.*\.sh"
do_started_count="`grep "${do_script_prefix}: .*: Running:" ${log_file} | wc -l`"
do_status_0_count="`grep "${do_script_prefix}: .*: Done. (status 0)" ${log_file} | wc -l`"
do_status_not_0_count="`grep "${do_script_prefix}: .*: Done. (status [^0]\{1,3\})" ${log_file} | wc -l`"
do_unaccounted_count=`expr ${do_started_count} - ${do_status_0_count} - ${do_status_not_0_count}`

runner_script_prefix="(support.runner.Runner)"
runner_started_count="`grep "${runner_script_prefix} Doing" ${log_file} | wc -l`"
runner_passed_count="`grep "^PASSED$" ${log_file} | wc -l`"
runner_failed_count="`grep "^FAILED$" ${log_file} | wc -l`"
runner_unaccounted_count=`expr ${runner_started_count} - ${runner_passed_count} - ${runner_failed_count}`
runner_passed_percent=$(( ${runner_passed_count} * 100 / ${runner_started_count} ))
runner_failed_percent=$(( ${runner_failed_count} * 100 / ${runner_started_count} ))
runner_unaccounted_percent=$(( ${runner_unaccounted_count} * 100 / ${runner_started_count} ))
log "- Runs:"
log "do_ Started            : ${do_started_count}"
log "do_ Status 0           : ${do_status_0_count}"
log "do_ Status not 0       : ${do_status_not_0_count}"
log "do_ Unaccounted-for    : ${do_unaccounted_count}"
log "Runner Started         : ${runner_started_count}"
log "Runner Passed          : ${runner_passed_count} - ${runner_passed_percent}%"
log "Runner Failed          : ${runner_failed_count} - ${runner_failed_percent}%"
log "Runner unaccounted-for : ${runner_unaccounted_count} - ${runner_unaccounted_percent}%"
log "- Errors:"
log "Total error messages   : `cat ${error_lines_file} | wc -l`"
log "Non-backend messages   : `cat ${error_messages_not_backend} | wc -l`"
log "Backend messages       : `cat ${error_messages_backend} | wc -l`"
log "- Errors, unique:"
log "Non-backend messages   : `cat ${error_messages_not_backend_unique} | wc -l`"
log "Backend messages       : `cat ${error_messages_backend_unique} | wc -l`"
log "Backend files          : `cat ${error_files_backend_unique_file} | wc -l`"

# Old support for do_one.sh with "(status 0)" etc. messages:
#log "To find status not 0 messages, do:"
#log "grep \"${do_script}: .*: Done. (status [^0]\{1,3\})\" ${log_file}"

