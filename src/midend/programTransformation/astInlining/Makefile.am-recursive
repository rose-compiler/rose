include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

libastinliningSources = inliner.C inlinerSupport.C isPotentiallyModified.C replaceExpressionWithStatement.C 

noinst_LTLIBRARIES = libastinlining.la
libastinlining_la_SOURCES = $(libastinliningSources)

include_HEADERS = replaceExpressionWithStatement.h inlinerSupport.h inliner.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core

EXTRA_DIST = CMakeLists.txt
