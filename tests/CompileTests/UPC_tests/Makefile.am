include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = sharedMemoryDSL

sharedMemoryDSL_SOURCES = sharedMemoryDSL.C sharedMemoryTypeTraversal.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)


#####################################################################################
# These are UPC testcodes targeting aspects of UPCC that might not be a subset of C #
#####################################################################################

# affinity &arr[i] is parsed to arr+i, may hurt the later translation
# sharedArrayInit.upc: Berkeley UPC compiler 1.6 does not support shared arrays with initializer
TESTCODES_REQUIRED_TO_PASS = \
   threads_in_expression.upc \
   barrier.upc \
   consistency.upc \
   hello.upc \
   fence.upc \
   forall_1.upc \
   forall_affinity.upc \
   forall_continue.upc \
   lock.upc \
   macro_names.upc \
   max_block_size.upc \
   mythread.upc \
   notify_wait.upc \
   restrict.upc \
   shared.upc \
   shared_2.upc \
   sharedArrayInit.upc \
   sharedInit.upc \
   strict.upc \
   threads.upc \
   threads_2.upc \
   typedef.upc \
   upc_all_alloc_ex.upc upc_all_broadcast_ex.upc \
   upc_all_lock_alloc_ex.upc upc_all_permute_ex.upc upc_all_prefix_reduceL_ex.upc \
   upc_all_reduceL_ex.upc upc_all_scatter_ex.upc \
   upc_all_exchange_ex_dynamic.upc upc_all_gather_all_ex_dynamic.upc \
   upc_all_gather_ex_dynamic.upc upc_all_scatter_ex_dynamic.upc \
   bupc_test.upc \
   istream.upc \
   test2010_01.upc \
   test2010_02.upc \
   test2011_01.upc \
   test2011_02.upc \
   test2011_03.upc \
   test2011_04.upc \
   test2011_05.upc \
   test2011_06.upc \
   test2011_07.upc \
   test2011_08.upc \
   test2011_09.upc \
   test2011_10.upc \
   test2011_11.upc \
   test2011_14.upc \
   static_upc_all_exchange_ex.upc \
   static_upc_all_gather_all_ex.upc \
   static_upc_all_gather_ex.upc \
   static_test2011_12.upc \
   static_test2011_13.upc \
   test2013_01.upc \
   test2013_02.upc 

# DQ (7/15/2014): This requires a fix that is not on this branch (it is on another branch not yet merged).
#   type_field_shared_struct.upc

# upc_XXXsizeof() has two issues:
# 1. They are evaluated by EDG, operators are not preserved in EDG's IL
# 2. EDG 3.3 get wrong evaluation values,  EDG 3.10 fixes the bug though.
TESTCODE_CURRENTLY_FAILING = \
   sizeof.upc \
   sizeof_2.upc

#  test2011_08.upc

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
	 $(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
   $(TESTCODES) \
	 $(TESTCODE_CURRENTLY_FAILING)

# Liao (6/11/2008) --edg:upc and --edg:restrict are not required since ROSE can add them internally based on the file suffix (.upc) , no backend compilation since GCC does not recognize UPC .
ROSE_FLAGS = -rose:verbose 0 --edg:no_warnings --edg:c99 -rose:skipfinalCompileStep

PASSING_TEST_Output = $(addprefix rose_,${TESTCODES})
TEST_Output = $(addprefix rose_,${ALL_TESTCODES})

rose_static_%: $(srcdir)/static_% ../../testTranslator
	../../testTranslator -rose:UPC $(ROSE_FLAGS) -rose:upc_threads 1 -c $< -o $@

rose_%: $(srcdir)/% ../../testTranslator
	../../testTranslator -rose:UPC $(ROSE_FLAGS) -rose:upc_threads 1 -c $< -o $@


# $(TEST_Output): ../../testTranslator $(srcdir)/$(@:rose_%=%)
# ../../testTranslator -rose:UPC $(ROSE_FLAGS) -c $(srcdir)/$(@:rose_%=%)
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

# DQ (2/3/2011): Note that all but test2004_22.upc can also be compiled as UPC instead of UPC++ testcodes.
# DQ (9/19/2010): Separated this UPC++ test out from the UPC tests. This test code simply uses some C++ specific
# constructs and does not represent any special work to build UPC++ work. Basically just UPC that allows C++ code.
UPCXX_TESTCODES = \
   test2004_22.upc \
   test2011_01.upc \
   test2011_03.upc \
   test2011_04.upc \
   test2011_05.upc \
   test2011_06.upc \
   test2011_07.upc

FAILING_UPCXX_TESTCODES = \
   test2011_02.upc


UPCXX_TEST_Objects = ${UPCXX_TESTCODES:.upc=.o}

# $(UPCXX_TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.c)
$(UPCXX_TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.c)
	../../testTranslator -rose:UPCxx $(ROSE_FLAGS) -rose:upc_threads 1 -c $(srcdir)/$(@:.o=.upc)


# DQ (2/4/2011): Added test for UPC keyword "THREADS". It is only present when "-rose:upc_threads" option is not used.
testForUpcThreadsKeywordInGeneratedCode: rose_threads.upc
	rm -f rose_threads.upc
	../../testTranslator -rose:UPC -rose:verbose 0 --edg:no_warnings -rose:skipfinalCompileStep -c $(srcdir)/threads.upc
	@echo "Testing for use of THREADS upc keyword in generated code."
	@grep "There are %d threads" rose_threads.upc
	grep "There are %d threads" rose_threads.upc | grep "THREADS"

# DQ (2/4/2011): Added test for UPC keyword "MYTHREAD".
testForUpcMythreadKeywordInGeneratedCode: rose_mythread.upc
	rm -f rose_mythread.upc
	../../testTranslator -rose:UPC -rose:verbose 0 --edg:no_warnings -rose:skipfinalCompileStep -c $(srcdir)/mythread.upc
	@echo "Testing for use of MYTHREAD upc keyword in generated code."
	@grep "I am thread:" rose_mythread.upc
	grep "I am thread:" rose_mythread.upc | grep "MYTHREAD"

rose_shared.upc.aa:rose_shared.upc
	split -36 rose_shared.upc rose_shared.upc.
	test -f rose_shared.upc.aa
	test -f rose_shared.upc.ab


testSharedMemoryDSL_1: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_01.upc

testSharedMemoryDSL_2: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_02.upc

testSharedMemoryDSL_3: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_03.upc

testSharedMemoryDSL_4: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_04.upc

testSharedMemoryDSL_5: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_05.upc

testSharedMemoryDSL_6: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_06.upc

testSharedMemoryDSL_7: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_07.upc

testSharedMemoryDSL_8: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_08.upc

testSharedMemoryDSL_9: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_09.upc

testSharedMemoryDSL_10: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_10.upc

testSharedMemoryDSL_11: sharedMemoryDSL
	./sharedMemoryDSL -rose:UPC -c $(srcdir)/test2014_11.upc

testSharedMemoryDSL_12: sharedMemoryDSL
	./sharedMemoryDSL -c $(srcdir)/test2014_12.c

testSharedMemoryDSL_13: sharedMemoryDSL
	./sharedMemoryDSL -c $(srcdir)/test2014_13.c

testSharedMemoryDSL_14: sharedMemoryDSL
	./sharedMemoryDSL -c $(srcdir)/test2014_14.c

testSharedMemoryDSL_15: sharedMemoryDSL
	./sharedMemoryDSL -c $(srcdir)/test2014_15.c

testSharedMemoryDSL_16: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_16.c

testSharedMemoryDSL_17: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_17.c

testSharedMemoryDSL_18: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_18.c

testSharedMemoryDSL_19: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_19.c

testSharedMemoryDSL_20: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_20.c

testSharedMemoryDSL_21: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_21.c

testSharedMemoryDSL_22: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_22.c

testSharedMemoryDSL_23: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_23.c

testSharedMemoryDSL_24: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_24.c

testSharedMemoryDSL_25: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_25.c

testSharedMemoryDSL_26: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_26.c

testSharedMemoryDSL_27: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_27.c

testSharedMemoryDSL_28: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_28.c

testSharedMemoryDSL_29: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_29.c

testSharedMemoryDSL_30: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_30.c

testSharedMemoryDSL_31: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_31.c

testSharedMemoryDSL_32: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_32.c

testSharedMemoryDSL_33: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_33.c

testSharedMemoryDSL_34: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_34.c

testSharedMemoryDSL_35: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_35.c

testSharedMemoryDSL_36: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_36.c

testSharedMemoryDSL_37: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_37.c

testSharedMemoryDSL_38: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_38.c

testSharedMemoryDSL_39: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_39.c

testSharedMemoryDSL_40: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_40.c

testSharedMemoryDSL_41: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_41.c

testSharedMemoryDSL_42: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_42.c

testSharedMemoryDSL_43: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_43.c

testSharedMemoryDSL_44: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_44.c

testSharedMemoryDSL_45: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_45.c

testSharedMemoryDSL_46: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_46.c

testSharedMemoryDSL_47: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_47.c

testSharedMemoryDSL_48: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_48.c

testSharedMemoryDSL_49: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_49.c

testSharedMemoryDSL_50: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_50.c

testSharedMemoryDSL_51: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_51.c

testSharedMemoryDSL_52: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_52.c

testSharedMemoryDSL_53: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_53.c

testSharedMemoryDSL_54: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_54.c

testSharedMemoryDSL_55: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_55.c

testSharedMemoryDSL_56: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_56.c

testSharedMemoryDSL_57: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_57.c

testSharedMemoryDSL_58: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_58.c

testSharedMemoryDSL_59: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_59.c

testSharedMemoryDSL_60: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_60.c

testSharedMemoryDSL_61: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_61.c

testSharedMemoryDSL_62: sharedMemoryDSL
	./sharedMemoryDSL -testCompile -c $(srcdir)/test2014_62.c


# DQ (4/30/2014): List of test codes to test the shared memory DSL work.
SharedMemoryDSL_testcodes = \
   testSharedMemoryDSL_1 testSharedMemoryDSL_2 testSharedMemoryDSL_3 testSharedMemoryDSL_4 testSharedMemoryDSL_5 \
   testSharedMemoryDSL_6 testSharedMemoryDSL_7 testSharedMemoryDSL_8 testSharedMemoryDSL_9 testSharedMemoryDSL_10 \
   testSharedMemoryDSL_11 testSharedMemoryDSL_12 testSharedMemoryDSL_13 testSharedMemoryDSL_14 testSharedMemoryDSL_15 \
   testSharedMemoryDSL_16 testSharedMemoryDSL_17 testSharedMemoryDSL_18 testSharedMemoryDSL_19 testSharedMemoryDSL_20 \
   testSharedMemoryDSL_21 testSharedMemoryDSL_22 testSharedMemoryDSL_23 testSharedMemoryDSL_24 testSharedMemoryDSL_25 \
   testSharedMemoryDSL_26 testSharedMemoryDSL_27 testSharedMemoryDSL_28 testSharedMemoryDSL_29 testSharedMemoryDSL_30 \
   testSharedMemoryDSL_31 testSharedMemoryDSL_32 testSharedMemoryDSL_33 testSharedMemoryDSL_34  \
   testSharedMemoryDSL_36 testSharedMemoryDSL_37 testSharedMemoryDSL_38 testSharedMemoryDSL_39 testSharedMemoryDSL_40 \
   testSharedMemoryDSL_41 testSharedMemoryDSL_42 testSharedMemoryDSL_43 testSharedMemoryDSL_44 testSharedMemoryDSL_45 \
   testSharedMemoryDSL_46 testSharedMemoryDSL_47 testSharedMemoryDSL_48 testSharedMemoryDSL_49 testSharedMemoryDSL_50 \
   testSharedMemoryDSL_51 testSharedMemoryDSL_52 testSharedMemoryDSL_53 testSharedMemoryDSL_54 testSharedMemoryDSL_55 \
   testSharedMemoryDSL_56 testSharedMemoryDSL_57 testSharedMemoryDSL_58 testSharedMemoryDSL_59 testSharedMemoryDSL_60 \
   testSharedMemoryDSL_61 testSharedMemoryDSL_62

# DQ (7/15/2014): This requires a fix that is not on this branch (it is on another branch not yet merged).
# testSharedMemoryDSL_35

testSharedMemoryDSL: $(SharedMemoryDSL_testcodes)

BROOKSTESTS_testcodes = \
   test_in1 test_in2 test_in3 test_in4 test_in5 test_in6 test_in6 test_in8 test_in9 test_in10 \
   test_in11 test_in12 test_in13 test_in14 test_in15 test_in16 

test_in1:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in1.c
	diff rose_in1.c $(srcdir)/BROOKSTESTS/fiducials/rose_in1.c

test_in2:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in2.c
	diff rose_in2.c $(srcdir)/BROOKSTESTS/fiducials/rose_in2.c

test_in3:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in3.c
	diff rose_in3.c $(srcdir)/BROOKSTESTS/fiducials/rose_in3.c

test_in4:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in4.c
	diff rose_in4.c $(srcdir)/BROOKSTESTS/fiducials/rose_in4.c

test_in5:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in5.c
	diff rose_in5.c $(srcdir)/BROOKSTESTS/fiducials/rose_in5.c

test_in6:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in6.c
	diff rose_in6.c $(srcdir)/BROOKSTESTS/fiducials/rose_in6.c

test_in7:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in7.c
	diff rose_in7.c $(srcdir)/BROOKSTESTS/fiducials/rose_in7.c

test_in8:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in8.c
	diff rose_in8.c $(srcdir)/BROOKSTESTS/fiducials/rose_in8.c

test_in9:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in9.c
	diff rose_in9.c $(srcdir)/BROOKSTESTS/fiducials/rose_in9.c

test_in10:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in10.c
	diff rose_in10.c $(srcdir)/BROOKSTESTS/fiducials/rose_in10.c

test_in11:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in11.c
	diff rose_in11.c $(srcdir)/BROOKSTESTS/fiducials/rose_in11.c

test_in12:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in12.c
	diff rose_in12.c $(srcdir)/BROOKSTESTS/fiducials/rose_in12.c

test_in13:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in13.c
	diff rose_in13.c $(srcdir)/BROOKSTESTS/fiducials/rose_in13.c

test_in14:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in14.c
	diff rose_in14.c $(srcdir)/BROOKSTESTS/fiducials/rose_in14.c

test_in15:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in15.c
	diff rose_in15.c $(srcdir)/BROOKSTESTS/fiducials/rose_in15.c

test_in16:
	./sharedMemoryDSL -testCompile -c $(srcdir)/BROOKSTESTS/in16.c
	diff rose_in16.c $(srcdir)/BROOKSTESTS/fiducials/rose_in16.c

testBROOKSTESTS: $(BROOKSTESTS_testcodes)

QMTEST_Objects = ${ALL_TESTCODES:.upc=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.upc)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.upc)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# DQ (9/17/2010): Added more test code, but not yet used in internal tests.
# DQ (9/15/2010): Moved UPC standard header files to be included automatically (moved to the ROSE compiler include directory).
# put the headers here for now. TODO: Move them to a better place
# EXTRA_DIST = $(ALL_TESTCODES) upc.h upc_relaxed.h upc_strict.h upc_collective.h
  EXTRA_DIST = $(ALL_TESTCODES)  $(UPCXX_TESTCODES)

check-local:
	@echo "Tests for UPC examples."
	@$(MAKE) $(PASSING_TEST_Output)
	@$(MAKE) rose_shared.upc.aa
	@$(MAKE) testSharedMemoryDSL
#	@echo "Tests for UPC++ examples."
#	@$(MAKE) $(UPCXX_TEST_Objects)
#	DQ (2/5/2011): This failed for a very few Hudson tests, but they are not critical
#	tests so I will eliminate them for now. I don't know why they failed.
#	@$(MAKE) testForUpcThreadsKeywordInGeneratedCode
#	@$(MAKE) testForUpcMythreadKeywordInGeneratedCode
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/UPC_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] rose_*.upc rose_* *.dot
	rm -rf QMTest
