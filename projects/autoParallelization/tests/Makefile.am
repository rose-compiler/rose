include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# -----------------------------------------------------------------
# the code which can not yet be parallelized now
# Or the needed analyses(def-use, liveness, or dependence analysis) cannot handle them yet.
# -----------------------------------------------------------------
TESTCODE_CURRENTLY_FAILING = \
multiReduction.c \
iterator1.C \
iterator_const1.C \
list1.C \
pointer.c \
tricky_1.c \
tricky_2.c \
vectorize_1.c \
vector_1.C \
vector_2.C
# bitRightShift.c # << operator is not supported, skipping the entire loop.
# null_for_test.c  TODO Liao 6/9/2016
# We also put C++ file list here
# StressWork_setup.C // need to assume no pointer aliasing
# 
# Liao, 2/5/202, new list of failing tests due to qing's latest push
# pointers.c

# -----------------------------------------------------------------
# This list is now copied to C_TESTCODES_REQUIRED_TO_VERIFY, Liao 3/9/2016
# the code which can be parallelized or confirmed as not parallelizable now
# -----------------------------------------------------------------
C_TESTCODES_REQUIRED_TO_PASS = \
accumulateForce.c \
anti_dep.c \
array_scalar.c \
break1.c \
break2.c \
c99loop.c \
coefficientSubscript.c \
complexCondition.c \
dep_distance.c \
doall.c \
firstprivate.c \
firstprivate2.c \
firstprivate3.c \
foo.c \
for-i-j.c \
functionCall.c \
globalArrays.c \
ifAndFor.c \
indirectIndex.c \
jacobi_seq.c \
lastprivate.c \
livenessTest.c \
matrixmultiply.c \
matrixmultiply2.c \
matrixVectormultiply.c \
minusMinus.c \
uniformIndirectIndexedArrayRefs.c \
outer_only.c \
output_dep.c \
output_dep2.c \
output_dep3.c \
plusAssign.c \
plusplus.c \
private.c \
reduction.c \
reduction_2.c \
reduction_fake.c \
reduction_max.c \
regression01.c \
scalar_anti.c \
scalar_output.c \
scalar_privatization.c \
scalar_true.c \
shared.c \
struct.c \
plusPlusOp.c \
true_dep.c \
true_dep_2.c \
true_dep_scalar.c \
true_l2.c \
bitRightShift.c \
slowInput.c \
Stress-1.c

#------------------------------------------------------------------------------
# a list of c tests for default use of autoPar: without passing any special commond options
#
#------------------------------------------------------------------------------
# rose_v1_input.c
C_TESTCODE_TO_PASS_DEFAULT_RESULTS = $(addprefix rose_v1_, ${C_TESTCODES_REQUIRED_TO_PASS})
# rose_v1_input.c.diff
C_TESTCODE_TO_PASS_DEFAULT_RESULTS_DIFF=$(C_TESTCODE_TO_PASS_DEFAULT_RESULTS:.c=.c.diff)

#------------------------------------------------------------------------------
# current failing if no aliasing cannot be assumed.
#------------------------------------------------------------------------------
CXX_TESTCODES_REQUIRED_TO_PASS = \
class_member.C \
ifstmt.C \
indirectIndex_transfered.C \
iterator2.C \
iteratorloop.C \
newOperator.C \
test20100223.C \
Hydro_applyVelocityBC.C \
Hydro_max_min.C \
pointer_dereference.C \
DelVolBaseLoopAlgorithm_loopExecute.C \
ofstream.C \
StressAccOptimizedV1_loopExecute.C \
pointer_noaliasing.C

# current failing if no aliasing cannot be assumed.
# name qualification of constructor is wrongly unparsed for 4.2 and 4.4 testing
if ROSE_USING_GCC_VERSION_LATER_4_8
CXX_TESTCODES_REQUIRED_TO_PASS += \
Hydro_constructor_1.C

endif

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

#------------------------------------------------------------------------------
# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

# Turned of the debug flag to have a cleaner screen output, Liao, 9/16/2009
#-rose:autopar:enable_distance
#-rose:autopar:enable_debug 
ROSE_COMMON_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict -rose:autopar:unique_indirect_index -rose:autopar:enable_patch -rose:autopar:no_aliasing
# For C input codes

ROSE_CFLAGS = $(ROSE_COMMON_FLAGS) -rose:C99 
# for C++ input codes
ROSE_CXXFLAGS = $(ROSE_COMMON_FLAGS)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =
#------------------------------------------------------------------------------

C_TEST_Objects = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.o}
#C_TEST_Output = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.out}

CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS:.C=.o}

../autoPar:
	$(MAKE) -C ../. autoPar

#------------------------------------------------------------------------------
# rules for testing the default use of autoPar 
# rose_v1_input.C
$(C_TESTCODE_TO_PASS_DEFAULT_RESULTS): ../autoPar
	$(VALGRIND) ../autoPar -c $(srcdir)/$(@:rose_v1_%=%) -rose:output $@

# we want to use diff to compare generated files with reference files
# Pending test cases: TODO
# TODO: diff patches is a better choice.  Tolerate other differences introduced by ROSE.
C_TESTCODES_REQUIRED_TO_VERIFY = $(C_TESTCODES_REQUIRED_TO_PASS)

CXX_TESTCODES_REQUIRED_TO_VERIFY = $(CXX_TESTCODES_REQUIRED_TO_PASS)


# when all semantics flags are turned on
# rose_input.c, 
C_TEST_Generated_Files = $(addprefix rose_,${C_TESTCODES_REQUIRED_TO_VERIFY})
CXX_TEST_Generated_Files = $(addprefix rose_,${CXX_TESTCODES_REQUIRED_TO_VERIFY})

DIFF=diff
REFERENCE_PATH=$(top_srcdir)/projects/autoParallelization/tests/references

C_TEST_DIFF_FILES=$(C_TEST_Generated_Files:.c=.c.diff)
CXX_TEST_DIFF_FILES=$(CXX_TEST_Generated_Files:.C=.C.diff)

$(CXX_TEST_Generated_Files):$(CXX_TEST_Objects)

$(C_TEST_Generated_Files):../autoPar 
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:rose_%=%) >> log.out

# object files means rose_ *.c files also exist
$(C_TEST_DIFF_FILES): %.c.diff:%.c
	echo "Verifying autoPar translation by using diff ..."; \
	if $(DIFF) $(@:.c.diff=.c) $(REFERENCE_PATH)/$(@:.c.diff=.c) > $@ ; then echo "Full Option C Test Passed" ; else echo "Files differ; test failed"; cat $@; rm -rf $@; exit 1; fi

$(CXX_TEST_DIFF_FILES): %.C.diff:%.C
	echo "Verifying autoPar translation by using diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/$(@:.C.diff=.C) > $@ ; then echo "Full Option C++ Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

# diff of default tests
# rose_v1_input.c.diff from rose_v1_input.c
$(C_TESTCODE_TO_PASS_DEFAULT_RESULTS_DIFF): %.c.diff:%.c
	echo "Verifying autoPar's default use by diff ..."; \
	if $(DIFF) $(@:.c.diff=.c) $(REFERENCE_PATH)/$(@:.c.diff=.c) > $@ ; then echo "Default option C Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

# a shortcut to test default use
check-default: $(C_TESTCODE_TO_PASS_DEFAULT_RESULTS_DIFF)

# special file to debug dep graph information
probe.o: ../autoPar $(@:.o=.c)
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) -rose:autopar:enable_debug $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) &> $(@).out 

$(C_TEST_Objects): ../autoPar $(@:.o=.c)
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) >> $(@).out 
#$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) | fold >$(@:.o=.out) # pipeline buries return code

#--------------------- C++ test with annotations
$(CXX_TEST_Objects): ../autoPar $(@:.o=.C)
	$(VALGRIND) ../autoPar $(ROSE_CXXFLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/../annot/tinyHydro.annot $(srcdir)/$(@:.o=.C) >> $(@).out

rose_interp1_elem.C: ../autoPar interp1_elem.C
	$(VALGRIND) ../autoPar $(ROSE_CXXFLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/floatArray.annot -annot $(srcdir)/funcs.annot -annot $(srcdir)/Index.annot $(srcdir)/interp1_elem.C	
rose_doall_vector.C: ../autoPar doall_vector.C
	$(VALGRIND) ../autoPar $(ROSE_CXXFLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/std_vector.annot $(srcdir)/doall_vector.C

rose_doall_vector2.C: ../autoPar doall_vector2.C
	$(VALGRIND) ../autoPar $(ROSE_CXXFLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/std_vector.annot $(srcdir)/doall_vector2.C

rose_Stress2.cc: ../autoPar Stress2.cc
	$(VALGRIND) ../autoPar $(ROSE_CXXFLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/clibfunc.annot $(srcdir)/Stress2.cc

#--------------------- test enable_diff
test_diff.out : ../autoPar $(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -rose:autopar:enable_diff -c $(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c > test_diff.out

# Two files are singled out since their output will be put in tutorial text
doall_2.out: ../autoPar doall_2.c 
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -enable_verbose -c $(srcdir)/doall_2.c > doall_2.out

# special flags, no aliasing flag is used        
inner_only.out: ../autoPar inner_only.c 
	$(VALGRIND) ../autoPar --edg:no_warnings -w -enable_verbose -rose:verbose 0 $(TESTCODE_INCLUDES) -c $(srcdir)/inner_only.c > inner_only.out

rose_inner_only.c.diff: inner_only.out
	echo "Verifying autoPar translation by using diff ..."; \
	if $(DIFF) $(@:.c.diff=.c) $(REFERENCE_PATH)/$(@:.c.diff=.c) > $@ ; then echo "Test Passed" ; else echo "Files differ; test failed"; cat $@; rm -rf $@; exit 1; fi

check-local:
	@echo "Test for ROSE automatic parallelization."
	@$(MAKE) $(C_TEST_Objects)
	@$(MAKE) $(CXX_TEST_Objects)
	@$(MAKE) rose_interp1_elem.C 
	@$(MAKE) rose_doall_vector.C 
	@$(MAKE) rose_doall_vector2.C 
	@$(MAKE) rose_Stress2.cc 
	@$(MAKE) test_diff.out
	@$(MAKE) rose_inner_only.c.diff
	@$(MAKE) doall_2.out
	@$(MAKE) $(C_TEST_DIFF_FILES) $(CXX_TEST_DIFF_FILES) $(C_TESTCODE_TO_PASS_DEFAULT_RESULTS_DIFF)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/autoParallelization/tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) funcs.annot floatArray.annot Index.annot simpleA++.h interp1_elem.C doall_vector.C doall_vector2.C \
	Stress2.cc clibfunc.annot SegDB.annot doall_2.c inner_only.c std_vector.annot

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.diff

