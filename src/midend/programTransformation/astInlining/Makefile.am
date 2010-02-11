include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
# libastinliningSources = inliner.C inlinerSupport.C isPotentiallyModified.C replaceExpressionWithStatement.C 
libastinliningSources = inliner.C inlinerSupport.C isPotentiallyModified.C
if !ROSE_USING_ROSE_CODE_GENERATION
   libastinliningSources += replaceExpressionWithStatement.C
endif

# lib_LTLIBRARIES = libastinlining.a
# libastinlining_a_SOURCES  = $(libastinliningSources)

noinst_LTLIBRARIES = libastinlining.la
libastinlining_la_SOURCES = $(libastinliningSources)

include_HEADERS = replaceExpressionWithStatement.h inlinerSupport.h inliner.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core

EXTRA_DIST = CMakeLists.txt
