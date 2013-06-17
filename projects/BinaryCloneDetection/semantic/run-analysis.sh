#!/bin/bash
# Runs semantic analysis.  Give zero or more specimens as arguments.

argv0="${0##*/}"
dir0="${0%$argv0}"
[ -n "$dir0" ] || dir0="."

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

# A hint for how much parallelism to employ
parallelism='$parallelism'

# Flags for running each test
run_tests_flags='$run_tests_flags'

# Flags for determining similarity of pairs of functions based on their output
func_similarity_flags='$func_similarity_flags'

# Flags for creating clusters from pairs of similar functions
clusters_from_pairs_flags='$clusters_from_pairs_flags'
EOF
    fi
}

###############################################################################################################################
###############################################################################################################################

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

[ -r "$configfile" ] && . "$configfile"

: ${ROSE_SRC:=$ROSEGIT_SRC}
: ${ROSE_BLD:=$ROSEGIT_BLD}
: ${SRCDIR:=$ROSE_SRC/projects/BinaryCloneDetection/semantic}
: ${BLDDIR:=$ROSE_BLD/projects/BinaryCloneDetection/semantic}

[ -n "$ROSE_SRC" -a -d "$ROSE_SRC/projects" ] || die "ROSE_SRC should be set to the root of the ROSE source directory"
[ -n "$ROSE_BLD" -a -d "$ROSE_BLD/projects" ] || die "ROSE_BLD should be set to the root of the ROSE build directory"
[ -n "$SRCDIR" -a -d "$SRCDIR" ]              || die "not a directory: $SRCDIR"
[ -n "$BLDDIR" -a -d "$BLDDIR" ]              || die "not a directory: $BLDDIR"

if [ "$interactive" = "yes" ]; then
    echo "=============================================================================="
    echo "Specify a database which will hold information about the analysis.  The data-"
    echo "base can be either a file name (SQLite3 database) or a URL.  The only URLs"
    echo "accepted at this time are PostgreSQL URLs that have the following format:"
    echo "    postgresql://[USER[:PASSWORD]@]HOST[:PORT][/DATABASE][?PARAM=VALUE&...]"
    echo "SQLite3 databases are created if they don't exist; PostgreSQL databases must"
    echo "be created before this analysis runs (see the 'createdb' man page)."
    read -e -p "Database name: " -i "$dbname" dbname
fi

if [ "$interactive" = "yes" ]; then
    echo
    echo "=============================================================================="
    echo "The analysis can incrementally build the database a few specimens at a time."
    echo "If you want to delete all existing tables and start over, then enter 'no' for"
    echo "this question..."
    recreate=$(yes_or_no "Do you want to (re)create the database?" ${recreate:-no})
    save_settings
fi
if [ "$recreate" = "yes" ]; then
    execute $BLDDIR/00-create-schema "$dbname" || exit 1
fi

if [ "$interactive" = "yes" ]; then
    echo
    echo "=============================================================================="
    echo "These are the flags for the 01-generate-inputs command:"
    $BLDDIR/01-generate-inputs --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    read -e -p "Switches for generating input groups: " -i "$generate_inputs_flags" generate_inputs_flags
    save_settings
fi
execute $BLDDIR/01-generate-inputs $generate_inputs_flags "$dbname" || exit 1

if [ "$#" -gt 0 ]; then
    if [ "$interactive" = "yes" ]; then
	echo
	echo "=============================================================================="
	echo "These are the flags for the 01-add-functions command:"
	$BLDDIR/01-add-functions --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
	read -e -p "Switches for adding specimens: " -i "$add_functions_flags" add_functions_flags
	save_settings
    fi
    for specimen in "$@"; do
	execute $BLDDIR/01-add-functions $add_functions_flags "$dbname" "$specimen" || exit 1
    done
fi

if [ "$interactive" ]; then
    echo
    echo "=============================================================================="
    echo "These are the flags for the 02-get-pending-tests command:"
    $BLDDIR/02-get-pending-tests --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    read -e -p "Switches for selecting tests to run: " -i "$get_pending_tests_flags" get_pending_tests_flags
    save_settings
fi
test_list_file="clone-analysis-$$"
execute $BLDDIR/02-get-pending-tests $get_pending_tests_flags "$dbname" >$test_list_file || exit 1
ntests_to_run=$(wc -l <$test_list_file)
if [ "$ntests_to_run" -eq 0 ]; then
    echo "No tests to run"
else
    # Figure out some settings for parallelism.  In the simplest case, if the user wants parallelism of N we could just split
    # the list of inputs into N parts. However, if the inputs include wildly different classes of functions in terms of how
    # long it takes to execute each one, then some parts might finish long before others.  Therefore, for N>1, we split the
    # input into 3*N parts.  We don't want the factor to be too large because each part runs in a separate process, and each
    # process may have to do an expensive specimen parsing and disassembly step. Also, we need to make sure that the user's
    # choice of parallelism in the config file is reasonable for the number of tests to run, but not change that setting in the
    # config file.  The config file variable is "parallelism" while we use "nprocs" and "nparts" in this script.
    echo "Number of tests to run: $ntests_to_run"
    [ $ntests_to_run -gt 1 -a "$interactive" = "yes" ] && \
	read -e -p "How many processes should run these tests? " -i "$parallelism" parallelism
    nprocs="$parallelism"
    [ "$nprocs" = "" ] && nprocs=1
    [ $nprocs -lt 1 ] && nprocs=1
    [ $nprocs -gt $ntests_to_run ] && nprocs=$ntests_to_run

    nparts=$nprocs;
    [ "$nparts" -gt 1 ] && nparts=$[3 * nparts]

    tests_per_part=$[(ntests_to_run + nparts - 1) / nparts]
    rm -f $test_list_file-[a-z] $test_list_file-[a-z][a-z] $test_list_file-[a-z][a-z][a-z]
    split --lines=$tests_per_part $test_list_file $test_list_file-
    test_list_files=$(ls "$test_list_file"-* |head -n $nprocs |tr '\n' ' ')
    rm "$test_list_file"
    echo "number of processes to run in parallel: $nprocs"
    save_settings

    if [ "$interactive" = "yes" ]; then
	echo
	echo "=============================================================================="
	echo "These are the flags for 03-run-tests:"
	$BLDDIR/03-run-tests --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
	read -e -p "Switches for running tests: " -i "$run_tests_flags" run_tests_flags
	save_settings
    fi
    if [ "$nparts" -gt 1 ]; then
	redirect=yes
	echo "Use this command in another window to watch the progress of this run: "
	echo "    watch 'tail -n1 $test_list_file-*.out.tmp'"
	sleep 2
    else
	redirect=no
    fi
    execute make -f $SRCDIR/run-analysis.mk -j $nprocs \
	BINDIR="$BLDDIR" \
	RUN_FLAGS="$run_tests_flags" \
	DBNAME="$dbname" \
	INPUTS="$test_list_files" \
	ERROR= \
	REDIRECT="$redirect" \
	|| exit 1
    rm -f $test_list_files
fi

if [ "$interactive" = "yes" ]; then
    echo
    echo "=============================================================================="
    echo "These are the flags for 04-func-similarity:"
    $BLDDIR/04-func-similarity --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    read -e -p "Switches for function similarity: " -i "$func_similarity_flags" func_similarity_flags
    save_settings
fi
execute $BLDDIR/04-func-similarity $func_similarity_flags "$dbname" || exit 1

if [ "$interactive" = "yes" ]; then
    echo
    echo "=============================================================================="
    echo "These are the flags for 05-clusters-from-pairs, which organizes pairs of"
    echo "similar functions into clusters:"
    $BLDDIR/05-clusters-from-pairs --help  2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
    read -e -p "Switches for creating clusters: " -i "$clusters_from_pairs_flags" clusters_from_pairs_flags
    save_settings
fi
execute $BLDDIR/05-clusters-from-pairs $clusters_from_pairs_flags "$dbname" semantic_funcsim semantic_clusters

echo
echo "=============================================================================="
execute $BLDDIR/06-list-clusters --summarize "$dbname"
