include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (9/27/2010): These directories are from gfortran tests.
SUBDIRS = 

# #######################
#      PASSING TESTS
# #######################


F90_TESTCODES_REQUIRED_TO_PASS = $(OCT_1_2010_F90_TESTCODES_REQUIRED_TO_PASS)

OCT_1_2010_F90_TESTCODES_REQUIRED_TO_PASS = \
   actual.f90 \
   allocate.f90 \
   bergervoet2.f90 \
   complex_1.f90 \
   contained_1.f90 \
   contained_2.f90 \
   contained_3.f90 \
   contained_4.f90 \
   contained_5.f90 \
   convert.f90 \
   defined_type_2.f90 \
   dummyfn.f90 \
   empty.f90 \
   empty_interface_1.f90 \
   emptyif-1.f90 \
   emptyif.f90 \
   fnresvar.f90 \
   gen_interf.f90 \
   implicit_2.f90 \
   inline_1.f90 \
   inquiry_1.f90 \
   io_end.f90 \
   mloc.f90 \
   module_common.f90 \
   module_expr.f90 \
   module_proc.f90 \
   module_result.f90 \
   name_clash.f90 \
   named_args.f90 \
   named_args_2.f90 \
   nested.f90 \
   parameter_1.f90 \
   parameter_3.f90 \
   pr26806.f90 \
   pr32417.f90 \
   pr41654.f90 \
   pr45634.f90 \
   shape_reshape.f90 \
   strparm_1.f90 \
   transfer-1.f90 \
   write.f90


# Oct 1 2010 tests that were supposed to pass (maybe a step backwards in robustness):
# Failed in OFP
#  ambig.f90
#  pr40421.f90 


# Oct 1 2010 tests that were supposed to pass (maybe a step backwards in robustness):
# Failing in gfortran syntax check:
#  20080806-1.f90 


# Oct 1 2010 tests that were supposed to pass (maybe a step backwards in robustness):
# Failing in ROSE:
#  defined_type_1.f90
#  defined_type_3.f90
#  implicit.f90
#  implicit_1.f90
#  logical-1.f90
#  pr30147.f90
#  pr36078.f90


# These are the passing tests as of Sept 1st 2010.
SEPT_1_2010_F90_TESTCODES_REQUIRED_TO_PASS = \
   actual.f90 \
   allocate.f90 \
   contained_1.f90 \
   contained_2.f90 \
   contained_3.f90 \
   empty_interface_1.f90 \
   emptyif-1.f90 \
   emptyif.f90 \
   io_end.f90 \
   mloc.f90 \
   module_expr.f90 \
   module_proc.f90 \
   name_clash.f90 \
   named_args.f90 \
   nested.f90 \
   parameter_1.f90 \
   parameter_3.f90 \
   shape_reshape.f90 \
   transfer-1.f90 \
   write.f90


OCT_1_2010_F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS = \
   pr32583.f

# This is supposed to be an empty list.
SEPT_1_2010_F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS = 


F03_TESTCODES_REQUIRED_TO_PASS = 


# F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS += $(OPENMP_TESTS_F77_TESTCODES_REQUIRED_TO_PASS)


# #######################
#      FAILING TESTS
# #######################

F90_TESTCODES_CURRENTLY_FAILING = $(OCT_1_2010_F90_TESTCODES_CURRENTLY_FAILING)

# This is a complete list of the failing tests...
OCT_1_2010_F90_TESTCODES_CURRENTLY_FAILING = \
   arrayio.f90 \
   data_1.f90 \
   do_1.f90 \
   enum_1.f90 \
   forall-1.f90 \
   parameter_2.f90 \
   pr33276.f90 \
   pr40413.f90 \
   pr42781.f90 \
   pr45598.f90 \
   vrp_1.f90

# This is an incomplete list (just the failing subset of those that were supposed to pass)
SEPT_1_2010_F90_TESTCODES_CURRENTLY_FAILING = \
   defined_type_1.f90 \
   defined_type_3.f90 \
   implicit.f90 \
   pr36078.f90

F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING = $(OCT_1_2010_F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)

# This is marked as a failing test.
OCT_1_2010_F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING = \
   noncontinuation_1.f \
   pr32663.f \
   pr39937.f \
   pr40421.f 

SEPT_1_2010_F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING = \
   noncontinuation_1.f 


F03_TESTCODES_CURRENTLY_FAILING = 


noinst_headers =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS)

F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS)

F77_FIXED_FORMAT_TESTCODES = \
$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS)

F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS) \
$(F90_TESTCODES_CURRENTLY_FAILING)

ALL_F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS) \
$(F77_TESTCODES_CURRENTLY_FAILING)

ALL_F77_FIXED_FORMAT_TESTCODES = \
$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS) \
$(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)

ALL_F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS) \
$(F03_TESTCODES_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/FORTRAN_ROSE_PERFORMANCE_DATA.csv

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict $(PERFORMANCE_REPORT_OPTION)
ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = -rose:verbose 0 -rose:output_parser_actions

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_F90_Objects = ${F90_TESTCODES:.f90=.o}
PASSING_TEST_F77_Objects = ${F77_TESTCODES:.f77=.o}

# DQ (3/11/2010): F77 tests were not being run, this fixes this.
# PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_TESTCODES:.f=.o}
PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS:.f=.o}

PASSING_TEST_F03_Objects = ${F03_TESTCODES:.f03=.o}
TEST_F90_Objects = ${ALL_F90_TESTCODES:.f90=.o}
TEST_F77_Objects = ${ALL_F77_TESTCODES:.f77=.o}
TEST_F77_FIXED_FORMAT_Objects = ${ALL_F77_FIXED_FORMAT_TESTCODES:.f=.o}
TEST_F03_Objects = ${ALL_F03_TESTCODES:.f03=.o}

PASSING_TEST_Objects = $(PASSING_TEST_F90_Objects) $(PASSING_TEST_F77_Objects) $(PASSING_TEST_F77_FIXED_FORMAT_Objects) $(PASSING_TEST_F03_Objects) 

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

testTranslator: ../../testTranslator

$(TEST_F90_Objects): ../../../../testTranslator ${ALL_TESTCODES}
	$(VALGRIND) ../../../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
#	$(VALGRIND) ../../../../testParser $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)

$(TEST_F77_Objects): ../../../../testTranslator ${ALL_TESTCODES}
	$(VALGRIND) ../../../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
#	$(VALGRIND) ../../../../testParser $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)

$(TEST_F77_FIXED_FORMAT_Objects): ../../../../testTranslator ${ALL_TESTCODES}
	$(VALGRIND) ../../../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
#	$(VALGRIND) ../../../../testParser $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)

# Note: If using gfortran version greater then 4.1, we can't call it with Fortran 2003 code since it is so buggy.
#       So we use -rose:skip_syntax_check to avoid the frontend syntax checking and -rose:skipfinalCompileStep
#       to avoid processing the generated code (which would have the same problem).  It is easier to just use
#       the version 4.1 gfortran which has better Fortran 2003 syntax checking support. This problem is confirmed 
#       on versions of gfortran 4.2 and 4.3.
$(TEST_F03_Objects): ../../../../testTranslator
	$(VALGRIND) ../../../../testTranslator $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
#	Since gfortran does not support F03, and thus we skip the compileation using the backend gfortran compiler, we have to force the generation of an object file. 
#	Build a object file since gfortran fails to do so (avoids remaking the makefile target)
	@echo "   ...force generation of $@, because gfortran compiler does not handle fortran 2003 standard (and the makefile needs $@ to avoid recompilation)."
	@touch $@




tests:
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests: make tests rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

check-local:
	@echo "Tests for Fortran."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These Fortran programs must be called externally to the test codes in the "TESTCODES" make variable
if !ROSE_USING_GFORTRAN_VERSION_4_0
	@$(MAKE) $(PASSING_TEST_Objects)
#	DQ (4/6/2010): This fails on Hudson, but I don't know why.
#	@$(MAKE) testParser
else
	@echo "Skipping Fortran code tests when using the gfortran version 4.0.x compiler (fails to support common command line options of later versions)."
endif
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

else

# Fortran support not available in ROSE unless specified on the configure command line.
check-local:
	@echo "***********************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests: FORTRAN NOT CONFIGURED IN ROSE ******"
	@echo "***********************************************************************************"
endif

# This will output the example testcodes that are failing
listProblemFiles: $(F77_TESTCODES_CURRENTLY_FAILING) $(F90_TESTCODES_CURRENTLY_FAILING) $(F03_TESTCODES_CURRENTLY_FAILING) $(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)
	egrep -n "*"  $(srcdir)/$(?)

# Make sure that all the test codes end up in the distribution
# EXTRA_DIST = $(ALL_TESTCODES)
EXTRA_DIST = $(ALL_F77_FIXED_FORMAT_TESTCODES) $(ALL_F77_TESTCODES) $(ALL_F90_TESTCODES) $(ALL_F03_TESTCODES)

clean-local:
	rm -f *.o rose_*.f* *.rmod
	rm -f *.s *.mod rose_*.f rose_*.f77 rose_*.f90 rose_*.f95 rose_*.f03 *.dot *.pdf *~ *.ps *.out *.log rose_performance_report_lockfile.lock

