include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (8/1/2005): Uncommented to force test code to build, but tests currently fail
# QY 11/9/04 comment out test
# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = astFileIO astFileRead astCompressionTest parallelMerge

astFileIO_SOURCES = astFileIO.C 
astFileIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astCompressionTest_SOURCES = astCompressionTest.C 
astCompressionTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astFileRead_SOURCES = astFileRead.C
astFileRead_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

parallelMerge_SOURCES = parallelMerge.C
parallelMerge_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# astFileIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

if !ROSE_USE_EDG_VERSION_4
# Process the standard collection of C++ regression test as example test codes.
include $(srcdir)/../../CompileTests/Cxx_tests/Makefile-pass.inc
LOCAL_EXAMPLE_TESTCODES_REQUIRED_TO_PASS = 
else
# Build a simpler local list of files to process.
EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
test_CplusplusMacro_Cpp.C \
test2001_01.C \
test2001_02.C \
test2001_03.C \
test2001_04.C \
test2001_05.C \
test2001_06.C \
test2001_07.C \
test2001_08.C \
test2001_09.C \
test2001_10.C \
test2001_11.C \
test2001_12.C \
test2001_13.C \
test2001_14.C \
test2001_15.C \
test2001_16.C \
test2001_17.C \
test2001_18.C \
test2001_19.C \
test2001_20.C \
test2001_21.C \
test2001_23.C \
test2001_24.C \
test2001_25.C \
test2001_26.C \
test2001_27.C \
test2001_28.C \
test2001_29.C \
test2002_01.C \
test2003_01.C \
test2003_02.C \
test2003_03.C \
test2003_04.C \
test2003_05.C \
test2003_06.C \
test2003_09.C \
test2003_10.C \
test2003_11.C \
test2003_12.C \
test2003_13.C \
test2003_14.C \
test2003_15.C \
test2003_16.C \
test2003_17.C \
test2003_18.C \
test2003_19.C \
test2003_20.C \
test2003_21.C \
test2003_23.C \
test2003_25.C \
test2003_26.C \
test2003_27.C \
test2003_28.C \
test2003_29.C \
test2003_30.C \
test2003_31.C \
test2003_32.C \
test2003_35.C \
test2003_36.C \
test2003_37.C \
test2003_38.C \
test2004_01.C \
test2004_02.C \
test2004_03.C \
test2004_04.C \
test2004_05.C \
test2004_06.C \
test2004_07.C \
test2004_08.C \
test2004_09.C \
test2004_10.C \
test2004_11.C \
test2004_12.C \
test2004_13.C \
test2004_14.C \
test2004_15.C \
test2004_16.C \
test2004_17.C \
test2004_18.C \
test2004_19.C \
test2004_23.C \
test2004_24.C \
test2004_26.C \
test2004_27.C \
test2004_28.C \
test2004_29.C \
test2004_30.C \
test2004_31.C \
test2004_32.C \
test2004_33.C \
test2004_34.C \
 test2004_35.C \
test2004_36.C \
test2004_37.C \
test2004_38.C \
test2004_39.C \
test2004_40.C \
test2004_42.C \
test2004_43.C \
test2004_44.C \
test2004_45.C \
test2004_47.C \
test2004_48.C \
test2004_49.C \
test2004_50.C \
test2004_52.C \
test2004_53.C \
test2004_54.C \
test2004_55.C \
test2004_56.C \
test2004_58.C \
test2004_59.C \
test2004_60.C \
test2004_61.C \
test2004_62.C \
test2004_63.C \
test2004_64.C \
test2004_65.C \
test2004_67.C \
test2004_68.C \
test2004_69.C \
test2004_70.C \
test2004_71.C \
test2004_72.C \
test2004_73.C \
test2004_74.C \
test2004_75.C \
test2004_76.C \
 test2004_77.C \
test2004_78.C \
test2004_79.C \
test2004_80.C \
test2004_81.C \
test2004_83.C \
test2004_84.C \
 test2004_85.C \
test2004_87.C \
test2004_88.C \
test2004_89.C \
test2004_90.C \
test2004_91.C \
test2004_93.C \
test2004_94.C \
test2004_95.C \
test2004_96.C \
test2004_97.C \
test2004_98.C \
test2004_99.C \
test2004_100.C \
test2004_101.C \
test2004_102.C \
test2004_103.C \
test2004_104.C \
test2004_105.C \
test2004_106.C \
test2004_107.C \
test2004_108.C \
test2004_109.C \
test2004_110.C \
test2004_111.C \
test2004_112.C \
test2004_113.C \
test2004_114.C \
test2004_115.C \
 test2004_116.C \
test2004_117.C \
test2004_118.C \
test2004_119.C \
 test2004_120.C \
test2004_121.C \
test2004_122.C \
test2004_123.C \
test2004_124.C \
test2004_125.C \
 test2004_127.C \
test2004_128.C \
 test2004_129.C \
test2004_131.C \
test2004_132.C \
test2004_133.C \
test2004_134.C \
test2004_135.C \
test2004_136.C \
test2004_137.C \
test2004_138.C \
test2004_139.C \
test2004_140.C \
test2004_141.C \
test2004_142.C \
 test2004_143.C \
test2004_144.C \
test2004_145.C \
test2004_146.C \
test2004_147.C \
test2004_148.C \
 test2004_149.C \
test2004_150.C \
test2004_151.C \
test2004_152.C \
test2004_153.C \
test2004_154.C \
test2004_155.C \
test2004_156.C \
test2004_157.C \
test2004_158.C \
test2004_159.C \
test2004_160.C \
test2004_161.C \
test2004_162.C \
test2004_163.C \
test2004_164.C \
test2005_01.C \
test2005_02.C \
test2005_03.C \
test2005_04.C \
test2005_05.C \
test2005_06.C \
test2005_07.C \
test2005_08.C \
test2005_10.C \
test2005_11.C \
test2005_12.C \
test2005_13.C \
test2005_14.C \
test2005_15.C \
test2005_16.C \
test2005_17.C \
test2005_19.C \
test2005_20.C \
test2005_21.C \
test2005_22.C \
test2005_23.C \
test2005_24.C \
test2005_25.C \
test2005_26.C \
test2005_27.C \
 test2005_28.C \
test2005_29.C \
test2005_30.C \
test2005_31.C \
test2005_33.C \
 test2005_34.C \
 test2005_35.C \
test2005_36.C \
test2005_37.C \
test2005_38.C \
 test2005_39.C \
test2005_40.C \
 test2005_42.C \
test2005_43.C \
test2005_44.C \
test2005_45.C \
test2005_46.C \
test2005_47.C \
test2005_48.C \
test2005_49.C \
test2005_50.C \
test2005_51.C \
test2005_52.C \
test2005_53.C \
test2005_55.C \
test2005_57.C \
test2005_58.C \
test2005_59.C \
test2005_60.C \
test2005_61.C \
test2005_62.C \
test2005_63.C \
test2005_64.C \
test2005_65.C \
test2005_66.C \
test2005_67.C \
test2005_68.C \
test2005_70.C \
test2005_71.C \
test2005_72.C \
test2005_73.C \
test2005_74.C \
test2005_75a.C \
test2005_75b.C \
test2005_76.C \
test2005_77.C \
test2005_78.C \
test2005_79.C \
test2005_80.C \
test2005_81.C \
 test2005_82.C \
test2005_83.C \
test2005_84.C \
test2005_86.C \
test2005_87.C \
test2005_88.C \
test2005_89.C \
test2005_90.C \
test2005_91.C \
test2005_92.C \
 test2005_93.C \
test2005_94.C \
test2005_95.C \
test2005_96.C \
test2005_97.C \
test2005_98.C \
test2005_99.C \
test2005_100.C \
test2005_101.C \
test2005_102.C \
test2005_103.C \
test2005_105.C \
test2005_106.C \
test2005_107.C \
test2005_108.C \
test2005_109.C \
test2005_110.C \
test2005_111.C \
test2005_113.C \
test2005_114.C \
test2005_116.C \
test2005_118.C \
test2005_119.C \
test2005_120.C \
test2005_121.C \
test2005_122.C \
test2005_123.C \
test2005_124.C \
test2005_125.C \
test2005_126.C \
test2005_127.C \
test2005_128.C \
test2005_129.C \
test2005_130.C \
test2005_131.C \
test2005_132.C \
test2005_133.C \
test2005_134.C \
test2005_135.C \
test2005_136.C \
test2005_137.C \
test2005_138.C \
test2005_139.C \
test2005_140.C \
test2005_141.C \
test2005_142.C \
test2005_143.C \
test2005_144.C \
test2005_145.C \
test2005_146.C \
test2005_147.C \
test2005_148.C \
test2005_149.C \
test2005_150.C \
test2005_152.C \
test2005_153.C \
test2005_155.C \
test2005_156.C \
test2005_157.C \
test2005_158.C \
test2005_159.C \
test2005_160.C \
test2005_162.C \
test2005_164.C \
test2005_165.C \
test2005_166.C \
test2005_168.C \
test2005_167.C \
test2005_169.C \
test2005_170.C \
test2005_171.C \
test2005_172.C \
test2005_173.C \
test2005_174.C \
test2005_175.C \
test2005_176.C \
test2005_177.C \
test2005_178.C \
test2005_180.C \
test2005_181.C \
test2005_182.C \
test2005_184.C \
test2005_185.C \
test2005_187.C \
test2005_189.C \
test2005_190.C \
test2005_191.C \
test2005_192.C \
test2005_193.C \
test2005_195.C \
   test2005_196.C \
test2005_197.C \
test2005_198.C \
test2005_202.C \
test2005_203.C \
   test2005_206.C \
   test2005_207.C \
test2005_208.C \
test2005_209.C \
test2006_01.C \
test2006_02.C \
test2006_03.C \
test2006_04.C \
test2006_05.C \
test2006_06.C \
test2006_07.C \
test2006_08.C \
test2006_10.C \
test2006_12.C \
test2006_13.C \
test2006_14.C \
test2006_16.C \
test2006_17.C \
test2006_18.C \
test2006_19.C \
test2006_20.C \
test2006_22.C \
test2006_23.C \
test2006_25.C \
test2006_26.C \
test2006_28.C \
test2006_34.C \
test2006_37.C \
test2006_38.C \
test2006_39.C \
test2006_40.C \
test2006_41.C \
test2006_42.C \
test2006_44.C \
test2006_46.C \
test2006_47.C \
test2006_50.C \
test2006_52.C \
test2006_53.C \
test2006_54.C \
test2006_55.C \
test2006_56.C \
test2006_57.C \
test2006_58.C \
test2006_59.C \
test2006_61.C \
test2006_62.C \
test2006_63.C \
test2006_66.C \
test2006_67.C \
test2006_68.C \
test2006_69.C \
test2006_70.C \
test2006_71.C \
test2006_72.C \
test2006_73.C \
test2006_74.C \
test2006_75.C \
test2006_76.C \
test2006_77.C \
test2006_78.C \
test2006_79.C \
test2006_80.C \
test2006_81.C \
test2006_82.C \
test2006_83.C \
test2006_85.C \
test2006_87.C \
test2006_88.C \
test2006_89.C \
test2006_90.C \
test2006_91.C \
test2006_92.C \
test2006_93.C \
test2006_94.C \
test2006_95.C \
test2006_96.C \
test2006_97.C \
test2006_98.C \
test2006_99.C \
test2006_101.C \
test2006_102.C \
test2006_103.C \
test2006_104.C \
test2006_105.C \
test2006_106.C \
test2006_107.C \
test2006_108.C \
test2006_109.C \
test2006_111.C \
test2006_113.C \
test2006_114.C \
test2006_115.C \
test2006_116.C \
test2006_117.C \
test2006_118.C \
test2006_119.C \
test2006_120.C \
test2006_122.C \
test2006_123.C \
test2006_124.C \
test2006_126.C \
test2006_129.C \
test2006_140.C \
test2006_141.C \
test2006_142.C \
test2006_143.C \
test2006_144.C \
test2006_145.C \
test2006_146.C \
test2006_148.C \
test2006_149.C \
test2006_150.C \
test2006_152.C \
test2006_153.C \
test2006_154.C \
   test2006_155.C \
   test2006_156.C \
   test2006_157.C \
   test2006_158.C \
test2006_160.C \
test2006_161.C \
test2006_162.C \
test2006_163.C \
test2006_164.C \
test2006_165.C \
test2006_166.C \
test2006_177.C \
test2006_178.C \
test2006_179.C \
test2006_181.C \
test2006_183.C \
test2006_185.C \
test2006_189.C \
test2007_02.C \
test2007_03.C \
test2007_04.C \
test2007_06.C \
test2007_07.C \
test2007_08.C \
test2007_09.C \
test2007_10.C \
test2007_11.C \
test2007_12.C \
test2007_14.C \
   test2007_15.C \
test2007_16.C \
test2007_21.C \
test2007_23.C \
test2007_24.C \
test2007_25.C \
test2007_26.C \
test2007_28.C \
test2007_29.C \
test2007_38.C \
test2007_40.C \
test2007_41.C \
test2007_43.C \
test2007_44.C \
test2007_46.C \
test2007_47.C \
test2007_48.C \
test2007_49.C \
test2007_50.C \
test2007_51.C \
test2007_52.C \
test2007_53.C \
test2007_57.C \
test2007_59.C \
test2007_62.C \
test2007_70.C \
test2007_72.C \
test2007_75.C \
test2007_77.C \
test2007_81.C \
test2007_82.C \
test2007_83.C \
test2007_84.C \
test2007_85.C \
test2007_90.C \
test2007_94.C \
test2007_98.C \
test2007_102.C \
test2007_103.C \
test2007_128.C \
test2007_130.C \
test2007_133.C \
test2007_135.C \
test2007_137.C \
test2007_140.C \
test2007_141.C \
   test2007_142.C \
test2007_147.C \
test2007_150.C \
test2007_151.C \
test2007_152.C \
test2007_154.C \
test2007_155.C \
test2007_157.C \
test2007_158.C \
test2007_159.C \
test2007_160.C \
test2007_161.C \
test2007_162.C \
test2007_163.C \
test2007_164.C \
test2007_165.C \
test2007_166.C \
test2007_167.C \
test2007_169.C \
test2007_170.C \
test2007_171.C \
test2007_172.C \
test2007_173.C \
test2008_01.C \
test2008_03.C \
test2008_05.C \
test2008_06.C \
test2008_08.C \
test2008_09.C \
test2008_10.C \
test2008_11.C \
test2008_12.C \
test2008_13.C \
test2008_14.C \
test2008_16.C \
test2009_01.C \
test2009_02.C \
test2009_03.C \
test2009_04.C \
test2009_05.C \
test2009_06.C \
test2009_08.C \
test2009_09.C \
test2009_10.C \
test2009_11.C \
test2009_12.C \
test2009_14.C \
test2009_15.C \
test2009_18.C \
test2009_19.C \
test2009_20.C \
test2009_21.C \
test2009_24.C \
test2009_27.C \
test2009_28.C \
test2009_30.C \
test2009_39.C \
test2009_40.C \
test2009_41.C \
test2009_42.C \
test2010_03.C \
test2010_04.C \
test2010_05.C \
test2010_06.C \
test2010_18.C \
test2010_19.C \
test2011_01.C \
test2011_02.C \
test2011_05.C \
test2011_06.C \
test2011_07.C \
test2011_19.C \
test2011_20.C \
test2011_21.C \
test2011_22.C \
test2011_23.C \
test2011_24.C \
test2011_25.C \
test2011_26.C \
test2011_27.C \
test2011_28.C \
test2011_29.C \
test2011_30.C \
test2011_31.C \
test2011_32.C \
test2011_33.C \
test2011_34.C \
test2011_35.C \
test2011_36.C \
test2011_37.C \
test2011_38.C \
test2011_39.C \
test2011_40.C \
test2011_41.C \
test2011_42.C \
test2011_43.C \
test2011_44.C \
test2011_45.C \
test2011_46.C \
test2011_47.C \
test2011_48.C \
test2011_49.C \
test2011_50.C \
test2011_51.C \
test2011_52.C \
test2011_53.C \
test2011_54.C \
test2011_55.C \
test2011_56.C \
test2011_57.C \
test2011_58.C \
test2011_59.C \
test2011_60.C \
test2011_61.C \
test2011_62.C \
test2011_63.C \
test2011_64.C \
test2011_67.C \
test2011_68.C \
test2011_69.C \
test2011_70.C \
test2011_71.C \
test2011_72.C \
test2011_73.C \
test2011_74.C \
test2011_75.C \
test2011_76.C \
test2011_77.C \
test2011_78.C \
test2011_79.C \
test2011_80.C \
test2011_81.C \
test2011_82.C \
test2011_83.C \
test2011_84.C \
test2011_85.C \
test2011_86.C \
test2011_87.C \
test2011_88.C \
test2011_90.C \
test2011_91.C \
test2011_92.C \
test2011_93.C \
test2011_94.C \
test2011_95.C \
test2011_96.C \
test2011_97.C \
test2011_98.C \
test2011_99.C \
test2011_100.C \
test2011_101.C \
test2011_110.C \
test2011_111.C \
test2011_115.C \
test2011_117.C \
test2011_118.C \
test2011_119.C \
test2011_120.C \
test2011_133.C \
test2011_135.C \
test2011_136.C \
test2011_137.C \
test2011_138.C \
test2011_139.C \
test2011_140.C \
test2011_141.C \
test2011_142.C \
test2011_143.C \
test2011_146.C \
test2011_147.C \
   test2005_112.C \
   test2005_117.C \
   test2005_154.C \
   test2005_163.C \
   test2005_179.C \
   test2005_183.C \
   test2005_194.C \
   test2005_199.C \
   test2006_84.C \
   test2006_86.C \
   test2006_121.C \
   test2006_139.C \
   test2006_147.C \
   test2006_151.C \
   test2006_159.C \
   test2007_01.C \
   test2007_13.C \
   test2007_17.C \
   test2007_55.C \
   test2007_78.C \
   test2008_04.C \
test_int_lexemes.C \
test_int_lexemes_donot_pass.C \
test_simple_int.C \
test_wchars.C \
lexPhase2003_01.C \
math.C \
inputForLoopLocator.C \
X.C \
inputBug317.C \
inputBug327.C \
test2005_09.C \
test2005_104.C \
test2010_24.C 




# DQ (12/7/2012): Failing tests.
# test2001_22.C 
# test2003_08.C
# test2004_51.C 
# test2004_86.C 
# test2005_41.C



LOCAL_EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
astFileIOtest2012_01.C 

endif

# These currently fail (will be fixed by Alin later when ROSE works better with template functions)
# template_function.C
# template_class.C

# These codes are DESIGNED to fail in the inlining
# fail1.C fail2.C

# DQ (2/13/2010): Optionally make this a longer test.
# TESTCODES += $(EXTRA_TESTCODES)

# DQ (2/20/2010): Use the standard list of test codes (if possible)
TESTCODES = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
LOCAL_TESTCODES = $(LOCAL_EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}
LOCAL_TEST_Objects = ${LOCAL_TESTCODES:.C=.o}

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=80 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND_OPTIONS = --tool=memcheck -v --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skip_unparse_asm_commands

# TEST_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileIO $(ROSE_FLAGS)
# READ_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileRead $(ROSE_FLAGS)
# TEST_TRANSLATOR = ./astFileIO $(ROSE_FLAGS)
TEST_TRANSLATOR = $(top_builddir)/tests/testAstFileIO $(ROSE_FLAGS)

# DQ (2/23/2010): This is the new AST Reader (merges function type tables, etc.)
# READ_TRANSLATOR = ./astFileRead $(ROSE_FLAGS)
READ_TRANSLATOR = $(VALGRIND) $(top_builddir)/tests/testAstFileRead
# READ_TRANSLATOR = $(top_builddir)/tests/parallelASTMerge

TESTCODE_INCLUDES = \
	-I$(top_srcdir)/tests/CompileTests/A++Code \
	-I$(srcdir)/../../CompileTests/Cxx_tests

../../testAstFileIO:
	make -C ../.. testAstFileIO
../../testAstFileRead:
	make -C ../.. testAstFileRead

# DQ (7/12/2004): Modified to run with make -j4 options
# $(TEST_Objects): astFileIO astFileRead $(TESTCODES)
$(TEST_Objects): astFileIO astFileRead ../../testAstFileIO
#	@echo "QY:skipping test code using $(TEST_TRANSLATOR) ..."
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	$(VALGRIND) $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C) -o $(@:.o=)
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir)/../../CompileTests/Cxx_tests -c $(srcdir)/../../CompileTests/Cxx_tests/$(@:.o=.C) -o $(@:.o=)
	$(VALGRIND) $(TEST_TRANSLATOR) $(TESTCODE_INCLUDES) -c $(srcdir)/../../CompileTests/Cxx_tests/$(@:.o=.C)
#	@echo "Running resulting executable ..."
#	./$(@:.o=)

# EXTRA_DIST = $(TESTCODES) test2001_05.h
# EXTRA_DIST = input_tiny_01.C input_tiny_02.C
EXTRA_DIST = input_tiny_01a.C  input_tiny_01b.C  input_tiny_02a.C  input_tiny_02b.C  input_tiny_03a.C  input_tiny_03b.C

$(LOCAL_TEST_Objects): astFileIO astFileRead ../../testAstFileIO
#	@echo "QY:skipping test code using $(TEST_TRANSLATOR) ..."
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	$(VALGRIND) $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C) -o $(@:.o=)
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir)/../../CompileTests/Cxx_tests -c $(srcdir)/../../CompileTests/Cxx_tests/$(@:.o=.C) -o $(@:.o=)
	$(VALGRIND) $(TEST_TRANSLATOR) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C)

test2001_01.C.binary: test2001_01.o

test2001_02.C.binary: test2001_02.o

test2001_03.C.binary: test2001_03.o

# This tests the reading of a collection of binary files (build by running $(MAKE) $(TEST_Objects)
# test-read: test2001_01.binary test2001_02.binary test2001_03.binary
test-read: test2001_01.C.binary test2001_02.C.binary test2001_03.C.binary ../../testAstFileRead
#	./astFileRead rose_test2001_01.C rose_test2001_02.C rose_test2001_03.C
#	$(READ_TRANSLATOR) test2001_01.C test2001_02.C test2001_03.C
	$(READ_TRANSLATOR) test2001_01.C test2001_02.C temp_output.C

#TEST_Binaries_short = test2003_*.C.binary
#TEST_Binaries_shortTrunk = $(TEST_Binaries_short:.binary=)
#test-read-medium: $(TEST_Binaries_short)
##	$(READ_TRANSLATOR) $(TEST_Binaries_short:.binary=)
#	$(READ_TRANSLATOR) `$(TEST_Binaries_shortTrunk)`

TEST_TINY_LIST = input_tiny_01.C input_tiny_02.C
# TEST_TINY_LIST_BINARIES = $(TEST_TINY_LIST:.C=.o)

# test-read-tiny: ../../testAstFileIO ../../testAstFileRead
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_01.C
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_02.C
#	$(READ_TRANSLATOR) $(TEST_TINY_LIST) temp_output_tiny.C

# test-read-tiny2: ../../testAstFileIO ../../testAstFileRead
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny2_01.C
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny2_02.C
##	$(READ_TRANSLATOR) input_tiny2_01.C input_tiny2_02.C temp_output_tiny2.C
#	$(READ_TRANSLATOR) input_tiny2_01.C input_tiny2_02.C temp_output_tiny2.C 

if !ROSE_USE_EDG_VERSION_4
   TEST_SHORT_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C
else
# DQ (12/7/2012): Removing test2003_08.C until it is supported in seperate AST File I/O tests.
   TEST_SHORT_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C
endif

# TEST_SHORT_LIST_BINARIES = $(TEST_SHORT_LIST:.C=.C.binary)
TEST_SHORT_LIST_BINARIES = $(TEST_SHORT_LIST:.C=.o)

test-read-short: $(TEST_SHORT_LIST_BINARIES) ../../testAstFileRead
	$(READ_TRANSLATOR) $(TEST_SHORT_LIST) temp_output_short.C

# 34
if !ROSE_USE_EDG_VERSION_4
ORIGINAL_TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C \
   test2003_16.C test2003_18.C test2003_20.C test2003_23.C test2003_25.C test2003_27.C test2003_29.C test2003_31.C \
   test2003_35.C test2003_37.C test2003_02.C test2003_04.C test2003_06.C test2003_09.C test2003_11.C test2003_13.C \
   test2003_15.C test2003_17.C test2003_19.C test2003_21.C test2003_24.C test2003_26.C test2003_28.C test2003_30.C \
   test2003_32.C test2003_36.C test2003_38.C
else
# DQ (12/7/2012): Removing test2003_24.C until it is supported in seperate AST File I/O tests.
ORIGINAL_TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C \
   test2003_16.C test2003_18.C test2003_20.C test2003_23.C test2003_25.C test2003_27.C test2003_29.C test2003_31.C \
   test2003_35.C test2003_37.C test2003_02.C test2003_04.C test2003_06.C test2003_09.C test2003_11.C test2003_13.C \
   test2003_15.C test2003_17.C test2003_19.C test2003_21.C test2003_26.C test2003_28.C test2003_30.C \
   test2003_32.C test2003_36.C test2003_38.C
endif

# DQ (6/23/2010): Debug using a smaller set of files.
# TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C test2003_35.C test2003_37.C 
# TEST_MEDIUM_LIST = test2003_01.C test2003_37.C 
# TEST_MEDIUM_LIST = test2003_15.C test2003_17.C test2003_19.C test2003_21.C
# TEST_MEDIUM_LIST = test2003_19.C test2003_21.C
# TEST_MEDIUM_LIST = test2003_19.C test2003_21.C
# TEST_MEDIUM_LIST = test2003_21.C
# TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C test2003_35.C test2003_37.C 
# TEST_MEDIUM_LIST = $(ORIGINAL_TEST_MEDIUM_LIST)
# TEST_MEDIUM_LIST = test2003_02.C test2004_52.C

# 72 files appear to merge and write out an AST, but 73 files always fail!
TEST_MEDIUM_LIST_EXTRA = \
test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_30.C  test2004_47.C test2004_63.C  test2004_79.C  test2004_95.C \
test2004_02.C   test2004_107.C  test2004_120.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
test2004_03.C   test2004_108.C  test2004_121.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C \
test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_33.C  test2004_67.C  test2004_82.C  test2004_98.C test2004_164.C

# Problem files:
# test2004_161.C 
# test2004_136.C

# DQ (9/24/2011): These files are a problem for the new original expression tree support (replacing constant values with the original expression trees).
# test2004_97.C

TEST_MEDIUM_PROBLEM_FILE_LIST = test2004_136.C test2004_161.C

# /home/dquinlan/ROSE/git-dq-main-rc/src/ROSETTA/Grammar/grammarAST_FileIoSource.code:363: static long unsigned int AST_FILE_IO::getGlobalIndexFromSgClassPointer(SgNode*): Assertion `globalIndex < AST_FILE_IO::getAccumulatedPoolSizeOfNewAst(pointer->variantT()+1)' failed.
# test2004_50.C 

# One of these (large) files is a problem
# test2004_135.C  test2004_149.C  

ALT2_TEST_MEDIUM_LIST_EXTRA = test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C \
test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C  test2004_82.C  test2004_98.C \
test2004_69.C   test2004_84.C test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  \
test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
test2004_08.C   test2004_112.C  test2004_140.C  test2004_154.C test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C 

# These files are a problem for the "medium" size test.
# test2004_85.C 
# test2004_127.C  
TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_85.C test2004_127.C

ALT3_TEST_MEDIUM_LIST_EXTRA = \
test2004_100.C  test2004_114.C  test2004_129.C  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
test2004_102.C  test2004_116.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_90.C 

# Problem files:
# test2004_101.C  
# test2004_115.C  
# test2004_12.C   
# test2004_143.C 
# test2004_157.C
# test2004_130.C  
# test2004_75.C 
# test2004_103.C  
# test2004_117.C  
# test2004_145.C 
# test2004_159.C 
# test2004_27.C  
# test2004_43.C  
# test2004_60.C  
TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_101.C test2004_115.C test2004_12.C test2004_143.C test2004_157.C test2004_130.C test2004_75.C test2004_103.C test2004_117.C test2004_145.C test2004_159.C test2004_27.C test2004_43.C test2004_60.C  

ALT4_TEST_MEDIUM_LIST_EXTRA = \
test2004_131.C  test2004_76.C test2004_91.C \
test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C  test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C

# Problem files:
# test2004_93.C 
# test2004_119.C 
# test2004_133.C  
# test2004_147.C 
# test2004_160.C 
# test2004_29.C 
# test2004_45.C 
# test2004_62.C 
# test2004_78.C
# test2004_94.C

# src/frontend/SageIII/sageInterface/generateUniqueName.C:494: std::string SageInterface::generateUniqueName(SgNode*, bool): Assertion `key.empty() == false' failed.
# test2004_105.C 

TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_93.C test2004_119.C test2004_133.C test2004_147.C test2004_160.C test2004_29.C test2004_45.C test2004_62.C test2004_78.C test2004_94.C

# DQ (7/10/2010): Deal with this later!
# test2004_105.C


if !ROSE_USE_EDG_VERSION_4
ALT_TEST_MEDIUM_LIST_EXTRA = \
test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_161.C  test2004_30.C  test2004_47.C  test2004_63.C  test2004_79.C  test2004_95.C \
test2004_02.C   test2004_107.C  test2004_120.C  test2004_135.C  test2004_149.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
test2004_03.C   test2004_108.C  test2004_121.C  test2004_136.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C   \
test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C  test2004_82.C  test2004_98.C \
test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C  test2004_69.C  test2004_84.C \
test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C   test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  test2004_85.C \
test2004_08.C   test2004_112.C  test2004_127.C  test2004_140.C  test2004_154.C  test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C \
test2004_100.C  test2004_114.C  test2004_129.C  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
test2004_101.C  test2004_115.C  test2004_12.C   test2004_143.C  test2004_157.C  test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
test2004_102.C  test2004_116.C  test2004_130.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_75.C  test2004_90.C \
test2004_103.C  test2004_117.C  test2004_131.C  test2004_145.C  test2004_159.C  test2004_27.C   test2004_43.C  test2004_60.C  test2004_76.C  test2004_91.C \
test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C   test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C  test2004_93.C \
test2004_105.C  test2004_119.C  test2004_133.C  test2004_147.C  test2004_160.C  test2004_29.C   test2004_45.C  test2004_62.C  test2004_78.C  test2004_94.C
else
# DQ (12/7/2012): Removing test2004_82.C until it is supported in seperate AST File I/O tests.
ALT_TEST_MEDIUM_LIST_EXTRA = \
test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_161.C  test2004_30.C  test2004_47.C  test2004_63.C  test2004_79.C  test2004_95.C \
test2004_02.C   test2004_107.C  test2004_120.C  test2004_135.C  test2004_149.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
test2004_03.C   test2004_108.C  test2004_121.C  test2004_136.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C   \
test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C  test2004_98.C \
test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_68.C  test2004_83.C  test2004_99.C \
test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C  test2004_69.C  test2004_84.C \
test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C   test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  test2004_85.C \
test2004_08.C   test2004_112.C  test2004_127.C  test2004_140.C  test2004_154.C  test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  \
test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C \
test2004_100.C  test2004_114.C  test2004_129.C  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
test2004_101.C  test2004_115.C  test2004_12.C   test2004_143.C  test2004_157.C  test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
test2004_102.C  test2004_116.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_75.C  test2004_90.C \
test2004_103.C  test2004_117.C  test2004_131.C  test2004_145.C  test2004_159.C  test2004_27.C   test2004_43.C  test2004_60.C  test2004_76.C  test2004_91.C \
test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C   test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C  test2004_93.C \
test2004_105.C  test2004_119.C  test2004_133.C  test2004_147.C  test2004_160.C  test2004_29.C   test2004_45.C  test2004_62.C  test2004_78.C  test2004_94.C
endif

# DQ (9/24/2011): These files are a problem for the new original expression tree support (replacing constant values with the original expression trees).
# test2004_97.C


# This make this a much larger test.
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST = $(ALT4_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(ALT4_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
TEST_MEDIUM_LIST = $(ALT_TEST_MEDIUM_LIST_EXTRA)

# Error (only works as seperate sets)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA) $(ALT4_TEST_MEDIUM_LIST_EXTRA)

# Error these will not merge (though they merge separately).
# TEST_MEDIUM_LIST += $(ALT3_TEST_MEDIUM_LIST_EXTRA)

# Error these will not merge (though they merge separately).
# TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA) $(ALT4_TEST_MEDIUM_LIST_EXTRA)

# Error these will not merge (though they merge separately).
# TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA) test2004_12.C

# Changing hte order causes an error 12,75,27 works but 12,27,75 fails
# TEST_MEDIUM_LIST = test2004_12.C test2004_75.C test2004_27.C 
# TEST_MEDIUM_LIST = test2004_12.C test2004_27.C test2004_75.C 




TEST_MEDIUM_LIST_BINARIES = $(TEST_MEDIUM_LIST:.C=.o)

test-read-medium: $(TEST_MEDIUM_LIST_BINARIES) ../../testAstFileRead
	$(READ_TRANSLATOR) $(TEST_MEDIUM_LIST) temp_output_medium.C

# FAILING_TEST_MEDIUM_LIST = $(TEST_MEDIUM_LIST) $(TEST_MEDIUM_PROBLEM_FILE_LIST)
# FAILING_TEST_MEDIUM_LIST = $(TEST_MEDIUM_PROBLEM_FILE_LIST)
FAILING_TEST_MEDIUM_LIST = test2004_105.C
FAILING_TEST_MEDIUM_LIST_BINARIES = $(FAILING_TEST_MEDIUM_LIST:.C=.o)

failing-test-read-medium: $(FAILING_TEST_MEDIUM_LIST_BINARIES) ../../testAstFileRead
	$(READ_TRANSLATOR) $(FAILING_TEST_MEDIUM_LIST) temp_output_medium.C


TEST_LARGE_LIST_BINARIES = $(TESTCODES:.C=.o)
TEST_Binaries = $(TESTCODES:.C=.C.binary)
# test-read-large: test2001_01.C.binary test2001_02.C.binary test2001_03.C.binary
test-read-large: $(TEST_LARGE_LIST_BINARIES) ../../testAstFileRead
	ls -s $(TEST_Binaries)
#	@echo "$(READ_TRANSLATOR) $(TEST_Binaries:.binary=)"
	$(READ_TRANSLATOR) $(TEST_Binaries:.C.binary=.C) temp_output_large.C

test-read-huge: ../../testAstFileRead
	$(READ_TRANSLATOR) temp_output_large.C temp_output_large.C temp_output_large.C temp_output_large.C temp_output_huge.C

test-read-really-huge: ../../testAstFileRead
	$(READ_TRANSLATOR) temp_output_huge.C temp_output_huge.C temp_output_huge.C temp_output_huge.C temp_output_really_huge.C


FailingTests = test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C test2003_21.C test2003_19.C test2003_17.C test2003_13.C
FailingTests_Binaries = $(FailingTests:.C=.o)
failingTest: $(FailingTests_Binaries)
#	../../../tests/testAstFileRead test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C test2003_21.C test2003_19.C test2003_17.C test2003_13.C temp_output.C
	../../../tests/testAstFileRead test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C

# test2003_21.C 
# test2003_19.C 
# test2003_17.C test2003_13.C temp_output.C

TEST_PARALLEL_SHORT_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C
TEST_PARALLEL_SHORT_LIST_BINARIES = $(TEST_PARALLEL_SHORT_LIST:.C=.o)
testParallelMerge-short: parallelMerge $(TEST_PARALLEL_SHORT_LIST_BINARIES)
	rm -rf data temp
	mkdir data
	mkdir temp
	./parallelMerge $(TEST_PARALLEL_SHORT_LIST) temp_output_parallel_short.C

# TEST_PARALLEL_MEDIUM_LIST = $(ORIGINAL_TEST_MEDIUM_LIST)
# TEST_PARALLEL_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C 
if !ROSE_USE_EDG_VERSION_4
TEST_PARALLEL_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C \
test2003_18.C test2003_20.C test2003_23.C test2003_25.C test2003_27.C test2003_35.C test2003_02.C test2003_06.C test2003_09.C test2003_11.C \
test2003_15.C test2003_17.C test2003_21.C test2003_24.C test2003_26.C test2003_28.C test2003_30.C test2003_32.C test2003_36.C 
else
# DQ (12/7/2012): Removing test2003_24.C until it is supported in seperate AST File I/O tests.
TEST_PARALLEL_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C \
test2003_18.C test2003_20.C test2003_23.C test2003_25.C test2003_27.C test2003_35.C test2003_02.C test2003_06.C test2003_09.C test2003_11.C \
test2003_15.C test2003_17.C test2003_21.C test2003_26.C test2003_28.C test2003_30.C test2003_32.C test2003_36.C 
endif

# This longer list fails when run in parallel, need to debug this.
# TEST_PARALLEL_MEDIUM_LIST = $(TEST_MEDIUM_LIST)

# Problem test for parallel tests:
# test2003_08.C 
# test2003_16.C 
# test2003_29.C
# test2003_31.C 
# test2003_37.C 
# test2003_04.C
# test2003_13.C 
# test2003_19.C 
# test2003_38.C

TEST_PARALLEL_MEDIUM_LIST_BINARIES = $(TEST_PARALLEL_MEDIUM_LIST:.C=.o)
testParallelMerge-medium: parallelMerge $(TEST_PARALLEL_MEDIUM_LIST_BINARIES)
	rm -rf data temp
	mkdir data
	mkdir temp
	./parallelMerge $(TEST_PARALLEL_MEDIUM_LIST) temp_output_parallel_medium.C

# This is a test of the same files used with parallel test but run in serial.
# This is different and usually faster for small problems than the singled threaded parallel merge.
testParellelMerge-medium-serial: $(TEST_PARALLEL_MEDIUM_LIST_BINARIES)
	$(READ_TRANSLATOR) $(TEST_PARALLEL_MEDIUM_LIST) temp_output_parallel_medium_serial.C

TEST_PARALLEL_LARGE_LIST = $(TESTCODES)
TEST_PARALLEL_LARGE_LIST_BINARIES = $(TEST_PARALLEL_LARGE_LIST:.C=.o)
testParallelMerge-large: parallelMerge $(TEST_PARALLEL_LARGE_LIST_BINARIES)
	rm -rf data temp
	mkdir data
	mkdir temp
	./parallelMerge $(TEST_PARALLEL_LARGE_LIST) temp_output_parallel_large.C

# DQ (3/1/2010): This is the sart of a general command to process all the
# binaries in a directory structure.
ls_generatedBinaries:
#	ls -l `find $(top_builddir)/src -name "*.C.binary" -print`
#	cd $(top_builddir)/src; ls -l `find . -name "*.C.binary" -print`
#	cd $(top_builddir); du -a * | grep "C.binary"
	find $(top_builddir) -name "*.C.binary" -print

ls_binaries:
	ls $(TEST_Binaries)

# TEST_Binaries_short = test2003_*.binary
ls_binaries_short:
	ls -l $(TEST_Binaries_shortTrunk)
#	ls -l $(TEST_Binaries_short:.binary=)


# This tests the compression of the AST internally, a step that makes it contiguious in 
# memory by copying it to intermediate storage and then rebuilding it from scratch.
# Putting the AST into blocks of contiguious storage is what makes the binary file I/O mechanism fast.
# This is a great test of all but the the binary file I/O part (which is the simpler part).
testCompression: astCompressionTest
	./astCompressionTest -c $(srcdir)/test2001_01.C

largeFileTest: astFileIO
	./astFileIO -rose:verbose 0 -c $(srcdir)/test2005_36.C -o test2005_36

largestFileTest: astFileIO
	./astFileIO -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar

largestRoseFileTest: astFileIO ../../testAstFileIO
#	./astFileIO -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar
	$(TEST_TRANSLATOR) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRose_testFile.C

locForRoseFile:
	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C | wc -l

test-read-tiny_01:
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_01a.C
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_01b.C
	$(READ_TRANSLATOR) input_tiny_01a.C input_tiny_01b.C temp_output_tiny_01.C 

test-read-tiny_02:
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_02a.C
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_02b.C
	$(READ_TRANSLATOR) input_tiny_02a.C input_tiny_02b.C temp_output_tiny_02.C 

test-read-tiny_03:
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_03a.C
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_03b.C
	$(READ_TRANSLATOR) input_tiny_03a.C input_tiny_03b.C temp_output_tiny_03.C 

testFileGeneration:
	$(MAKE) $(TEST_Objects)

check-local:
	$(MAKE) $(TEST_Objects)
	$(MAKE) test-read
	$(MAKE) test-read-tiny_01
	$(MAKE) test-read-tiny_02
	$(MAKE) test-read-tiny_03
	$(MAKE) test-read-short
# Liao 2/9/2011. boost thread_group may have bug on Mac OS X 10.6
if !OS_MACOSX	
	$(MAKE) testParallelMerge-short
	$(MAKE) testParallelMerge-medium
	$(MAKE) testParellelMerge-medium-serial
endif
if ROSE_USE_LONG_MAKE_CHECK_RULE
#	DQ (7/4/2010): Make this only if running longer tests.
	$(MAKE) test-read-medium
#	This is a bit too large of a test to run on most people's machines so skip it for now.
#	$(MAKE) test-read-large
endif
	@echo ""
	@echo "*******************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astFileIOTests: make check rule complete (terminated normally) ***"
	@echo "*******************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files pass[1-9] pass1[0-9] pass2[0-9] rose_*.C *.ti *.binary *.C.pdf test1 test200?_*
	rm -rf test_int_lexemes  test_int_lexemes_donot_pass  test_simple_int  test_wchars  X Cxx_Grammar
	rm -f *.dot *.C_identity inputBug317  inputBug327  inputForLoopLocator  lexPhase2003_01  math  test2010_03  test2010_04  test2010_05  test2010_06  test_CplusplusMacro_Cpp
#	Remove some generated files by the parallel merge.
	rm -f *.txt
	rm -rf tmp? data temp_output_*

distclean-local:
	rm -rf Templates.DB 

test1: test1A.C test1B.C
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	DQ(7/13/2004): This is an error since it specifies the output file twice in the vendor compile line
#	$(TEST_TRANSLATOR) -rose:verbose 0 -o test1A.o -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1B.C
	g++ -o test1 rose_test1A.o rose_test1B.o
	@echo "Running resulting executable ..."
	./test1

# DQ (2/5/2009): This is a demonstration that the AST generated 
# from a binary executable fails when being written to disk.
# Ideas about how to simplify this problem:
#  1) Skip the dwarf information to make the problem smaller
#
# Example from Thomas.
test-binary: astFileIO
	./astFileIO -rose:verbose 0 $(srcdir)/buffer2.bin
