#!/bin/bash
#
# Runs semantic/syntactic clone detection over a set of binary specimens that are specified on the command line.

#------------------------------------------------------------------------------------------------------------------------------
# Configuration, either by changing these values or setting environment variables...

# If set to anything but an empty string, then entirely skip syntactic clone detection.
: ${syntactic_skip:=}

# Name of the syntactic database
: ${syntactic_dbname:=syntactic.db}

# Number of machine instructions in the syntactic clone detection window
: ${syntactic_winsize:=100}

# Number of machine instructions by which to slide the syntactic window
: ${syntactic_stride:=1}

# Precision needed to classify two functions as syntactically similar.  Values are 0 (everything is a clone) to 1 (exact matching)
: ${syntactic_precision:=0.9}

# Minimum coverage in order for two functions to be considered syntactic clones when building the combined clones table. When
# findClones runs, it might find functions that are 80% similar but the metric it uses is a function of feature vectors, not a
# function of how many instructions the similar feature vectors cover.  The value specified for syntactic_coverage is a
# ratio of the number of bytes covered by similar feature vectors to the total number of instruction bytes in the
# function. Both functions must exceed the specified coverage in order to be considered a syntactic pair.
: ${syntactic_coverage:=0.9}



# If set to anything but an empty string, then entirely skip semantic clone detection
: ${semantic_skip:=}

# Name of the semantic database
: ${semantic_dbname:=semantic.db}

# Number of analysis processes to run in parallel.
: ${semantic_parallelism:=10}

# Parallelism granularity, either 'specimen' or 'test'
: ${semantic_granularity:=test}

# Minimum function size -- skip all functions that have fewer than this many instructions.  The default is the same as
# the syntactic window size.
: ${semantic_minfuncsize:=$syntactic_winsize}

# Number of times to run each function when fuzz testing
: ${semantic_nfuzz:=100}

# Number of pointer values to supply as inputs each time we test a function.  The supplied values are randomly either null
# or non-null. Non-null pointers always point to a valid page of memory. If the function needs more pointer inputs
# then null pointers are supplied.
: ${semantic_npointers:=20}

# Number of non-pointer values to supply as inputs each time we test a function.  The supplied values are random values
# less than 256.  If the function needs more non-pointer inputs then zeros are supplied.
: ${semantic_nnonpointers:=100}

# Maximum number of instructions to simulate in each function.  This limit prevents infinite loops during fuzz testing.
: ${semantic_maxinsns:=5000}

# Extra flags to pass to semantic analysis.  See CloneDetection --help for documentation
: ${semantic_flags:=}



# Name of the combined database
: ${combined_dbname:=clones.db}

# End of configuration variables
#------------------------------------------------------------------------------------------------------------------------------

: ${ROSE_SRC:=$ROSEGIT_SRC}
: ${ROSE_BLD:=$ROSEGIT_BLD}
: ${SEMANTIC_BLD:=$ROSE_BLD/projects/BinaryCloneDetection/semantic}
: ${SYNTACTIC_BLD:=$ROSE_BLD/projects/BinaryCloneDetection/syntactic}


show_settings () {
    if [ -n "$syntactic_skip" ]; then
	echo "syntactic clone detection was skipped"
    else
        echo "syntactic_dbname:        $syntactic_dbname"
	echo "syntactic_winsize:       $syntactic_winsize"
	echo "syntactic_stride:        $syntactic_stride"
	echo "syntactic_precision:     $syntactic_precision"
	echo "syntactic_coverage:      $syntactic_coverage"
    fi
    echo
    if [ -n "$semantic_skip" ]; then
	echo "semantic clone detection was skipped"
    else
	echo "semantic_dbname:         $semantic_dbname"
	echo "semantic_parallelism:    $semantic_parallelism"
	echo "semantic_granularity:    $semantic_granularity"
	echo "semantic_minfuncsize:    $semantic_minfuncsize"
	echo "semantic_nfuzz:          $semantic_nfuzz"
	echo "semantic_npointers:      $semantic_npointers"
	echo "semantic_nnonpointers:   $semantic_nnonpointers"
	echo "semantic_maxinsns:       $semantic_maxinsns"
	echo "semantic_flags:          $semantic_flags"
    fi
    echo
    echo "combined_dbname:         $combined_dbname"
    echo
    echo "specimens:"
    echo "$@" |sed 's/[ \t]\+/\n/g' |sed 's/^/    /'
}
show_settings "$@";
[ "$#" -ne 0 ] && show_settings "$@" >AnalysisSettings-$(date '+%Y%m%d%H%M%S').txt

die () {
    echo "$0: $*" >&2
    exit 1
}

[ -n "$ROSE_SRC" -a -d "$ROSE_SRC/projects" ] || die "ROSE_SRC should be set to the root of the ROSE source directory"
[ -n "$ROSE_BLD" -a -d "$ROSE_BLD/projects" ] || die "ROSE_BLD should be set to the root of the ROSE build directory"
[ -n "$SEMANTIC_BLD" -a -d "$SEMANTIC_BLD" ] || die "SEMANTIC_BLD ($SEMANTIC_BLD) is not a directory"
[ -n "$SYNTACTIC_BLD" -a -d "$SYNTACTIC_BLD" ] || die "SYNTACTIC_BLD ($SYNTACTIC_BLD) is not a directory"

# Make sure everything we need has been built.  The projects/BinaryCloneDetection/syntactic has some errors that cause the build
# to fail if we try to build everything.
if [ "$syntactic_skip" = "" ]; then
    make -C $SYNTACTIC_BLD -k -j \
        createVectorsBinary findClones lshCloneDetection computeClusterPairs \
	|| die "failed to build syntactic clone detection targets in projects/BinaryCloneDetection/syntactic"
fi
if [ "$semantic_skip" = "" ]; then
    make -C $SEMANTIC_BLD -k -j \
	|| die "failed to build semantic clone detection targets in projects/BinaryCloneDetection/semantic"
fi

[ "$#" -eq 0 ] && exit 0

# Build the databases, one for syntactic and one for semantic
if [ -e "$syntactic_dbname" -o -e "$semantic_dbname" ]; then
    echo
    echo "Analysis will append to these existing databases:"
    [ -e "$syntactic_dbname" ] && echo "  $syntactic_dbname"
    [ -e "$semantic_dbname" ]  && echo "  $semantic_dbname"
    echo -n "Interrupt now if you don't want to append (continuing in 10 seconds)... ";
    sleep 10
    echo
fi
rm -f "$combined_dbname";


# Semantic analysis can be run in parallel because it uses transactions to control access to the database.  The easiest way
# to get this to run in parallel with error handling, limiting, cleanup, etc. is to use GNU make's "-j" switch.  The
# granularity is either "test" (each fuzz test for each specimen is run as its own process) or "specimen" (all fuzz tests for
# a single specimen are run by a single executable).
build_makefile () {
    local granularity="$1"; shift
    local mf="clones-$$.mk"

    echo "all: all_analyses" >>$mf
    echo >>$mf
    echo "SEMANTIC_BLD = $SEMANTIC_BLD" >>$mf
    echo -n "SWITCHES = --database=$semantic_dbname --ninputs=$semantic_npointers,$semantic_nnonpointers" >>$mf
    echo " --min-function-size=$semantic_minfuncsize --max-insns=$semantic_maxinsns $semantic_flags" >>$mf
    echo "CloneDetection = \$(SEMANTIC_BLD)/CloneDetection" >>$mf
    echo >>$mf

    local all_outputs=
    for specimen in "$@"; do
	local base=$(basename $specimen)
	local hash=$(md5sum $specimen |cut -c1-16)

	if [ "$granularity" = "test" ]; then
	    for fuzz in $(seq 0 $[semantic_nfuzz-1]); do
		local output="$base-$fuzz-$hash.out"
		all_outputs="$all_outputs $output"
		echo "$output: $specimen" >>$mf
		echo -e "\t\$(CloneDetection) \$(SWITCHES) --nfuzz=1,$fuzz $specimen >\$@.tmp 2>&1" >>$mf
		echo -e "\tmv \$@.tmp \$@" >>$mf
	    done
	elif [ "$granularity" = "specimen" ]; then
	    local output="$base-$hash.out"
	    all_outputs="$all_outputs $output"
	    echo "$output: $specimen" >>$mf
	    echo -e "\t\$(CloneDetection) \$(SWITCHES) --nfuzz=$semantic_nfuzz $specimen >\$@.tmp 2>&1" >>$mf
	    echo -e "\tmv \$@.tmp \$@" >>$mf
	else
	    echo "$0: invalid parallelism granularity: $granularity" >&2
	    echo "$0: granularity should be 'test' or 'specimen'" >&2
	    exit 1
	fi
    done

    echo "outputs = $all_outputs" >>$mf
    echo "all_analyses: \$(outputs)" >>$mf
    echo ".PHONY: clean" >>$mf
    echo "clean:; rm -f \$(outputs) \$(append .tmp, \$(outputs))" >>$mf

    echo $mf
}

if [ "$semantic_skip" = "" ]; then
    echo "================================================================================"
    echo "                           SEMANTIC ANALYSIS"
    makefile=$(build_makefile "$semantic_granularity" "$@")
    make -f $makefile clean
    make -k -j$semantic_parallelism -f $makefile || die "semantic clone detection failed"
fi


if [ "$syntactic_skip" = "" ]; then
    echo "================================================================================"
    echo "                           SYNTACTIC ANALYSIS"

    $SYNTACTIC_BLD/createSchema "$syntactic_dbname"

    # Syntactic analysis must be run serially
    for specimen in "$@"; do
	$SYNTACTIC_BLD/createVectorsBinary \
            --database "$syntactic_dbname" \
            --tsv-directory "$specimen" \
	    --stride $syntactic_stride \
            --windowSize $syntactic_winsize \
	    || die "syntactic clone detection failed for $specimen"
    done

    # Find syntactic clones
    echo "================================================================================"
    echo "                         FINDING SYNTACTIC CLONES"
    $SYNTACTIC_BLD/findClones --database "$syntactic_dbname" -t $syntactic_precision \
	|| die "syntactic clone detection failed in findClones with precision $syntactic_precision"
    $SYNTACTIC_BLD/computeClusterPairs "$syntactic_dbname" \
	|| die "could not compute syntactic cluster pairs"
fi

# Combine syntactic and semantic databases, find semantic clones, combine clones
echo "================================================================================"
echo "                             COMPUTING CLUSTERS"
cp "$semantic_dbname" "$combined_dbname" || exit 1
echo .dump | sqlite3 "$syntactic_dbname" | sqlite3 "$combined_dbname"

# FIXME: Thse two run parameters should be stored in their respective database rather than only the combined database
echo "update run_parameters set min_coverage = $syntactic_coverage;"      |sqlite3 "$combined_dbname"
echo "update run_parameters set min_func_ninsns = $semantic_minfuncsize;" |sqlite3 "$combined_dbname"

# Resolve output groups. The main executable compares output groups exactly, but we might not want that for this
# analysis.  This command updates the values of the semantic_fio.effective_outputgroup column.
echo "Renumbering fuzz test outputs"
$SEMANTIC_BLD/compare_outputs "$combined_dbname"

# Run a batch of SQL statements.  Some of this might take a long time
echo "Running SQL commands from $SEMANTIC_BLD/queries.sql"
sqlite3 "$combined_dbname" <$SEMANTIC_BLD/queries.sql

# Generate cluster tables from similarity pairs
echo "Building cluster tables"
$SEMANTIC_BLD/clusters_from_pairs "$combined_dbname" syntactic_clone_pairs syntactic_clusters
$SEMANTIC_BLD/clusters_from_pairs "$combined_dbname" semantic_clone_pairs semantic_clusters
$SEMANTIC_BLD/clusters_from_pairs "$combined_dbname" combined_clone_pairs combined_clusters

# Generate call-graph pairs from similarity pairs, and then call-graph clusters from call-graph pairs
echo "Building CG-cluster tables"
$SEMANTIC_BLD/call_graph_clones   "$combined_dbname" syntactic_clone_pairs   syntactic_cgclone_pairs
$SEMANTIC_BLD/clusters_from_pairs "$combined_dbname" syntactic_cgclone_pairs syntactic_cgclusters
$SEMANTIC_BLD/call_graph_clones   "$combined_dbname" semantic_clone_pairs    semantic_cgclone_pairs
$SEMANTIC_BLD/clusters_from_pairs "$combined_dbname" semantic_cgclone_pairs  semantic_cgclusters
$SEMANTIC_BLD/call_graph_clones   "$combined_dbname" combined_clone_pairs    combined_cgclone_pairs
$SEMANTIC_BLD/clusters_from_pairs "$combined_dbname" combined_cgclone_pairs  combined_cgclusters

# Show the results (see --help for other options)
$SEMANTIC_BLD/show_results "$combined_dbname" combined cgclusters
echo
echo "results have been saved in the combined_clusters table in $combined_dbname"
exit 0
