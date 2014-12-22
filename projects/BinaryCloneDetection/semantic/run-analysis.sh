#!/bin/bash
# Runs semantic analysis.  Give zero or more specimens as arguments.

argv0="${0##*/}"
dir0="${0%$argv0}"
[ -n "$dir0" ] || dir0="."
worklist="clone-analysis-$$"

# Show an error message and exit
die () {
    echo "$argv0: $*" >&2
    exit 1
}

# Parse command-line
interactive=yes
configfile=.run-analysis.conf
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

###############################################################################################################################
# Functions
###############################################################################################################################

usage () {
    local exit_status="${1:-0}"
    echo "usage: $argv0 [SWITCHES] [--] [SPECIMENS...]" >&2
    echo "    This command runs the semantic analysis steps interactively.  The following" >&2
    echo "    switches are understood:" >&2
    echo "        --batch" >&2
    echo "            Instead of being interactive, use only the default values stored in" >&2
    echo "            the configuration file.  Beware of the \$recreate value, which might" >&2
    echo "            cause the database to be recreated from scratch each run rather than" >&2
    echo "            building it incrementally, or vice versa." >&2
    echo "        --config=FILE" >&2
    echo "            Use the specified file for reading and/or storing configuration info." >&2
    echo "            the default file is '.run-analysis.conf'." >&2
    echo "        --[no-]save" >&2
    echo "            The --save switch (the default) saves configuration information into the" >&2
    echo "            configuration file specified with the --config switch (or a default)." >&2
    exit $exit_status
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
    local maxprocs="${1-0}"
    local nwork=$(wc -l <$worklist)

    # Choose number of processors
    if [ $nwork -le 1 ]; then
	echo $nwork
	return 0
    fi
    local nprocs=$(grep --perl-regexp '^processor\s*:' /proc/cpuinfo 2>/dev/null |wc -l)
    [ $nprocs -gt 1 ] && nprocs=$[nprocs-1]
    [ $nprocs -gt $nwork ] && nprocs=$nwork
    [ $maxprocs -gt 0 -a $nprocs -gt $maxprocs ] && nprocs=$maxprocs

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
split_worklist_1d () {
    local nprocs="$1" job_multiplier="${2-5}"
    local nwork=$(wc -l <$worklist)

    # Choose number of parts to create
    local nparts=$[job_multiplier * nprocs]
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

# Split list of pairs into parts of approx equal size in 2d.  The goal is to partition the space into rectangular reagions
# that have approximately the same number of points but which minimize the length of the perimeter.
split_worklist_2d () {
    local nprocs="$1" njobs="${2-64}"
    $BLDDIR/31-split-2dworklist --threads=$nprocs $njobs <$worklist | $SRCDIR/31-split-into-files --prefix=$worklist-
    rm $worklist
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

# Should the database tables be dropped and recreated?
recreate='$recreate'

# Flags for generating inputs
generate_inputs_flags='$generate_inputs_flags'

# Flags for adding functions
add_functions_flags='$add_functions_flags'

# Flags for obtaining the list of functions to test
get_pending_tests_flags='$get_pending_tests_flags'

# Flags for running each test
run_tests_cmd='$run_tests_cmd'
run_tests_nprocs='$run_tests_nprocs'
run_tests_job_multiplier='$run_tests_job_multiplier'
run_tests_flags='$run_tests_flags'

# Flags for obtaining the list of function pairs for which similarity needs
# to be calculated
func_similarity_worklist_flags='$func_similarity_worklist_flags'

# Flags for determining similarity of pairs of functions based on their output
func_similarity_flags='$func_similarity_flags'
EOF
    fi
}

###############################################################################################################################
# Main program
###############################################################################################################################

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
    echo
    echo "=================================================================================================="
    echo "The analysis can incrementally build the database a few specimens at a time. If you want to delete"
    echo "all existing tables and start over, then enter 'no' for this question..."
    recreate=$(yes_or_no "Do you want to (re)create the database?" ${recreate:-no})
    save_settings
fi
if [ "$recreate" = "yes" ]; then
    execute $BLDDIR/00-create-schema "$dbname" || exit 1
fi

if [ "$interactive" = "yes" ]; then
    echo
    echo "=================================================================================================="
    echo "These are the flags for the 10-generate-inputs command. This command creates input groups, each"
    echo "having a list of input values.  One input group is supplied to each test.  Input groups that are"
    echo "already present in the database are not modified by this command."
    $BLDDIR/10-generate-inputs --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    read -e -p "Switches for generating input groups: " -i "$generate_inputs_flags" generate_inputs_flags
    save_settings
fi
if [ -n "$generate_inputs_flags" ]; then
    execute $BLDDIR/10-generate-inputs $generate_inputs_flags "$dbname" || exit 1
fi

if [ "$#" -gt 0 ]; then
    if [ "$interactive" = "yes" ]; then
	echo
	echo "=================================================================================================="
	echo "These are the flags for the 11-add-functions command, which parses a specimen and adds its"
	echo "functions to the database.  You will be able to select which functions to test in the next step."
	$BLDDIR/11-add-functions --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
	read -e -p "Switches for adding specimens: " -i "$add_functions_flags" add_functions_flags
	save_settings
    fi
    for specimen in "$@"; do
	execute $BLDDIR/11-add-functions $add_functions_flags "$dbname" "$specimen" || exit 1
    done
fi

if [ "$interactive" ]; then
    echo
    echo "=================================================================================================="
    echo "These are the flags for the 20-get-pending-tests command, which consults the database to determine"
    echo "which combinations of function and input group need to be tested."
    $BLDDIR/20-get-pending-tests --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    $BLDDIR/20-get-pending-tests --specimens=list "$dbname"
    read -e -p "Switches for selecting tests to run: " -i "$get_pending_tests_flags" get_pending_tests_flags
    save_settings
fi
execute $BLDDIR/20-get-pending-tests $get_pending_tests_flags "$dbname" >$worklist || exit 1
nwork=$(wc -l <$worklist)
if [ "$nwork" -eq 0 ]; then
    echo "No tests to run"
else
    if [ "$interactive" = "yes" ]; then
	echo
	echo "=================================================================================================="
	echo "Would you like to run the 25-run-tests-fork rather than 25-run-tests?  The fork version of the"
	echo "command handles its perallelism internally by forking new processes, while the non-fork version"
	echo "uses a parallel makefile (which will be generated automatically)."
	echo
	[ "$run_tests_cmd" = "" ] && run_tests_cmd="25-run-tests";
	read -e -p "Command for running tests (without path): " -i "$run_tests_cmd" run_tests_cmd
	save_settings
    fi

    if [ "$interactive" = "yes" ]; then
	echo
	echo "=================================================================================================="
	echo "These are the flags for running tests."
	$BLDDIR/$run_tests_cmd --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
	read -e -p "Switches for running tests: " -i "$run_tests_flags" run_tests_flags
	save_settings
    fi

    if [ "$run_tests_cmd" = "25-run-tests-fork" ]; then
	execute $BLDDIR/$run_tests_cmd $run_tests_flags "$dbname" <$worklist || exit 1
	rm -f $worklist;
    else
	nprocs=$(parallelism $run_tests_nprocs)
	worklist_parts=$(split_worklist_1d $nprocs $run_tests_job_multiplier)
	nparts=$(count_args $worklist_parts)
	save_settings

	if [ "$nparts" -gt 1 ]; then
	    redirect=yes
	    echo
	    echo "Use this command in another window to monitor the progress of 25-run-tests:"
	    echo "    watch 'tail -n1 $worklist-[a-z][a-z].rt.out.tmp'"
	    echo
	    sleep 5
	else
	    redirect=no
	fi
	execute make -f $SRCDIR/run-analysis.mk -j $nprocs \
	    BINDIR="$BLDDIR" \
	    RUN_FLAGS="$run_tests_flags" \
	    DBNAME="$dbname" \
	    INPUTS="$worklist_parts" \
	    ERROR= \
	    REDIRECT="$redirect" \
	    run-tests || exit 1
	rm -f $worklist_parts
    fi
fi

if [ "$interactive" = "yes" ]; then
    echo "=================================================================================================="
    echo "These are the flags for 31-func-similarity-worklist. This command generates a list of function"
    echo "pairs for which similarity needs to be calculated."
    $BLDDIR/31-func-similarity-worklist --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    read -e -p "Switches for func-similarity-worklist: " -i "$func_similarity_worklist_flags" func_similarity_worklist_flags
    save_settings
fi
execute $BLDDIR/31-func-similarity-worklist $func_similarity_worklist_flags "$dbname" >$worklist || exit 1
nwork=$(wc -l <$worklist)
if [ "$nwork" -eq 0 ]; then
    echo "No similarities to compute"
else
    nprocs=$(parallelism)
    worklist_parts=$(split_worklist_2d $nprocs)
    nparts=$(count_args $worklist_parts)
    save_settings
    $SRCDIR/31-analyze-split $worklist_parts

    if [ "$interactive" = "yes" ]; then
	echo
	echo "=================================================================================================="
	echo "These are the flags for 32-func-similarity. This tool measures similarity between pairs of"
	echo "functions by comparing output groups that were produced by the tests.  Basically, if two functions"
	echo "produced similar outputs for the same input then the functions are similar."
	$BLDDIR/32-func-similarity --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
	read -e -p "Switches for function similarity: " -i "$func_similarity_flags" func_similarity_flags
	save_settings
    fi
    if [ "$nparts" -gt 1 ]; then
	redirect=yes
	echo
	echo "Use this command in another window to monitor the progress of 32-func-similarity:"
	echo "    watch 'tail -n1 $worklist-[a-z][a-z].fs.out.tmp'"
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
	func-sim || exit 1
    rm -f $worklist_parts
fi
