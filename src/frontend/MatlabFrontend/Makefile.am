include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

#include $(srcdir)/sage_support/Makefile_variables

# DQ (5/2/2009): This supports the optimization of the new 
# Graph IR nodes (makes about 25% improvement, I think).
# ROSE_OPTIMIZATION_LEVEL = -O3
ROSE_OPTIMIZATION_LEVEL =
# ROSE_VERBOSE = -rose:verbose 2


# Bison flags
AM_YFLAGS = -d -v
# Flex flags
AM_LFLAGS =

CXXFLAGS += $(ROSE_OPTIMIZATION_LEVEL) $(ROSE_VERBOSE) -I$(srcdir)/parser_support -I$(srcdir)

lib_LTLIBRARIES = libroseMatlabFrontend.la
#noinst_LTLIBRARIES = libroseMatlabFrontend.la

#BUILT_SOURCES =  matlab_parse.h

INCLUDES = $(ROSE_INCLUDES)
#librosematlabFrontend_la_CXXFLAGS = -I$(srcdir)/parser_support -I$(srcdir)
libroseMatlabFrontend_la_DEPENDENCIES =
libroseMatlabFrontend_la_SOURCES = \
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
# DQ (9/29/2009): Renamed preproc.ll to preproc-c.ll in SVN.
# This will hopefull avoid a linking problem where multiple files
# are called preproc.o (from here and from EDG).
#EXTRA_DIST = CMakeLists.txt matlab_lex.ll matlab_parse.yy

#DISTCLEANFILES = preproc-c.cc ompparser.cc ompparser.h omplexer.cc

# matlab_parse.h: matlab_parse.cc
# 	if test -f matlab_parse.hh; then $(LN_S) -f matlab_parse.hh matlab_parse.h; fi;

# clean-local:
# 	rm -rf Templates.DB ii_files ti_files cxx_templates
# 	rm -rf Cxx_Grammar* AST_FILE_IO.C StorageClasses.C
# 	rm -rf AST_FILE_IO.h StorageClasses.h AstQueryMemoryPool.h
# 	rm -rf generated_abstractcppgrammar.atg
# 	rm -f sage3basic.h.gch
# 	rm -f *.binary

# distclean-local:
# 	rm -rf Templates.DB
# 	rm -f sage3basic.h.gch

# removeHashLineDirectivesFromGeneratedCode:
# 	cp Cxx_Grammar.h  Cxx_Grammar.h.withLineDirectives
# 	grep -v "#line" Cxx_Grammar.h > Cxx_Grammar.h.withoutLineDirectives
# 	cp Cxx_Grammar.h.withoutLineDirectives Cxx_Grammar.h












