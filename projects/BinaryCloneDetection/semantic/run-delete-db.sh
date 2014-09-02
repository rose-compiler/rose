#!/bin/bash
# Deletes dbs

argv0="${0##*/}"
dir0="${0%$argv0}"
[ -n "$dir0" ] || dir0="."

usage () {
    local exit_status="${1:-0}"
    echo "usage: $argv0 [SWITCHES] [--] [SPECIMENS...]" >&2
    echo "    This command delete dbs with a prefix." >&2
    echo "    switches are understood:" >&2
    echo "        --dry-run" >&2
    echo "            Perform a dry run without deleting any actual db." >&2
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
dryrun="no"

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
	--dry-run)
	    dryrun="yes"
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


dbs_to_delete=`psql template1 -t -c "select datname from pg_database where datname LIKE '$prefix%'" `

for cur_db in $dbs_to_delete; do
  if [ $dryrun = "no" ]; then
     echo "Deleting $cur_db"
     dropdb $cur_db
  else    
     echo "Would delete $cur_db"
 
  fi
done
