include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# the code which can be parallelized or confirmed as not parallelizable now
C_TESTCODES_REQUIRED_TO_PASS = \
doall.c 

CXX_TESTCODES_REQUIRED_TO_PASS = 

# the code which can not yet be parallelized now
TESTCODE_CURRENTLY_FAILING = \
anti_dep.c \
doall_2.c \
firstprivate.c \
inner_only.c \
jacobi_seq.c \
matrixmultiply.c \
output_dep2.c \
output_dep3.c \
output_dep.c \
pointer.c \
private.c \
reduction.c \
shared.c \
true_dep_2.c \
true_dep.c \
true_l2.c \
vectorize_1.c \
doall_vector.C \
vector_1.C \
vector_2.C

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w -rose:C -rose:verbose 0 --edg:restrict

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

C_TEST_Objects = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.o}
CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS:.C=.o}

$(C_TEST_Objects): ../autoPar $(@:.o=.c)
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.c)
$(CXX_TEST_Objects): ../autoPar $(@:.o=.C)
	$(VALGRIND) ../autoPar $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

../autoPar:../autoPar.C
	cd .. 
	make
check-local:
	@echo "Test for ROSE automatic parallelization."
	@$(MAKE) $(C_TEST_Objects)
	@$(MAKE) $(CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/autoParallelization/tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES)

clean-local:
	rm -f *.o rose_*.[cC] 

	


