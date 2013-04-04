#!/bin/bash
#
# Runs semantic/syntactic clone detection over a set of binary specimens that are specified on the command line.

#------------------------------------------------------------------------------------------------------------------------------
# Configuration, either by changing these values or setting environment variables...

# Number of machine instructions in the syntactic clone detection window
: ${syntactic_winsize:=10}

# Number of machine instructions by which to slide the syntactic window
: ${syntactic_stride:=1}

# Precision needed to classify two functions as syntactically similar.  Values are 0 (everything is a clone) to 1 (exact matching)
: ${syntactic_precision:=0.5}


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
rm -f syntactic.db clones.db
for specimen in "$@"; do
    $ROSE_BLD/projects/BinaryCloneDetection/createVectorsBinary --database syntactic.db --tsv-directory $specimen \
	--stride $syntactic_stride --windowSize $syntactic_winsize \
	|| die "syntactic clone detection failed for $specimen"
    $ROSE_BLD/projects/simulator/CloneDetection $specimen \
	|| die "semantic clone detection failed for $specimen"
done


# Find syntactic clones
$ROSE_BLD/projects/BinaryCloneDetection/findClones --database syntactic.db -t $syntactic_precision \
    || die "syntactic clone detection failed in findClones with precision $syntactic_precision"

# Combine syntactic and semantic databases
echo .dump | sqlite3 syntactic.db | sqlite3 clones.db

# Find semantic clone pairs and combine the syntactic and semantic clone pairs.
sqlite3 clones.db <$ROSE_BLD/projects/simulator/clone_detection/queries.sql

# Convert combined clone pairs into clusters
./clusters_from_pairs clones.db combined_clone_pairs combined_clusters

# Show the results
sqlite3 clones.db <<EOF
.headers on
.mode column
.width 10 11 13 32 32
select a.cluster_id, b.id as function_id, b.entry_va, b.funcname, b.filename
    from combined_clusters a
    join semantic_functions b on a.func_id = b.id
    order by a.cluster_id, a.func_id;
EOF
echo "results have been saved in the combined_clusters table in clones.db"
exit 0
