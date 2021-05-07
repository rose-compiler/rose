# This header file is only for things in the ::Rose namespace. If you're
# adding something that's not in ::Rose you're doing it wrong!

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Namespaces within ::Rose (see top comment)
SUBDIRS =					\
	BinaryAnalysis				\
	Color					\
	FileSystem				\
	StringUtility

# .C files for the ::Rose namespace (see top comment)
AM_CPPFLAGS = $(ROSE_INCLUDES)
noinst_LTLIBRARIES = librose_partial.la
librose_partial_la_SOURCES =			\
	Progress.C

# Header files are installed from parent makefiles.
