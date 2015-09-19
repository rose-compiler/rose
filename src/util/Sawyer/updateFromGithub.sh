#!/bin/bash
# Update ROSE with the latest Sawyer source code
set -e

SAWYER_REPO=${1-https://github.com/matzke1/sawyer}

emit_cpp_warning() {
    echo "// WARNING: Changes to this file must be contributed back to Sawyer or else they will"
    echo "//          be clobbered by the next update from Sawyer.  The Sawyer repository is at"
    echo "//          $SAWYER_REPO."
    echo
    echo
    echo
    echo
}

# Download the latest version of the source code
SAWYER_ROOT="sawyer-$(date +%Y%m%d)"
if [ -d "$SAWYER_ROOT" ]; then
    (
	cd "$SAWYER_ROOT"
	git fetch "$SAWYER_REPO" master
	git merge FETCH_HEAD
    )
else
    git clone "$SAWYER_REPO" "$SAWYER_ROOT"
fi
	
# Copy some of Sawyer's source files into the ROSE source tree. Don't worry about overwriting ROSE-local changes--they
# should have been contributed back to the Sawyer project by now (besides, that's what Git is for)!
for f in \
    Access Assert AddressMap AddressSegment AllocatingBuffer BiMap BitVector BitVectorSupport Buffer CommandLine Cached \
    Callbacks DefaultAllocator DistinctList Graph GraphBoost GraphTraversal IndexedList Interval IntervalMap IntervalSet \
    IntervalSetMap Map MappedBuffer Markup MarkupPod Message NullBuffer Optional PoolAllocator ProgressBar Sawyer Set \
    SharedPointer SmallObject Stack StaticBuffer Stopwatch Synchronization WarningsOff WarningsRestore
do
    srcbase="$SAWYER_ROOT/Sawyer/$f";
    ( emit_cpp_warning; cat "$srcbase.h" ) > ./$f.h
    [ -f "$srcbase.C" ] && ( emit_cpp_warning; cat "$srcbase.C" ) > ./$f.C
done

# Copy some of Sawyers examples. These are needed by ROSE's doxygen
mkdir -p docs/examples
for f in \
    commandLineEx1.C commandLineEx1.out
do
    cp "$SAWYER_ROOT/docs/examples/$f" docs/examples/.
done


# Add a comment to the Message.h file
sed --in-place -e '1i// See also rose::Diagnostics in $ROSE/src/roseSupport/Diagnostics.h' Message.h
