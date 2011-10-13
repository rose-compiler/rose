include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#inputBug320.c , this is not directly related to OpenMP, removed, Liao 11/14/2007
#inputNomain.c: assume all tests are selfcontained. so skip this one

C_TESTCODES_REQUIRED_TO_PASS = \
inputBug313.c \
inputBug327.c \
inputBug329.c \
inputBug330-1.c \
inputBug330.c \
inputBug333.c \
inputBug342.c \
inputFlush.c \
inputHello_c.c \
inputJacobi.c \
inputMaster.c \
inputOmpfor.c \
inputOmpfor2.c \
inputOrphanedFor.c \
inputParallelFor.c \
inputSingle.c \
inputSection.c \
inputVariables.c \
pointer2Array.c \
pointer2Array2.c \
pointer2Array3.c \
pointer2ArrayRegular.c \
logics.c

# DQ: Need to get C++ codes into tests but this is a bit of a problem for our simple make rule
CXX_TESTCODES_REQUIRED_TO_PASS = \
ctor-1.C \
ctor-2.C \
ctor-7.C \
inputBug298.C \
inputBug308.C \
inputHello.C \
vector_dot_product.cpp

TESTCODES_REQUIRED_TO_PASS = \
$(C_TESTCODES_REQUIRED_TO_PASS)
# moved them to test code required to pass
# Liao, 11/14/2007
TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# $(CXX_TESTCODES_REQUIRED_TO_PASS)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
# ROSE_FLAGS = --edg:no_warnings -w -rose:C -rose:verbose 0 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w -rose:C -rose:verbose 0 --edg:restrict -rose:detect_dangling_pointers 1

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

OMNI_RUNTIME_LIBRARY_PATH = @omni_omp_runtime_support_path@

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}

$(TEST_Objects): ../../ompTranslator $(@:.o=.c)
	$(VALGRIND) ../../ompTranslator -D_OPENMP $(ROSE_FLAGS) -I$(srcdir)/../.. -I$(srcdir)/../../includes -c $(srcdir)/$(@:.o=.c)
QMTEST_Objects = ${ALL_TESTCODES:.c=.qmt}

if ROSE_USE_OMNI_OPENMP
#Liao, 11/14/2007, enable linking and running compiled code 
# leverage the simple tests support from automake
# Note: assume each test code is self-contained and has main() inside.
PASSING_TEST_Executables = ${TEST_Objects:.o=.out}

check_PROGRAMS = $(PASSING_TEST_Executables)
LDADD = -L$(OMNI_RUNTIME_LIBRARY_PATH) -lompc -lpthread -lm
#LDADD = -L$(OMNI_RUNTIME_LIBRARY_PATH) -lompc -ltlog -lpthread -lm
#LDADD = $(OMNI_RUNTIME_LIBRARY_PATH)/libompc.a $(OMNI_RUNTIME_LIBRARY_PATH)/libtlog.a -lpthread -lm
TESTS = $(check_PROGRAMS)
else  # otherwise just generate object files
check-local:
	@echo "Test for ROSE OpenMP Translator."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/OpenMP_Translator/tests/developmentTests: make check rule complete (terminated normally) ******"
	@echo "****** configure option --with-omni_omp_runtime_support can be used to specify Omni runtime library path           ******"
	@echo "****** for example: --with-omni_omp_runtime_support=/home/liao6/opt/omni-1.6/lib/openmp/lib                        ******"
	@echo "****** then make check will not only compile the tests, but also run them in multithreads                     ******"
	@echo "***********************************************************************************************************"

endif 


# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../ompTranslator $(@:.qmt=.c)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh o rose.RoseTest $(CURRENT_DIRECTORY)/../../ompTranslator NULL $(ROSE_FLAGS) -I$(srcdir)/../.. -c $(srcdir)/$(@:.qmt=.c)

# This is a rule to add the current files into a QMTest database
build_qmtest_database: $(QMTEST_Objects)
#	@echo "Building QMTest database using file "
	rm -rf QMTest;
	mkdir QMTest;
	rm -rf QMTest/*.qmt
	mv *.qmt QMTest
	cd QMTest; qmtest -D`pwd` create-tdb

run_qm_tests:
	cd QMTest; qmtest run -o TestResults.qmr -j4

run_qm_gui:
	cd QMTest; qmtest gui

EXTRA_DIST = $(ALL_TESTCODES)

clean-local:
	rm -f *.o rose_*.[cC] runme test.log *.out
	rm -rf QMTest

