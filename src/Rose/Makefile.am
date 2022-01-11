# This header file is only for things in the ::Rose namespace. If
# you're adding something that's not in ::Rose you're doing it wrong!

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc

# Namespaces within ::Rose (see top comment). Every namespace will
# have a directory listed here, and every directory here is a
# namespace.
SUBDIRS =					\
	BinaryAnalysis				\
	Color					\
	CommandLine				\
	Diagnostics				\
	FileSystem				\
	StringUtility

# .C files for the ::Rose namespace (see top comment)
AM_CPPFLAGS = $(ROSE_INCLUDES)
noinst_LTLIBRARIES = librose_partial.la
librose_partial_la_SOURCES =			\
	FormattedTable.C			\
	Initialize.C				\
	Progress.C				\
	SourceLocation.C

# Public header files are installed from parent makefiles.
