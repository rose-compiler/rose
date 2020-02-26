include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =

SUBDIRS = stringSupport commandlineProcessing support graphs Sawyer

# AUTOMAKE_OPTIONS = subdir-objects

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

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
	compilationFileDatabase.C		\
	FileSystem.C				\
	LinearCongruentialGenerator.C		\
	processSupport.C			\
	Progress.C				\
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
	BitFlags.h				\
	Color.h					\
	compilationFileDatabase.h		\
	FileSystem.h				\
	FormatRestorer.h			\
	GraphUtility.h				\
	LinearCongruentialGenerator.h		\
	Map.h					\
	timing.h				\
	ParallelSort.h				\
	processSupport.h			\
	Progress.h				\
	RecursionCounter.h			\
	rose_constants.h			\
	rose_getline.h				\
	rose_isnan.h				\
	rose_override.h				\
	rose_paths.h				\
	rose_strtoull.h				\
	roseTraceLib.c				\
	setup.h

EXTRA_DIST = CMakeLists.txt setup.h utilDocumentation.docs


########################################################################################################################
# automake boilerplate

# DQ (4/5/2009): Added rose_paths.C to clean rule, instead of to the DISTCLEANFILES
clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files rose_paths.C

distclean-local:
	rm -rf Templates.DB
