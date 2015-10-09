include ../../../config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (2/8/2004): Disabled
# bin_PROGRAMS = cgraphpp

# list of dependencies for all example programs
EXAMPLE_PROGRAMS_DEPENDENCY_LIST = $(ROSE_LIBS) 

# DQ (2/8/2004): Disabled
# cgraphpp_SOURCES = ControlStructure.C  GenGeneration.C  GenRepresentation.C helpFunctions.C  main.C

# DQ (2/8/2004): Disabled
# Can't use $(ROSE_LIBS) since it would make librewrite.a be compiled 
# (and it appears after src in the SUBDIRS line).  But then we can't
# compile the cgraphpp translator.  Need tomove the AST_RewriteMechanism 
# directory into ROSE/src so that the ordering can be handled in the 
# ROSE/src/Makefile.am.  This is too much to do right now.  So it
# is reseverd for the next checkin.
# cgraphpp_DEPENDENCIES = $(EXAMPLE_PROGRAMS_DEPENDENCY_LIST)
# cgraphpp_DEPENDENCIES = 

LDADD = $(ROSE_LIBS)

## Code added to permit tests in this directory as part of development

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = 

# DQ (2/8/2004): Disabled
#   exampleTest.C

# Use of this test code requires specification of EDG include files
# dataStructureTest_1.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# ROSE_FLAGS = -rose:dot -rose:test 6 -rose:outputGrammarTreeFiles -rose:outputGrammarTreeFilesForHeaderFiles
ROSE_FLAGS = -rose:dot -rose:test 6 -rose:outputGrammarTreeFiles -rose:outputGrammarTreeFilesForHeaderFiles -rose:outputGrammarTreeFilesForEDG

# How to run Andreas' graphing preprocessor (note -gall option causes g++ warning -fgall ???)
# 1) cgraphpp --gall exampleTest.C
# 2) dotgl Static_exampleTest.C.dot &

$(TEST_Objects): $(TESTCODES)
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)
	./cgraphpp --gall exampleTest.C
	: remove this so that we can force the examples to be run
	rm -f $(TEST_Objects)

# helpFunctions.h need to be put into the util directory
pkginclude_HEADERS = \
     ControlStructure.h  GenGeneration.h  GenRepresentation.h  helpFunctions.h

EXTRA_DIST = CMakeLists.txt $(TESTCODES)

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@$(MAKE) $(TEST_Objects)

