include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# the code which can not yet be parallelized now
# Or the needed analyses(def-use, liveness, or dependence analysis) cannot handle them yet
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

# This list is now copied to C_TESTCODES_REQUIRED_TO_VERIFY, Liao 3/9/2016
# the code which can be parallelized or confirmed as not parallelizable now
#null_for_test.c  TODO Liao 6/9/2016
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
ifAndFor.c \
indirectIndex.c \
jacobi_seq.c \
lastprivate.c \
livenessTest.c \
matrixmultiply.c \
matrixmultiply2.c \
matrixVectormultiply.c \
minusMinus.c \
outer_only.c \
output_dep.c \
output_dep2.c \
output_dep3.c \
plusAssign.c \
plusplus.c \
pointers.c \
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
bitRightShift.c

# These are different depending upon use of EDG 4.7 and EDG 4.8 vs. EDG 4.9.
# This is because translation of floating poitn is more precise using EDG 4.9
# (examples such as "0." are not normalized to "0.0").
#  ROSE_USE_EDG_VERSION_4_9
if ROSE_USE_EDG_VERSION_4_7
C_TESTCODES_REQUIRED_TO_PASS += \
slowInput_47.c \
Stress-1_47.c
else
if ROSE_USE_EDG_VERSION_4_8
C_TESTCODES_REQUIRED_TO_PASS += \
slowInput_47.c \
Stress-1_47.c
else
C_TESTCODES_REQUIRED_TO_PASS += \
slowInput.c \
Stress-1.c
endif
endif

# current failing if no aliasing cannot be assumed.
# StressWork_setup.C // need to assume no pointer aliasing
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

C_TEST_Objects = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.o}
#C_TEST_Output = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.out}

CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS:.C=.o}

../autoPar:
	$(MAKE) -C ../. autoPar

# we want to use diff to compare generated files with reference files
# Pending test cases: TODO
# TODO: diff patches is a better choice.  Tolerate other differences introduced by ROSE.
C_TESTCODES_REQUIRED_TO_VERIFY = $(C_TESTCODES_REQUIRED_TO_PASS)

CXX_TESTCODES_REQUIRED_TO_VERIFY = $(CXX_TESTCODES_REQUIRED_TO_PASS)

# Old list, not in use

# TODO discard this list once all files are verified

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
# TODO not good enough, we want to depend on individual generated file here
$(C_TEST_DIFF_FILES): $(C_TEST_Generated_Files) 
	echo "Verifying autoPar translation by using diff ..."; \
	if $(DIFF) $(@:.c.diff=.c) $(REFERENCE_PATH)/$(@:.c.diff=.c) > $@ ; then echo "Test Passed" ; else echo "Files differ; test failed"; cat $@; rm -rf $@; exit 1; fi

$(CXX_TEST_DIFF_FILES): $(CXX_TEST_Generated_Files) 
	echo "Verifying autoPar translation by using diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/$(@:.C.diff=.C) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

# special file to debug dep graph information
probe.o: ../autoPar $(@:.o=.c)
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) -rose:autopar:enable_debug $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) &> $(@).out 

$(C_TEST_Objects): ../autoPar $(@:.o=.c)
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) >> $(@).out 
#$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) | fold >$(@:.o=.out) # pipeline buries return code

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

test_diff.out : ../autoPar $(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -rose:autopar:enable_diff -c $(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests/jacobi.c > test_diff.out

# Two files are singled out since their output will be put in tutorial text
doall_2.out: ../autoPar doall_2.c 
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/doall_2.c > doall_2.out
inner_only.out: ../autoPar inner_only.c 
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/inner_only.c > inner_only.out
check-local:
	@echo "Test for ROSE automatic parallelization."
	@$(MAKE) $(C_TEST_Objects)
	@$(MAKE) $(CXX_TEST_Objects)
	@$(MAKE) rose_interp1_elem.C > interp1_elem.out
	@$(MAKE) rose_doall_vector.C > doall_vector.out
	@$(MAKE) rose_doall_vector2.C > doall_vector2.out
	@$(MAKE) rose_Stress2.cc > Stress2.out
	@$(MAKE) test_diff.out
	@$(MAKE) inner_only.out
	@$(MAKE) doall_2.out
	@$(MAKE) $(C_TEST_DIFF_FILES) $(CXX_TEST_DIFF_FILES)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/autoParallelization/tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) funcs.annot floatArray.annot Index.annot simpleA++.h interp1_elem.C doall_vector.C doall_vector2.C \
	Stress2.cc clibfunc.annot SegDB.annot doall_2.c inner_only.c std_vector.annot

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.diff




