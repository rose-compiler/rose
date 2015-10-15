include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Separate Transformations and specialied support is isolated into separate directories
SUBDIRS = \
     IndexOffsetQuery \
     ArrayAssignmentStatementQuery \
     ScalarIndexingStatementQuery

AM_CPPFLAGS = $(ROSE_INCLUDES) \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/ArrayAssignmentStatementQuery \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/ScalarIndexingStatementQuery \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/IndexOffsetQuery

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = ArrayPreprocessor

# list of dependencies for all example programs
EXAMPLE_PROGRAMS_DEPENDENCY_LIST = \
     programTransformation.o \
     arrayTransformationSupport.o \
     transformationOptions.o \
     operandDataBase.o \
     arrayVariableDataBase.o \
     indexVariableDataBase.o \
     $(top_builddir)/SAGE/insert.o \
     $(top_builddir)/SAGE/stripWrapper.o \
     $(top_builddir)/SAGE/generateAST.o \
     $(top_builddir)/ExamplePreprocessors/A++P++Preprocessor/IndexOffsetQuery/libindexoffsetquery.a \
     $(top_builddir)/ExamplePreprocessors/A++P++Preprocessor/ArrayAssignmentStatementQuery/libarrayassignmentquery.a \
     $(top_builddir)/ExamplePreprocessors/A++P++Preprocessor/ScalarIndexingStatementQuery/libscalarIndexingQuery.a \
     $(top_builddir)/QueryLibs/NameQuery/libnamequery.a \
     $(top_builddir)/QueryLibs/NumberQuery/libnumberquery.a \
     $(top_builddir)/src/librose.a \
     $(EDG_LIBS) \
     $(top_builddir)/util/libroseutil.a \
     $(top_builddir)/SAGE/libsage3.a \
     $(top_builddir)/PDF/libpdf.a \
     $(top_builddir)/AST_RewriteMechanism/librewrite.a

# version of A++ preprocessor (initial development)
ArrayPreprocessor_SOURCES = \
     arrayInheritedAttributeBaseClass.C \
     arrayTransformationSupport.C \
     programTransformation.C \
     transformationOptions.C \
     operandDataBase.C \
     arrayVariableDataBase.C \
     indexVariableDataBase.C \
     ArrayPreprocessor.C

# changes to the templates in ../AST_RewriteMechanism/rewriteTreeTraversal.C should force
# several files using those templates to be recompiled.  Express this explicitly:
programTransformation.o: $(top_builddir)/AST_RewriteMechanism/rewriteTreeTraversal.o

ArrayPreprocessor_DEPENDENCIES = $(EXAMPLE_PROGRAMS_DEPENDENCY_LIST)

# Note that because rose dependes upon the sage and sage 
# depends upon rose we have to list the sage library twice.
LDADD = \
     $(ROSE_LIBS) \
     $(top_builddir)/ExamplePreprocessors/A++P++Preprocessor/ArrayAssignmentStatementQuery/libarrayassignmentquery.a \
     $(top_builddir)/ExamplePreprocessors/A++P++Preprocessor/ScalarIndexingStatementQuery/libscalarIndexingQuery.a \
     $(top_builddir)/ExamplePreprocessors/A++P++Preprocessor/IndexOffsetQuery/libindexoffsetquery.a \
     $(ROSE_LIBS)

## Code added to permit tests in this directory as part of development

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = 

#    testAxx2002_07.C

#    testNewTraversal.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# ROSE_FLAGS = -rose:dot -rose:test 6 -rose:outputGrammarTreeFiles -rose:outputGrammarTreeFilesForHeaderFiles
ROSE_FLAGS =

# This rule generates a dependency that forces the "make check" 
# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:

$(TEST_Objects): $(TESTCODES) FORCE_TEST_CODES_TO_RUN
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./ArrayPreprocessor $(ROSE_FLAGS) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.C)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)

APP_INCLUDES = -I$(top_srcdir)/TESTS/CompileTests/A++Code -I$(top_srcdir)/include
APP_TESTS    = testApp.o
$(APP_TESTS): example7
	./example7 -DHAVE_CONFIG_H $(ROSE_FLAGS) $(APP_INCLUDES) $(AM_CPPFLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

EXAMPLE7_INPUT_TEST = testApp
test_ArrayPreprocessor: all ArrayPreprocessor $(ArrayPreprocessor_DEPENDENCIES)
	rm -f $(EXAMPLE7_INPUT_TEST).o
	./ArrayPreprocessor $(ROSE_FLAGS) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(srcdir)/$(EXAMPLE7_INPUT_TEST).C
	rm -f $(EXAMPLE7_INPUT_TEST).o

test: $(TESTCODES) test_ArrayPreprocessor
#	rm -f testTransformationSpecification.o
#	./example1 $(ROSE_FLAGS) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(srcdir)/testTransformationSpecification.C
#	./example2 $(ROSE_FLAGS) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(srcdir)/testTransformationSpecification.C
#	rm -f testTransformationSpecification.o

include_HEADERS = \
     simpleA++.h \
     arrayInheritedAttributeBaseClass.h \
     operandDataBase.h \
     transformationMacro-length.h \
     transformationMacro-variable_size.h \
     programTransformation.h \
     transformationMacro-size.h \
     transformationMacros.h \
     arrayTransformationSupport.h \
     transformationMacro-stride.h \
     transformationOptions.h \
     transformationMacro-uniform_size.h \
     transformationSubscriptMacros.h \
     arrayPreprocessor.h \
     transformationOptions.h

EXTRA_DIST = $(TESTCODES)

# DISTCLEANFILES = $(TESTCODES) serial_test_script

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.int.C rose_directives_list.txt core
	rm -rf ti_files

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@$(MAKE) $(TEST_Objects)




























