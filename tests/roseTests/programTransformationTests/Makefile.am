include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = \
   preTest \
   patternRewriteTest \
   finiteDifferencingTest \
   finiteDifferencingDemo \
   testFunctionNormalization \
   constantFoldingTest \
   implicitCodeGenerationTest

# DQ (3/14/2006): Switch to using multiple libs (since it is faster)
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

preTest_SOURCES = preTest.C
preTest_LDADD = $(LDADD)

patternRewriteTest_SOURCES = patternRewriteTest.C
patternRewriteTest_LDADD = $(LDADD)

finiteDifferencingTest_SOURCES = finiteDifferencingTest.C
finiteDifferencingTest_LDADD = $(LDADD)

finiteDifferencingDemo_SOURCES = finiteDifferencingDemo.C
finiteDifferencingDemo_LDADD = $(LDADD)

testFunctionNormalization_SOURCES = testFunctionNormalization.C
testFunctionNormalization_LDADD = $(LDADD)

constantFoldingTest_SOURCES = constantFoldingTest.C
constantFoldingTest_LDADD = $(LDADD)

implicitCodeGenerationTest_SOURCES = implicitCodeGenerationTest.C
implicitCodeGenerationTest_LDADD = $(LDADD)

# test to always compile the test code
FORCE_TEST_CODES_TO_RUN:
	$(srcdir)/TestDriver $(srcdir)
	@echo "SKIPPING TEST CODES FOR functionNormalization RUN BY \"make check\" rule in tests/roseTests/programTransformationTests directory ..."
#	testFunctionNormalization -c $(src_dir)/functionNormalizationTest1.C
#	testFunctionNormalization -c $(src_dir)/functionNormalizationTest2.C

testPass1: preTest
	./preTest $(srcdir)/pass1.C

testPass2: preTest
	./preTest $(srcdir)/pass2.C

testPass3: preTest
	./preTest $(srcdir)/pass3.C

testConstantFolding: constantFoldingTest
	./constantFoldingTest -c $(srcdir)/inputCode_constantFoldingTransformation.C

testImplicitCodeGeneration: implicitCodeGenerationTest
	./implicitCodeGenerationTest -icg:defaultFunctionGenerator -icg:shortCircuitingTransformation -icg:destructorCallAnnotator -c $(top_srcdir)/tests/CompileTests/Cxx_tests/test2004_116.C
	./implicitCodeGenerationTest -icg:defaultFunctionGenerator -icg:shortCircuitingTransformation -icg:destructorCallAnnotator -c $(srcdir)/inputCode_implicitCodeGenerationTest.C

EXTRA_DIST = TestDriver pass1.C pass2.C pass3.C rose_pass1.C.save rose_pass2.C.save rose_pass3.C.save \
             rewrite_test1.C rose_rewrite_test1.C.save \
             finitediff_test1.C rose_finitediff_test1.C.save \
             finitediff_test2.C rose_finitediff_test2.C.save \
             finitediff_test3.C rose_finitediff_test3.C.save \
             functionNormalizationTest1.C \
             functionNormalizationTest2.C \
             inputCode_constantFoldingTransformation.C \
             inputCode_implicitCodeGenerationTest.C

# DQ (3/21/2009): These tests don't appear to run under Cygwin.
if ROSE_BUILD_OS_IS_CYGWIN
check-local:
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/programTransformationTests: (commented out all tests for Cygwin) ******"
	@echo "***************************************************************************************************"
else
check-local:
	$(MAKE) testConstantFolding testImplicitCodeGeneration
	$(MAKE) FORCE_TEST_CODES_TO_RUN
	@echo "*******************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programTransformationTests: make check rule complete (terminated normally) ***"
	@echo "*******************************************************************************************************"
endif

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates rose_*.C

distclean-local:
	rm -rf Templates.DB







