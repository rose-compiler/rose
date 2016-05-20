#!/bin/bash
# Update ROSE with the latest Sawyer source code
set -e

SAWYER_DOC_REPO=https://github.com/matzke1/sawyer
SAWYER_REPO=${1-$SAWYER_DOC_REPO}


emit_cpp_warning() {
    echo "// WARNING: Changes to this file must be contributed back to Sawyer or else they will"
    echo "//          be clobbered by the next update from Sawyer.  The Sawyer repository is at"
    echo "//          $SAWYER_DOC_REPO."
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
    Access Assert AddressMap AddressSegment AllocatingBuffer Attribute BiMap BitVector BitVectorSupport Buffer CommandLine \
    Cached Callbacks DefaultAllocator DenseIntegerSet Exception DistinctList Graph GraphAlgorithm GraphBoost GraphTraversal \
    IndexedList Interval IntervalMap IntervalSet IntervalSetMap Map MappedBuffer Markup MarkupPod Message NullBuffer Optional \
    PoolAllocator ProgressBar Sawyer Set SharedPointer SmallObject Stack StaticBuffer Stopwatch Synchronization ThreadWorkers \
    WarningsOff WarningsRestore
do
    srcbase="$SAWYER_ROOT/Sawyer/$f";
    ( emit_cpp_warning; cat "$srcbase.h" ) > ./$f.h
    [ -f "$srcbase.C" ] && ( emit_cpp_warning; cat "$srcbase.C" ) > ./$f.C
done

# Copy some of Sawyers examples. These are needed by ROSE's doxygen
mkdir -p docs/examples
for f in \
    commandLineEx1.C commandLineEx1.out attribute.C graphIso.C
do
    cp "$SAWYER_ROOT/docs/examples/$f" docs/examples/.
done

# Copy some of Sawyer's unit tests into the ROSE source tree.  The destination location should be changed to be the same
# as the rest of the unit testing, but that location has not been decided yet.
for f in                                       \
    Container/addressMapUnitTests.C            \
    Container/bitvecTests.C                    \
    Container/denseIntegerSetUnitTests.C       \
    Container/distinctListUnitTests.C          \
    Container/graphIsomorphismTests.C          \
    Container/graphUnitTests.C                 \
    Container/indexedGraphDemo.C               \
    Container/intervalSetMapUnitTests.C                \
    Container/intervalUnitTests.C              \
    Container/lineVectorUnitTests.C            \
    Container/listUnitTests.C                  \
    Container/mapUnitTests.C                   \
    Container/optionalUnitTests.C              \
    Container/setUnitTests.C
do
    srcbase="$SAWYER_ROOT/tests/$f";
    ( emit_cpp_warning; cat "$srcbase" ) > ../../../tests/roseTests/utilTests/sawyer-$(basename "$f")
done


# Add a comment to the Message.h file
sed --in-place -e '1i// See also rose::Diagnostics in $ROSE/src/roseSupport/Diagnostics.h' Message.h
