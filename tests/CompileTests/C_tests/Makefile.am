include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USING_ALTERNATE_BACKEND_C_COMPILER
# TOO (2/10/11): set the correct linker and linker-flags for the alternate
# backend C compiler.
override CCLD=@BACKEND_C_COMPILER@
override LIBS=

# TOO (2/10/11): don't run these tests if they were cross-compiled, otherwise
# they will just fail.
check-TESTS:
	@echo "Skipping execution of cross-compiled tests: $(TESTS)"
endif

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

OLD_TESTCODES_REQUIRED_TO_PASS = \
     test_CplusplusMacro_C.c \
     gconv_info.c \
     stdio.c \
     testCvsCpp.c \
     testAnsiC.c \
     YardenPragmaPackExample.c \
     commentTest.c \
     test2003_12.c \
     test2004_20.c \
     test2004_21.c \
     test2005_172.c \
     test2005_186.c \
     test2006_48.c  \
     test2006_110.c \
     test2006_114.c \
     test2006_132.c \
     test2006_133.c \
     test2006_135.c \
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
     test2010_04.c \
     test2010_06.c \
     test2010_07.c \
     test2010_08.c \
     test2010_09.c \
     test2010_10.c \
     test2010_12.c \
     test2010_13.c \
     test2010_15.c \
     test2010_17.c \
     test2011_05.c \
     test2007_177.c \
     test2006_65.c \
     test2010_11.c \
     test2010_14.c \
     math.c \
     complex_01.c \
     constants.c

# This is listed seperately below with the required path to find the "sla.h" header file.
# test2005_168.c 

TESTCODES_REQUIRED_TO_PASS = \
     test_CplusplusMacro_C.c \
     gconv_info.c \
     testCvsCpp.c \
     testAnsiC.c \
     YardenPragmaPackExample.c \
     commentTest.c \
     test_multiline_string_01.c \
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
     test2012_02.c \
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
     test2012_17.c \
     test2012_18.c \
     test2012_19.c \
     test2012_20.c \
     test2012_21.c \
     test2012_22.c \
     test2012_23.c \
     test2012_24.c \
     test2012_25.c \
     test2012_26.c \
     test2012_27.c \
     test2012_28.c \
     test2012_29.c \
     test2012_30.c \
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
     test2012_63.c \
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
     test2012_91.c \
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
     test2012_108.c \
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
     test2012_163.c \
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
     test2013_17.c \
     test2013_18.c \
     test2013_19.c \
     test2013_20.c \
     test2013_21.c \
     test2013_22.c \
     test2013_23.c \
     test2013_24.c \
     test2013_25.c \
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
     test2013_40.c \
     test2013_41.c \
     test2013_42.c \
     test2013_43.c \
     test2013_44.c \
     test2013_45.c \
     test2013_46.c \
     test2013_47.c \
     test2013_48.c \
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
     test2013_78.c \
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
     test2013_96.c \
     test2013_97.c \
     test2013_98.c \
     test2013_99.c \
     test2013_100.c \
     test2013_101.c \
     test2013_102.c \
     test2013_103.c \
     test2013_104.c \
     test2013_105.c \
     test2013_106.c \
     test2013_107.c \
     test2013_108.c \
     test2013_109.c \
     test2013_110.c \
     test2013_111.c \
     test2013_112.c \
     test2013_113.c \
     test2014_01.c \
     test2014_02.c \
     test2014_04.c \
     test2014_05.c \
     test2014_06.c \
     test2014_07.c \
     test2014_08.c \
     test2014_09.c \
     test2014_12.c \
     test2014_13.c \
     test2014_14.c \
     test2014_15.c \
     test2014_16.c \
     test2014_17.c \
     test2014_18.c \
     test2014_19.c \
     test2014_20.c \
     test2014_21.c \
     test2014_22.c \
     test2014_23.c \
     test2014_24.c \
     test2014_25.c \
     test2014_26.c \
     test2014_27.c \
     test2014_28.c \
     test2014_29.c \
     test2014_30.c \
     test2014_31.c \
     test2014_32.c \
     test2014_33.c \
     test2014_34.c \
     test2014_35.c \
     test2014_36.c \
     test2014_37.c \
     test2014_38.c \
     test2014_39.c \
     test2014_40.c \
     test2014_41.c \
     test2014_42.c \
     test2014_43.c \
     test2014_44.c \
     test2014_46.c \
     test2014_47.c \
     test2014_48.c \
     test2014_49.c \
     test2014_50.c \
     test2014_51.c \
     test2014_52.c \
     test2014_53.c \
     test2014_55.c \
     test2014_56.c \
     test2014_57.c \
     test2014_58.c \
     test2014_60.c \
     test2014_62.c \
     test2014_63.c \
     test2014_64.c \
     test2014_65.c \
     test2014_66.c \
     test2014_67.c \
     test2014_68.c \
     test2014_69.c \
     test2014_70.c \
     test2014_72.c \
     test2014_76.c \
     test2014_77.c \
     test2014_78.c \
     test2014_79.c \
     test2014_80.c \
     test2014_81.c \
     test2014_82.c \
     test2014_83.c \
     test2014_84.c \
     test2014_85.c \
     test2014_86.c \
     test2014_87.c \
     test2014_88.c \
     test2014_89.c \
     test2014_91.c \
     test2014_92.c \
     test2014_93.c \
     test2014_94.c \
     test2014_95.c \
     test2014_100.c \
     test2014_200.c \
     test2014_201.c \
     test2014_202.c \
     math.c \
     complex_01.c \
     stdio.c \
     test2006_48.c  \
     test2006_132.c \
     test2010_04.c \
     test2010_10.c \
     test2010_12.c \
     test2010_11.c \
     test2010_14.c \
     constants.c \
     test2015_01.c \
     test2015_02.c \
     test2015_03.c \
     test2015_04.c \
     test2015_05.c \
     test2015_06.c \
     test2015_07.c \
     test2015_08.c \
     test2015_09.c \
     test2015_10.c \
     test2015_11.c \
     test2015_12.c \
     test2015_13.c \
     test2015_14.c \
     test2015_15.c \
     test2015_16.c \
     test2015_17.c \
     test2015_18.c \
     test2015_19.c \
     test2015_20.c \
     test2015_21.c \
     test2015_22.c \
     test2015_23.c \
     test2015_24.c \
     test2015_25.c \
     test2015_26.c \
     test2015_27.c \
     test2015_28.c \
     test2015_29.c \
     test2015_30.c \
     test2015_31.c \
     test2015_32.c \
     test2015_33.c \
     test2015_34.c \
     test2015_35.c \
     test2015_36.c \
     test2015_37.c \
     test2015_38.c \
     test2015_39.c \
     test2015_40.c \
     test2015_41.c \
     test2015_42.c \
     test2015_43.c \
     test2015_44.c \
     test2015_45.c \
     test2015_47.c \
     test2015_48.c \
     test2015_49.c \
     test2015_50.c \
     test2015_51.c \
     test2015_52.c \
     test2015_53.c \
     test2015_54.c \
     test2015_55.c \
     test2015_57.c \
     test2015_58.c \
     test2015_59.c \
     test2015_60.c \
     test2015_61.c \
     test2015_62.c \
     test2015_63.c \
     test2015_64.c \
     test2015_65.c \
     test2015_66.c \
     test2015_67.c \
     test2015_68.c \
     test2015_69.c \
     test2015_70.c \
     test2015_71.c \
     test2015_72.c \
     test2015_73.c \
     test2015_74.c \
     test2015_75.c \
     test2015_76.c \
     test2015_77.c \
     test2015_78.c \
     test2015_79.c \
     test2015_80.c \
     test2015_81.c \
     test2015_82.c \
     test2015_83.c \
     test2015_89.c \
     test2015_100.c \
     test2015_105.c \
     test2015_107.c \
     test2015_108.c \
     test2015_109.c \
     test2015_110.c \
     $(EXTRA_OS_SPECIFIC_SOURCE_FILES)

# DQ (4/11/2015): New failing tests:
FAILING_TESTCODES_XEN = \
    test2015_56.c \
    test2015_84.c \
    test2015_85.c \
    test2015_86.c \
    test2015_87.c \
    test2015_88.c \
    test2015_89.c \
    test2015_90.c \
    test2015_91.c \
    test2015_92.c \
    test2015_93.c \
    test2015_94.c \
    test2015_95.c \
    test2015_96.c \
    test2015_97.c \
    test2015_98.c \
    test2015_99.c \
    test2015_101.c \
    test2015_102.c \
    test2015_103.c \
    test2015_104.c \
    test2015_106.c \
    test2015_111.c 

# DQ (4/13/2015): The best examples of remaining bugs are:
# Makefile rules:
#    test_using_explicit_output_file_option:
#    test_rtd_use: 
#    test_m32_use: 
# And test codes:
#    test2015_85.c (demonstrates bug in designated initializer handling)
#    test2015_87.c (demonstrated need for 32-bit mode for EDG front-end, waiting on fix from EDG)
#    test2015_88.c (may require edits to source code where this happens)
#    test2015_89.c (only fails when tested using -m32 -mrtd options using makefile rule: test_rtd_use)
#    test2015_93.c  (demonstrates use of GNU statement-expression in default case of switch statement)
#    test2015_95.c  (demonstrates 32-bit mode requirement for EDG front-end)
#    test2015_100.c (demonstrates that the typeof opertor not output in argument type for function type unparsing)
#    test2015_102.c (demonstrates unparsing of alias attribute on non-defining declaration)
#    test2015_103.c (demonstrates unparsing of CPP directive before closing ";" of expanded macro)
#    test2015_105.c (demonstrates unparsing of __asm__() in wrong position)
#    test2015_106.c (demonstrated use of volatile with va_list (not allowed)


# DQ (3/22/2015): Still failing:
#    test2015_31.c 
#    test2015_32.c 


if OS_VENDOR_REDHAT_REL5
   EXTRA_OS_SPECIFIC_SOURCE_FILES = test2012_173.c
endif

if OS_VENDOR_REDHAT_REL6
   EXTRA_OS_SPECIFIC_SOURCE_FILES = test2012_173_redhat_rel6.c
endif

# DQ (8/14/2014) List these files explicitly to avoid failing the policy tests.
FILES_NOT_PASSING = \
     test2014_71.c \
     test2014_73.c \
     test2014_74.c \
     test2014_75.c \
     test2014_90.c \
     test2014_96.c \
     test2014_97.c \
     test2014_98.c \
     test2014_99.c 

# DQ (1/5/2014): Some failing test codes:
#     test2014_03.c  (demonstrates nested functions (not supported in EDG))
#     test2014_10.c  (demonstrates enum type declaration at top of switch body (before first case); bug in ROSE switch implementation)
#     test2014_11.c  (simpler case of test2014_10.c)
#     test2014_45.c  (inconsitant function declarations, but consistant through typedefs)
#     test2014_54.c  (old-stype C function parameters in wrong order, appears to be legal C but a problem for ROSE)
#     test2014_59.c  (complex macro handling where variables are named using the __LINE__ macro)
#     test2014_61.c  (example of assigned goto (gnu extension))
#     test2014_71.c  (macro namespace intrudes into application namespace (macro suppression using "()")
#     test2014_73.c  (example of anonymous union in an initializer)
#     test2014_74.c  (another example similar to test2014_73.c)
#     test2014_75.c  (incomplete code from application example, will not compile with gcc).
#     test2014_90.c  (asm command that is incomplete -- the version built using CPP is in test2014_91.c).


# DQ (9/9/2013): This is a problem for the new block support.
#     test2012_23.c 
#     test2012_25.c 
#     test2012_36.c 
#     test2012_42.c
#     test2012_101.c
#     test2012_102.c
#     test2012_103.c 
#     test2012_104.c 
#     test2012_105.c 
#     test2012_116.c 
#     test2012_118.c 
#     test2012_119.c 


# DQ (5/23/2013): This is a failing case (reported by Sara). Case of "void foo(int N,int a[N]){}"
# test2013_04.c 

# DQ (9/2/2013): This is now fixed.
# DQ (6/19/2013): Failing case of include used to initialize array (still working on this case).
# test2013_05.c 


# DQ (9/28/2012): Failing tests.
# test2012_04.c (infinite recursion!) (DQ: 11/21/2012: NOW FIXED)
# test2012_30.c (break statement not unparsed in loop)
# test2012_42.c (more complex (original) case of test2012_43.c)
# test2012_43.c (macro for compile-time assertions)
# test2012_44.c (asm issue stat64 undefined after compilation)
# test2012_45.c (same problems at test2012_44.c)
# test2012_59.c (C-style comment in quoted string (inside of #if-def comment, so should be ignored)
# test2012_78.c (variable with initializer using #include file to provide value (classic source-to-source issue))
# test2012_79.c (test code demonstrating an outstanding error (generated before focusing on switch support))
# test2012_80.c (same as test2012_79.c)
# test2012_101.c (example of failing test from zsh : jobs.c file: NOW FIXED)
# test2012_102.c (example of failing test from zsh : module.c file)
# test2012_103.c (example of failing test from zsh : params.c file)
# test2012_107.c (fails to unparse "..." in function type: NOW FIXED)
# test2012_128.c (label attached to case option statement is a current bug, but not yet found in application code)
# test2012_132.c (this has a overly complex duff's device like switch statement that I have rewritten to put the
#                 default case at the same scope as the other case option statements. Adding this level of support
#                 to the switch statment support in ROSE is deferred to later. test2012_137.c is a required test
#                 code to pass; so that we can test the rewritten version of the switch.
# test2012_145.c NOT A FAILING test, but tested below with -D options.
# test2012_155.c (array with initializer using #include file to provide value (classic source-to-source issue))
# test2012_160.c (application code with label on default case option statement: full size)
# test2012_161.c (small test code demonstrating bug in test2012_160.c)

# Failing tests of interest relative to C application codes:
# test2012_128
# test2012_31
# test2012_132
# test2012_138
# test2012_159



# This is a C99 test code (moved to the C99_tests directory). (These are not really C99 codes, I thought they were).
# test2012_20.c 
# test2012_23.c
# test2012_25.c
# test2012_58.c 
# test2012_100.c 
# test2012_104.c
# test2012_105.c 
# test2012_106.c 


if ROSE_USE_CLANG_FRONTEND
   TESTCODES_REQUIRED_TO_PASS += \
     stdio.c \
     test2006_48.c \
     test2006_132.c \
     test2010_04.c \
     test2006_14.c  \
     test2006_100.c \
     test2006_136.c \
     test2010_02.c \
     test2010_03.c \
     test2011_03.c \
     test2011_04.c \
     Yarden_client.c
# Failling tests for CLANG only:
#  -  test2004_21.c
#  -  test2005_168.c
#  -  test2006_65.c
#  -  test2010_11.c
#  -  test2010_14.c
# Failling tests for CLANG but also in TESTCODE_CURRENTLY_FAILING
#  -  test2004_126.c
#  -  test2006_134.c
#  -  test2009_06.c
#  -  test2010_05.c     -> Unknown statement kind: CompoundLiteralExpr
#  -  test2010_16.c     -> Unknown statement kind: CompoundLiteralExpr
endif

if !ROSE_USE_CLANG_FRONTEND
   TESTCODES_REQUIRED_TO_PASS += \
     test2004_21.c \
     test2006_65.c
if !ROSE_USE_EDG_VERSION_4_4
if !ROSE_USE_EDG_VERSION_4_7
   TESTCODES_REQUIRED_TO_PASS += \
     stdio.c \
     test2006_48.c \
     test2006_132.c \
     test2010_04.c \
     test2010_11.c \
     test2010_12.c \
     test2010_14.c
endif
endif
endif

# DQ (9/26/2009): Conditionally compile this file (fails in OSX due to lack of OSX
# support for thread local storage). Liao know more about the details of this test.
# test2009_19.c
if !OS_MACOSX
   TESTCODES_REQUIRED_TO_PASS += test2009_19.c
endif

#Liao, 5/16/2009 add a set of self-verifiable test code with main() 
# and let them run during make check
# TV, 9/7/12 do not run them if using Clang
TESTCODES_REQUIRED_TO_RUN =

if !ROSE_USE_CLANG_FRONTEND
   TESTCODES_REQUIRED_TO_RUN += \
     test2009_05.c \
     test2009_17.c
endif

TESTCODE_CURRENTLY_FAILING = \
     test2004_126.c \
     test2006_14.c  \
     test2006_100.c \
     test2006_134.c \
     test2006_136.c \
     test2009_06.c \
     test2010_02.c \
     test2010_03.c \
     test2010_05.c \
     test2010_16.c \
     test2011_03.c \
     test2011_04.c \
     Yarden_client.c



# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
# ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING) $(OLD_TESTCODES_REQUIRED_TO_PASS)
ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
# PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/C_ROSE_PERFORMANCE_DATA.csv

# DQ (3/28/2013): Now that we have changed the default C behavior to C99 (from C89), we need 
# to explicitly specify the C89 behavior for make of the C test codes in this directory.
# ROSE_FLAGS = -rose:C
ROSE_FLAGS = -rose:C89
# ROSE_FLAGS = -rose:C89 -rose:unparse_tokens
if !ROSE_USE_CLANG_FRONTEND
   ROSE_FLAGS += --edg:no_warnings --edg:restrict
endif
ROSE_FLAGS += -w

# TOO1 (2013-09-17): Required by test2005_168.o to find "rosedll.h",
#                    introduced by CMake work.
ROSE_FLAGS += -I$(top_srcdir)/src/frontend/SageIII/

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
PASSING_TEST_Objects_passed = ${TESTCODES:.c=.o.passed}
PASSING_TEST_Source_passed = ${TESTCODES:.c=.c.passed}
TEST_Objects = ${ALL_TESTCODES:.c=.o}
TEST_Objects_passed = ${ALL_TESTCODES:.c=.o.passed}
TEST_Source_passed = ${ALL_TESTCODES:.c=.c.passed}
TEST_Exe_Objects = ${TESTCODES_REQUIRED_TO_RUN:.c=.o}

PASSING_TEST_Executables =  ${TEST_Exe_Objects:.o=.out}

# DQ (4/18/2015): Uncommented this so that it can be properyl tested after being fixed!
# if !ROSE_USE_EDG_VERSION_4_4
# if !ROSE_USE_EDG_VERSION_4_7
# any programs needed by your tests that does not need to be built by make all.
check_PROGRAMS = $(PASSING_TEST_Executables)
# a list of programs or scripts to run in order to do the testing 
TESTS= $(check_PROGRAMS) caller.out 
# endif
# endif

testTranslator = ../../testTranslator

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.c)
#$(TEST_Objects): ../../testTranslator
#	$(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.c)

# DQ (9/23/2013): Added better control to run tests more cleanly (command-line and any output put into *.passed file, or generates similar *.failed file is testing is unsuccessful).
$(TEST_Source_passed): %.passed: ../../testTranslator $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) CMD="$(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/$(@:.c.passed=.c)" $(top_srcdir)/scripts/test_exit_status $@

# This is a redundant test with it also being in the list TESTCODES_REQUIRED_TO_PASS (above).
# test2005_168.o: $(srcdir)/test2005_168.c
test2005_168.o: $(srcdir)/test2005_168.c
#	$(testTranslator) $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/test2005_168.c
	@$(RTH_RUN) CMD="$(testTranslator) -I$(srcdir) $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/test2005_168.c" $(top_srcdir)/scripts/test_exit_status $@

# DQ (11/1/2012): Added test code to test string concatination using a value defined 
# via a macro specified on the command line. This tests the string quoting mechanism.
test2012_145.o: $(srcdir)/test2012_145.c
#	NOTE: Additional escapes are required to handle quoted string handed on command line for ROSE Test Harness (RTH).
#	$(testTranslator) $(ROSE_FLAGS) -DSTRING_MACRO=\"some_text\" -c $(srcdir)/test2012_145.c
	@$(RTH_RUN) CMD="$(testTranslator) -I$(srcdir) $(ROSE_FLAGS) -DSTRING_MACRO=\\\"some_text\\\" -c $(srcdir)/test2012_145.c" $(top_srcdir)/scripts/test_exit_status $@

$(TEST_Exe_Objects): %.o: $(srcdir)/%.c $(testTranslator)
	$(testTranslator) $(ROSE_FLAGS) -c $<

test_build_generated_file_in_same_directory_as_input_file.passed: %.passed: $(testTranslator)
	@mkdir -p test_directory
	@cp $(srcdir)/test2013_73.c test_directory/test2013_73.c
#	$(testTranslator) -rose:unparse_in_same_directory_as_input_file $(ROSE_FLAGS) -c test_directory/test2013_73.c -o test_directory/test2013_73.o
	@$(RTH_RUN) CMD="$(testTranslator) -rose:unparse_in_same_directory_as_input_file $(ROSE_FLAGS) -c test_directory/test2013_73.c -o test_directory/test2013_73.o" $(top_srcdir)/scripts/test_exit_status $@

# DQ (9/13/2013): Added new test for -rose:unparse_includes (which has a bug in how it unparses C++ style comments
# and should maybe not be unparsing declarations that are front-end specific (e.g. builtin function declarations).
test_unparse_includes_option.passed: %.passed: $(testTranslator)
#	$(testTranslator) -rose:unparse_includes $(ROSE_FLAGS) -c $(srcdir)/test2013_75.c -o test2013_75_unparse_headers.o
	@$(RTH_RUN) CMD="$(testTranslator) -rose:unparse_includes $(ROSE_FLAGS) -c $(srcdir)/test2013_75.c -o test2013_75_unparse_headers.o" $(top_srcdir)/scripts/test_exit_status $@

# DQ (9/16/2013): Test linking the resulting translated file. Phil Miller reported an error and the issue
# is that va_start() and va_end() must be unparsed to be __builtin_va_start() and __builtin_va_end() (at least for GNU).
test_unparse_includes_option_with_link.passed: %.passed: $(testTranslator)
#	$(testTranslator) -rose:unparse_includes $(ROSE_FLAGS) $(srcdir)/test2013_76.c -o test2013_76_unparse_headers
	@$(RTH_RUN) CMD="$(testTranslator) -rose:unparse_includes $(ROSE_FLAGS) -c $(srcdir)/test2013_76.c -o test2013_76_unparse_headers.o" $(top_srcdir)/scripts/test_exit_status $@

# DQ (9/17/2013): Added autoconf style link test.
test_common_configure_test_with_link.passed: %.passed: $(testTranslator)
#	$(testTranslator) -I$(srcdir) $(ROSE_FLAGS) $(srcdir)/conftest.c -o conftest_configure_test
	@$(RTH_RUN) CMD="$(testTranslator) -I$(srcdir) $(ROSE_FLAGS) $(srcdir)/conftest.c -o conftest_configure_test" $(top_srcdir)/scripts/test_exit_status $@

test_tokenMapping: 
#	rm -f test2003_12.c.mapping.passed
#	@$(MAKE) test2003_12.c.mapping.passed
	$(testTranslator) -rose:C89 -rose:unparse_tokens -c $(top_srcdir)/tests/CompileTests/C_tests/test2003_12.c

# DQ (1/19/2014): Test the use of the gnu "-S" option.
test_gnu_S_option:
	$(testTranslator) -S -o test2014_41.s $(top_srcdir)/tests/CompileTests/C_tests/test2014_41.c

test_link_using_alias_attribute: $(testTranslator)
	$(testTranslator) -o test2014_93 $(top_srcdir)/tests/CompileTests/C_tests/test2014_93.c

# DQ (2/17/2014): Added to force test of two week old asm fix for linking that didn't make it out in github.
test_link_using_asm_symbol_injection: $(testTranslator)
	$(testTranslator) -o test2014_83 $(top_srcdir)/tests/CompileTests/C_tests/test2014_83.c

# Liao 11/19/2009
# Custom tests to test rose translators as a linker (generating the final executables) for multiple files
# simplest case: generate .o with -c
caller.o: $(srcdir)/callee.c $(srcdir)/caller.c
	$(testTranslator) $(ROSE_FLAGS) -c $^
# default case: generate executable a.out from any number of input files	
caller.out: $(srcdir)/callee.c $(srcdir)/caller.c
	$(testTranslator) $(ROSE_FLAGS) $^ -o caller.out
# DQ (4/18/2015): Add the execution of the generated executable.
	./caller.out


# Customized test that modifies testTranslator to remove empty elses to test dangling else unparsing
# test2008_02.o: $(srcdir)/test2008_02.c
#	env ROSE_TEST_ELSE_DISAMBIGUATION=x $(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/test2008_02.c

# test2008_02.o: $(srcdir)/test2008_02.c
test_else_case_disambiguation.passed: $(srcdir)/test2008_02.c
	@cp $(srcdir)/test2008_02.c else_case_disambiguation_test.c
	@$(RTH_RUN) CMD="env ROSE_TEST_ELSE_DISAMBIGUATION=x $(testTranslator) $(ROSE_FLAGS) -c else_case_disambiguation_test.c" $(top_srcdir)/scripts/test_exit_status $@

# DQ (4/8/2015): This causes an error due to a bug which outputs a redundant "-o <file>" option
test_using_explicit_output_file_option:
	$(testTranslator) -rose:verbose 0 -MMD -MF .subdirs-install.d $(srcdir)/test2015_86.c -o test2015_86

test_using_implicit_output_file_option:
	$(testTranslator) -rose:verbose 0 -MMD -MF .subdirs-install.d $(srcdir)/test2015_86.c

# DQ (4/9/2015): Added test for using -m32 -mrtd options (required for Xen).
test_rtd_use: $(testTranslator) $(srcdir)/test2015_89.h
	$(testTranslator) -m32 -mrtd -rose:C89 -c $(srcdir)/test2015_89.c

# DQ (4/10/2015): Added test for using -m32 option which fouces __uint128_t to be undefined (required for Xen).
test_m32_use: $(testTranslator)
	$(testTranslator) -m32 -rose:C89 -c $(srcdir)/test2015_95.c


../../testTranslator:
	cd ../..; $(MAKE) testTranslator

#QMTEST_Objects = ${ALL_TESTCODES:.c=.qmt}
#
# Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd`
#$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.c)
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.c)
#
#test2005_168.qmt: $(srcdir)/test2005_168.c
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/$(@:.qmt=.c)
#
# Include makefile rules specific to QMTest
#include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES) builtin-types.def callee.c caller.c c-common.def \
             predict.def test2009_18.c test2009_20.c test2014_20_inc.c test2014_27_inc.c test2014_28_inc.c \
	     test2012_145.c conftest.c test2005_168.c \
	     confdefs.h grep_verify.h verify.h \
	     test2006_134.h \
	     test2010_08.h \
	     test2010_15.h \
	     test2012_155.h \
	     test2012_156.h \
	     test2012_63.h \
	     test2012_75.h \
	     test2012_78.h \
	     test2013_05.h \
	     test2013_06.h \
	     test2013_114.h \
	     test2014_06.h \
	     test2014_07.h \
	     test2014_19.h \
	     test2014_33.h \
	     test2014_36.h \
	     test2014_43.h \
	     test2014_56.h \
	     test2014_59.h \
	     test2014_60_data_members.h \
	     test2014_60_values.h \
	     test2014_66.h \
	     test2014_73.h

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/CompileTests/C_tests
	cp $(srcdir)/*.c $(top_srcdir)/tests/CompileTests/C_tests

check-local:
	@echo "Tests for C."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
#	@$(MAKE) $(PASSING_TEST_Objects_passed)
	@$(MAKE) $(PASSING_TEST_Source_passed)
#	@$(MAKE) caller.o caller.out
	@$(MAKE) test2012_145.o
	@$(MAKE) test_link_using_asm_symbol_injection
	@$(MAKE) test_link_using_alias_attribute
	@$(MAKE) test_build_generated_file_in_same_directory_as_input_file.passed
	@$(MAKE) test_unparse_includes_option.passed
	@$(MAKE) test_unparse_includes_option_with_link.passed
	@$(MAKE) test_else_case_disambiguation.passed
	@$(MAKE) test_common_configure_test_with_link.passed
	@$(MAKE) test2005_168.o
	@$(MAKE) test_using_explicit_output_file_option
	@$(MAKE) test_using_implicit_output_file_option
	@$(MAKE) test_rtd_use
#	@$(MAKE) test_m32_use
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] rose_performance_report_lockfile.lock *.out *.dot
	rm -rf QMTest
	rm -rf test_directory
	rm -rf test2013_76_unparse_headers conftest_configure_test else_case_disambiguation_test.c
	rm -f *.err *.passed *.failed
	rm -f token_trailing_*.c token_leading_*.c
	rm -f test2014_83 test2014_93

################################################################################
#
# Addition to show how to hook QMTest ROSE_Regression with automake
# Uncomment lines 115,$
#

#export ROSE_TEST_REGRESSION_ROOT=/home/yuan5/tmp/18/ROSE_Regression
#QMTEST=/home/yuan5/opt/qmtest-2.3/bin/qmtest
#ALL_Objects = ${ALL_TESTCODES:.c=.o}
#
#force:
#
#testTranslator_wrapper: force
#	rm -f ../../testTranslator
#	echo -e "#!/bin/bash\n${ROSE_TEST_REGRESSION_ROOT}/roseQMGen/bin/qm.sh ../../../testTranslator \"\$$@\"" > ../../testTranslator && chmod +x ../../testTranslator
#
#rrqm_generate: testTranslator_wrapper $(ALL_Objects)
#	test -d ./c_tests_testdatabase || mkdir c_tests_testdatabase 
#	rm -rf ./c_tests_testdatabase/*
#	find . -type f -mindepth 1 -maxdepth 1 -name "*.qmt" -exec mv {} c_tests_testdatabase/ \;
#	$(QMTEST) -D`pwd`/c_tests_testdatabase create-tdb
#	rm -f ../../testTranslator
#	cd ../..; $(MAKE) testTranslator
#
#c_tests_testdatabase/QMTest:
#	$(MAKE) rrqm_generate
#
#rrqm_run: c_tests_testdatabase/QMTest
#	-cd c_tests_testdatabase && $(QMTEST) run
#
#c_tests_testdatabase/results.qmr:
#	$(MAKE) rrqm_run
#
#rrqm_report: c_tests_testdatabase/results.qmr
#	sh -c "source ${ROSE_TEST_REGRESSION_ROOT}/scripts/rosetestrc; report.sh c_tests_testdatabase/results.qmr c_tests_testdatabase report_c_tests"
#	@echo -e "\nThe results are in `pwd`/report_c_tests/results.xml"
