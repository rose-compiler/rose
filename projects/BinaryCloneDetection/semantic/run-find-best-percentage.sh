#!/bin/bash
# Runs intellectual property theft detection.  Give zero or more specimens as arguments.

argv0="${0##*/}"
dir0="${0%$argv0}"
[ -n "$dir0" ] || dir0="."

usage () {
    local exit_status="${1:-0}"
    echo "usage: $argv0 [SWITCHES] [--] [SPECIMENS...]" >&2
    echo "    This command iterates over databases with a prefix and computes the aggregates" >&2
    echo "    like e.g precision, recall, and specificity.  The following" >&2
    echo "    switches are understood:" >&2
    echo "        --prefix=PREFIX" >&2
    echo "            The prefix of all databases for which we are to compute the aggregate." >&2
    exit $exit_status
}

# Show an error message and exit
die () {
    echo "$argv0: $*" >&2
    exit 1
}

# For affirmative answers, echo "yes" and return success; otherwise silently return failure
yes_or_no () {
    local prompt="$1" dflt="$2" ans
    read -e -p "$prompt " -i "$dflt" ans
    [ "$ans" = "" ] && ans="$dflt"
    if [ "${ans:0:1}" = "y" -o "${ans:0:1}" = "Y" ]; then
	echo "yes"
	return 0
    fi
    return 1;
}

# Execute the command, and show users what it is
execute () {
    echo "+" "$@" >&2
    "$@"
    return $?
}

# Echos the number of processors that should be used
parallelism () {
    local nprocs=$(grep --perl-regexp '^processor\s*:' /proc/cpuinfo 2>/dev/null |wc -l)
    nprocs=$[nprocs-1]

    echo $nprocs
}

# Counts the number of arguments
count_args () {
    echo "$#"
}



###############################################################################################################################
###############################################################################################################################

# Parse command-line
prefix=""
sem_threshold=""
cg_threshold=""
path_threshold=""

while [ "$#" -gt 0 -a "${1:0:1}" = "-" ]; do
    arg="$1"; shift
    case "$arg" in
	--)
	    break
	    ;;
	--help|-h)
	    usage 0
	    ;;
	--prefix=*)
	    prefix=${arg#--prefix=}
	    ;;
	*)
	    echo "$argv0: unknown command-line switch: $arg" >&2
	    echo "$argv0: see --help for more info" >&2
	    exit 1
	    ;;
    esac
done

if [ "12$prefix" = "12" ]; then
    echo "Please provide a prefix" 
    usage 0
fi


: ${ROSE_SRC:=$ROSEGIT_SRC}
: ${ROSE_BLD:=$ROSEGIT_BLD}
: ${SRCDIR:=$ROSE_SRC/projects/BinaryCloneDetection/semantic}
: ${BLDDIR:=$ROSE_BLD/projects/BinaryCloneDetection/semantic}

[ -n "$ROSE_SRC" -a -d "$ROSE_SRC/projects" ] || die "ROSE_SRC should be set to the root of the ROSE source directory"
[ -n "$ROSE_BLD" -a -d "$ROSE_BLD/projects" ] || die "ROSE_BLD should be set to the root of the ROSE build directory"
[ -n "$SRCDIR" -a -d "$SRCDIR" ]              || die "not a directory: $SRCDIR"
[ -n "$BLDDIR" -a -d "$BLDDIR" ]              || die "not a directory: $BLDDIR"




nprocs=$(parallelism)
redirect="yes"

echo "$dbs_to_compute_for"

results_db_name="results_db_$prefix"
threshold_db_name="find_best_thresholds_$prefix"

execute dropdb   $threshold_db_name
execute createdb $threshold_db_name

execute psql  $threshold_db_name -c "create table search_for_best_precision( \
          sem_threshold double precision, cg_threshold double precision, path_threshold double precision, \
          recall_min double precision, recall_max double precision, \
          recall_mean double precision, recall_standard_deviation double precision, \
          specificity_min double precision, specificity_max double precision, \
          specificity_mean double precision, specificity_standard_deviation double precision, \
          precision_min double precision, precision_max double precision, \
          precision_mean double precision, precision_standard_deviation double precision);"

for sem_threshold in $(seq 0.000 0.100 1.00); 
do 
  for cg_threshold in $(seq 0.000 0.100 1.00); 
  do
     for path_threshold in $(seq 0.000 0.100 1.00); 
     do
        
       execute $SRCDIR/run-compute-aggregate.sh --prefix=$prefix \
                                                --sem-threshold=$sem_threshold \
                                                --cg-threshold=$cg_threshold \
                                                --path-threshold=$path_threshold || exit 1

      recall_min=`psql $results_db_name -t -c "select recall_min from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      recall_max=`psql $results_db_name -t -c "select recall_max from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      recall_mean=`psql $results_db_name -t -c "select recall_mean from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      recall_standard_deviation=`psql $results_db_name -t -c "select recall_standard_deviation from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      
      specificity_min=`psql $results_db_name -t -c "select specificity_min from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      specificity_max=`psql $results_db_name -t -c "select specificity_max from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      specificity_mean=`psql $results_db_name -t -c "select specificity_mean from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      specificity_standard_deviation=`psql $results_db_name -t -c "select specificity_standard_deviation from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `

      precision_min=`psql $results_db_name -t -c "select precision_min from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      precision_max=`psql $results_db_name -t -c "select precision_max from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      precision_mean=`psql $results_db_name -t -c "select precision_max from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
      precision_standard_deviation=`psql $results_db_name -t -c "select precision_standard_deviation from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `

      execute psql $threshold_db_name -c "insert into search_for_best_precision(sem_threshold, cg_threshold, path_threshold, \
        recall_min, recall_max, recall_mean, recall_standard_deviation, \
        specificity_min, specificity_max, specificity_mean, specificity_standard_deviation, \
        precision_min, precision_max, precision_mean, precision_standard_deviation) \
        values ($sem_threshold, $cg_threshold, $path_threshold, $recall_min, $recall_max, $recall_mean, $recall_standard_deviation, \
            $specificity_min, $specificity_max, $specificity_mean, $specificity_standard_deviation, \
            $precision_min, $precision_max, $precision_mean, $precision_standard_deviation)"

    done                                                 
 done
done
