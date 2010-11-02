include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)
bin_PROGRAMS = generateVirtualCFG

generateVirtualCFG_SOURCES = generateVirtualCFG.C

noinst_PROGRAMS = testVirtualCFG

testVirtualCFG_SOURCES = testVirtualCFG.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# Import list of C++ test cases that should pass:
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT
#
include $(srcdir)/../Cxx_tests/Makefile-pass.inc


EXAMPLE_C_TESTCODES_VERIFIED_TO_PASS = \
test_CplusplusMacro_C.C \
test2001_02.C \
test2001_03.C \
test2001_05.C \
test2001_07.C \
test2001_08.C \
test2001_09.C \
test2001_14.C \
test2001_17.C \
test2001_23.C \
test2001_24.C \
test2001_25.C \
test2001_26.C \
test2002_01.C \
test2003_04.C \
test2003_05.C \
test2003_06.C \
test2003_10.C \
test2003_11.C \
test2003_13.C \
test2003_14.C \
test2003_15.C \
test2003_16.C \
test2003_18.C \
test2003_19.C \
test2003_20.C \
test2003_21.C \
test2003_23.C \
test2003_24.C \
test2003_25.C \
test2003_26.C \
test2003_29.C \
test2003_30.C \
test2003_31.C \
test2003_32.C \
test2003_35.C \
test2003_36.C \
test2004_03.C \
test2004_04.C \
test2004_05.C \
test2004_13.C \
test2004_14.C \
test2004_19.C \
test2004_23.C \
test2004_28.C \
test2004_29.C \
test2004_49.C \
test2004_53.C \
test2004_60.C \
test2004_61.C \
test2004_72.C \
test2004_74.C \
test2004_94.C \
test2004_96.C \
test2004_102.C \
test2004_105.C \
test2004_107.C \
test2004_108.C \
test2004_114.C \
test2004_118.C \
test2004_131.C \
test2004_152.C \
test2004_153.C \
test2004_154.C \
test2004_163.C \
test2005_16.C \
test2005_20.C \
test2005_21.C \
test2005_22.C \
test2005_23.C \
test2005_26.C \
test2005_48.C \
test2005_62.C \
test2005_84.C \
test2005_92.C \
test2005_95.C \
test2005_100.C \
test2005_111.C \
test2005_116.C \
test2005_125.C \
test2005_157.C \
test2005_159.C \
test2005_164.C \
test2005_165.C \
test2005_169.C \
test2005_170.C \
test2005_176.C \
test2005_180.C \
test2005_182.C \
test2005_184.C \
test2005_187.C \
test2005_191.C \
test2005_198.C \
test2005_202.C \
test2005_209.C \
test2006_07.C \
test2006_10.C \
test2006_12.C \
test2006_14.C \
test2006_16.C \
test2006_23.C \
test2006_34.C \
test2006_40.C \
test2006_50.C \
test2006_52.C \
test2006_53.C \
test2006_55.C \
test2006_56.C \
test2006_57.C \
test2006_62.C \
test2006_69.C \
test2006_75.C \
test2006_76.C \
test2006_82.C \
test2006_83.C \
test2006_88.C \
test2006_91.C \
test2006_93.C \
test2006_94.C \
test2006_97.C \
test2006_99.C \
test2006_103.C \
test2006_104.C \
test2006_105.C \
test2006_106.C \
test2006_107.C \
test2006_108.C \
test2006_109.C \
test2006_111.C \
test2006_113.C \
test2006_116.C \
test2006_137.C \
test2006_140.C \
test2006_142.C \
test2006_145.C \
test2006_146.C \
test2006_147.C \
test2006_148.C \
test2006_149.C \
test2006_150.C \
test2006_151.C \
test2006_152.C \
test2006_153.C \
test2006_154.C \
test2006_181.C \
test_int_lexemes.C \
test_int_lexemes_donot_pass.C \
test_simple_int.C \
test_wchars.C \
X.C

# JJW 5-22-2008 This test is not portable to Mac OS X
# test2006_44.C

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
     test2007_263.f90 \
     canonicalExample.f90 

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
     test2007_261.f

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

C99_TESTCODES_REQUIRED_TO_PASS = \
     bool.c \
     complex_01.c \
     complex_03.c \
     constants.c \
     test2005_186.c \
     test2006_127.c \
     test2006_143.c \
     test2008_01.c


# Configure-time automake conditional
if ROSE_USE_LONG_MAKE_CHECK_RULE

# This is the long test of ROSE (takes a while!)
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

else # !ROSE_USE_LONG_MAKE_CHECK_RULE

# Shorten the list of code to compile to simplify checks using "make distcheck"
# Note that the tests test2001_02.C and test2001_03.C are required because the
# tests in ../C_subset_of_Cxx use them and that test2004_116.C is required 
# because the tests in ROSE/tests/roseTests/programTransformationTests use it.
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT)

endif # ROSE_USE_LONG_MAKE_CHECK_RULE

# Some of these test codes reference A++ header fiels as part of their tests
# Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) 

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

CXX_FILES = ${TESTCODES_REQUIRED_TO_PASS:.C=.CXX-o}
C_FILES   = ${EXAMPLE_C_TESTCODES_VERIFIED_TO_PASS:.C=.C-o}
C99_FILES = ${C99_TESTCODES_REQUIRED_TO_PASS:.c=.C99-o}
F90_FILES = ${F90_TESTCODES_REQUIRED_TO_PASS:.f90=.F90-o}
F77_FILES = ${F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS:.f=.F-o}
F03_FILES = ${F03_TESTCODES_REQUIRED_TO_PASS:.f03=.F03-o}

$(CXX_FILES): testVirtualCFG
	./testVirtualCFG $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -I$(srcdir)/../Cxx_tests $(TESTCODE_INCLUDES) -c $(srcdir)/../Cxx_tests/$(@:.CXX-o=.C) && touch $@

# ${EXAMPLE_C_TESTCODES_VERIFIED_TO_PASS:.C.o}: testVirtualCFG
$(C_FILES): testVirtualCFG $(srcdir)/../Cxx_tests/$(@:.C-o=.C)
	cp $(srcdir)/../Cxx_tests/$(@:.C-o=.C) $(@:.C-o=.temp.c)
	./testVirtualCFG $(ROSE_FLAGS) -I$(srcdir)/../Cxx_tests $(TESTCODE_INCLUDES) -c $(@:.C-o=.temp.c) && touch $@

$(C99_FILES): testVirtualCFG $(srcdir)/../C99_tests/$(@:.C99-o=.c)
	cp $(srcdir)/../C99_tests/$(@:.C99-o=.c) $(@:.C99-o=.temp.c)
	./testVirtualCFG $(ROSE_FLAGS) -I$(srcdir)/../C99_tests $(TESTCODE_INCLUDES) -c $(@:.C99-o=.temp.c) && touch $@

# DQ (6/4/2008): I have commented these out while we do more development of ROSE Fortran support.
$(F90_FILES): ./testVirtualCFG
#	./testVirtualCFG $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/../Fortran_tests/$(@:.F90-o=.f90) && touch $@

# DQ (6/4/2008): I have commented these out while we do more development of ROSE Fortran support.
$(F77_FILES): ./testVirtualCFG
#	./testVirtualCFG $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/../Fortran_tests/$(@:.F-o=.f) && touch $@

# DQ (6/4/2008): I have commented these out while we do more development of ROSE Fortran support.
$(F03_FILES): ./testVirtualCFG
#	./testVirtualCFG $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/../Fortran_tests/$(@:.F03-o=.f03) && touch $@


QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): testVirtualCFG $(srcdir)/$(@:.qmt=.C)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/testVirtualCFG NULL $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = 

check-cxx: $(CXX_FILES)
check-c: $(C_FILES)
check-c99: $(C99_FILES)
check-f90: $(F90_FILES)
check-f77: $(F77_FILES)
check-f03: $(F03_FILES)
# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
check-fortran: check-f90 check-f77 check-f03
else
check-fortran:
endif

check-local: check-cxx check-c check-c99 check-fortran
# check-local: $(CXX_FILES) $(C_FILES)
	@echo "******************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/virtualCFG_tests: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************"

clean-local:
	rm -f *.o *.temp.c *-o *.mod *.dot
	rm -rf QMTest

