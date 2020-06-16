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
rm -rf "$SAWYER_ROOT"
git clone "$SAWYER_REPO" "$SAWYER_ROOT"

# Apply any patches related to ROSE. For instance, we need to patch the serialization unit test to avoid ODR violations
# reported by Address Sanitizer.
for patch in *.patch; do
    if [ -e "$patch" ]; then
	(cd "$SAWYER_ROOT" && patch -p1) <"$patch"
    fi
done

cp "$SAWYER_ROOT/LICENSE" LICENSE
        
# Copy some of Sawyer's source files into the ROSE source tree. Don't worry about overwriting ROSE-local changes--they
# should have been contributed back to the Sawyer project by now (besides, that's what Git is for)!
for f in                                                                                                                                \
    Access AddressMap AddressSegment AllocatingBuffer Assert Attribute BiMap BitVector BitVectorSupport Buffer Cached                   \
    Callbacks Clexer CommandLine CommandLineBoost Database DatabasePostgresql DatabaseSqlite DefaultAllocator DenseIntegerSet           \
    DistinctList DocumentBaseMarkup DocumentMarkup DocumentPodMarkup DocumentTextMarkup Exception FileSystem Graph GraphAlgorithm       \
    GraphBoost GraphIteratorBiMap GraphIteratorMap GraphIteratorSet GraphTraversal IndexedList Interval IntervalMap IntervalSet         \
    IntervalSetMap HashMap Lexer LineVector Map MappedBuffer Message NullBuffer Optional PoolAllocator ProgressBar Sawyer Set           \
    SharedObject SharedPointer SmallObject Stack StackAllocator StaticBuffer Stopwatch Synchronization ThreadWorkers Trace Tracker      \
    Tree Type WarningsOff WarningsRestore WorkList;                                                                                     \
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

# Copy some of Sawyer's unit tests into the ROSE source tree.
for f in                                        \
    CommandLine/cmdUnitTests.C                  \
    Container/addressMapUnitTests.C             \
    Container/attributeUnitTests.C              \
    Container/bitvecTests.C                     \
    Container/denseIntegerSetUnitTests.C        \
    Container/distinctListUnitTests.C           \
    Container/graphIsomorphismTests.C           \
    Container/graphUnitTests.C                  \
    Container/indexedGraphDemo.C                \
    Container/intervalSetMapUnitTests.C         \
    Container/intervalUnitTests.C               \
    Container/hashMapUnitTests.C                \
    Container/lineVectorUnitTests.C             \
    Container/listUnitTests.C                   \
    Container/mapUnitTests.C                    \
    Container/optionalUnitTests.C               \
    Container/setUnitTests.C                    \
    Container/traceUnitTests.C                  \
    Database/databaseUnitTests.C                \
    Markup/markupUnitTests.C                    \
    Message/mesgUnitTests.C                     \
    Pointers/ptrUnitTests.C                     \
    Serialization/serializationUnitTests.C      \
    Threads/workListTests.C
do
    srcbase="$SAWYER_ROOT/tests/$f";
    ( emit_cpp_warning; cat "$srcbase" ) > ../../../tests/smoke/unit/Sawyer/$(basename "$f")
done

# Copy some of Sawyer's tools into the ROSE source tree
for f in stringifyEnums.C; do
    srcbase="$SAWYER_ROOT/tools/$f";
    (emit_cpp_warning; cat "$srcbase" ) > ./$f
done


# Add a comment to the Message.h file
sed --in-place -e '1i// See also Rose::Diagnostics in $ROSE/src/roseSupport/Diagnostics.h' Message.h

rm -rf "$SAWYER_ROOT"
