include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (6/8/2013): Use command line option to alternatively test the experimental fortran frontend.
ROSE_FLAGS = -rose:verbose 0 -rose:experimental_fortran_frontend -I$(srcdir)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# DQ (3/8/2014): Adding OFP test code
# bin_PROGRAMS = traverse
# traverse_SOURCES = traverse.cpp

# ATERM_ROOT = /home/dquinlan/bin/aterm-2.5-install-64bit
ATERM_ROOT = $(ATERM_INSTALL_PATH)
ATERM_INCLUDE = -I$(ATERM_ROOT)/include
AM_CPPFLAGS = -I$(top_srcdir)/src/3rdPartyLibraries/experimental-fortran-parser/rose_traverse $(ROSE_INCLUDES) $(ATERM_INCLUDE)
# LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -L../../../../src/3rdPartyLibraries/experimental-fortran-parser/rose_traverse -lofp
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -L../../../../src/3rdPartyLibraries/experimental-fortran-parser/aterm_traversal -lexperimentalRoseFortran

# TEST_F90_Objects = main_program_2008_01.f90 main_program_2014_01.f90
# F90_TESTCODES = main_program_2008_01.f90 main_program_2014_01.f90
F90_TESTCODES = test2014_02.f90

TEST_F90_Objects = ${F90_TESTCODES:.f90=.o}
TEST_F90_Sources = ${F90_TESTCODES:.f90=.f90.passed}

PASSING_TEST_Objects = $(TEST_F90_Objects)

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../../testTranslator:
	cd ../../..; $(MAKE) testTranslator

testTranslator: ../../../testTranslator

SGLRI_WITH_PATH = /home/dquinlan/bin/strategoxt-0.17-install-32bit/bin/sglri

# DQ (9/14/2014): This forces the relinking of the testTranslator, which is a work 
# around until the OFP and ATerm support is included in ROSE using shared libraries.
recompile_translator:
	rm ../../../testTranslator
	$(MAKE) ../../../testTranslator

$(TEST_F90_Objects): recompile_translator ../../../testTranslator
#	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#	sglri -i $(srcdir)/junk.f90 -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl | implode-asfix | pp-aterm -o junk.f90.aterm
#	sglri -i $(srcdir)/$(@:.o=.f90) -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/syntax-v0.14/Fortran.tbl -o $(@:.o=.f90.aterm)
#	$(SGLRI_WITH_PATH) -i $(srcdir)/$(@:.o=.f90) -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/sdf_syntax/Fortran.tbl -o $(@:.o=.f90.aterm)
#	./traverse -i $(@:.o=.f90.aterm)
#	../../../testTranslator -i $(@:.o=.f90.aterm)
	../../../testTranslator -rose:experimental_fortran_frontend -c $(srcdir)/$(@:.o=.f90)
endif

junk.ast-info.pp: $(srcdir)/junk.f90 $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl
#	sglri -i $(srcdir)/junk.f90 -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl | addPosInfo -i junk.pt -p $(srcdir)/junk.f90 -m | implode-asfix | pp-aterm -o junk.ast-info.pp
	sglr -i $(srcdir)/junk.f90 -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl | addPosInfo -p $(srcdir)/junk.f90 -m | implode-asfix | pp-aterm -o junk.ast-info.pp

junk : $(srcdir)/junk.f90 $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl
	sglri -i $(srcdir)/junk.f90 -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/Fortran.tbl | implode-asfix | pp-aterm -o junk.f90.aterm

test_ofp: traverse
#	sglri -i $(srcdir)/main_program_2008_01.f90 -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/syntax-v0.14/FortranFixed.tbl -o main_program_2008_01.f90.aterm
	sglri -i $(srcdir)/main_program_2008_01.f90 -p $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser/syntax-v0.14/Fortran.tbl -o main_program_2008_01.f90.aterm
	./traverse -i main_program_2008_01.f90.aterm

clean-local:
	rm -f rose_*.* *.aterm *.o

check-local:
	@echo "Tests for experimental Fortran frontend."
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects)
#	@$(MAKE) test_ofp
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests/experimental_frontend_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************************"
else
	@echo "*******************************************************************************************"
	@echo "***** Skipping tests of experimental Fortran frontend (must be configured explicitly) *****"
	@echo "*******************************************************************************************"
endif

