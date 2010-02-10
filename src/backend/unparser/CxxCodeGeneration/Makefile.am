include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

unparseCxxCodeGeneration_headers = unparseCxx.h unparseCxx_types.h

# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
# unparseCxxCodeGeneration_sources = unparseCxx_expressions.C unparseCxx_statements.C unparseCxx_types.C
unparseCxxCodeGeneration_sources = unparseCxx_types.C
if !ROSE_USING_ROSE_CODE_GENERATION
   unparseCxxCodeGeneration_sources += unparseCxx_expressions.C unparseCxx_statements.C
endif

include_HEADERS = $(unparseCxxCodeGeneration_headers)

noinst_LTLIBRARIES = libunparseCxx.la

libunparseCxx_la_SOURCES = $(unparseCxxCodeGeneration_sources)

clean-local:
	rm -rf ii_files ti_files core


EXTRA_DIST = CMakeLists.txt
