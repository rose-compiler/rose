include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

####################################################################################
# These are ROSE translators and test the compilation ROSE applications using ROSE #
####################################################################################


# Files that represent header files from ROSE that we can now compilie with ROSE.
TESTCODES_REQUIRED_TO_PASS = \
testRoseHeaders_00.C \
testRoseHeaders_01.C \
testRoseHeaders_02.C \
testRoseHeaders_03.C \
testRoseHeaders_04.C \
testRoseHeaders_05.C \
testRoseHeaders_06.C \
testRoseHeaders_07.C \
testRoseHeaders_08.C

EXTRA_TESTCODES_REQUIRED_TO_PASS = \
     test2003_33.C \
     test2003_34.C \
     test2004_46.C \
     test2006_43.C 

TESTCODE_CURRENTLY_FAILING = \
     $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = --edg:no_warnings -w --edg:restrict 

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
PASSING_TEST_Sources = ${TESTCODES:.C=.C.passed}
TEST_Objects = ${ALL_TESTCODES:.C=.o}
TEST_Sources = ${ALL_TESTCODES:.C=.C.passed}

TEST_TRANSLATOR = ../../testTranslator

# DQ (2/10/2013): Make all of the tests codes
$(TEST_TRANSLATOR):
	cd ../..; $(MAKE) test

# $(TEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C)
#	../../testTranslator $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.o=.C)

# RPM (2014-04-07) Increased timeout from 15 minutes to 45 minutes because this was failing when lots of debugging is enabled.
#                  Yes, it really does take nearly 45 minutes (38 actually) to run when when ROSE is compiled with
#                  "-D_GLIBCXX_DEBUG -O0" with g++ 4.4.5 on x86_64-linux-gnu, Debian Squeeze, 12 cores at 3.33GHz, 48GB memory.
# RPM (2016-02-29) Added MEMORY=20GB so that the test is skipped (passes) on machines that don't have this much memory.
$(TEST_Sources): %.passed: $(TEST_TRANSLATOR) $(srcdir)/%
	@$(RTH_RUN)														\
		TIMEOUT=45m													\
		MEMORY=20GB													\
		CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.C.passed=.C)"	\
		$(top_srcdir)/scripts/test_exit_status $@

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.qmt=.C)

# DQ (9/13/2005): Test the compilation of ROSE using ROSE!
# This is just a test of compiling the Sage III IR (the largest file; most but not all of Sage III)
$(top_builddir)/src/frontend/SageIII/Cxx_Grammar.o: $(TEST_TRANSLATOR)
	../../testTranslator -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

$(top_builddir)/src/frontend/SageIII/Cxx_Grammar.qmt: $(TEST_TRANSLATOR)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc


# ****************************************************************************************************
#     Test codes or selected ROSE code that can compile using ROSE or is the subject of current work 
# ****************************************************************************************************

../../testGraphGeneration:
	$(MAKE) testGraphGeneration -C $(top_builddir)/tests

# This tests the long ROSE command line of ROSE compiling ROSE (the file is empty).
test0: $(srcdir)/testRoseHeaders_00.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_00.C

# This tests compiling: sage3basic.h (note that the command line generated for the backend compile step is currupted.
test1: $(srcdir)/testRoseHeaders_01.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_01.C

# This tests compiling: sage3basic.h and sage3.h
test2: $(srcdir)/testRoseHeaders_02.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_02.C

# This tests compiling: rose.h
test3: $(srcdir)/testRoseHeaders_03.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_03.C

# This tests compiling: rose.h
test3_debug: $(srcdir)/testRoseHeaders_03.C $(TEST_TRANSLATOR)
	libtool gdb --args ../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_03.C

# Same compilation but with optimization (which only effects the g++ compilation performance)
test3_O2: $(srcdir)/testRoseHeaders_03.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -O2 -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_03.C

# Same compilation but with optimization (which only effects the g++ compilation performance)
test3_O3: $(srcdir)/testRoseHeaders_03.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -O3 -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_03.C

# This tests compiling: Cxx_Grammar.C: fails "ROSE_ASSERT (kind == iek_statement);" in edgRose.C:20798
test4: $(srcdir)/testRoseHeaders_04.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_04.C

test5: $(srcdir)/testRoseHeaders_05.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_05.C

test6: $(srcdir)/testRoseHeaders_06.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_06.C

test7: $(srcdir)/testRoseHeaders_07.C $(TEST_TRANSLATOR)
	../../testTranslator $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_07.C

test8: $(srcdir)/testRoseHeaders_08.C $(TEST_TRANSLATOR)
#	../../testGraphGeneration -rose:verbose 2 -isystem /nfs/casc/overture/ROSE/opt/rhel5/x86_64/boost/1_45_0/gcc/4.4.5/include -c $(srcdir)/testRoseHeaders_08.C
	../../testTranslator -rose:verbose 2 $(ROSE_BOOST_PREINCLUDE_PATH) -c $(srcdir)/testRoseHeaders_08.C

buildCompiledFile_3:
	g++ -E -DUSE_ROSE -D_GLIBCXX__PTHREADS=1 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_03.C > $(srcdir)/testRoseHeaders_03_combined.C

buildCompiledFile_4:
	g++ -E -DUSE_ROSE -D_GLIBCXX__PTHREADS=1 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_04.C > $(srcdir)/testRoseHeaders_04_combined.C

buildCompiledFile_5:
	g++ -E -DUSE_ROSE -D_GLIBCXX__PTHREADS=1 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_05.C > $(srcdir)/testRoseHeaders_05_combined.C

buildCompiledFile_6:
	g++ -E -DUSE_ROSE -D_GLIBCXX__PTHREADS=1 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_06.C > $(srcdir)/testRoseHeaders_06_combined.C

deltaDebug_3:
	delta -in_place -suffix=.C -test=/home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/deltaDebuggingScript_3 /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/test3.C

deltaDebug_4:
	delta -in_place -suffix=.C -test=/home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/deltaDebuggingScript_4 /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/test4.C

deltaDebug_5:
	delta -in_place -suffix=.C -test=/home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/deltaDebuggingScript_5 /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/test5.C

deltaDebug_6:
	delta -in_place -suffix=.C -test=/home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/deltaDebuggingScript_6 /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/RoseExample_tests/test6.C

HPCRUN    = ~dquinlan/ROSE/HPCToolKit_2/HPCToolkit/hpctoolkit-install/bin/hpcrun
HPCSTRUCT = ~dquinlan/ROSE/HPCToolKit_2/HPCToolkit/hpctoolkit-install/bin/hpcstruct
HPCPROF   = ~dquinlan/ROSE/HPCToolKit_2/HPCToolkit/hpctoolkit-install/bin/hpcprof

hpctoolkit-test2: ../../testGraphGeneration
	ls -l $(HPCRUN)
	export LD_LIBRARY_PATH=/home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.2.4-dq-edg4x-rc-nodebug-optimized-enable-assembly-semantics/src:${LD_LIBRARY_PATH}
	echo ${LD_LIBRARY_PATH}
	$(HPCRUN) -e REALTIME -o MEASURE_ASSERT ../../.libs/testGraphGeneration $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_02.C
	$(HPCSTRUCT) ../../.libs/testGraphGeneration
	$(HPCPROF) -S testGraphGeneration.hpcstruct -o database_assert MEASURE_ASSERT

hpctoolkit-postprocess:

# MAKE SURE YOU HAVE SET HPCTOOLKIT_HOME=/path/to/hpctoolkit
#Some useful events we want to gather
#These events may not work on all platforms. We picked it on a machine where it worked.
HPCTOOLKIT__PAPI_EVENTS=PAPI_TOT_CYC@2000000 PAPI_L1_DCM@50000 PAPI_L2_DCM@50000 PAPI_L3_DCM@5000 PAPI_L1_ICM@50000 PAPI_L2_ICM@50000 PAPI_L3_ICM@5000 PAPI_L1_TCM@50000 PAPI_L2_TCM@50000 PAPI_L3_TCM@5000 PAPI_TLB_DM@10000 PAPI_TLB_IM@10000 PAPI_BR_MSP@50000 PAPI_RES_STL@100000 PAPI_MEM_WCY@100000 PAPI_MEM_RCY@100000
hpctoolkit-test3: ../../testGraphGeneration
	test ! -d "$${HPCTOOLKIT_HOME}" && \
	( \
			echo "[FATAL] Environment variable \$$HPCTOOLKIT_HOME must be set to your HPCToolkit installation directory." && \
			exit 1 \
	) || exit 0;
	rm -rf  database_* measure_*
	for papi_event in $$($(top_srcdir)/tests/utilities/hpctoolkit-get_list_of_papi_events.sh); do \
			echo "$(HPCTOOLKIT__PAPI_EVENTS)" | grep "$${papi_event}"; \
			if test $$? -eq 0; then                     \
			$(HPCTOOLKIT_HOME)/bin/hpcrun               \
					-e $${papi_event}                       \
					-o measure_$${papi_event}               \
					../../.libs/testGraphGeneration         \
							$(ROSE_FLAGS)                       \
							-rose:skipfinalCompileStep          \
							-rose:verbose 2                     \
							-I$(top_builddir)                   \
							$(ROSE_INCLUDES)                    \
							-c $(srcdir)/testRoseHeaders_03.C;  \
			else                                        \
					echo "[INFO] Skipping unsupported PAPI event: '$${papi_event}'"; \
			fi                                                                   \
	done
	$(HPCTOOLKIT_HOME)/bin/hpcstruct  ../../.libs/testGraphGeneration
	$(HPCTOOLKIT_HOME)/bin/hpcprof -S testGraphGeneration.hpcstruct -o database_papi_testRoseHeaders_03 measure_*

callcount-test3:
	rm -f rose_call_count_test_roseRoseHeaders_03.out
	${VALGRIND_HOME}/bin/valgrind --tool=callgrind --callgrind-out-file=rose_call_count_test_roseRoseHeaders_03.out ../../.libs/testGraphGeneration $(ROSE_FLAGS) -rose:skipfinalCompileStep -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_03.C
	@echo "*******************************************************************************************************************************"
	@echo "****** : Call counts generated via VALGRIND. Open via kcachegrind GUI (usually present with your OS) for visualization ********"
	@echo "*******************************************************************************************************************************"





# Exclude the Cxx_Grammar.C file from being part of the distribution
EXTRA_DIST = $(TESTCODES)

check-local:
	@echo "Tests of ROSE example translators (compiling ROSE using ROSE)."
#	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(PASSING_TEST_Sources)
	@echo "*******************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/RoseExample_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot
	rm -rf QMTest
	rm -rf measure_* database_* *.hpcstruct
	rm -f *.passed *.failed
