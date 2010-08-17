include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I$(top_builddir) $(ROSE_INCLUDES)

# BIN = correctAllComments correctExternalComments
# TEST = testParser01 testParser02 testParser03 testParser06 testParser08 testParser09 testParser10 testParser11 
# LIB = commentObj.o doxygenComment.o doxygenGrammar.o sageDoxygen.o

bin_PROGRAMS = correctAllComments correctExternalComments \
	       correctExternalCommentsAndCompile classifyDeprecated createDocumentationForROSE docLint
bin_SCRIPTS  = readDoxyfile correctExternalCommentsAndCompileQuietly

noinst_LIBRARIES = libROSEDoxygen.a
libROSEDoxygen_a_SOURCES = commentObj.C doxygenComment.C doxygenGrammar.C sageDoxygen.C

correctAllComments_SOURCES                = correctAllComments.C
correctExternalComments_SOURCES           = correctExternalComments.C
correctExternalCommentsAndCompile_SOURCES = correctExternalCommentsAndCompile.C
classifyDeprecated_SOURCES                = classifyDeprecated.C
createDocumentationForROSE_SOURCES        = createDocumentationForROSE.C
docLint_SOURCES                           = docLint.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) libROSEDoxygen.a

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

doxygenComment.C: doxygenComment.lex
#	-Pdx changes generated symbols to use "dx" intead of "yy", this avoids conflicts within ROSE.
#	lex -Pdx -o$@ $<
#	$(LEX) -Pdx -o$(srcdir)/doxygenComment.C $(srcdir)/doxygenComment.lex
	$(LEX) -Pdx -odoxygenComment.C $(srcdir)/doxygenComment.lex

# This tests the generation of the Doxygen *.docs file separate from the sorce code
test1: correctExternalComments $(srcdir)/testAutoDocumentation_01.C $(srcdir)/testAutoDocumentation_01.docsz
	cp $(srcdir)/testAutoDocumentation_01.docsz testAutoDocumentation_01.docs
	./correctExternalComments $(srcdir)/testAutoDocumentation_01.C testAutoDocumentation_01.docs

# This tests the updating of any Doxygen comments into the generated source code.
test2: correctAllComments $(srcdir)/testAutoDocumentation_02.C
	./correctAllComments $(srcdir)/testAutoDocumentation_02.C

# OPTIONS = -doxygen:createNewFiles -doxygen:classNameTemplate %s.docs -doxygen:functionNameTemplate global.docs -doxygen:variableNameTemplate global.docs 
OPTIONS = -doxygen:createNewFiles -doxygen:classNameTemplate $(PWD)/docs/%s.docs -doxygen:functionNameTemplate $(PWD)/docs/global.docs -doxygen:variableNameTemplate $(PWD)/docs/global.docs 

# PC (8/21/2006): This test takes a long time (up to 12 minutes on a 2.8GHz Intel Xeon)
# and thus is not performed as part of the normal run
# For some reason this test also generates a 'limits' file and 2 headers, which
# confuse the build process if rebuilt, so we remove them.
# docs/SgNode.docs: $(top_srcdir)/docs/testDoxygen_copy/SgNode.docs
docs/SgNode.docs: $(top_srcdir)/docs/testDoxygen/SgNode.docs
	@echo "Build a directory to put a copy of all the Doxygen input files"
	mkdir -p docs
	@echo "Remove all existing files from the docs directory"
	rm -f docs/*
	@echo "Copy the Doxygen specific input files to a new location (so avoid possible overwrites)"
#	cp -a $(top_srcdir)/docs/testDoxygen_copy/Sg*.docs docs
	cp -a $(top_srcdir)/docs/testDoxygen/*.docs docs

sage_doxygen_documentation.h: prepareDocFiles docs/SgNode.docs
	@echo "Modify the *.docs files (to add an IR node so that all comments will be attached to the AST) and build sage_doxygen_documentation.h"
	$(srcdir)/prepareDocFiles docs

testHeaderFilesROSE: docs/SgNode.docs createDocumentationForROSE sage_doxygen_documentation.h $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	$(VALGRIND) $(top_builddir)/tests/testTranslator -H --edg:no_warnings   -c $(INCLUDES) $(srcdir)/sage_doxygen_documentation.C $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

testDocsROSE: docs/SgNode.docs createDocumentationForROSE sage_doxygen_documentation.h $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	$(VALGRIND) ./createDocumentationForROSE --edg:no_warnings   -rose:collectAllCommentsAndDirectives -rose:includeCommentsAndDirectives SageIII -rose:includeCommentsAndDirectives docs $(OPTIONS) $(INCLUDES) -I. -Idocs $(srcdir)/sage_doxygen_documentation.C $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	@echo "Due to possible bug in ROSE, a few header files are generated ... remove them"
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h
	@echo "Overwrite a copy of the test files in $(top_srcdir)/docs/testDoxygen_copy"
#	Moved this to the location used for the construction of all ROSE documentation.
#	mv -f *.docs $(top_srcdir)/docs/testDoxygen_copy
	rm -f $(top_srcdir)/src/frontend/SageIII/docs/*.docs
	mv -f *.docs $(top_srcdir)/src/frontend/SageIII/docs

testDocsROSE_small: docs/SgNode.docs createDocumentationForROSE sage_doxygen_documentation.h $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.h
	cd $(top_builddir)/src/frontend/SageIII; $(MAKE) removeHashLineDirectivesFromGeneratedCode
	$(VALGRIND) ./createDocumentationForROSE -rose:verbose 0 --edg:no_warnings -rose:collectAllCommentsAndDirectives -rose:includeCommentsAndDirectives SageIII -rose:includeCommentsAndDirectives docs $(OPTIONS) $(INCLUDES) -I. -Idocs $(srcdir)/sage_doxygen_documentation.C $(srcdir)/roseLite.C
	@echo "Due to possible bug in ROSE, a few header files are generated ... remove them"
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h
	@echo "Overwrite a copy of the test files in $(top_srcdir)/docs/testDoxygen_copy"
	rm -f $(top_srcdir)/src/frontend/SageIII/docs/*.docs
	mv -f *.docs $(top_srcdir)/src/frontend/SageIII/docs


testSage: docs/SgNode.docs correctExternalComments sage_doxygen_documentation.h $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	$(VALGRIND) ./correctExternalComments  --edg:no_warnings   $(OPTIONS) $(INCLUDES) -I. -Idocs $(srcdir)/sage_doxygen_documentation.C $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	@echo "Due to possible bug in ROSE, a few header files are generated ... remove them"
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h
	@echo "Overwrite a copy of the test files in $(top_srcdir)/docs/testDoxygen_copy"
	mv -f *.docs $(top_srcdir)/docs/testDoxygen_copy

testClassifyDeprecated: docs/SgNode.docs sage_doxygen_documentation.h classifyDeprecated $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	$(VALGRIND) ./classifyDeprecated  --edg:no_warnings   $(OPTIONS) $(INCLUDES) -I. -Idocs $(srcdir)/sage_doxygen_documentation.C $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	@echo "Due to possible bug in ROSE, a few header files are generated ... remove them"
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h
	@echo "Overwrite a copy of the test files in $(top_srcdir)/docs/testDoxygen_copy"
	mv -f *.docs $(top_srcdir)/docs/testDoxygen_copy

testDocLint: docs/SgNode.docs sage_doxygen_documentation.h docLint $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	$(VALGRIND) ./docLint  --edg:no_warnings   $(OPTIONS) $(INCLUDES) -I. -Idocs $(srcdir)/sage_doxygen_documentation.C $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	@echo "Due to possible bug in ROSE, a few header files are generated ... remove them"
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h

testSageOnly: correctExternalComments $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	./correctExternalComments $(OPTIONS) $(INCLUDES) $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h

testSageSmall: correctExternalComments sage_doxygen_documentation.h $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	./correctExternalComments $(OPTIONS) -I. $(INCLUDES) $(srcdir)/sage_doxygen_documentation.C $(srcdir)/small.C
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h

testSageSmaller: correctExternalComments sage_doxygen_documentation.h $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	./correctExternalComments $(OPTIONS) $(INCLUDES) $(srcdir)/sage_doxygen_documentation.C $(srcdir)/smaller.C
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h

testClassifyDeprecatedSmall: docs/SgNode.docs sage_doxygen_documentation.h classifyDeprecated $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
	$(VALGRIND) ./classifyDeprecated  --edg:no_warnings   $(OPTIONS) $(INCLUDES) -I. -Idocs $(srcdir)/sage_doxygen_documentation.C $(srcdir)/small.C
	@echo "Due to possible bug in ROSE, a few header files are generated ... remove them"
	-rm -f limits basic_file.h locale_facets.h stl_threads.h  stl_tree.h
	@echo "Overwrite a copy of the test files in $(top_srcdir)/docs/testDoxygen_copy"
	mv -f *.docs $(top_srcdir)/docs/testDoxygen_copy

EXTRA_DIST = doxygenComment.lex testAutoDocumentation_01.C \
     testAutoDocumentation_02.C testAutoDocumentation_01.docsz $(bin_SCRIPTS) \
     testParser01.C testParser02.C testParser03.C testParser06.C testParser08.C \
     testParser09.C testParser10.C testParser11.C testParser12.C

include_HEADERS = \
     commentObj.h \
     doxygenComment.h \
     sageDoxygen.h \
     doxygenGrammar.h

check-local:
	@$(MAKE) test1
	@$(MAKE) test2
	@echo "********************************************************************************************"
	@echo "*** ROSE/projects/DocumentationGenerator: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C doxygenComment.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files
	rm -f *.docs sage_doxygen_documentation.h
	rm -rf docs

