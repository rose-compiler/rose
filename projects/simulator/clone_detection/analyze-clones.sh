#!/bin/bash
#
# Runs semantic/syntactic clone detection over a set of binary specimens that are specified on the command line.

#------------------------------------------------------------------------------------------------------------------------------
# Configuration, either by changing these values or setting environment variables...

# Name of the syntactic database
: ${syntactic_dbname:=syntactic.db}

# Name of the semantic database
: ${semantic_dbname:=semantic.db}

# Name of the combined database
: ${combined_dbname:=clones.db}

# Number of machine instructions in the syntactic clone detection window
: ${syntactic_winsize:=10}

# Number of machine instructions by which to slide the syntactic window
: ${syntactic_stride:=1}

# Precision needed to classify two functions as syntactically similar.  Values are 0 (everything is a clone) to 1 (exact matching)
: ${syntactic_precision:=0.5}

# Number of times to run each function when fuzz testing
: ${semantic_nfuzz:=10}

# Number of pointer values to supply as inputs each time we test a function.  The supplied values are randomly either null
# or non-null. Non-null pointers always point to a valid page of memory. If the function needs more pointer inputs
# then null pointers are supplied.
: ${semantic_npointers:=3}

# Number of non-pointer values to supply as inputs each time we test a function.  The supplied values are random values
# less than 256.  If the function needs more non-pointer inputs then zeros are supplied.
: ${semantic_nnonpointers:=3}

# Maximum number of instructions to simulate in each function.  This limit prevents infinite loops during fuzz testing.
: ${semantic_maxinsns:=256}

# End of configuration variables
#------------------------------------------------------------------------------------------------------------------------------









: ${ROSE_SRC:=$ROSEGIT_SRC}
: ${ROSE_BLD:=$ROSEGIT_BLD}

die () {
    echo "$0: $*" >&2
    exit 1
}

[ -n "$ROSE_SRC" -a -d "$ROSE_SRC/projects" ] || die "ROSE_SRC should be set to the root of the ROSE source directory"
[ -n "$ROSE_BLD" -a -d "$ROSE_BLD/projects" ] || die "ROSE_BLD should be set to the root of the ROSE build directory"

# Make sure everything we need has been built.  The projects/BinaryCloneDetection has some errors that cause the build
# to fail if we try to build everything.
make -C $ROSE_BLD/projects/BinaryCloneDetection -k -j createVectorsBinary findClones lshCloneDetection \
    || die "failed to build syntactic clone detection targets in projects/BinaryCloneDetection"
make -C $ROSE_BLD/projects/simulator -k -j CloneDetection clusters_from_pairs \
    || die "failed to build semantic clone detection targets in projects/simulator"


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

for specimen in "$@"; do
    banner $(basename "$specimen") 2>/dev/null
    echo "================================================================================"
    echo "                           SYNTACTIC ANALYSIS"
    $ROSE_BLD/projects/BinaryCloneDetection/createVectorsBinary --database "$syntactic_dbname" --tsv-directory "$specimen" \
	--stride $syntactic_stride --windowSize $syntactic_winsize \
	|| die "syntactic clone detection failed for $specimen"
    echo "================================================================================"
    echo "                           SEMANTIC ANALYSIS"
    $ROSE_BLD/projects/simulator/CloneDetection --debug --database="$semantic_dbname" --nfuzz=$semantic_nfuzz \
        --ninputs=$semantic_npointers,$semantic_nnonpointers --max-insns=$semantic_maxinsns "$specimen" \
	|| die "semantic clone detection failed for $specimen"
done


# Find syntactic clones
echo "================================================================================"
echo "                         FINDING SYNTACTIC CLONES"
$ROSE_BLD/projects/BinaryCloneDetection/findClones --database "$syntactic_dbname" -t $syntactic_precision \
    || die "syntactic clone detection failed in findClones with precision $syntactic_precision"

# Combine syntactic and semantic databases, find semantic clones, combine clones
echo "================================================================================"
echo "                   FINDING SEMANTIC CLONES & COMBINING"
cp "$semantic_dbname" "$combined_dbname" || exit 1
echo .dump | sqlite3 "$syntactic_dbname" | sqlite3 "$combined_dbname"
sqlite3 "$combined_dbname" <$ROSE_BLD/projects/simulator/clone_detection/queries.sql
./clusters_from_pairs "$combined_dbname" combined_clone_pairs combined_clusters

# Show the results
echo "================================================================================"
echo "                                FINAL RESULTS"
sqlite3 "$combined_dbname" <<EOF
.headers on
.mode column
.width 10 11 13 32 32
select a.cluster_id, b.id as function_id, b.entry_va, b.funcname, b.filename
    from combined_clusters a
    join semantic_functions b on a.func_id = b.id
    order by a.cluster_id, a.func_id;
EOF
echo "results have been saved in the combined_clusters table in $combined_dbname"
exit 0
