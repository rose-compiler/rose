include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

###########################################################################
# These are Fortran testcodes targeting the Rice CAF2 language extensions #
###########################################################################

# #######################
#      PASSING TESTS
# #######################

CAF_TESTCODES_REQUIRED_TO_PASS =

CAF_TESTCODES_REQUIRING_PREPROCESSOR_REQUIRED_TO_PASS =




# #######################
#      FAILING TESTS
# #######################

CAF_TESTCODES_CURRENTLY_FAILING =




noinst_headers =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
CAF_TESTCODES = \
$(CAF_TESTCODES_REQUIRED_TO_PASS)

CAF_TESTCODES_REQUIRING_PREPROCESSOR = \
$(CAF_TESTCODES_REQUIRING_PREPROCESSOR_CURRENTLY_FAILING)

# QMTest allows both passing and failing tests.
ALL_CAF_TESTCODES = \
$(CAF_TESTCODES_REQUIRED_TO_PASS) \
$(CAF_TESTCODES_CURRENTLY_FAILING)

ALL_CAF_TESTCODES_REQUIRING_PREPROCESSOR = \
$(CAF_TESTCODES_REQUIRING_PREPROCESSOR_REQUIRED_TO_PASS) \
$(CAF_TESTCODES_REQUIRING_PREPROCESSOR_CURRENTLY_FAILING)

PERFORMANCE_REPORT_OPTION =

ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

# How to call a ROSE translator with a Rice CAF source file.
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
#	$(VALGRIND) ../../testParser $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)

$(TEST_REQUIRING_PREPROCESSOR_F90_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)

$(TEST_F77_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f77)
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)

$(TEST_F77_FIXED_FORMAT_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f)
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)


# Note: If using gfortran version greater then 4.1, we can't call it with Fortran 2003 code since it is so buggy.
#       So we use -rose:skip_syntax_check to avoid the frontend syntax checking and -rose:skipfinalCompileStep
#       to avoid processing the generated code (which would have the same problem).  It is easier to just use
#       the version 4.1 gfortran which has better Fortran 2003 syntax checking support. This problem is confirmed 
#       on versions of gfortran 4.2 and 4.3.
$(TEST_F03_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration 
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f03)
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
#	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
#	Since gfortran does not support F03, and thus we skip the compileation using the backend gfortran compiler, we have to force the generation of an object file. 
#	Build a object file since gfortran fails to do so (avoids remaking the makefile target)
	@echo "   ...force generation of $@, because gfortran compiler does not handle fortran 2003 standard (and the makefile needs $@ to avoid recompilation)."
	@touch $@

QMTEST_Objects = ${ALL_TESTCODES:.f90=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.c)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.f90)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# DQ (11/11/2008): Module test copied from developersScratchSpace/Dan/Fortran_tests
# This file declares a module used by module_B_file.f90
module_A_file.o: testTranslator
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)

# Force representation of module dependency so that mod files are properly generated before use.
module_B_file.o: testTranslator module_A_file.o
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)

test2010_01.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

test2010_02.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

test2010_05.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

# Output spew test from John.
outputSpew.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:caf -rose:skip_syntax_check -c $(srcdir)/$(@:.o=.caf)

# Makefile rule to reproduce a current OFP bug that causes OFP to fail silently (not returning a proper exit code).
current_OFP_error:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:exit_after_parser -c $(srcdir)/test2010_31.f90



# We will build a larger collection of module tests soon (supported by makefile dependence rules).
PASSING_MODULE_TEST_Objects = module_B_file.o

# DQ (10/4/2010): Test calling OFP directly and ROSE for different filename extensions.
CALL_OFP = java -Djava.class.path=$(top_srcdir)/src/3rdPartyLibraries/antlr-jars/antlr-3.5.2-complete.jar:$(top_builddir)/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-0.8.5.jar fortran.ofp.FrontEnd
testSuffixRules:
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_01.f
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_01.f
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_02.f77
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_02.f77
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_03.f90
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_03.f90
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_04.f95
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_04.f95
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_05.f03
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_05.f03
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_06.F
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_06.F
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_07.F90
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_07.F90
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_08.F95
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_08.F95
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_09.F03
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_09.F03
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_10.F08
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_10.F08
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_11.f08
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_11.f08

testOFP:
	 $(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_01.f


bug3:
	../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:output_parser_actions -I$(srcdir) -c $(srcdir)/test2010_81.f90


# DQ (2/2/2011): We are now enforcing the module names are unique so that we can always run the 
# test code in parallel and avoid name conflicts with *.mod (gfortran) and *.rmod (rose) files.
enforce_module_name_policy:
	$(top_srcdir)/scripts/policies/UniqueFortranModules.pl $(srcdir)


tests:
	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(PASSING_MODULE_TEST_Objects)
	@$(MAKE) testCPP_Defines
	@$(MAKE) testMPItypes
	@$(MAKE) testMultipleFortranFiles
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Fortran_tests: make tests rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

check-local:
	@echo "Tests for Fortran."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These Fortran programs must be called externally to the test codes in the "TESTCODES" make variable
if !ROSE_USING_GFORTRAN_VERSION_4_0
# DQ (2/2/2011): This test presently fails (test codes are being fixed).
	@$(MAKE) enforce_module_name_policy
	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(PASSING_MODULE_TEST_Objects)
	@$(MAKE) testCPP_Defines
	@$(MAKE) testMPItypes
	@$(MAKE) testMultipleFortranFiles
#	DQ (4/6/2010): This fails on Hudson, but I don't know why.
#	@$(MAKE) testParser
else
	@echo "Skipping Fortran code tests when using the gfortran version 4.0.x compiler (fails to support common command line options of later versions)."
endif
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Fortran_tests: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

else

# Fortran support not available in ROSE unless specified on the configure command line.
check-local:
	@echo "***********************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Fortran_tests: FORTRAN NOT CONFIGURED IN ROSE ******"
	@echo "***********************************************************************************"
endif

# This will output the example testcodes that are failing
listProblemFiles: $(F77_TESTCODES_CURRENTLY_FAILING) $(F90_TESTCODES_CURRENTLY_FAILING) $(F03_TESTCODES_CURRENTLY_FAILING) $(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)
	egrep -n "*"  $(srcdir)/$(?)


# Make sure that all the test codes end up in the distribution
# EXTRA_DIST = $(ALL_TESTCODES)
EXTRA_DIST = $(ALL_F77_FIXED_FORMAT_TESTCODES) $(ALL_F77_TESTCODES) $(ALL_F90_TESTCODES) $(ALL_F03_TESTCODES) \
   $(ALL_F90_TESTCODES_REQUIRING_PREPROCESSOR) \
   test2007_suffixTest_01.f test2007_suffixTest_02.f77 test2007_suffixTest_03.f90 test2007_suffixTest_04.f95 \
   test2007_suffixTest_05.f03 test2007_suffixTest_06.F test2007_suffixTest_07.F90 test2007_suffixTest_08.F95 \
   test2007_suffixTest_09.F03 test2007_suffixTest_10.F08 test2007_suffixTest_11.f08 \
   module_A_file.f90 module_B_file.f90 test2010_31_header.f90 test2010_50.h test2010_54.h test2010_55.h \
   original_mpif.h inputUsingDefinesOnCommandline.F90 fortran_foo_single_quote.h fortran_foo_double_quote.h \
   cpp_foo.h cpp_foobar.h ISO_C_BINDING.f03 mpi_f08_types.f03 mpi_f08_interfaces_test.f03 mpif.h mpiof.h

# DQ (3/12/2010): Fortran 2008 tests from OFP
# f08-tests

clean-local:
	rm -f *.o rose_*.f* *.rmod
	rm -f *.s *.mod rose_*.f rose_*.f77 rose_*.f90 rose_*.f95 rose_*.f03 rose_*.f08 *.dot *.pdf *~ *.ps *.out *.log rose_performance_report_lockfile.lock
	rm -f rose_*.F rose_*.F77 rose_*.F90 rose_*.F95 rose_*.F03 rose_*.F08 test2007_suffixTest_0?_postprocessed.*
	rm -f *_postprocessed.* 
	rm -rf QMTest

