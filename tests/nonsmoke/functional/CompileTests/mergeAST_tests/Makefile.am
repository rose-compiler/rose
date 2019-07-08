include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##############################################################################
# These are testcodes specific to the ROSE mechanism to merge different ASTs #
##############################################################################

# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(srcdir)/../colorAST_tests
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

# This is a shorted list that tests more quickly.
TESTCODES_REQUIRED_TO_PASS = \
     mergeTest_01.C \
     mergeTest_02.C \
     mergeTest_03.C \
     mergeTest_08.C \
     mergeTest_10.C \
     mergeTest_11.C \
     mergeTest_12.C \
     mergeTest_13.C \
     mergeTest_14.C \
     mergeTest_15.C \
     mergeTest_16.C \
     mergeTest_17.C \
     mergeTest_18.C \
     mergeTest_19.C \
     mergeTest_20.C \
     mergeTest_21.C \
     mergeTest_22.C \
     mergeTest_23.C \
     mergeTest_24.C \
     mergeTest_25.C \
     mergeTest_26.C \
     mergeTest_27.C \
     mergeTest_28.C \
     mergeTest_29.C \
     mergeTest_30.C \
     mergeTest_31.C \
     mergeTest_32.C \
     mergeTest_33.C \
     mergeTest_34.C \
     mergeTest_35.C \
     mergeTest_36.C \
     mergeTest_37.C \
     mergeTest_38.C \
     mergeTest_43.C \
     mergeTest_44.C \
     mergeTest_46.C \
     mergeTest_47.C \
     mergeTest_48.C \
     mergeTest_50.C \
     mergeTest_51.C \
     mergeTest_52.C \
     mergeTest_53.C \
     mergeTest_54.C \
     mergeTest_55.C \
     mergeTest_56.C \
     mergeTest_57.C \
     mergeTest_58.C \
     mergeTest_60.C \
     mergeTest_61.C \
     mergeTest_62.C \
     mergeTest_63.C \
     mergeTest_64.C \
     mergeTest_65.C \
     mergeTest_66.C \
     mergeTest_67.C \
     mergeTest_68.C \
     mergeTest_69.C \
     mergeTest_70.C \
     mergeTest_71.C \
     mergeTest_73.C \
     mergeTest_74.C \
     mergeTest_76.C \
     mergeTest_77.C \
     mergeTest_78.C \
     mergeTest_79.C \
     mergeTest_80.C \
     mergeTest_81.C \
     mergeTest_82.C \
     mergeTest_83.C \
     mergeTest_84.C \
     mergeTest_85.C \
     mergeTest_86.C \
     mergeTest_87.C \
     mergeTest_88.C \
     mergeTest_91.C \
     mergeTest_92.C \
     mergeTest_94.C \
     mergeTest_95.C \
     mergeTest_96.C \
     mergeTest_97.C \
     mergeTest_100.C \
     mergeTest_102.C \
     mergeTest_103.C \
     mergeTest_104.C \
     mergeTest_105.C \
     mergeTest_106.C \
     mergeTest_107.C \
     mergeTest_110.C \
     mergeTest_112.C \
     mergeTest_114.C \
     mergeTest_115.C \
     mergeTest_116.C \
     mergeTest_118.C \
     mergeTest_119.C \
     mergeTest_120.C \
     mergeTest_128.C \
     mergeTest_129.C \
     mergeTest_130.C \
     mergeTest_131.C \
     mergeTest_133.C \
     mergeTest_134.C \
     mergeTest_138.C 

# DQ (7/7/2019): This is a problem after corrections made to the AST to remove redundnat declarations.
#    mergeTest_101.C 

# DQ (1/19/2017): These are a problem for GNU 6.1.
if !ROSE_USING_GCC_VERSION_LATER_6_1
TESTCODES_REQUIRED_TO_PASS += \
     mergeTest_04.C \
     mergeTest_05.C

# DQ (4/9/2017): This appears to be a problem for GNU 4.8 using EDG 4.9.
if !ROSE_USE_EDG_VERSION_4_9
# DQ (2/26/2017): This is a file that fails for GNU 5.1 and later versions.
if !ROSE_USING_GCC_VERSION_LATER_5_1
TESTCODES_REQUIRED_TO_PASS += \
     mergeTest_111.C 
endif
else
# DQ (4/9/2017): Older compiler versions appear to be fine.
if !ROSE_USING_GCC_VERSION_LATER_4_8
TESTCODES_REQUIRED_TO_PASS += \
     mergeTest_111.C 
endif
endif
endif

# TV (11/15/2018): EDG 5.0 tells us that a forward declaration of an enumeration is "nonstandard".
if !ROSE_USE_EDG_VERSION_5_0
TESTCODES_REQUIRED_TO_PASS += \
     mergeTest_09.C \
     mergeTest_135.C
endif

# DQ (8/22/2013): Failing test:
# This test code is a large complex test, it fails out-side of the debugger, 
# but runs fine in the debugger.  Valgrind reports a number of specific problems, 
# all too complex to handle at present. I elect to comment out this specific 
# test (merge on two files containing only: "#include<string>" and reevaluate
# on the larger collection of regression tests in ROSE.
#    mergeTest_06.C


# DQ (5/26/2013) (mergeTest_121.C) This is a failing test on Jenkins that I can now reproduce on my machine.
# Command line to generate problem: ./testMerge --edg:no_warnings -w --edg:restrict -rose:astMerge -c /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/nonsmoke/functional/CompileTests/mergeAST_tests/mergeTest_121.C mergeTest_121_alt.C
# TOO1 (5/28/2013) (mergeTest_121.C) Add to EXTRA_DIST (below) to fix UnusedSourcesPolicy test.
TESTCODES_TO_FIX = \
	mergeTest_121.C


# DQ (9/18/2011): Failing tests after new contant folding default to subsitute original expression tress for constant values.
#    mergeTest_90.C
#    mergeTest_124.C 

# DQ (9/24/2011): Comment these out since they do not pass with the new original expression tree handling, and will have to be debugged seperately.
TESTCODES_REQUIRED_TO_PASS += mergeTest_90.C mergeTest_124.C


# This fails for GNU g++ version 4.3
#     mergeTest_136.C  # Liao, 7/27/2010, __is_pod  __is_empty become builtin functions in GCC 4.3 and above
#    mergeTest_07.C

# DQ (7/16/2010): Test code which demonstrates that we can't delete the SgStorageModifier IR nodes (I don't know why).
#    mergeTest_137.C


# Separate out the files that are only failing to unparse correctly after the AST merge.
# This is important eventually, but for analysis we only need the merge without unparsing.
TESTCODES_CURRENTLY_FAILING_UNPARSE = \
     mergeTest_45.C \
     mergeTest_49.C \
     mergeTest_59.C \
     mergeTest_72.C \
     mergeTest_75.C \
     mergeTest_93.C \
     mergeTest_98.C \
     mergeTest_99.C \
     mergeTest_109.C \
     mergeTest_113.C \
     mergeTest_117.C \
     mergeTest_122.C \
     mergeTest_123.C \
     mergeTest_125.C \
     mergeTest_126.C \
     mergeTest_127.C \
     mergeTest_132.C

TESTCODES_CURRENTLY_FAILING = \
     $(TESTCODES_CURRENTLY_FAILING_UNPARSE) \
     mergeTest_39.C \
     mergeTest_40.C \
     mergeTest_41.C \
     mergeTest_42.C \
     mergeTest_89.C \
     mergeTest_108.C

# DQ (12/5/2007): New failing files
# mergeTest_89.C 

bin_PROGRAMS = testMerge
testMerge_SOURCES = testMerge.C

LDADD = $(ROSE_SEPARATE_LIBS)

testMerge_single: testMerge
	./testMerge -c $(srcdir)/inputCode_merge.C

# testMerge: testMerge
#	./testMerge -c $(srcdir)/inputCode_merge.C $(srcdir)/inputCode_merge_alt.C

testODR_single: testMerge
	./testMerge -c -I$(srcdir) $(srcdir)/odr_module1.C

testODR: testMerge
	./testMerge -c -I$(srcdir) $(srcdir)/odr_module1.C $(srcdir)/odr_module2.C

testODR_paper: testMerge
	./testMerge -c -I$(srcdir) $(srcdir)/odr_module.C $(srcdir)/odr_attacker.C $(srcdir)/odr_main.C

testMerge_bug: testMerge
	./testMerge -c $(srcdir)/inputCode_test.C

testMergeC_bug: testMerge
	./testMerge -rose:C_only -c $(srcdir)/inputCode_test.c

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=80 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND_OPTIONS = --tool=memcheck -v --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --free-fill=deadbeef
# VALGRIND_OPTIONS = --tool=memcheck -v --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --malloc-fill="0000" --free-fill="0000"
VALGRIND_OPTIONS = --tool=memcheck --trace-children=yes
# VALGRIND = /usr/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
# VALGRIND = $(ADDRESS_SANITIZER_OPTIONS)
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w --edg:restrict -rose:astMerge

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# Use ../colorAST_tests/colorTraversal.h
#$(TEST_Objects): merge
#	./merge $(ROSE_FLAGS) -I../colorAST_tests -c $(srcdir)/$(@:.o=.C)

$(TEST_Objects): testMerge
	@echo "Copy the file $@ to build a second alternative one to test the AST merge ..."
	@cp $(srcdir)/$(@:.o=.C) $(@:.o=_alt.C)
# 	$(VALGRIND) ./testMerge $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)
	$(VALGRIND) ./testMerge $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C) $(@:.o=_alt.C)
#	Build an object file to prevent make rebuilding target (astMerge is an analysis only).
	@touch $@


QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): testMerge $(srcdir)/$(@:.qmt=.C)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/testMerge NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# EXTRA_DIST = $(ALL_TESTCODES) inputCode_merge.h  odr_base.h  odr.h
EXTRA_DIST = $(ALL_TESTCODES) $(TESTCODES_TO_FIX)

check-local:
	@echo "Tests for AST merge mechanism."
#	@$(MAKE) $(PASSING_TEST_Objects)
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
	@echo "C++11 tests using EDG 4.9 and GNU version 4.9 backend compiler are disabled."
# Exclude these tests on GNU 4.9 and later compilers).
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_8
# If this is not the CLANG or Intel compilers and an earlier version of GNU than GNU 4.9 compiler, so include these files.
#	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == false"
	@$(MAKE) $(PASSING_TEST_Objects)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
#	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
#	@echo "USING_CLANG_COMPILER == true"
	@echo "C++11 tests using EDG 4.9: Clang backend compiler are NOT allowed with the EDG 4.9 frontend."
else # NOT USING_CLANG_COMPILER
#	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
#	@echo "USING_INTEL_COMPILER == true"
	@echo "C++11 tests using EDG 4.9 Intel backend compiler are allowed."
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
#	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
#	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "C++11 tests require configuration with either EDG version 4.9 and GNU version 4.8 or backend compiler or EDG 4.12 and 4.8 or any later GNU compiler."
	@echo "C++11 tests using EDG 4.12 and any supported backend compiler are allowed."
	@$(MAKE) $(PASSING_TEST_Objects)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "C++11 tests using EDG 4.9: Clang backend compiler using EDG 4.12 frontend."
# DQ (2/28/2017): Skip these files with Clang for now and come back to them later (when we understand the limits of portability better).
#	@$(MAKE) $(PASSING_TEST_Objects)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "C++11 tests using EDG 4.12 Intel backend compiler are allowed."
	@$(MAKE) $(PASSING_TEST_Objects)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9
	@echo "************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/mergeAST_tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot
	rm -rf QMTest

distclean-local:
	rm -rf Templates.DB *alt.C

