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

DATE=`date +%m_%d_`
results_db_name="results_db_$prefix"
threshold_db_name="thresholds_$DATE$prefix"

execute dropdb   $threshold_db_name
execute createdb $threshold_db_name

#create table to hold precision,recall etc for separate computations for Ox vs Oy
execute psql  $threshold_db_name -c "create table rates_over_Ox_Oy_pairs( \
          sem_threshold double precision, cg_threshold double precision, path_threshold double precision, \
          min_insns integer, max_cluster_size integer, \
          recall_min double precision, recall_max double precision, \
          recall_mean double precision, recall_standard_deviation double precision, \
          specificity_min double precision, specificity_max double precision, \
          specificity_mean double precision, specificity_standard_deviation double precision, \
          precision_min double precision, precision_max double precision, \
          precision_mean double precision, precision_standard_deviation double precision, \
          fscore_mean double precision, fscore_min double precision, fscore_max double precision, \
          fscore_standard_deviation double precision );"

#create table to hold precision, recall etc computing a unified value over all dbs
execute psql  $threshold_db_name -c "create table total_rates( \
          sem_threshold double precision, cg_threshold double precision, path_threshold double precision, \
          min_insns integer, max_cluster_size integer, \
          recall double precision, specificity double precision, precision double precision, \
          fscore double precision );"

#create table to hold precision, recall etc for each optimization pair
execute psql  $threshold_db_name -c "create table Ox_Oy_pairs( \
          sem_threshold double precision, cg_threshold double precision, path_threshold double precision, \
          min_insns integer, max_cluster_size integer, db_group text, \
          recall double precision, specificity double precision, precision double precision, \
          fscore double precision );"





for sem_threshold in 0.75 1.00 0.000 ; 
do 
  for cg_threshold in $(seq 0.000 0.100 1.000 ); 
  do
     for path_threshold in $(seq 0.000 0.100 1.000 )  ;  
     do

      for min_insns in 100;
      do

        for max_cluster_size in -1;
        do
          execute $SRCDIR/run-compute-aggregate.sh --prefix=$prefix \
                                               --min-insns=$min_insns \
                                               --max-cluster-size=$max_cluster_size \
                                               --sem-threshold=$sem_threshold \
                                               --cg-threshold=$cg_threshold \
                                               --hamming-threshold=-1 \
                                               --euclidean-threshold=-1 \
                                               --path-threshold=$path_threshold || exit 1

          #compute the recall, precision etc total for each optimization pair
          DB_GROUPS=`psql $results_db_name -t -c "select db_group from resilience_to_optimization" | tr "\\n"  " "`
          
          for GROUP in $DB_GROUPS; do
            recall=`psql $results_db_name -t -c "select recall from resilience_to_optimization where db_group='$GROUP' limit 1" | tr "\\n"  " " `
            precision=`psql $results_db_name -t -c "select precision from resilience_to_optimization where db_group='$GROUP' limit 1" | tr "\\n"  " " `
            specificity=`psql $results_db_name -t -c "select specificity from resilience_to_optimization where db_group='$GROUP' limit 1" | tr "\\n"  " " `
      
            if [ $precision = 0 -a  $recall = 0  ];
            then
              fscore="0"
            else 
              fscore="2*$precision*$recall/($precision+$recall)"
            fi

            execute psql $threshold_db_name -c "insert into Ox_Oy_pairs(sem_threshold, cg_threshold, path_threshold, min_insns, max_cluster_size, db_group, \
              recall, precision, specificity, fscore) \
              values ($sem_threshold, $cg_threshold, $path_threshold, $min_insns, $max_cluster_size, '$GROUP', $recall, $precision, $specificity, $fscore)" || exit 1

          done
         

          #compute recall, precision etc total over all dbs
          recall=`psql $results_db_name -t -c "select recall from overall_rates limit 1" | tr "\\n"  " " `
          precision=`psql $results_db_name -t -c "select precision from overall_rates limit 1" | tr "\\n"  " " `
          specificity=`psql $results_db_name -t -c "select specificity from overall_rates limit 1" | tr "\\n"  " " `
      
          if [ $precision = 0 -a  $recall = 0  ];
          then
            fscore="0"
          else 
            fscore="2*$precision*$recall/($precision+$recall)"
          fi

          execute psql $threshold_db_name -c "insert into total_rates(sem_threshold, cg_threshold, path_threshold, min_insns, max_cluster_size, \
            recall, precision, specificity, fscore) \
            values ($sem_threshold, $cg_threshold, $path_threshold, $min_insns, $max_cluster_size, $recall, $precision, $specificity, $fscore)" || exit 1


          #compute recall, precision, etc over separate computations for Ox vs Oy
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
          precision_mean=`psql $results_db_name -t -c "select precision_mean from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
          precision_standard_deviation=`psql $results_db_name -t -c "select precision_standard_deviation from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `

          fscore_min=`psql $results_db_name -t -c "select fscore_min from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
          fscore_max=`psql $results_db_name -t -c "select fscore_max from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
          fscore_mean=`psql $results_db_name -t -c "select fscore_mean from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `
          fscore_standard_deviation=`psql $results_db_name -t -c "select fscore_standard_deviation from resilience_to_optimization_rate limit 1" | tr "\\n"  " " `


          execute psql $threshold_db_name -c "insert into rates_over_Ox_Oy_pairs(sem_threshold, cg_threshold, path_threshold, \
            min_insns, max_cluster_size, \
            recall_min, recall_max, recall_mean, recall_standard_deviation, \
            specificity_min, specificity_max, specificity_mean, specificity_standard_deviation, \
            precision_min, precision_max, precision_mean, precision_standard_deviation, \
            fscore_mean, fscore_min, fscore_max, fscore_standard_deviation) \
            values ($sem_threshold, $cg_threshold, $path_threshold, $min_insns, $max_cluster_size, $recall_min, $recall_max, $recall_mean, $recall_standard_deviation, \
               $specificity_min, $specificity_max, $specificity_mean, $specificity_standard_deviation, \
               $precision_min, $precision_max, $precision_mean, $precision_standard_deviation, \
               $fscore_mean, $fscore_min, $fscore_max, $fscore_standard_deviation )" || exit 1

        done
      done
    done                                                 
 done
done
