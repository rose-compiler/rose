include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = \
   testQuery testQuery2 testQuery3

# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)
# ROSE_DEVELOPMENT_LIBS = $(ROSE_LIBS)

testQuery_SOURCES = testQuery.C
testQuery_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

testQuery2_SOURCES = testQuery2.C
testQuery2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

testQuery3_SOURCES = testQuery3.C
testQuery3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)


TESTCODES = \
   input1.C 

# These codes are DESIGNED to fail in the inlining
# fail1.C fail2.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = 
TEST_TRANSLATOR_1 = ./testQuery $(ROSE_FLAGS)
TEST_TRANSLATOR_2 = ./testQuery2 $(ROSE_FLAGS)
TEST_TRANSLATOR_3 = ./testQuery3 $(ROSE_FLAGS)

TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
# DQ (12/24/2008): Removed use of $(INCLUDES) since this now causes
# the -isystem option to be passed to ROSE which does not understand 
# it (processed include directory as a file).
$(TEST_Objects): $(TESTCODES) testQuery
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	$(TEST_TRANSLATOR_1) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	$(TEST_TRANSLATOR_2) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	$(TEST_TRANSLATOR_3) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	@echo "Running resulting executable ..."
#	./$(@:.o=)

EXTRA_DIST = $(TESTCODES)

check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "****************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astQueryTests: make check rule complete (terminated normally) ***"
	@echo "****************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files *.o rose_input*.C input1

distclean-local:
	rm -rf Templates.DB 

test1: test1A.C test1B.C
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	DQ(7/13/2004): This is an error since it specifies the output file twice in the vendor compile line
#	$(TEST_TRANSLATOR) -rose:verbose 0 -o test1A.o -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1B.C
	g++ -o test1 rose_test1A.o rose_test1B.o
	@echo "Running resulting executable ..."
	./test1

test2: testQuery2
	./testQuery2 $(ROSE_FLAGS) -rose:verbose 0 -c $(srcdir)/input1.C 

test3: testQuery3
	./testQuery3  -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C
