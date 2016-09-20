include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
EXTRA_DIST =  tests
bin_PROGRAMS =
MOSTLYCLEANFILES =

AM_CPPFLAGS = $(ROSE_INCLUDES) 

EDG_VERSION = @ROSE_EDG_MAJOR_VERSION_NUMBER@

#------------------------------------------------------------------------------------------------------------------------
# Scope matching
bin_PROGRAMS += testScopeMatching
testScopeMatching_SOURCES = testScopeMatching.C  
testScopeMatching_LDADD = $(ROSE_LIBS)

# NOTE: testScopeMatching was apparently never tested.

#------------------------------------------------------------------------------------------------------------------------
# Linearization
bin_PROGRAMS += testLinearization
testLinearization_SOURCES = testLinearization.C  
testLinearization_LDADD = $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# Type Transformations
bin_PROGRAMS += testTypeTransformation
# testSctestTypeTransformation_SOURCES = testTypeTransformation.C
testTypeTransformation_SOURCES = testTypeTransformation.C
testTypeTransformation_LDADD = $(ROSE_LIBS)

LIN_SPECIMENS = testLin1.c testLin2.c
LIN_TEST_TARGETS = $(addprefix lin_, $(addsuffix .passed, $(LIN_SPECIMENS)))
LIN_TEST_CONFIG = $(srcdir)/testLinearization.conf
$(LIN_TEST_TARGETS): lin_%.passed: ${srcdir}/tests/% testLinearization $(LIN_TEST_CONFIG)
	@$(RTH_RUN) SPECIMEN=$< EDG=$(EDG_VERSION) $(LIN_TEST_CONFIG) $@

EXTRA_DIST += $(LIN_TEST_CONFIG)

MOSTLYCLEANFILES += $(LIN_TEST_TARGETS) $(LIN_TEST_TARGETS:.passed=.failed)



#------------------------------------------------------------------------------------------------------------------------
# Token representation

TESTCODES_REQUIRED_TO_PASS = \
     test_CplusplusMacro_C.c \
     gconv_info.c \
     testCvsCpp.c \
     testAnsiC.c \
     commentTest.c \
     YardenPragmaPackExample.c \
     test2003_12.c \
     test2004_20.c \
     test2005_172.c \
     test2005_186.c \
     test2006_110.c \
     test2006_114.c \
     test2006_133.c \
     test2006_135.c \
     test2007_177.c \
     test2008_01.c \
     test2008_02.c \
     test2009_01.c \
     test2009_02.c \
     test2009_03.c \
     test2009_04.c \
     test2009_07.c \
     test2009_08.c \
     test2009_16.c \
     test2009_21.c \
     test2009_22.c \
     test2010_01.c \
     test2010_06.c \
     test2010_07.c \
     test2010_08.c \
     test2010_09.c \
     test2010_13.c \
     test2010_15.c \
     test2010_17.c \
     test2011_05.c \
     test2012_01.c \
     test2012_03.c \
     test2012_04.c \
     test2012_05.c \
     test2012_06.c \
     test2012_07.c \
     test2012_08.c \
     test2012_09.c \
     test2012_10.c \
     test2012_11.c \
     test2012_12.c \
     test2012_13.c \
     test2012_14.c \
     test2012_15.c \
     test2012_16.c \
     test2012_18.c \
     test2012_20.c \
     test2012_21.c \
     test2012_22.c \
     test2012_23.c \
     test2012_24.c \
     test2012_25.c \
     test2012_26.c \
     test2012_27.c \
     test2012_29.c \
     test2012_32.c \
     test2012_33.c \
     test2012_34.c \
     test2012_35.c \
     test2012_36.c \
     test2012_37.c \
     test2012_38.c \
     test2012_39.c \
     test2012_40.c \
     test2012_42.c \
     test2012_41.c \
     test2012_43.c \
     test2012_46.c \
     test2012_47.c \
     test2012_48.c \
     test2012_49.c \
     test2012_50.c \
     test2012_51.c \
     test2012_52.c \
     test2012_53.c \
     test2012_54.c \
     test2012_55.c \
     test2012_56.c \
     test2012_57.c \
     test2012_58.c \
     test2012_59.c \
     test2012_60.c \
     test2012_61.c \
     test2012_62.c \
     test2012_64.c \
     test2012_65.c \
     test2012_66.c \
     test2012_67.c \
     test2012_68.c \
     test2012_69.c \
     test2012_70.c \
     test2012_71.c \
     test2012_72.c \
     test2012_73.c \
     test2012_74.c \
     test2012_75.c \
     test2012_76.c \
     test2012_77.c \
     test2012_79.c \
     test2012_80.c \
     test2012_81.c \
     test2012_82.c \
     test2012_83.c \
     test2012_84.c \
     test2012_85.c \
     test2012_86.c \
     test2012_87.c \
     test2012_88.c \
     test2012_89.c \
     test2012_90.c \
     test2012_92.c \
     test2012_93.c \
     test2012_94.c \
     test2012_95.c \
     test2012_96.c \
     test2012_97.c \
     test2012_98.c \
     test2012_99.c \
     test2012_100.c \
     test2012_101.c \
     test2012_102.c \
     test2012_103.c \
     test2012_104.c \
     test2012_105.c \
     test2012_106.c \
     test2012_107.c \
     test2012_109.c \
     test2012_110.c \
     test2012_111.c \
     test2012_112.c \
     test2012_113.c \
     test2012_114.c \
     test2012_115.c \
     test2012_116.c \
     test2012_117.c \
     test2012_118.c \
     test2012_119.c \
     test2012_120.c \
     test2012_121.c \
     test2012_122.c \
     test2012_123.c \
     test2012_124.c \
     test2012_125.c \
     test2012_126.c \
     test2012_127.c \
     test2012_129.c \
     test2012_130.c \
     test2012_131.c \
     test2012_133.c \
     test2012_134.c \
     test2012_135.c \
     test2012_136.c \
     test2012_137.c \
     test2012_139.c \
     test2012_140.c \
     test2012_141.c \
     test2012_142.c \
     test2012_143.c \
     test2012_144.c \
     test2012_146.c \
     test2012_147.c \
     test2012_148.c \
     test2012_149.c \
     test2012_150.c \
     test2012_151.c \
     test2012_152.c \
     test2012_153.c \
     test2012_154.c \
     test2012_156.c \
     test2012_157.c \
     test2012_158.c \
     test2012_159.c \
     test2012_162.c \
     test2012_164.c \
     test2012_165.c \
     test2012_166.c \
     test2012_167.c \
     test2012_168.c \
     test2012_169.c \
     test2012_170.c \
     test2012_171.c \
     test2012_172.c \
     test2012_174.c \
     test2012_175.c \
     test2013_01.c \
     test2013_02.c \
     test2013_03.c \
     test2013_05.c \
     test2013_06.c \
     test2013_07.c \
     test2013_08.c \
     test2013_09.c \
     test2013_10.c \
     test2013_11.c \
     test2013_12.c \
     test2013_13.c \
     test2013_14.c \
     test2013_15.c \
     test2013_16.c \
     test2013_18.c \
     test2013_20.c \
     test2013_21.c \
     test2013_22.c \
     test2013_23.c \
     test2013_24.c \
     test2013_26.c \
     test2013_27.c \
     test2013_28.c \
     test2013_29.c \
     test2013_30.c \
     test2013_31.c \
     test2013_32.c \
     test2013_33.c \
     test2013_34.c \
     test2013_35.c \
     test2013_36.c \
     test2013_37.c \
     test2013_38.c \
     test2013_39.c \
     test2013_41.c \
     test2013_42.c \
     test2013_43.c \
     test2013_44.c \
     test2013_45.c \
     test2013_46.c \
     test2013_49.c \
     test2013_50.c \
     test2013_51.c \
     test2013_52.c \
     test2013_53.c \
     test2013_54.c \
     test2013_55.c \
     test2013_56.c \
     test2013_57.c \
     test2013_58.c \
     test2013_59.c \
     test2013_60.c \
     test2013_61.c \
     test2013_62.c \
     test2013_63.c \
     test2013_64.c \
     test2013_65.c \
     test2013_66.c \
     test2013_67.c \
     test2013_68.c \
     test2013_69.c \
     test2013_70.c \
     test2013_71.c \
     test2013_72.c \
     test2013_73.c \
     test2013_74.c \
     test2013_75.c \
     test2013_76.c \
     test2013_77.c \
     test2013_79.c \
     test2013_80.c \
     test2013_81.c \
     test2013_82.c \
     test2013_83.c \
     test2013_84.c \
     test2013_85.c \
     test2013_86.c \
     test2013_87.c \
     test2013_88.c \
     test2013_89.c \
     test2013_90.c \
     test2013_91.c \
     test2013_92.c \
     test2013_93.c \
     test2013_94.c \
     test2013_95.c \
     stdio.c \
     test2006_48.c  \
     test2006_132.c \
     test2010_04.c \
     test2010_10.c \
     test2010_12.c \
     test2010_11.c \
     test2010_14.c \
     constants.c

# DQ (2/28/2015): Failing test with newest token unparsing support (but this version passes new_apps tests).
#  test2012_02.c 
#  test2012_30.c 
#  test2012_63.c 
#  test2012_108.c 
#  test2013_40.c 
#  test2013_78.c 
#  math.c 


# DQ (11/12/2014): Failing test with new leading and trailing edge token unparsing tests.
#    test2013_25.c 


# DQ (3/25/2014): This is a RH5 specific tests (we don't need this test).
# test2012_173.c

# test2012_17.c and test2012_163.c fail because the recursive macro handling for "#define sa_handler union_in_X.sa_handler"
# need to be tunred off when we are able to unparse the original token stream.
FAILING_TESTS = \
   test2012_17.c \
   test2012_163.c 

# These test codes fail this token unparsing test.
#     test_multiline_string_01.c 

# These are identical files but ROSE has to generate backend specific code (e.g. "restrict" should be "__retrict__"). 
#     test2012_19.c 
#     test2012_28.c 
#     test2012_91.c 
#     test2013_17.c 
#     test2013_19.c 
#     test2013_47.c 
#     test2013_48.c
#     complex_01.c 

CXX_TESTCODES_REQUIRED_TO_PASS = \
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
test2001_22.C \
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
test2003_08.C \
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
test2004_51.C \
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
test2004_86.C \
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
test2005_41.C \
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
test2006_04.C \
test2006_05.C \
test2006_06.C \
test2006_07.C \
test2006_10.C \
test2006_12.C \
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

CXX_TESTCODE_CURRENTLY_FAILING = \
test2004_67.C \
test2004_140.C \
test2005_128.C \
test2005_166.C \
test2006_03.C \
test2006_08.C \
test2006_13.C \
test2006_59.C \
test2006_116.C \
test2006_185.C \
test2007_51.C \
test2007_151.C \
test2010_19.C \
test2011_111.C \
test2011_142.C 


CXX_TESTCODE_CURRENTLY_FAILING_FOR_GNU4_8 = \
test2005_73.C \
test2005_193.C 


TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)
CXX_TESTCODES = $(CXX_TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)
ALL_CXX_TESTCODES = $(CXX_TESTCODES_REQUIRED_TO_PASS) $(CXX_TESTCODE_CURRENTLY_FAILING)

# DQ (11/11/2014): Note that we need verbose level 1 to internally activate some internal testing that builds the 
# output files: token_leading_* and token_trailing_* which are used in the TEST_Mapping_Source_passed tests.
# DQ (12/1/2013): Use verbose level to set force additional token specific files to be generated (tested below).
# ROSE_FLAGS = -rose:C89 -rose:unparse_tokens
# ROSE_FLAGS = -rose:C89 -rose:unparse_tokens -rose:verbose 1
# ROSE_FLAGS = -rose:C89 -rose:unparse_tokens -rose:verbose 0
ROSE_FLAGS = -rose:C89 -rose:unparse_tokens -rose:verbose 0

PASSING_TEST_Source_passed = ${TESTCODES:.c=.c.passed}
TEST_Source_passed = ${ALL_TESTCODES:.c=.c.passed}

PASSING_TEST_Cxx_Source_passed = ${CXX_TESTCODES:.C=.C.passed}
TEST_Cxx_Source_passed = ${ALL_CXX_TESTCODES:.C=.C.passed}

TEST_TRANSLATOR = ../../testTranslator

# DQ (11/12/2014): If this has not been built yet (as can be the case when Jenkins runs tests in only specific directories)
# then build the test translator using this run in the tests directory.
$(TEST_TRANSLATOR):
	cd ../..; $(MAKE) testTranslator

# DQ (9/23/2013): Added better control to run tests more cleanly (command-line and any output put into *.passed file, or generates similar *.failed file is testing is unsuccessful).
$(TEST_Source_passed): %.passed: $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) -c $(top_srcdir)/tests/CompileTests/C_tests/$(@:.c.passed=.c)" $(top_srcdir)/scripts/test_exit_status $@
	diff -u $(top_srcdir)/tests/CompileTests/C_tests/$(@:.c.passed=.c) ./rose_$(@:.c.passed=.c)

$(TEST_Cxx_Source_passed): %.passed: $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) -rose:unparse_tokens -I$(top_srcdir)/tests/CompileTests/Cxx_tests -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.C.passed=.C)" $(top_srcdir)/scripts/test_exit_status $@
	diff -u $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.C.passed=.C) ./rose_$(@:.C.passed=.C)

#------------------------------------------------------------------------------------------------------------------------
# Token Stream Mapping
bin_PROGRAMS += tokenStreamMapping testUnparsingUsingTokenStream

tokenStreamMapping_SOURCES = tokenStreamMapping.C  
tokenStreamMapping_LDADD = $(ROSE_LIBS)

testUnparsingUsingTokenStream_SOURCES = testUnparsingUsingTokenStream.C
testUnparsingUsingTokenStream_LDADD = $(ROSE_LIBS)


PASSING_TEST_Mapping_Source_passed = ${TESTCODES:.c=.c.mapping.passed}
TEST_Mapping_Source_passed = ${ALL_TESTCODES:.c=.c.mapping.passed}

# DQ (9/23/2013): Added better control to run tests more cleanly (command-line and any output put into *.passed file, or generates similar *.failed file is testing is unsuccessful).
$(TEST_Mapping_Source_passed): %.mapping.passed: tokenStreamMapping $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) CMD="./tokenStreamMapping $(ROSE_FLAGS) -rose:unparse_using_leading_and_trailing_token_mappings -c $(top_srcdir)/tests/CompileTests/C_tests/$(@:.c.mapping.passed=.c)" $(top_srcdir)/scripts/test_exit_status $@
	gcc -I$(top_srcdir)/tests/CompileTests/C_tests -c token_leading_$(@:.c.mapping.passed=.c)
	gcc -I$(top_srcdir)/tests/CompileTests/C_tests -c token_trailing_$(@:.c.mapping.passed=.c)

testTokenMapping:
	rm -f test2003_12.c.mapping.passed
	@$(MAKE) test2003_12.c.mapping.passed
	./tokenStreamMapping -rose:C89 -rose:unparse_tokens -c $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c

# Test code using -rose:verbose 2 to trigger output of evaluation files (*.c and *.dot files).
test_unparsingUsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_testUnparsingUsingTokenStream.c 

test_unparsingEmptyFileUsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_test_empty_file.c 

test_unparsingFileWithCR_UsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_test_file_with_CR.c 

test_unparsingFileWithText_UsingTokens : testUnparsingUsingTokenStream
	./testUnparsingUsingTokenStream -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_test_file_with_text.c 

test_typeTransformation_UsingTokens : testTypeTransformation
	./testTypeTransformation -rose:C89 -rose:unparse_tokens -rose:verbose 0 -c $(srcdir)/input_test_type_transformations.c 

#------------------------------------------------------------------------------------------------------------------------

test_binary_after_token_unparsing:
	gcc $(ROSE_FLAGS) -c $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c -o gcc_test2003_12.o
	./tokenStreamMapping $(ROSE_FLAGS) -c $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c
	gcc -c GlobalScopeDeclarationTokenStream.c -o rose_test2003_12.o
	diff $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c GlobalScopeDeclarationTokenStream.c
	diff gcc_test2003_12.o rose_test2003_12.o

#------------------------------------------------------------------------------------------------------------------------
EXTRA_DIST += input_testUnparsingUsingTokenStream.c input_test_empty_file.c input_test_file_with_CR.c input_test_file_with_text.c \
              input_test_02.c

cxx_tests:
	@$(MAKE) $(PASSING_TEST_Cxx_Source_passed)


# Additional automake targets
# check-local: $(LIN_TEST_TARGETS)
check-local:
	@$(MAKE) test_unparsingUsingTokens
	@$(MAKE) test_unparsingEmptyFileUsingTokens
	@$(MAKE) test_unparsingFileWithCR_UsingTokens
	@$(MAKE) test_unparsingFileWithText_UsingTokens
	@$(MAKE) $(PASSING_TEST_Mapping_Source_passed)
	@$(MAKE) $(PASSING_TEST_Source_passed)
#	@$(MAKE) $(LIN_TEST_TARGETS)
	@echo "******************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astTokenStreamTests: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************"


clean-local:
	rm -f rose*.c *.o
	rm -f *.passed *.failed *.err
	rm -f *.dot
	rm -f token_leading_*.c
	rm -f token_trailing_*.c

