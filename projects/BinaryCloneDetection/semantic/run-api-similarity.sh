#!/bin/bash
# Runs intellectual property theft detection.  Give zero or more specimens as arguments.

argv0="${0##*/}"
dir0="${0%$argv0}"
[ -n "$dir0" ] || dir0="."
worklist="ip-theft-analysis-$$"

usage () {
    local exit_status="${1:-0}"
    echo "usage: $argv0 [SWITCHES] [--] [SPECIMENS...]" >&2
    echo "    This command runs the semantic analysis steps interactively.  The following" >&2
    echo "    switches are understood:" >&2
    echo "        --batch" >&2
    echo "            Instead of being interactive, use only the default values stored in" >&2
    echo "            the configuration file." >&2
    echo "        --config=FILE" >&2
    echo "            Use the specified file for reading and/or storing configuration info." >&2
    echo "            the default file is '.run-analysis.conf'." >&2
    echo "        --[no-]save" >&2
    echo "            The --save switch (the default) saves configuration information into the" >&2
    echo "            configuration file specified with the --config switch (or a default)." >&2
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
    local nwork=$(wc -l <$worklist)

    # Choose number of processors
    if [ $nwork -le 1 ]; then
	echo $nwork
	return 0
    fi
    local nprocs=$(grep --perl-regexp '^processor\s*:' /proc/cpuinfo 2>/dev/null |wc -l)
    [ $nprocs -gt 1 ] && nprocs=$[nprocs-1]
    [ $nprocs -gt $nwork ] && nprocs=$nwork
    echo "Number of items in work list: $nwork" >&2
    if [ $nprocs -gt 1 -a "$interactive" = "yes" ]; then
	read -e -p "How many parallel processes should be used? " -i "$nprocs"
	if [ -n "$REPLY" ]; then
	    nprocs="$REPLY"
	    [ $nprocs -gt $nwork ] && nprocs=$nwork
	fi
    fi

    echo $nprocs
}

# Split a file into parts of equal size based on the parallelism desired.
split_worklist () {
    local nprocs="$1"
    local nwork=$(wc -l <$worklist)

    # Choose number of parts to create
    local nparts=$[5 * nprocs]
    [ $nprocs -eq 1 ] && nparts=1
    [ $nparts -gt $nwork ] && nparts=$nwork
    if [ $nprocs -gt 1 -a "$interactive" = "yes" ]; then
	read -e -p "How many units of work for $nprocs processors? " -i "$nparts"
	if [ -n "$REPLY" ]; then
	    nparts="$REPLY"
	    [ $nparts -gt $nwork ] && nparts=$nwork;
	fi
    fi
    [ $nparts -gt 676 ] && nparts=676 # 26*26

    # Create parts
    if [ $nparts -gt 1 ]; then
	local work_per_part=$[(nwork + nparts - 1) / nparts]
	split --lines=$work_per_part $worklist $worklist-
	rm $worklist
    else
	mv $worklist $worklist-aa
    fi
    echo $worklist-[a-z][a-z]
}

# Counts the number of arguments
count_args () {
    echo "$#"
}

save_settings () {
    if [ "$save_config" = "yes" ]; then
	cat >$configfile <<EOF
# Name or URL of the database
dbname='$dbname'

# Flags for obtaining the list of function pairs for which api similarity needs
# to be calculated
api_similarity_worklist_flags='$api_similarity_worklist_flags'

# Flags for determining api similarity of pairs of functions based on their output
api_similarity_flags='$api_similarity_flags'
EOF
    fi
}

###############################################################################################################################
###############################################################################################################################

# Parse command-line
interactive=yes
configfile=.run-api-similarity.conf
save_config=yes
while [ "$#" -gt 0 -a "${1:0:1}" = "-" ]; do
    arg="$1"; shift
    case "$arg" in
	--)
	    break
	    ;;
	--help|-h)
	    usage 0
	    ;;
	--batch)
	    interactive=""
	    ;;
	--config=*)
	    configfile=${arg#--config=}
	    ;;
	--save)
	    save_config=yes
	    ;;
	--no-save)
	    save_config=
	    ;;
	*)
	    echo "$argv0: unknown command-line switch: $arg" >&2
	    echo "$argv0: see --help for more info" >&2
	    exit 1
	    ;;
    esac
done

if [ -r "$configfile" ]; then
    echo "$0: reading from config file: $configfile" >&2
    . "$configfile"
else
    echo "$0: creating configuration file: $configfile" >&2
fi

: ${ROSE_SRC:=$ROSEGIT_SRC}
: ${ROSE_BLD:=$ROSEGIT_BLD}
: ${SRCDIR:=$ROSE_SRC/projects/BinaryCloneDetection/semantic}
: ${BLDDIR:=$ROSE_BLD/projects/BinaryCloneDetection/semantic}

[ -n "$ROSE_SRC" -a -d "$ROSE_SRC/projects" ] || die "ROSE_SRC should be set to the root of the ROSE source directory"
[ -n "$ROSE_BLD" -a -d "$ROSE_BLD/projects" ] || die "ROSE_BLD should be set to the root of the ROSE build directory"
[ -n "$SRCDIR" -a -d "$SRCDIR" ]              || die "not a directory: $SRCDIR"
[ -n "$BLDDIR" -a -d "$BLDDIR" ]              || die "not a directory: $BLDDIR"

if [ "$interactive" = "yes" ]; then
    echo "=================================================================================================="
    echo "Specify a database which will hold information about the analysis.  The database can be either a"
    echo "file name (SQLite3 database) or a URL.  URLs that have the following syntac:"
    echo "    postgresql://[USER[:PASSWORD]@]HOST[:PORT][/DATABASE][?PARAM=VALUE&...]"
    echo "    sqlite3://[FILENAME[?PARAM[=VALUE]&...]"
    echo "SQLite3 databases are created if they don't exist; PostgreSQL databases must be created before this"
    echo "analysis runs (see the 'createdb' man page)."
    read -e -p "Database name: " -i "$dbname" dbname
fi

if [ "$interactive" = "yes" ]; then
    echo "=================================================================================================="
    echo "These are the flags for 31-func-similarity-worklist. This command generates a list of function"
    echo "pairs for which similarity needs to be calculated."
    $BLDDIR/101-api-similarity-worklist --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    read -e -p "Switches for api-similarity-worklist: " -i "$api_similarity_worklist_flags" api_similarity_worklist_flags
    save_settings
fi
execute $BLDDIR/101-api-similarity-worklist $api_similarity_worklist_flags "$dbname" >$worklist || exit 1
nwork=$(wc -l <$worklist)
if [ "$nwork" -eq 0 ]; then
    echo "No similarities to compute"
else
    nprocs=$(parallelism)
    worklist_parts=$(split_worklist $nprocs)
    nparts=$(count_args $worklist_parts)
    save_settings

    if [ "$interactive" = "yes" ]; then
	echo
	echo "=================================================================================================="
	echo "These are the flags for 105-api-similarity. This tool measures api similarity between pairs of"
	echo "functions by comparing api calls made by the tests.  Basically, if two functions"
	echo "produced similar api calls for the same input then the functions are similar."
	$BLDDIR/105-api-similarity --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
	read -e -p "Switches for api similarity: " -i "$api_similarity_flags" api_similarity_flags
	save_settings
    fi
    if [ "$nparts" -gt 1 ]; then
	redirect=yes
	echo
	echo "Use this command in another window to monitor the progress of 32-func-similarity:"
	echo "    watch 'tail -n1 $worklist-[a-z][a-z].as.out.tmp'"
	echo
	sleep 5
    else
	redirect=no
    fi
    execute make -f $SRCDIR/run-analysis.mk -j $nprocs \
	BINDIR="$BLDDIR" \
	RUN_FLAGS="$func_similarity_flags" \
	DBNAME="$dbname" \
	INPUTS="$worklist_parts" \
	ERROR= \
	REDIRECT="$redirect" \
	api-sim || exit 1
    rm -f $worklist_parts
fi
