include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
SUBDIRS =
EXTRA_DIST =
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS) $(filter-out %/librose.la,$(ROSE_LIBS))

noinst_LTLIBRARIES = libroseSawyer.la
libroseSawyer_la_CPPFLAGS = $(ROSE_INCLUDES) -DCOMPILING_ROSE
libroseSawyer_la_SOURCES =			\
	Assert.C				\
	Attribute.C				\
	Clexer.C				\
	CommandLine.C				\
	CommandLineBoost.C			\
	DocumentBaseMarkup.C			\
	DocumentMarkup.C			\
	DocumentPodMarkup.C			\
	DocumentTextMarkup.C			\
	GraphTraversal.C			\
	LineVector.C				\
	Message.C				\
	ProgressBar.C				\
	Sawyer.C				\
	SmallObject.C				\
	Stopwatch.C				\
	Synchronization.C			\
	useHeaders.C
nobase_pkginclude_HEADERS   =			\
	Access.h				\
	AddressMap.h				\
	AddressSegment.h			\
	AllocatingBuffer.h			\
	Assert.h				\
	Attribute.h				\
	BiMap.h					\
	BitVector.h				\
	BitVectorSupport.h			\
	Buffer.h				\
	Cached.h				\
	Callbacks.h				\
	Clexer.h				\
	CommandLine.h				\
	CommandLineBoost.h			\
	Database.h				\
	DatabasePostgresql.h			\
	DatabaseSqlite.h			\
	DefaultAllocator.h			\
	DenseIntegerSet.h			\
	DistinctList.h				\
	DocumentBaseMarkup.h			\
	DocumentMarkup.h			\
	DocumentPodMarkup.h			\
	DocumentTextMarkup.h			\
	Exception.h				\
	FileSystem.h				\
	Graph.h					\
	GraphAlgorithm.h			\
	GraphBoost.h				\
	GraphIteratorBiMap.h			\
	GraphIteratorMap.h			\
	GraphIteratorSet.h			\
	GraphTraversal.h			\
	HashMap.h				\
	IndexedList.h				\
	Interval.h				\
	IntervalMap.h				\
	IntervalSet.h				\
	IntervalSetMap.h			\
	Lexer.h					\
	LineVector.h				\
	Map.h					\
	MappedBuffer.h				\
	Message.h				\
	NullBuffer.h				\
	Optional.h				\
	PoolAllocator.h				\
	ProgressBar.h				\
	Sawyer.h				\
	Set.h					\
	SharedObject.h				\
	SharedPointer.h				\
	SmallObject.h				\
	Stack.h					\
	StackAllocator.h			\
	StaticBuffer.h				\
	Stopwatch.h				\
	Synchronization.h			\
	ThreadWorkers.h				\
	Trace.h					\
	Tracker.h				\
	Tree.h					\
	Type.h					\
	WarningsOff.h				\
	WarningsRestore.h

EXTRA_DIST += CMakeLists.txt

# These are used in the doxygen documentation examples
EXTRA_DIST += docs/examples/commandLineEx1.C docs/examples/graphIso.C

bin_PROGRAMS += stringify-enums
stringify_enums_SOURCES = stringifyEnums.C
stringify_enums_LDADD = libroseSawyer.la
