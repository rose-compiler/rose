include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

libastinliningSources = \
   inliner.C inlinerSupport.C isPotentiallyModified.C \
   replaceExpressionWithStatement.C 

# lib_LTLIBRARIES = libastinlining.a
# libastinlining_a_SOURCES  = $(libastinliningSources)

if DEV_LIBS
lib_LTLIBRARIES = libastinlining.la
else
noinst_LTLIBRARIES = libastinlining.la
endif
libastinlining_la_SOURCES = $(libastinliningSources)

include_HEADERS = replaceExpressionWithStatement.h inlinerSupport.h inliner.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core
