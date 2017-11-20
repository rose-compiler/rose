include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

BUILT_SOURCES = \
  parser_support/comment-list.cc \
  parser_support/error.cc \
  parser_support/input.cc \
  parser_support/ourtoken.cc \
  parser_support/base-list.h \
  parser_support/comment-list.h \
  parser_support/input.h \
  parser_support/lex.h \
  parser_support/octave.gperf.h \
  parser_support/ourtoken.h


#include $(srcdir)/sage_support/Makefile_variables

ROSE_OPTIMIZATION_LEVEL =
# ROSE_VERBOSE = -rose:verbose 2


# Bison flags
AM_YFLAGS = -d -v
# Flex flags
AM_LFLAGS =

CXXFLAGS += $(ROSE_OPTIMIZATION_LEVEL) $(ROSE_VERBOSE) -I$(srcdir)/parser_support -I$(srcdir) -I. -I./parser_support

lib_LTLIBRARIES = libexperimentalRoseMatlab.la

AM_CPPFLAGS = $(ROSE_INCLUDES)
libexperimentalRoseMatlab_la_DEPENDENCIES =
libexperimentalRoseMatlab_la_SOURCES = \
	MatlabParser.cc \
	matlab_parse.yy \
	matlab_lex.ll \
	parser_support/comment-list.cc \
	parser_support/error.cc \
	parser_support/input.cc \
	parser_support/ourtoken.cc \
	parser_support/StatementList.cc \
	parser_support/SymbolFinder.cc \
	parser_support/MatlabFunctionBuilder.cc

pkginclude_HEADERS = MatlabParser.h

modified_octave_parser_files.lo:
	tar zxf $(OCTAVE_PARSER_INSTALL_TARFILE)
	touch modified_octave_parser_files.lo

matlab_parse.yy: modified_octave_parser_files.lo
matlab_lex.ll: modified_octave_parser_files.lo
parser_support/comment-list.cc: modified_octave_parser_files.lo
parser_support/error.cc: modified_octave_parser_files.lo
parser_support/input.cc: modified_octave_parser_files.lo
parser_support/ourtoken.cc: modified_octave_parser_files.lo
parser_support/base-list.h: modified_octave_parser_files.lo
parser_support/comment-list.h: modified_octave_parser_files.lo
parser_support/input.h: modified_octave_parser_files.lo
parser_support/lex.h: modified_octave_parser_files.lo
parser_support/octave.gperf.h: modified_octave_parser_files.lo
parser_support/ourtoken.h: modified_octave_parser_files.lo

noinst_HEADERS = \
  parser_support/MatlabFunctionBuilder.h \
  parser_support/StatementList.h \
  parser_support/SymbolFinder.h \
  parser_support/base-list.h \
  parser_support/comment-list.h \
  parser_support/input.h \
  parser_support/lex.h \
  parser_support/octave.gperf.h \
  parser_support/ourtoken.h


clean-local:
	rm -f *.la matlab_* version
	rm -rf parser_support
