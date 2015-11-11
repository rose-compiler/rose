include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = stringSupport commandlineProcessing support graphs

# AUTOMAKE_OPTIONS = subdir-objects

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (7/4/2004): Need this to be explicit so that it will not be set to "libroseutil.a.c"
# DIST_SOURCES =

# lib_LIBRARIES = libroseutil.a
# libroseutil_a_DEPENDENCIES = $(libroseutilDependencies)

# DQ (4/5/2009): I have moved rose_paths.h file from src/frontend/SageIII to src/util
# so that it is better matched with where the source file is build (even though the
# source file is generated in the associated compile tree instead of the source tree).
# The rose_paths.[hC] data is referenced by both ROSETTA and ROSE (so it is located in src/util).
rose_paths.C:
	cd $(top_builddir); $(MAKE) src/util/rose_paths.C

noinst_LTLIBRARIES = libroseutil.la

# libroseutil_la_SOURCES = processSupport.C processSupport.h
libroseutil_la_SOURCES =			\
	Color.C					\
	Combinatorics.C				\
	compilationFileDatabase.C		\
	FileSystem.C				\
	LinearCongruentialGenerator.C		\
	processSupport.C			\
	rose_getline.C				\
	rose_strtoull.C
nodist_libroseutil_la_SOURCES = rose_paths.C
libroseutil_la_LIBADD = \
	stringSupport/libRoseStringSupport.la \
	commandlineProcessing/libRoseCommandlineUtil.la

# DQ (4/5/2009): I don't think this needs to be specified using this Automake technique.
# The use of this technique for force rose_paths.C to be built was also what was causing
# large parts of ROSE to be rebuilt when configure was return (this appears to be fixed now).
# BUILT_SOURCES = rose_paths.C

# DQ (4/5/2009): Added rose_paths.C to clean rule, instead of to the DISTCLEANFILES
# DISTCLEANFILES = rose_paths.C

pkginclude_HEADERS =				\
	Color.h					\
	Combinatorics.h				\
	compilationFileDatabase.h		\
	FileSystem.h				\
	FormatRestorer.h			\
	GraphUtility.h				\
	LinearCongruentialGenerator.h		\
	Map.h					\
	ParallelSort.h				\
	processSupport.h			\
	rose_getline.h				\
	rose_override.h				\
	rose_paths.h				\
	rose_strtoull.h				\
	roseTraceLib.c				\
	setup.h

EXTRA_DIST = CMakeLists.txt setup.h utilDocumentation.docs

########################################################################################################################
# rules for the Sawyer subdirectory

noinst_LTLIBRARIES += libroseSawyer.la
libroseSawyer_la_SOURCES =			\
	Sawyer/Assert.C				\
	Sawyer/Attribute.C			\
	Sawyer/CommandLine.C			\
	Sawyer/GraphTraversal.C			\
	Sawyer/Markup.C				\
	Sawyer/MarkupPod.C			\
	Sawyer/Message.C			\
	Sawyer/ProgressBar.C			\
	Sawyer/Sawyer.C				\
	Sawyer/SmallObject.C			\
	Sawyer/Stopwatch.C			\
	Sawyer/Synchronization.C		\
	Sawyer/useHeaders.C
nobase_pkginclude_HEADERS   =			\
	Sawyer/Access.h				\
	Sawyer/AddressMap.h			\
	Sawyer/AddressSegment.h			\
	Sawyer/AllocatingBuffer.h		\
	Sawyer/Assert.h				\
	Sawyer/Attribute.h			\
	Sawyer/BiMap.h				\
	Sawyer/BitVector.h			\
	Sawyer/BitVectorSupport.h		\
	Sawyer/Buffer.h				\
	Sawyer/Cached.h				\
	Sawyer/Callbacks.h			\
	Sawyer/CommandLine.h			\
	Sawyer/DefaultAllocator.h		\
	Sawyer/DistinctList.h			\
	Sawyer/Graph.h				\
	Sawyer/GraphBoost.h			\
	Sawyer/GraphTraversal.h			\
	Sawyer/IndexedList.h			\
	Sawyer/Interval.h			\
	Sawyer/IntervalMap.h			\
	Sawyer/IntervalSet.h			\
	Sawyer/IntervalSetMap.h			\
	Sawyer/Map.h				\
	Sawyer/MappedBuffer.h			\
	Sawyer/Markup.h				\
	Sawyer/MarkupPod.h			\
	Sawyer/Message.h			\
	Sawyer/NullBuffer.h			\
	Sawyer/Optional.h			\
	Sawyer/PoolAllocator.h			\
	Sawyer/ProgressBar.h			\
	Sawyer/Sawyer.h				\
	Sawyer/Set.h				\
	Sawyer/SharedPointer.h			\
	Sawyer/SmallObject.h			\
	Sawyer/Stack.h				\
	Sawyer/StaticBuffer.h			\
	Sawyer/Stopwatch.h			\
	Sawyer/Synchronization.h		\
	Sawyer/WarningsOff.h			\
	Sawyer/WarningsRestore.h
EXTRA_DIST += Sawyer/CMakeLists.txt

########################################################################################################################
# automake boilerplate

# DQ (4/5/2009): Added rose_paths.C to clean rule, instead of to the DISTCLEANFILES
clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files rose_paths.C

distclean-local:
	rm -rf Templates.DB
