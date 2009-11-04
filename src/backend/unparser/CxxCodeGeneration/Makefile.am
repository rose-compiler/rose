include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

unparseCxxCodeGeneration_headers = unparseCxx.h unparseCxx_types.h

unparseCxxCodeGeneration_sources = unparseCxx_expressions.C unparseCxx_statements.C unparseCxx_types.C

include_HEADERS = $(unparseCxxCodeGeneration_headers)

noinst_LTLIBRARIES = libunparseCxx.la

libunparseCxx_la_SOURCES = $(unparseCxxCodeGeneration_sources)

clean-local:
	rm -rf ii_files ti_files core


EXTRA_DIST = CMakeLists.txt
