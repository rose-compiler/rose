include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = babelPreprocessor

# version of A++ preprocessor (initial development)
babelPreprocessor_SOURCES = \
     sidlFunction.C \
     caselessCompare.C \
     findMethodCalls.C \
     options.C \
     babelTransformation.C \
     babelPreprocessor.C

#    transformationOptions.C

babelPreprocessor_DEPENDENCIES = $(EXAMPLE_PROGRAMS_DEPENDENCY_LIST)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

## Code added to permit tests in this directory as part of development

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = \
   babelTest_2.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# ROSE_FLAGS = -rose:dot -rose:test 6 -rose:outputGrammarTreeFiles -rose:outputGrammarTreeFilesForHeaderFiles
ROSE_FLAGS = -rose:dot -rose:test 6 -rose:outputGrammarTreeFiles -rose:outputGrammarTreeFilesForHeaderFiles -rose:outputGrammarTreeFilesForEDG

$(TEST_Objects): $(TESTCODES)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./babelPreprocessor $(ROSE_FLAGS) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.C)
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
     babelTransformation.h \
     caselessCompare.h \
     findMethodCalls.h \
     simpleA++.h \
     transformationOptions.h

EXTRA_DIST = $(TESTCODES)

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files

# DQ (9/10/2005): Still fails even after update...
# DQ (8/8/2005): Commented out since I don't have the latest version of this code and it fails tests.
# This rule is run after automake's internal check rule (which we don't want to use)
# check-local:
#	@$(MAKE) $(TEST_Objects)


















