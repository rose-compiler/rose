include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = librose_partial.la
librose_partial_la_SOURCES = \
	Convert.C Diagnostics.C Escape.C NumberToString.C Predicate.C SplitJoin.C StringToNumber.C

# Headers are installed by parent makefiles
