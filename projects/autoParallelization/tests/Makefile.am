include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# the code which can be parallelized or confirmed as not parallelizable now
C_TESTCODES_REQUIRED_TO_PASS = \
anti_dep.c \
array_scalar.c \
c99loop.c \
complexCondition.c \
doall.c \
firstprivate.c \
foo.c \
functionCall.c \
ifAndFor.c \
indirectIndex.c \
jacobi_seq.c \
lastprivate.c \
livenessTest.c \
matrixmultiply.c \
minusMinus.c \
multiReduction.c \
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
regression01.c \
scalar_anti.c \
scalar_output.c \
scalar_privatization.c \
scalar_true.c \
shared.c \
slowInput.c \
Stress-1.c \
plusPlusOp.c \
true_dep.c \
true_dep_2.c \
true_dep_scalar.c \
true_l2.c 

CXX_TESTCODES_REQUIRED_TO_PASS = \
class_member.C \
ifstmt.C \
iterator2.C \
iteratorloop.C \
test20100223.C 

# the code which can not yet be parallelized now
# Or the needed analyses(def-use, liveness, or dependence analysis) cannot handle them yet
TESTCODE_CURRENTLY_FAILING = \
iterator1.C \
iterator_const1.C \
list1.C \
pointer.c \
tricky_1.c \
tricky_2.c \
vectorize_1.c \
vector_1.C \
vector_2.C

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
# Turned of the debug flag to have a cleaner screen output, Liao, 9/16/2009
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict -rose:autopar:unique_indirect_index #-rose:autopar:enable_debug
ROSE_CFLAGS = -rose:C99 --edg:no_warnings -w -rose:verbose 0 --edg:restrict -rose:autopar:unique_indirect_index -rose:autopar:enable_patch #-rose:autopar:enable_debug

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

C_TEST_Objects = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.o}
#C_TEST_Output = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.out}

CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS:.C=.o}

../autoPar:
	$(MAKE) -C ../. autoPar


$(C_TEST_Objects): ../autoPar $(@:.o=.c)
	$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) >> log.out 
#$(VALGRIND) ../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.c) | fold >$(@:.o=.out) # pipeline buries return code

$(CXX_TEST_Objects): ../autoPar $(@:.o=.C)
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) >> log.out
rose_interp1_elem.C: ../autoPar interp1_elem.C
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/floatArray.annot -annot $(srcdir)/funcs.annot -annot $(srcdir)/Index.annot $(srcdir)/interp1_elem.C	
rose_doall_vector.C: ../autoPar doall_vector.C
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/std_vector.annot $(srcdir)/doall_vector.C

rose_doall_vector2.C: ../autoPar doall_vector2.C
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/std_vector.annot $(srcdir)/doall_vector2.C

rose_Stress2.cc: ../autoPar Stress2.cc
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c -annot $(srcdir)/clibfunc.annot $(srcdir)/Stress2.cc

test_diff.out : ../autoPar $(top_srcdir)/tests/CompileTests/OpenMP_tests/jacobi.c
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -rose:autopar:enable_diff -c $(top_srcdir)/tests/CompileTests/OpenMP_tests/jacobi.c > test_diff.out

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
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/autoParallelization/tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) funcs.annot floatArray.annot Index.annot simpleA++.h interp1_elem.C doall_vector.C doall_vector2.C \
	Stress2.cc clibfunc.annot SegDB.annot doall_2.c inner_only.c std_vector.annot

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch

	


