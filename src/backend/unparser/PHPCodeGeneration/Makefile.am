include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

unparsePHPCodeGeneration_headers = unparsePHP.h

unparsePHPCodeGeneration_sources = unparsePHP.C

include_HEADERS = $(unparsePHPCodeGeneration_headers)

noinst_LTLIBRARIES = libunparsePHP.la

libunparsePHP_la_SOURCES = $(unparsePHPCodeGeneration_sources)

clean-local:
	rm -rf ii_files ti_files core


EXTRA_DIST = CMakeLists.txt
