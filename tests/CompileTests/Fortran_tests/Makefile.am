include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/22/2010): These files are open source and distributed on the web.
SUBDIRS = LANL_POP

###########################################################################
# These are Fortran testcodes targeting aspects F66, F77, F90, F95, F2003 #
###########################################################################

# #######################
#      PASSING TESTS
# #######################

F90_TESTCODES_REQUIRED_TO_PASS = \
     test2007_01.f90 \
     test2007_02.f90 \
     test2007_03.f90 \
     test2007_04.f90 \
     test2007_05.f90 \
     test2007_06.f90 \
     test2007_07.f90 \
     test2007_08.f90 \
     test2007_09.f90 \
     test2007_10.f90 \
     test2007_11.f90 \
     test2007_12.f90 \
     test2007_13.f90 \
     test2007_14.f90 \
     test2007_15.f90 \
     test2007_16.f90 \
     test2007_17.f90 \
     test2007_19.f90 \
     test2007_20.f90 \
     test2007_21.f90 \
     test2007_22.f90 \
     test2007_23.f90 \
     test2007_24.f90 \
     test2007_25.f90 \
     test2007_26.f90 \
     test2007_27.f90 \
     test2007_28.f90 \
     test2007_29.f90 \
     test2007_32.f90 \
     test2007_33.f90 \
     test2007_34.f90 \
     test2007_36.f90 \
     test2007_38.f90 \
     test2007_39.f90 \
     test2007_40.f90 \
     test2007_48.f90 \
     test2007_49.f90 \
     test2007_50.f90 \
     test2007_51.f90 \
     test2007_52.f90 \
     test2007_53.f90 \
     test2007_54.f90 \
     test2007_55.f90 \
     test2007_56.f90 \
     test2007_57.f90 \
     test2007_58.f90 \
     test2007_59.f90 \
     test2007_60.f90 \
     test2007_61.f90 \
     test2007_62.f90 \
     test2007_63.f90 \
     test2007_64.f90 \
     test2007_65.f90 \
     test2007_66.f90 \
     test2007_67.f90 \
     test2007_68.f90 \
     test2007_69.f90 \
     test2007_71.f90 \
     test2007_72.f90 \
     test2007_73.f90 \
     test2007_74.f90 \
     test2007_75.f90 \
     test2007_76.f90 \
     test2007_77.f90 \
     test2007_78.f90 \
     test2007_79.f90 \
     test2007_80.f90 \
     test2007_89.f90 \
     test2007_90.f90 \
     test2007_91.f90 \
     test2007_92.f90 \
     test2007_93.f90 \
     test2007_94.f90 \
     test2007_95.f90 \
     test2007_96.f90 \
     test2007_97.f90 \
     test2007_99.f90 \
     test2007_100.f90 \
     test2007_101.f90 \
     test2007_102.f90 \
     test2007_103.f90 \
     test2007_104.f90 \
     test2007_105.f90 \
     test2007_106.f90 \
     test2007_109.f90 \
     test2007_110.f90 \
     test2007_111.f90 \
     test2007_117.f90 \
     test2007_118.f90 \
     test2007_119.f90 \
     test2007_121.f90 \
     test2007_122.f90 \
     test2007_126.f90 \
     test2007_127.f90 \
     test2007_135.f90 \
     test2007_136.f90 \
     test2007_137.f90 \
     test2007_138.f90 \
     test2007_139.f90 \
     test2007_140.f90 \
     test2007_141.f90 \
     test2007_143.f90 \
     test2007_149.f90 \
     test2007_158.f90 \
     test2007_159.f90 \
     test2007_160.f90 \
     test2007_161.f90 \
     test2007_168.f90 \
     test2007_169.f90 \
     test2007_178.f90 \
     test2007_179.f90 \
     test2007_181.f90 \
     test2007_182.f90 \
     test2007_190.f90 \
     test2007_219.f90 \
     test2007_236.f90 \
     test2007_238.f90 \
     test2007_239.f90 \
     test2007_244.f90 \
     test2007_248.f90 \
     test2007_249.f90 \
     test2007_250.f90 \
     test2007_262.f90 \
     test2010_15.f90 \
     test2010_16.f90 \
     test2010_18.f90 \
     test2010_20.f90 \
     test2010_23.f90 \
     test2010_25.f90 \
     test2010_28.f90 \
     canonicalExample.f90 


if !ROSE_USING_GFORTRAN_VERSION_4_0
 # These are tests that fail using the gfrotran version 4.0 compiler (but pass with all other versions of gfortran).
   F90_TESTCODES_REQUIRED_TO_PASS += test2007_263.f90
endif


F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS = \
     test2007_124.f \
     test2007_125.f \
     test2007_128.f \
     test2007_129.f \
     test2007_132.f \
     test2007_145.f \
     test2007_148.f \
     test2007_150.f \
     test2007_151.f \
     test2007_153.f \
     test2007_154.f \
     test2007_155.f \
     test2007_156.f \
     test2007_157.f \
     test2007_162.f \
     test2007_163.f \
     test2007_164.f \
     test2007_175.f \
     test2007_183.f \
     test2007_184.f \
     test2007_185.f \
     test2007_186.f \
     test2007_187.f \
     test2007_188.f \
     test2007_189.f \
     test2007_191.f \
     test2007_192.f \
     test2007_193.f \
     test2007_194.f \
     test2007_195.f \
     test2007_196.f \
     test2007_197.f \
     test2007_198.f \
     test2007_200.f \
     test2007_201.f \
     test2007_202.f \
     test2007_204.f \
     test2007_205.f \
     test2007_206.f \
     test2007_207.f \
     test2007_208.f \
     test2007_209.f \
     test2007_211.f \
     test2007_212.f \
     test2007_213.f \
     test2007_214.f \
     test2007_215.f \
     test2007_216.f \
     test2007_217.f \
     test2007_218.f \
     test2007_220.f \
     test2007_221.f \
     test2007_222.f \
     test2007_223.f \
     test2007_224.f \
     test2007_226.f \
     test2007_227.f \
     test2007_228.f \
     test2007_229.f \
     test2007_230.f \
     test2007_231.f \
     test2007_232.f \
     test2007_234.f \
     test2007_242.f \
     test2007_243.f \
     test2007_245.f \
     test2007_246.f \
     test2007_247.f \
     test2007_251.f \
     test2007_253.f \
     test2007_256.f \
     test2007_258.f \
     test2007_259.f \
     test2007_260.f \
     test2007_261.f \
     test2010_29.f \
     test2010_30.f

F03_TESTCODES_REQUIRED_TO_PASS = \
     test2007_30.f03 \
     test2007_31.f03 \
     test2007_35.f03 \
     test2007_37.f03 \
     test2007_41.f03 \
     test2007_42.f03 \
     test2007_43.f03 \
     test2007_44.f03 \
     test2007_45.f03 \
     test2007_47.f03 \
     test2007_98.f03 \
     test2007_112.f03 \
     test2007_113.f03 \
     test2007_114.f03 \
     test2007_115.f03 \
     test2007_116.f03 \
     test2007_120.f03 \
     test2007_123.f03 \
     test2007_130.f03 \
     test2007_133.f03 \
     test2007_134.f03 \
     test2007_174.f03 \
     test2007_176.f03

# DQ (4/3/2010): Added test code used in Fortran OpenMP tests.
OPENMP_TESTS_F77_TESTCODES_REQUIRED_TO_PASS = \
     jacobi.f \
     default.f \
     flush.f \
     exmpaleA221f.f \
     exampleA251f.f \
     schedule.f \
     testNewOFP.f

F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS += $(OPENMP_TESTS_F77_TESTCODES_REQUIRED_TO_PASS)


if !ROSE_USING_GFORTRAN_VERSION_4_1
   F03_TESTCODES_REQUIRED_TO_PASS += test2007_43.f03
endif

# #######################
#      FAILING TESTS
# #######################

F90_TESTCODES_CURRENTLY_FAILING = \
     test2007_46.f90 \
     test2007_70.f90 \
     test2007_81.f90 \
     test2007_82.f90 \
     test2007_83.f90 \
     test2007_84.f90 \
     test2007_87.f90 \
     test2007_88.f90 \
     test2007_107.f90 \
     test2007_108.f90 \
     test2007_131.f90 \
     test2007_144.f90 \
     test2007_152.f90 \
     test2007_165.f90 \
     test2007_166.f90 \
     test2007_170.f90 \
     test2007_177.f90 \
     test2007_237.f90 \
     test2007_240.f90 \
     test2007_254.f90 \
     test2007_255.f90 \
     test2007_257.f90 \
     test2007_LANL.f90 \
     test2007_example.f90 \
     test2009_1.f90 \
     test2010_01.f90 \
     test2010_02.f90 \
     test2010_03.f90 \
     test2010_04.f90 \
     test2010_05.f90 \
     test2010_17.f90 \
     test2010_19.f90 \
     test2010_21.f90 \
     test2010_22.f90 \
     test2010_24.f90 \
     test2010_26.f90 \
     test2010_31.f90 \
     fCaller.f90 \
     triangle.f90

# JJW -- this can't be in the f90 file list because it does not have the .f90 extension
#    test2008_02.f

F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING = \
     test2007_85.f \
     test2007_86.f \
     test2007_142.f \
     test2007_146.f \
     test2007_147.f \
     test2007_167.f \
     test2007_199.f \
     test2007_203.f \
     test2007_210.f \
     test2007_225.f \
     test2007_233.f \
     test2007_235.f \
     test2007_241.f \
     test2007_252.f \
     test2008_02.f \
     test2008_03.f

F03_TESTCODES_CURRENTLY_FAILING = \
     test2007_18.f03 \
     test2007_171.f03 \
     test2007_172.f03 \
     test2007_173.f03 \
     test2007_180.f03 \
     test2010_27.f03


noinst_headers =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS)

F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS)

F77_FIXED_FORMAT_TESTCODES = \
$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS)

F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_F90_TESTCODES = \
$(F90_TESTCODES_REQUIRED_TO_PASS) \
$(F90_TESTCODES_CURRENTLY_FAILING)

ALL_F77_TESTCODES = \
$(F77_TESTCODES_REQUIRED_TO_PASS) \
$(F77_TESTCODES_CURRENTLY_FAILING)

ALL_F77_FIXED_FORMAT_TESTCODES = \
$(F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS) \
$(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)

ALL_F03_TESTCODES = \
$(F03_TESTCODES_REQUIRED_TO_PASS) \
$(F03_TESTCODES_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/FORTRAN_ROSE_PERFORMANCE_DATA.csv

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict $(PERFORMANCE_REPORT_OPTION)
ROSE_FLAGS = -rose:verbose 0

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_F90_Objects = ${F90_TESTCODES:.f90=.o}
PASSING_TEST_F77_Objects = ${F77_TESTCODES:.f77=.o}

# DQ (3/11/2010): F77 tests were not being run, this fixes this.
# PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_TESTCODES:.f=.o}
PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS:.f=.o}

PASSING_TEST_F03_Objects = ${F03_TESTCODES:.f03=.o}
TEST_F90_Objects = ${ALL_F90_TESTCODES:.f90=.o}
TEST_F77_Objects = ${ALL_F77_TESTCODES:.f77=.o}
TEST_F77_FIXED_FORMAT_Objects = ${ALL_F77_FIXED_FORMAT_TESTCODES:.f=.o}
TEST_F03_Objects = ${ALL_F03_TESTCODES:.f03=.o}

PASSING_TEST_Objects = $(PASSING_TEST_F90_Objects) $(PASSING_TEST_F77_Objects) $(PASSING_TEST_F77_FIXED_FORMAT_Objects) $(PASSING_TEST_F03_Objects) 

if ROSE_USE_OPEN_FORTRAN_PARSER

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

testTranslator: ../../testTranslator

$(TEST_F90_Objects): ../../testTranslator ${ALL_TESTCODES}
# $(TEST_F90_Objects): ../../testTranslator
# Example of how to call the OpenFortranParser
#	java parser.java.FortranMain --class parser.java.FortranParserActionJNI $(srcdir)/$(@:.o=.f90)
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f90)
# How to call a ROSE translator with a Fortran source file.
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
#	$(VALGRIND) ../../testParser $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)

$(TEST_F77_Objects): ../../testTranslator ${ALL_TESTCODES}
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f77)
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)

$(TEST_F77_FIXED_FORMAT_Objects): ../../testTranslator ${ALL_TESTCODES}
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f)
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)

# Note: If using gfortran version greater then 4.1, we can't call it with Fortran 2003 code since it is so buggy.
#       So we use -rose:skip_syntax_check to avoid the frontend syntax checking and -rose:skipfinalCompileStep
#       to avoid processing the generated code (which would have the same problem).  It is easier to just use
#       the version 4.1 gfortran which has better Fortran 2003 syntax checking support. This problem is confirmed 
#       on versions of gfortran 4.2 and 4.3.
$(TEST_F03_Objects): ../../testTranslator
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f03)
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
#	Since gfortran does not support F03, and thus we skip the compileation using the backend gfortran compiler, we have to force the generation of an object file. 
#	Build a object file since gfortran fails to do so (avoids remaking the makefile target)
	@echo "   ...force generation of $@, because gfortran compiler does not handle fortran 2003 standard (and the makefile needs $@ to avoid recompilation)."
	@touch $@

# DQ (4/6/2010): Added test for parsing only (tested here for C++ code)
testParser:../../testParser
	$(VALGRIND) ../../testParser $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/test2007_01.f90


QMTEST_Objects = ${ALL_TESTCODES:.f90=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.c)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.f90)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# DQ (11/11/2008): Module test copied from developersScratchSpace/Dan/Fortran_tests
# This file declares a module used by module_B_file.f90
module_A_file.o: testTranslator
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)

# Force representation of module dependency so that mod files are properly generated before use.
module_B_file.o: testTranslator module_A_file.o
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)

test2010_01.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

test2010_02.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

test2010_05.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

# Output spew test from John.
outputSpew.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:caf -rose:skip_syntax_check -c $(srcdir)/$(@:.o=.caf)

# We will build a larger collection of module tests soon (supported by makefile dependence rules).
PASSING_MODULE_TEST_Objects = module_B_file.o

testSuffixRules:
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_01.f
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_01.f
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_02.f77
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_02.f77
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_03.f90
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_03.f90
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_04.f95
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_04.f95
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_05.f03
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_05.f03
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_06.F
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_06.F
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_07.F90
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_07.F90
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_08.F95
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_08.F95
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_09.F03
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_09.F03
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_10.F08
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_10.F08
	java parser.java.FortranMain --dump $(srcdir)/test2007_suffixTest_11.f08
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_11.f08

check-local:
	@echo "Tests for Fortran."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These Fortran programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(PASSING_MODULE_TEST_Objects)
#	DQ (4/6/2010): This fails on Hudson, but I don't know why.
#	@$(MAKE) testParser
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

else

# Fortran support not available in ROSE unless specified on the configure command line.
check-local:
	@echo "***********************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests: FORTRAN NOT CONFIGURED IN ROSE ******"
	@echo "***********************************************************************************"
endif

# This will output the example testcodes that are failing
listProblemFiles: $(F77_TESTCODES_CURRENTLY_FAILING) $(F90_TESTCODES_CURRENTLY_FAILING) $(F03_TESTCODES_CURRENTLY_FAILING) $(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)
	egrep -n "*"  $(srcdir)/$(?)

# Make sure that all the test codes end up in the distribution
# EXTRA_DIST = $(ALL_TESTCODES)
EXTRA_DIST = $(ALL_F77_FIXED_FORMAT_TESTCODES) $(ALL_F77_TESTCODES) $(ALL_F90_TESTCODES) $(ALL_F03_TESTCODES) \
   test2007_suffixTest_01.f test2007_suffixTest_02.f77 test2007_suffixTest_03.f90 test2007_suffixTest_04.f95 \
   test2007_suffixTest_05.f03 test2007_suffixTest_06.F test2007_suffixTest_07.F90 test2007_suffixTest_08.F95 \
   test2007_suffixTest_09.F03 test2007_suffixTest_10.F08 test2007_suffixTest_11.f08 \
   module_A_file.f90 module_B_file.f90

# DQ (3/12/2010): Fortran 2008 tests from OFP
# f08-tests

clean-local:
	rm -f *.o rose_*.f* *.rmod
	rm -f *.s *.mod rose_*.f rose_*.f77 rose_*.f90 rose_*.f95 rose_*.f03 *.dot *.pdf *~ *.ps *.out *.log rose_performance_report_lockfile.lock
	rm -rf QMTest

