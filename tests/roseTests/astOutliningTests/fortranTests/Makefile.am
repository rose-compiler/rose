# Liao, 12/28/2007, test outliner for Fortran
# based on the example: tests/CompileTests/Fortran_tests/Makefile.am
#-----------------------------------
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#srcdir = workingRose/src/midend/astOutlining/tests
#builddir = buildWorkingRose/tests/roseTests/astOutliningTests

F77_TESTCODES_REQUIRED_TO_PASS = \
assignment.f \
assignmentmain2.f \
assignmentmain.f \
dimension.f \
doloop.f \
format.f \
function2.f \
function.f \
hello.f \
integer.f \
lengthlimit.f \
loan.f \
parameter2.f \
parameter.f \
ugoto.f \
write.f

F77_TESTCODES_CURRENTLY_FAILING =
# arithmeticIf.f, bug 166 refer to labels 
# doloop2.f , bug 178, shared continue of two loops
# print.f, bug 170
# read.f, need label symbol for format field
# read2.f , bug 179, openfortran parser's problem, won't fix
# real8.f bug 168, won't fix
# format2.f, bug 180, not my problem, test it again when Dan's ready
# power.f, END SUBROUTINE problem

F90_TESTCODES_REQUIRED_TO_PASS = \
blockdo.f90 \
contains.f90 \
doloop_nested.f90 \
doloop3.f90 \
doloop4.f90 \
intent.f90 \
moduleParameter.f90 \
stop.f90 \
stop1.f90 \
subroutine.f90 

F90_TESTCODES_CURRENTLY_FAILING = 
# contains1.f90, subprogram in module not supported
# contains.f90, bug 182, contains is missing
# doloop5.f90 bug 183, lack of end for do loops
# module3.f90, contains in module not supported
# module.f90, similar to the above
# moduleSubprogram.f90 , similar to above
# stop2.f90, comments are attached before a hidden function definition node??
# useModule1.f90 , USE not supported
# module1.f90, supluous END

F03_TESTCODES_REQUIRED_TO_PASS = 

F03_TESTCODES_CURRENTLY_FAILING =

noinst_headers =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS)

F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS)

F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS) \
$(F90_TESTCODES_CURRENTLY_FAILING)

ALL_F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS) \
$(F77_TESTCODES_CURRENTLY_FAILING)

ALL_F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS) \
$(F03_TESTCODES_CURRENTLY_FAILING)

# will conflict with implict rules and cause circular dependency!
#ALL_TESTCODES = \
#$(ALL_F77_TESTCODES) \
#$(ALL_F90_TESTCODES) \
#$(ALL_F03_TESTCODES)

ROSE_FLAGS = -rose:verbose 0 -rose:outline:seq 7 -ffixed-line-length-none 
# for default pragma-controlled targets
ROSE_FLAGS2 = -rose:verbose 0 -ffixed-line-length-none 
#-rose:outline:parameter_wrapper
#ROSE_F77_FLAGS = -rose:outline:seq 7 -rose:f77 -ffixed-line-length-none
#ROSE_F90_FLAGS = -rose:outline:seq 7 -rose:f90
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes

VALGRIND =

#TEST_F77_Objects = ${F77_TESTCODES:.f77=.o}
TEST_F77_Objects = ${F77_TESTCODES:.f=.o}
TEST_F90_Objects = ${F90_TESTCODES:.f90=.o}
TEST_F03_Objects = ${F03_TESTCODES:.f03=.o}

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
#test for both selection-based and pragma-based targets
$(TEST_F77_Objects): ../outlineSelection  ../outline
	$(VALGRIND) ../outlineSelection $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.f)
	$(VALGRIND) ../outline $(ROSE_FLAGS2) -c $(srcdir)/$(@:.o=.f)

#$(TEST_F90_Objects): ../outlineSelection $(F90_TESTCODES_REQUIRED_TO_PASS) 
$(TEST_F90_Objects): ../outlineSelection $(ALL_TESTCODES) ../outline
	$(VALGRIND) ../outlineSelection $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.f90)
	$(VALGRIND) ../outline $(ROSE_FLAGS2) -c $(srcdir)/$(@:.o=.f90)

$(TEST_F03_Objects): ../outlineSelection $(ALL_TESTCODES) ../outline
	$(VALGRIND) ../outlineSelection $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.f03)
	$(VALGRIND) ../outline $(ROSE_FLAGS2) -c $(srcdir)/$(@:.o=.f03)

../outlineSelection:
	cd ..; $(MAKE) outlineSelection
../outline:
	cd ..; $(MAKE) outline

PASSING_TEST_Objects = \
$(TEST_F77_Objects) \
$(TEST_F90_Objects) \
$(TEST_F03_Objects)

# DQ (8/29/2008): I have commented this out since it failed when 
# running "make -j<n> check" in the parent directory for n > 1.
# QMTEST_Objects = ${ALL_TESTCODES:.f90=.qmt}
# Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd` $(QMTEST_Objects): ../outlineSelection $(srcdir)/$(@:.qmt=.c)
#	@echo "Calling QMTEST_Objects rule: "
#	$(top_srcdir)/scripts/qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../outlineSelection NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.f90)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

if ROSE_USE_SSL_SUPPORT
# DQ and Liao (4/20/2009): When using SSL library we can't use Fortran support in ROSE
# (this is disabled in the frontend, but we also have to skip these tests).
check-local: ../outline ../outlineSelection
	@echo "Disabled tests for outlining Fortran when using libssl."
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astOutliningTests/fortranTests check rule disabled (can't run this test whenusing libssl) ******"
	@echo "***************************************************************************************************"
else
check-local: ../outline ../outlineSelection
	@echo "Tests for outlining Fortran."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astOutliningTests/fortranTests check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"
endif

else

# Fortran support not available in ROSE unless specified on the configure command line.
check-local:
	@echo "***********************************************************************************"
	@echo "****** ROSE/tests/roseTests/astOutliningTests/fortranTests: FORTRAN NOT CONFIGURED IN ROSE ******"
	@echo "***********************************************************************************"
endif

# This will output the example testcodes that are failing
listProblemFiles: $(F77_TESTCODES_CURRENTLY_FAILING) $(F90_TESTCODES_CURRENTLY_FAILING) $(F03_TESTCODES_CURRENTLY_FAILING) 
	egrep -n "*"  $(srcdir)/$(?)

EXTRA_DIST = $(ALL_F90_TESTCODES) \
             $(ALL_F77_TESTCODES) \
             $(ALL_F03_TESTCODES)


clean-local:
	rm -f rose_* *.o *.dot *.out *.mod core* *.s *.dot *.pdf *.log *.ps *~ *.rmod *_copy_graph_old_nodes
	rm -rf QMTest

