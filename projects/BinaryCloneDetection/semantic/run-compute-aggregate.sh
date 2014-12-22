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
    nprocs=32
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
min_insns=""
max_cluster_size=""
euclidean_threshold=""
hamming_threshold=""

while [ "$#" -gt 0 -a "${1:0:1}" = "-" ]; do
    arg="$1"; shift
    case "$arg" in
	--)
	    break
	    ;;
	--help|-h)
	    usage 0
	    ;;
        --sem-threshold=*)
	    sem_threshold=${arg#--sem-threshold=}
	    ;;
	--cg-threshold=*)
	    cg_threshold=${arg#--cg-threshold=}
	    ;;
	--path-threshold=*)
	    path_threshold=${arg#--path-threshold=}
	    ;;
 	--hamming-threshold=*)
	    hamming_threshold=${arg#--hamming-threshold=}
	    ;;
       	--euclidean-threshold=*)
	    euclidean_threshold=${arg#--euclidean-threshold=}
	    ;;
	--min-insns=*)
	    min_insns=${arg#--min-insns=}
	    ;;
	--max-cluster-size=*)
	    max_cluster_size=${arg#--max-cluster-size=}
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


if [ "12$sem_threshold" = "12" ]; then
    echo "Please provide a semantic threshold" 
    usage 0
fi

if [ "12$cg_threshold" = "12" ]; then
    echo "Please provide a callgraph threshold" 
    usage 0
fi

if [ "12$path_threshold" = "12" ]; then
    echo "Please provide a path threshold" 
    usage 0
fi

if [ "12$hamming_threshold" = "12" ]; then
    echo "Please provide a hamming threshold" 
    usage 0
fi

if [ "12$euclidean_threshold" = "12" ]; then
    echo "Please provide a euclidean threshold" 
    usage 0
fi



if [ "12$min_insns" = "12" ]; then
    echo "Please provide a min insns threshold" 
    usage 0
fi


if [ "12$max_cluster_size" = "12" ]; then
    echo "Please provide a max cluster size" 
    usage 0
fi



cg_computation_flags="--sem-threshold=$sem_threshold --cg-threshold=$cg_threshold --path-threshold=$path_threshold --min-insns=$min_insns --hamming-threshold=$hamming_threshold --euclidean-threshold=$euclidean_threshold --max-cluster-size=$max_cluster_size"


: ${ROSE_SRC:=$ROSEGIT_SRC}
: ${ROSE_BLD:=$ROSEGIT_BLD}
: ${SRCDIR:=$ROSE_SRC/projects/BinaryCloneDetection/semantic}
: ${BLDDIR:=$ROSE_BLD/projects/BinaryCloneDetection/semantic}

[ -n "$ROSE_SRC" -a -d "$ROSE_SRC/projects" ] || die "ROSE_SRC should be set to the root of the ROSE source directory"
[ -n "$ROSE_BLD" -a -d "$ROSE_BLD/projects" ] || die "ROSE_BLD should be set to the root of the ROSE build directory"
[ -n "$SRCDIR" -a -d "$SRCDIR" ]              || die "not a directory: $SRCDIR"
[ -n "$BLDDIR" -a -d "$BLDDIR" ]              || die "not a directory: $BLDDIR"

dbs_to_compute_for=`psql template1 -t -c "select datname from pg_database where datname LIKE '$prefix%'" | tr "\\n"  " "`
nprocs=$(parallelism)
redirect="yes"

echo "$dbs_to_compute_for"

execute make -f $SRCDIR/run-analysis.mk -j $nprocs \
  BINDIR="$BLDDIR" \
  RUN_FLAGS="$cg_computation_flags" \
  INPUTS="$dbs_to_compute_for" \
  ERROR= \
  REDIRECT="$redirect" \
  ca-computation || exit 1


results_db_name="results_db_$prefix"

execute dropdb $results_db_name
execute createdb $results_db_name

execute $BLDDIR/111-compute-overall-aggregate --prefix=$prefix || exit 1


