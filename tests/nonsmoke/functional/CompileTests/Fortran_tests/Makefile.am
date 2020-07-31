SHELL := /bin/bash  # support diff <() <() 
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (10/22/2016): Change order of tests (simple --> hardest).
# SUBDIRS = LANL_POP gfortranTestSuite
# SUBDIRS = . gfortranTestSuite LANL_POP

SUBDIRS = . LANL_POP
if USING_GNU_COMPILER
SUBDIRS += gfortranTestSuite
else
if USING_CLANG_COMPILER
SUBDIRS += gfortranTestSuite
else
if USING_INTEL_COMPILER
# DQ (2/21/2017): This directory has some failing test codes for ROSE when compiled using the Intel v16 compiler, for example:
#   gfortranTestSuite/gfortran.dg/continuation_6.f
#   gfortranTestSuite/gfortran.dg/continuation_4.f90
# SUBDIRS += gfortranTestSuite
endif
endif
endif

if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
# Rasmussen (3/7/2020): Removed testing of the experimental (ATerm based) Fortran. It is now obsolete because
# it is being replaced by experimental_flang_frontend
#
#   SUBDIRS += experimental_frontend_tests
#
endif


###########################################################################
# These are Fortran testcodes targeting aspects F66, F77, F90, F95, F2003 #
###########################################################################

# #######################
#      PASSING TESTS
# #######################

F90_TESTCODES_REQUIRED_TO_PASS =  \
     jira-par-2_literal_max.f90   \
     jira-par-11_write_format.f90 \
     jira-par-18_common_block.f90 \
     R832.f90        \
     R837.f90        \
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
     test2007_244.f90 \
     test2007_248.f90 \
     test2007_249.f90 \
     test2007_250.f90 \
     test2007_262.f90 \
     test2010_01.f90 \
     test2010_02.f90 \
     test2010_15.f90 \
     test2010_16.f90 \
     test2010_18.f90 \
     test2010_20.f90 \
     test2010_23.f90 \
     test2010_25.f90 \
     test2010_28.f90 \
     test2010_32.f90 \
     test2010_33.f90 \
     test2010_34.f90 \
     test2010_37.f90 \
     test2010_38.f90 \
     test2010_40.f90 \
     test2010_41.f90 \
     test2010_42.f90 \
     test2010_44.f90 \
     test2010_45.f90 \
     test2010_46.f90 \
     test2010_47.f90 \
     test2010_49.f90 \
     test2010_50.f90 \
     test2010_59.f90 \
     test2010_60.f90 \
     test2010_61.f90 \
     test2010_62.f90 \
     test2010_63.f90 \
     test2010_64.f90 \
     test2010_111.f90 \
     test2010_112.f90 \
     test2010_113.f90 \
     test2010_114.f90 \
     test2010_115.f90 \
     test2010_118.f90 \
     test2010_120.f90 \
     test2010_121.f90 \
     test2010_122.f90 \
     test2010_123.f90 \
     test2010_124.f90 \
     test2010_125.f90 \
     test2010_127.f90 \
     test2010_128.f90 \
     test2010_129.f90 \
     test2010_132.f90 \
     test2010_133.f90 \
     test2010_134.f90 \
     test2010_135.f90 \
     test2010_136.f90 \
     test2010_137.f90 \
     test2010_138.f90 \
     test2010_139.f90 \
     test2010_140.f90 \
     test2010_141.f90 \
     test2010_142.f90 \
     test2010_143.f90 \
     test2010_144.f90 \
     test2010_150.f90 \
     test2010_151.f90 \
     test2010_152.f90 \
     test2010_153.f90 \
     test2010_155.f90 \
     test2010_154.f90 \
     test2010_156.f90 \
     test2010_158.f90 \
     test2010_159.f90 \
     test2010_160.f90 \
     test2010_162.f90 \
     test2010_163.f90 \
     test2010_165.f90 \
     test2010_166.f90 \
     test2010_167.f90 \
     test2010_168.f90 \
     test2010_172.f90 \
     test2010_173.f90 \
     test2010_174.f90 \
     test2010_182.f90 \
     test2010_183.f90 \
     test2010_184.f90 \
     test2011_01.f90 \
     test2011_02.f90 \
     test2011_04.f90 \
     test2011_05.f90 \
     test2011_06.f90 \
     test2011_07.f90 \
     test2011_08.f90 \
     test2011_09.f90 \
     test2011_10.f90 \
     test2011_12.f90 \
     test2011_13.f90 \
     test2011_14.f90 \
     test2011_15.f90 \
     test2011_16.f90 \
     test2011_17.f90 \
     test2011_19.f90 \
     test2011_20.f90 \
     test2011_36.f90 \
     test2011_39.f90 \
     test2011_40.f90 \
     test2011_Rice_allocate-derived-type-scope.f90 \
     test2011_Rice_asterisk-length.f90 \
     test2011_Rice_attrs-dimension.f90 \
     test2011_Rice_caseSensitive.f90 \
     test2011_Rice_char-array-decl-asterisk.f90 \
     test2011_Rice_char-kind-decl.f90 \
     test2011_Rice_constructor-initializer.f90 \
     test2011_Rice_data-implied-do.f90 \
     test2011_Rice_data-implied-do-object-list.f90 \
     test2011_Rice_derived-type-array.f90 \
     test2011_Rice_derived-type-2d-component.f90 \
     test2011_Rice_derived-type-3d-component.f90 \
     test2011_Rice_elseif.f90 \
     test2011_Rice_empty-string-constant.f90 \
     test2011_Rice_eq.f90 \
     test2011_Rice_explicit-shape1.f90 \
     test2011_Rice_explicit-shape2.f90 \
     test2011_Rice_forward-decl-func-type.f90 \
     test2011_Rice_forward-ref-derived-type.f90 \
     test2011_Rice_func-char-len.f90 \
     test2011_Rice_funcdecl-1.f90 \
     test2011_Rice_function-implicit-result-1.f90 \
     test2011_Rice_function-implicit-result-2.f90 \
     test2011_Rice_function-result-2.f90 \
     test2011_Rice_function-where-var-expected.f90 \
     test2011_Rice_ifConstruct.f90 \
     test2011_Rice_ifStmt.f90 \
     test2011_Rice_interface.f90 \
     test2011_Rice_if-stop.f90 \
     test2011_Rice_inquire-in-ifthen.f90 \
     test2011_Rice_intent.f90 \
     test2011_Rice_io-implied-do-with-stride.f90 \
     test2011_Rice_keyword-arg-extra-paren.f90 \
     test2011_Rice_keyword-arg-extra-paren-2.f90 \
     test2011_Rice_lost-kindtype-1.f90 \
     test2011_Rice_lost-kindtype-2.f90 \
     test2011_Rice_multi-char-decl.f90 \
     test2011_Rice_nullify.f90 \
     test2011_Rice_pause-no-stop-code.f90 \
     test2011_Rice_pointer.f90 \
     test2011_Rice_pointer-after-dimension.f90 \
     test2011_Rice_pointer-init.f90 \
     test2011_Rice_pointer-to-forward-ref-derived-type.f90 \
     test2011_Rice_public.f90 \
     test2011_Rice_return-with-expr.f90 \
     test2011_Rice_singleton-array-constructor.f90 \
     test2011_Rice_stop-no-stop-code.f90 \
     test2011_Rice_unary-plus-minus.f90 \
     test2011_Rice_use-only-rename.f90 \
     test2011_Rice_use-rename.f90 \
     test2011_Rice_vector-constructor-1.f90 \
     test2011_Rice_vector-constructor-2.f90 \
     test2011_Rice_vector-constructor-3.f90 \
     test2011_Rice_vector-constructor-4.f90 \
     test2011_Rice_write-no-output-item-list.f90 \
     test2011_Rice_wrong-func-type-3.f90 \
     test2011_Rice_wrong-func-type.f90 \
     test2011_Rice_wrong-func-type-2.f90 \
     test2019_derived_type.f90 \
     test2019_class_var_decl.f90 \
     test2019_write_io_control_spec.f90 \
     test2019_recursive_function.f90 \
     test2019_derived_type_name_conflict.f90 \
     test2019_derived_type_name_conflict_2.f90 \
     test2019_rewind.f90 \
     test2019_renameList.f90 \
     test2019_designator.f90 \
     test2019_contiguous_attr.f90 \
     test2019_doconstruct.f90 \
     test2019_OpenStmt_Unit.f90 \
     test2019_common_block_name.f90 \
     test2019_multi_associate.f90 \
     test2019_accessibility_attribute.f90 \
     test2019_accessibility_attribute2.f90 \
     test2020_use_iso_c_binding.f90 \
     test2020_use_iso_c_binding_2.f90 \
     test2020_select_case.f90 \
     test2020_commonblock_1.f90 \
     test2020_contiguous_attr.f90 \
     rose-issue-par-39.f90 \
     canonicalExample.f90 \
     fCaller.f90 \
     rose-1612.f90

# test2020_comment_0.f90 the full test     
#     test2020_comment_3.f90
# Liao: 6/18/2020. tests with comments
F90_COMMENTS_REQUIRED_TO_PASS  =  \
    test2020_comment_1.f90 \
    test2020_comment_2.f90

# result is rose_input.f90
F90_COMMENTS_REQUIRED_TO_PASS_RESULTS =  $(addprefix rose_, ${F90_COMMENTS_REQUIRED_TO_PASS})    

# $(srcdir)/$(@:rose_%=%) :  source/ for each output target @, convert rose_input.f90 to input.f90 
$(F90_COMMENTS_REQUIRED_TO_PASS_RESULTS): $(srcdir)/$(@:rose_%=%) ../../testTranslator
	../../testTranslator -c $(srcdir)/$(@:rose_%=%)

# result's diff is input.f90.diff
F90_COMMENTS_REQUIRED_TO_PASS_RESULTS_DIFF = $(addsuffix .diff, $(F90_COMMENTS_REQUIRED_TO_PASS))  

# rule to generate the diff file: depending on rose_input.f90  
# grep comments from rose_input.f90
# grep comments from sourcetree/input.f90
# diff the output of two grep command lines
$(F90_COMMENTS_REQUIRED_TO_PASS_RESULTS_DIFF): %.diff:rose_%
	diff <(grep -n "\!"  $(@:%.diff=rose_%)) <(grep -n "\!" $(srcdir)/$(@:%.diff=%)) > $@

# DQ (11/11/2014): These test fail due to changes to support C++11 work (need to look into this separately).
#    test2010_154.f90

# SKW 8/21/2011: these test fails with gfortran 4.1.2, but there is no 'ROSE_USING_GFORTRAN_VERSION_4_1_2'
# yet so I am just commenting them out for now. They pass with gfortran 4.4.1
#     test2007_108.f90
#     test2010_03.f90
#     test2010_81.f90

# DQ (4/10/2011): If it is not gfortran 4.5, then add this test code (these fail syntax checking with 4.5).
if !ROSE_USING_GFORTRAN_VERSION_4_5
   F90_TESTCODES_REQUIRED_TO_PASS += \
     test2010_35.f90 \
     test2011_Rice_funcdecl-4.f90
endif

# These fail and have been moved to be tested unde 4.2.x only.
# test2011_Rice_alternate-return-argument.f90
# test2011_Rice_alt-return-call.f90

# SKW 4/17/2011: tests which may actually be incorrect Fortran 90 -- need to study the standard.
#     test2011_Rice_false-forward-decl-func-type.f90
#     test2011_Rice_forward-ref-with-sub.f90
#     test2011_Rice_lost-func-return-type.f90

# SKW 3/18/2011: tests which are correct and pass in ROSE, but sometimes fail
# when run in Hudson (nondeterministically, on amd64-linux with gcc 4.1.2)
#     test2011_Rice_A-cant-assert-keyword-nonnull.f90
#     test2011_Rice_B-cant-assert-keyword-nonnull.f90
#     test2011_Rice_C-cant-assert-keyword-nonnull.f90

#Liao 2/11/2010, 'include' issue on gfortran 4.2.0 of Mac OS X
# 4.2.1 can handle them properly
#if !OS_MACOSX
#  F90_TESTCODES_REQUIRED_TO_PASS += \
#       test2007_29.f90 \
#       test2010_01.f90 \
#       test2010_50.f90
#endif

# DQ (2/12/2011): This test codes appears to randomly fail (failed on GNU 4.1.2)
# I will look into this separately. 
# Within Hudson is has failed for:
#       test#2044 on hudson-rose-04 using GNU 4.3.2
#       test#2056 on tux284 using GNU 4.1.2
# It passes without error using Valgrind and I can't get it to fail locally 
# on tux321 (using GNU 4.2.4).  One intermediate step would be to have it be 
# tested by Hudson on only 4.2.4 machines.
#    test2010_161.f90

# DQ (1/31/2011): Some F90 testcodes need to be be F03 testcodes.  These are
# fixed as only F03 codes (note that test2011_26.f03 is already a F03 code).
#     test2011_24.f90
#     test2011_25.f90
#     test2011_27.f90
#     test2011_28.f90
#     test2011_29.f90
#     test2011_30.f90
#     test2011_31.f90
#     test2011_32.f90
#     test2011_33.f90
#     test2011_34.f90
#     test2011_35.f90

# DQ (1/18/2011): Failing tests.
#     test2007_100.f90
#     test2007_135.f90 
#     test2007_219.f90 
#     test2010_35.f90
#     test2010_49.f90
#     test2010_59.f90 
#     test2010_136.f90 
#     test2010_151.f90 
#     test2010_154.f90 
#     test2010_174.f90 
#     test2011_05.f90 


# DQ (1/20/2011): Failing tests.
#     test2007_248.f90 
#     test2010_49.f90 
#     test2010_136.f90 
#     test2010_151.f90 

# This used to work, but fails how that test2010_172.f90 and test2010_184.f90 pass
# It seems that these are mutually exclusive!!!
#    test2007_103.f90
#    test2007_139.f90
#    test2007_189.f
#    test2007_227.f
#    test2010_29.f
#    test2010_30.f 
#    test2010_120.f90


# DQ (12/5/2010): This generates an internal gfortran compiler error.
# retest on Hudson later after we get the current work checked in.
# test2010_119.f90:29: internal compiler error: in emit_move_insn, at expr.c:3162
#     test2010_119.f90


# DQ (3/1/2017): If using Clang or Intel compilers then these variable might not be set. So avoid
# using GNU 5.4 compiler on Ubuntu system by mistake (see ROSE Matrix Testing for 2/28/2017).
if USING_GNU_COMPILER

if !ROSE_USING_GFORTRAN_VERSION_4_0
 # These are tests that fail using the gfrotran version 4.0 compiler (but pass with all other versions of gfortran).
   F90_TESTCODES_REQUIRED_TO_PASS += test2007_263.f90
endif

# DQ (2/1/2011): These test codes will not pass the initial syntax checking with the GNU gfortran version 4.4.x compiler.
#if !ROSE_USING_GFORTRAN_VERSION_4_4
if !ROSE_USING_GFORTRAN_VERSION_LATER_4_4
#  F90_TESTCODES_REQUIRED_TO_PASS += test2010_164.f90 test2011_37.f90 
   F90_TESTCODES_REQUIRED_TO_PASS += \
     test2010_164.f90 

# DQ (4/10/2011): And if it is not gfortran 4.5, then add this test code.
#if !ROSE_USING_GFORTRAN_VERSION_4_5
if !ROSE_USING_GFORTRAN_VERSION_LATER_4_5
   F90_TESTCODES_REQUIRED_TO_PASS += \
     test2011_37.f90 
endif
endif

endif


# DQ (4/9/2011): Insure++ has identified problems with these tests, great news but I am not prepared to fix 
# them yet. Currently these fail for Insure++, they also report errors which will be valuable in identifing 
# the root cause of the problems that we have seen with the altrnative-return support in Fortran.
# But as a basis for continuted testing, we first isolate these tests to NOT be run when Insure++
# is in use so that we can focus on this in a later iteration after we have initial testing into place.
# I want to make sure that these are the only problem that cause Insure++ to fail, then we can re-enable 
# these tests and track down the problem.
if !ROSE_USE_INSURE
# DQ (4/3/2011): The test2011_Rice_alternate-return-argument.f90 failes too frequently (not frequently 
# enough to reproduce the failure in a controled way, but frequently enough to hold back other progress
# in ROSE (e.g for other language and analysis support). I have forced this test to run where it appears
# to be reliable, but have skipped testing in on 4.1.x systems where it appears to be unreliable. The
# fault is not in the test but somewhere in ROSE (we expect), but it can't be reproduced so far in a 
# controled way. We are standing up more agressive static analysis and dynamic analysis testing in ROSE 
# which we hope will help address this issue.
# DQ (3/29/2011): Added test2011_Rice_alternate-return-argument.f90 to short list of test codes that
# appear to have a problem on 4.1.x version compilers.  Both of these codes implement the alternate 
# return but have been a problem on 4.1.x and maybe 4.3.x compilers (though might have less to do 
# with the gfortran versions than something else either in ROSE or in the JVM on these machines.
# The goal is to restrict these tests and see if the seamingly statistical failures on the Hudson
# machines stop, then we can expand the testing with this information.  Organizing the tests this 
# way will at least make sure that the tests are run, if only for one version of the compiler.
# DQ (2/13/2011): GNU gfortran 4.2 is a gnu configuration where I want to have test2010_161.f90 be tested.
# It has previously demonstrated a statistical failure on 4.1.2 and 4.3.2 (running it on 4.2 machines
# is an incremental step to getting it into more uniform testing, since I can't get it to fail locally).
# It also passes valgrind just fine, so this is a bit of a mystery at present.
# (4/19/2011): Removed problem tests from being tested in ROSE for now.
#if ROSE_USING_GFORTRAN_VERSION_4_2
#   F90_TESTCODES_REQUIRED_TO_PASS += \
#     test2010_161.f90 \
#     test2011_Rice_alternate-return-argument.f90 \
#     test2011_Rice_alt-return-call.f90
#endif
endif

# DQ (3/29/2011): This is now passing the LLVM tests, so I will leave it alone and thus leave it in to run on all platforms.
# test2011_Rice_alternate-return-argument.f90

# DQ (3/29/2011): Since Rice is using the gfortran 4.4.x compiler we want to also have these tests 
# run there. This should simplify their testing, while avoiding the 4.1.x and 4.3.x compilers where 
# this has been a problem.  However, it is still not clear that this problem anything to do with the 
# gfortran version (even though it appears to corrolate perfectly).
if ROSE_USING_GFORTRAN_VERSION_4_4
# DQ (4/2/2011): Removed this test from use on GNU 4.4.x machines since it appears to fail there.
# We are standing up more agressive static analysis and dynamic analysis testing in ROSE which 
# we hope will help address this issue.
#  F90_TESTCODES_REQUIRED_TO_PASS += \
# test2010_161.f90 
endif

F90_TESTCODES_REQUIRING_PREPROCESSOR_REQUIRED_TO_PASS =


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
     test2010_30.f \
     test2010_68.f \
     test2010_69.f \
     test2011_Rice_char-star1-function.f \
     test2011_Rice_char-star2-function.f \
     test2011_Rice_computed-goto.f \
     test2011_Rice_continuation-spits-token.f \
     test2011_Rice_initial-tab.f \
     test2011_Rice_intrinsic-mixup.f \
     test2011_Rice_lost-func-return-type.f \
     test2011_Rice_shared_continue.f \
     test2011_Rice_shared_label_stmt.f 


# DQ (1/18/2011): Failing tests.
#     test2007_150.f 
#     test2007_209.f 
#     test2007_227.f 
#     test2007_232.f 

# DQ (1/20/2011): Failing tests.
#     test2007_227.f 


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
     test2007_176.f03 \
     test2010_176.f03 \
     test2010_177.f03 \
     test2010_178.f03 \
     test2010_179.f03 \
     test2010_180.f03 \
     test2010_181.f03 \
     test2011_24.f03 \
     test2011_25.f03 \
     test2011_27.f03 \
     test2011_28.f03 \
     test2011_29.f03 \
     test2011_30.f03 \
     test2011_31.f03 \
     test2011_32.f03 \
     test2011_33.f03 \
     test2011_34.f03 \
     test2011_35.f03 \
     test2011_Rice_attrs-value-intent.f03 \
     test2011_Rice_attrs-pointer-array.f03 \
     test2011_Rice_bindc-name-2.f03 \
     test2011_Rice_bindc-name-sub.f03 \
     test2011_Rice_bindc-name.f03 \
     test2011_Rice_bindc.f03



# DQ (1/18/2011): Failing tests.
#     test2007_133.f03 

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
# This now passes as part of the F03_TESTCODES_REQUIRED_TO_PASS tests
#   F03_TESTCODES_REQUIRED_TO_PASS += test2007_43.f03
endif

# DXN (as of 08/10/2011):
F90_TESTCODES_REQUIRED_TO_PASS += \
     test2007_107.f90 \
     test2007_131.f90 \
     test2007_144.f90 \
     test2007_165.f90 \
     test2007_166.f90 \
     test2007_170.f90 \
     test2007_240.f90 \
     test2007_254.f90 \
     test2007_255.f90 \
     test2007_257.f90 \
     test2008_30.f90 \
     test2009_1.f90 \
     test2010_04.f90 \
     test2010_05.f90 \
     test2010_17.f90 \
     test2010_21.f90 \
     test2010_22.f90 \
     test2010_24.f90 \
     test2010_31.f90 \
     test2010_36.f90 \
     test2010_43.f90 \
     test2010_48.f90 \
     test2010_51.f90 \
     test2010_65.f90 \
     test2010_66.f90 \
     test2010_67.f90 \
     test2010_72.f90 \
     test2010_73.f90 \
     test2010_78.f90 \
     test2010_79.f90 \
     test2010_82.f90 \
     test2010_84.f90 \
     test2010_85.f90 \
     test2010_86.f90 \
     test2010_90.f90 \
     test2010_100.f90 \
     test2010_107.f90 \
     test2010_108.f90 \
     test2010_109.f90 \
     test2010_116.f90 \
     test2010_117.f90 \
     test2010_130.f90 \
     test2010_131.f90 \
     test2010_145.f90 \
     test2010_146.f90 \
     test2010_147.f90 \
     test2010_148.f90 \
     test2010_149.f90 \
     test2010_169.f90 \
     test2010_170.f90 \
     test2011_03.f90 \
     test2011_18.f90 \
     test2011_21.f90 \
     test2011_22.f90 \
     test2011_41.f90 \
     test2011_43.f90 \
     test2011_44.f90 \
     test2011_45.f90 \
     test2011_46.f90 \
     test2011_50.f90 \
     test2011_51.f90 \
     test2011_52.f90 \
     test2011_53.f90 \
     test2011_54.f90 \
     test2011_55.f90 \
     test2011_57.f90 \
     test2011_60.f90 \
     test2011_62.f90 \
     test2011_63.f90 \
     test2011_64.f90 \
     test2011_68.f90 \
     test2011_69.f90 \
     test2011_71.f90 \
     test2011_72.f90 \
     test2011_75.f90 \
     test2011_76.f90 

# DQ (11/11/2014): These test fail due to changes to support C++11 work (need to look into this seperately).
#    test2010_107.f90 

# DQ (3/4/13): Failing test for edg4x branch
# test2010_48.f90 

# DQ (2/21/13): Failing test for edg4x branch
# test2010_48.f90 
# test2010_66.f90 
# test2010_67.f90 
# test2010_146.f90 
# test2010_147.f90 
# test2010_149.f90 
# test2011_03.f90 
# test2011_53.f90 
# test2011_54.f90 
# test2011_55.f90 
# test2011_57.f90 
# test2011_62.f90 
# test2011_69.f90 
# phlin (as 08/22/2012):
# test case for ROSE-bugs #597
if ROSE_USING_GFORTRAN_VERSION_LATER_4_5
F90_TESTCODES_REQUIRED_TO_PASS += \
     test2012_SavedCommonBlock.f90 
endif

# DQ (9/30/2011): There is a long history of this being a statistical failure on at least OSX systems. 
# test2010_161.f90 
# Liao 12/15/2011. This one also uses alternative return according to Dan
#     test2011_66.f90  


# DXN (as of 08/10/2011):
F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS += \
     test2007_86.f \
     test2007_142.f \
     test2007_147.f \
     test2007_199.f \
     test2007_203.f \
     test2007_225.f \
     test2007_235.f \
     test2007_241.f \
     test2007_252.f \
     test2008_02.f \
     test2008_03.f \
     test2010_52.f \
     test2010_53.f \
     test2010_54.f \
     test2010_70.f \
     test2010_105.f \
     test2010_157.f \
     test2011_94.f 

# DXN (as of 08/10/2011):
F03_TESTCODES_REQUIRED_TO_PASS += \
     test2007_172.f03 \
     test2007_173.f03 \
     test2010_77.f03 \
     test2010_106.f03 \
     test2010_171.f03 \
     test2011_74.f03

# DQ (11/11/2014): These test fail due to changes to support C++11 work (need to look into this seperately).
#     test2010_106.f03 


# #######################
#      FAILING TESTS
# #######################

## Cray pointer Tests
F90_TESTCODES_CURRENTLY_FAILING = \
     test2007_108.f90 \
     test2007_46.f90 \
     test2007_81.f90 \
     test2007_82.f90 \
     test2007_83.f90 \
     test2007_84.f90 


F90_TESTCODES_CURRENTLY_FAILING += \
     test2007_70.f90 \
     test2007_87.f90 \
     test2007_88.f90 \
     test2007_152.f90 \
     test2007_177.f90 \
     test2007_237.f90 \
     test2007_239.f90 \
     test2007_LANL.f90 \
     test2007_example.f90 \
     test2010_19.f90 \
     test2010_26.f90 \
     test2010_71.f90 \
     test2010_80.f90 \
     test2010_83.f90 \
     test2010_87.f90 \
     test2010_88.f90 \
     test2010_89.f90 \
     test2010_91.f90 \
     test2010_92.f90 \
     test2010_93.f90 \
     test2010_94.f90 \
     test2010_95.f90 \
     test2010_96.f90 \
     test2010_97.f90 \
     test2010_99.f90 \
     test2010_101.f90 \
     test2010_102.f90 \
     test2010_103.f90 \
     test2010_110.f90 \
     test2010_175.f90 \
     test2011_11.f90 \
     test2011_23.f90 \
     test2011_38.f90 \
     test2011_42.f90 \
     test2011_47.f90 \
     test2011_48.f90 \
     test2011_49.f90 \
     test2011_56.f90 \
     test2011_58.f90 \
     test2011_59.f90 \
     test2011_61.f90 \
     test2011_65.f90 \
     test2011_67.f90 \
     test2011_73.f90 \
     test2011_Rice_A-cant-assert-keyword-nonnull.f90 \
     test2011_Rice_B-cant-assert-keyword-nonnull.f90 \
     test2011_Rice_C-cant-assert-keyword-nonnull.f90 \
     test2011_Rice_lost-func-return-type.f90 \
     triangle.f90 \
     X-comment-in-wrong-place.f90 \
     X-enddo-plus-label.f90  \
     X-multi-linebreaks.f90 \
     X-print-no-output-item-list.f90

# "X-*" tests are Rice-reported bugs not yet fixed.
# DQ(1/30/2011): test2010_150.* - test2010_168.* are Rice tests

# JJW -- this can't be in the f90 file list because it does not have the .f90 extension
#    test2008_02.f

F90_TESTCODES_REQUIRING_PREPROCESSOR_CURRENTLY_FAILING =  \
    test2008_01.F90
#if !OS_MACOSX
#  # Liao 2/11/2011. gfortran 4.2.0 on Mac cannot handle include '/long/absolute/path/file.h'
#  F90_TESTCODES_REQUIRING_PREPROCESSOR_CURRENTLY_FAILING += test2008_01.F90
#endif


F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING = \
     test2007_85.f \
     test2007_146.f \
     test2007_167.f \
     test2007_210.f \
     test2007_233.f \
     test2010_55.f \
     test2010_56.f \
     test2010_57.f 

F03_TESTCODES_CURRENTLY_FAILING = \
     test2007_18.f03 \
     test2007_171.f03 \
     test2007_180.f03 \
     test2010_27.f03 \
     test2010_74.f03 \
     test2010_75.f03 \
     test2010_76.f03 \
     test2010_104.f03 \
     test2010_126.f03 \
     test2011_26.f03 

F08_TESTCODES_CURRENTLY_FAILING = \
     test2010_98.f08

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

# DQ (11/20/2010): Added test codes requiring preprocessing
F90_TESTCODES_REQUIRING_PREPROCESSOR = \
$(F90_TESTCODES_REQUIRING_PREPROCESSOR_CURRENTLY_FAILING)

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

ALL_F90_TESTCODES_REQUIRING_PREPROCESSOR = \
$(F90_TESTCODES_REQUIRING_PREPROCESSOR_REQUIRED_TO_PASS) \
$(F90_TESTCODES_REQUIRING_PREPROCESSOR_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/FORTRAN_ROSE_PERFORMANCE_DATA.csv

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict $(PERFORMANCE_REPORT_OPTION)
# ROSE_FLAGS = -rose:verbose 3 -rose:output_parser_actions -I$(srcdir)
# ROSE_FLAGS = -rose:verbose 0 -rose:unparse_tokens -rose:output_parser_actions -I$(srcdir)
# ROSE_FLAGS = -rose:verbose 0 -rose:unparse_tokens -I$(srcdir)
# ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

# DQ (9/26/2011): Added optional test for dangling pointers to IR nodes in the AST (level 2 causes assertion if dangling pointers is found).
ROSE_FLAGS = -rose:verbose 0 -rose:detect_dangling_pointers 2 -I$(srcdir)

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

PASSING_TEST_F90_Objects = ${F90_TESTCODES:.f90=.o}
PASSING_TEST_F90_Sources = ${F90_TESTCODES:.f90=.f90.passed}
PASSING_TEST_REQUIRING_PREPROCESSOR_F90_Objects = ${F90_TESTCODES_REQUIRING_PREPROCESSOR:.F90=.o}
PASSING_TEST_REQUIRING_PREPROCESSOR_F90_Sources = ${F90_TESTCODES_REQUIRING_PREPROCESSOR:.F90=.F90.passed}
PASSING_TEST_F77_Objects = ${F77_TESTCODES:.f77=.o}
PASSING_TEST_F77_Sources = ${F77_TESTCODES:.f77=.f77.passed}

# DQ (3/11/2010): F77 tests were not being run, this fixes this.
# PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_TESTCODES:.f=.o}
PASSING_TEST_F77_FIXED_FORMAT_Objects = ${F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS:.f=.o}
PASSING_TEST_F77_FIXED_FORMAT_Sources = ${F77_FIXED_FORMAT_TESTCODES_REQUIRED_TO_PASS:.f=.f.passed}

PASSING_TEST_F03_Objects = ${F03_TESTCODES:.f03=.o}
PASSING_TEST_F03_Sources = ${F03_TESTCODES:.f03=.f03.passed}

TEST_F90_Objects = ${ALL_F90_TESTCODES:.f90=.o}
TEST_F90_Sources = ${ALL_F90_TESTCODES:.f90=.f90.passed}
TEST_REQUIRING_PREPROCESSOR_F90_Objects = ${ALL_F90_TESTCODES_REQUIRING_PREPROCESSOR:.F90=.o}
TEST_REQUIRING_PREPROCESSOR_F90_Sources = ${ALL_F90_TESTCODES_REQUIRING_PREPROCESSOR:.F90=.F90.passed}
TEST_F77_Objects = ${ALL_F77_TESTCODES:.f77=.o}
TEST_F77_Sources = ${ALL_F77_TESTCODES:.f77=.f77.passed}
TEST_F77_FIXED_FORMAT_Objects = ${ALL_F77_FIXED_FORMAT_TESTCODES:.f=.o}
TEST_F77_FIXED_FORMAT_Sources = ${ALL_F77_FIXED_FORMAT_TESTCODES:.f=.f.passed}
TEST_F03_Objects = ${ALL_F03_TESTCODES:.f03=.o}
TEST_F03_Sources = ${ALL_F03_TESTCODES:.f03=.f03.passed}

# PASSING_TEST_Objects = $(PASSING_TEST_F90_Objects) $(PASSING_TEST_F77_Objects) $(PASSING_TEST_F77_FIXED_FORMAT_Objects) $(PASSING_TEST_F03_Objects) 
PASSING_TEST_Objects = $(PASSING_TEST_REQUIRING_PREPROCESSOR_F90_Objects) $(PASSING_TEST_F90_Objects) $(PASSING_TEST_F77_Objects) $(PASSING_TEST_F77_FIXED_FORMAT_Objects) $(PASSING_TEST_F03_Objects) 
PASSING_TEST_Sources = $(PASSING_TEST_REQUIRING_PREPROCESSOR_F90_Sources) $(PASSING_TEST_F90_Sources) $(PASSING_TEST_F77_Sources) $(PASSING_TEST_F77_FIXED_FORMAT_Sources) $(PASSING_TEST_F03_Sources) 

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../testTranslator:
	cd ../..; $(MAKE) testTranslator
if ROSE_USE_INSURE
	@echo "Force the lt-testTranslator to be built in ../../.libs so that we can rename it for Insure++ runs."
	../../testTranslator --help > /dev/null
endif

testTranslator: ../../testTranslator

../../testGraphGeneration:
	cd ../..; $(MAKE) testGraphGeneration

testGraphGeneration: ../../testGraphGeneration

../../testTokenGeneration:
	cd ../..; $(MAKE) testTokenGeneration

testTokenGeneration: ../../testTokenGeneration

# DQ (10/4/2010): Added tests to make sure that the AST graphs can be generated for all
# Fortran input test codes. This tests a problem that was reported previously by Rice.
# $(TEST_F90_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
# # $(TEST_F90_Objects): ../../testTranslator
# # Example of how to call the OpenFortranParser
# #	java parser.java.FortranMain --class parser.java.FortranParserActionJNI $(srcdir)/$(@:.o=.f90)
# #	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f90)
# # How to call a ROSE translator with a Fortran source file.
# if !ROSE_USE_INSURE
# 	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
# 	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
# 	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
# #	$(VALGRIND) ../../testParser $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
# else
# #	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
# #	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
# 	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
# 	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
# 	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
# 	echo "Removing the just built copy of lt-testTranslator to save space..."
# 	rm -f testTranslator_$(@:.o=)
# endif

# Rasmussen (12/20/2018): Path to the SDF Fortran parse table
SDF_OFP = $(top_srcdir)/src/3rdPartyLibraries/experimental-fortran-parser/share/rose

$(TEST_F90_Sources): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
if !ROSE_USE_INSURE
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@
#	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
	@$(RTH_RUN) CMD="../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $(@:.f90.passed=.f90.testGraphGeneration.passed)
#	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
	@$(RTH_RUN) CMD="../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $(@:.f90.passed=.f90.testTokenGeneration.passed)
#	$(VALGRIND) ../../testParser $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#	sglri -i $(srcdir)/$(@:.f90.passed=.f90) -p $(top_srcdir)/src/3rdPartyLibraries/experimental-fortran-parser/share/rose/Fortran.tbl | pp-aterm -o $(@:.f90.passed=.f90).aterm
#	@$(RTH_RUN) CMD="sglri -i $(srcdir)/$(@:.f90.passed=.f90) -p $(SDF_OFP)/Fortran.tbl | pp-aterm -o $(@:.f90.passed=.f90).aterm" $(top_srcdir)/scripts/test_exit_status $(@:.f90.passed=.f90.testAtermGeneration.passed) || true
# Rasmussen (12/20/2018): Turned off running experimental frontend on normal Fortran tests.  Not ready for these tests yet!
#	@$(RTH_RUN) CMD="../../testTranslator -rose:experimental_fortran_frontend $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $(@:.f90.passed=.f90.testTokenGeneration.passed)
if ROSE_WITH_ATERM
# Rasmussen (12/20/2018): Turned off running experimental frontend on normal Fortran tests.  Not ready for these tests yet!
#	@$(RTH_RUN) CMD="../../testAtermGraphGeneration $(@:.f90.passed=.f90.aterm)" $(top_srcdir)/scripts/test_exit_status $(@:.f90.passed=.f90.testAtermGraphGeneration.passed) || true
endif
endif
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
#	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)
	echo "Removing the just built copy of lt-testTranslator to save space..."
	rm -f testTranslator_$(@:.o=)
endif

# $(TEST_REQUIRING_PREPROCESSOR_F90_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
# if !ROSE_USE_INSURE
# 	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
# 	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
# 	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
# else
# #	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
# 	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
# 	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
# 	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
# 	echo "Removing the just built copy of lt-testTranslator to save space..."
# 	rm -f testTranslator_$(@:.o=)
# endif

$(TEST_REQUIRING_PREPROCESSOR_F90_Sources): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
if !ROSE_USE_INSURE
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.F90.passed=.F90)" $(top_srcdir)/scripts/test_exit_status $@
#	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
	@$(RTH_RUN) CMD="../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.F90.passed=.F90)" $(top_srcdir)/scripts/test_exit_status $(@:.F90.passed=.F90.testGraphGeneration.passed)
#	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
	@$(RTH_RUN) CMD="../../testTokenGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.F90.passed=.F90)" $(top_srcdir)/scripts/test_exit_status $(@:.F90.passed=.F90.testTokenGeneration.passed)
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#	@$(RTH_RUN) CMD="sglri -i $(srcdir)/$(@:.F90.passed=.F90) -p $(SDF_OFP)/Fortran.tbl | pp-aterm -o $(@:.F90.passed=.F90).aterm" $(top_srcdir)/scripts/test_exit_status $(@:.F90.passed=.F90.testAtermGeneration.passed) || true
# Rasmussen (12/20/2018): Turned off running experimental frontend on normal Fortran tests.  Not ready for these tests yet!
#	@$(RTH_RUN) CMD="../../testTranslator -rose:experimental_fortran_frontend $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.F90.passed=.F90)" $(top_srcdir)/scripts/test_exit_status $(@:.F90.passed=.F90.testTokenGeneration.passed)
endif
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.F90)
	echo "Removing the just built copy of lt-testTranslator to save space..."
	rm -f testTranslator_$(@:.o=)
endif

# $(TEST_F77_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
# #	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f77)
# if !ROSE_USE_INSURE
# 	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
# 	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
# 	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
# else
# #	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
# #	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
# 	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
# 	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
# 	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
# 	echo "Removing the just built copy of lt-testTranslator to save space..."
# 	rm -f testTranslator_$(@:.o=)
# endif

$(TEST_F77_Sources): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f77)
if !ROSE_USE_INSURE
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f77.passed=.f77)" $(top_srcdir)/scripts/test_exit_status $@
#	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
	@$(RTH_RUN) CMD="../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f77.passed=.f77)" $(top_srcdir)/scripts/test_exit_status $(@:.f77.passed=.f77.testGraphGeneration.passed)
#	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
	@$(RTH_RUN) CMD="../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f77.passed=.f77)" $(top_srcdir)/scripts/test_exit_status $(@:.f77.passed=.f77.testTokenGeneration.passed)
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#	@$(RTH_RUN) CMD="sglri -i $(srcdir)/$(@:.f77.passed=.f77) -p $(SDF_OFP)/Fortran.tbl | pp-aterm -o $(@:.f77.passed=.f77).aterm" $(top_srcdir)/scripts/test_exit_status $(@:.f77.passed=.f77.testAtermGeneration.passed) || true
# Rasmussen (12/20/2018): Turned off running experimental frontend on normal Fortran tests.  Not ready for these tests yet!
#	@$(RTH_RUN) CMD="../../testTranslator -rose:experimental_fortran_frontend $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f77.passed=.f77)" $(top_srcdir)/scripts/test_exit_status $(@:.f77.passed=.f77.experimental.passed)
endif
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
#	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f77)
	echo "Removing the just built copy of lt-testTranslator to save space..."
	rm -f testTranslator_$(@:.o=)
endif

# $(TEST_F77_FIXED_FORMAT_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
# #	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f)
# if !ROSE_USE_INSURE
# 	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
# 	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
# 	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
# else
# #	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
# #	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
# 	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
# 	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
# 	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
# 	echo "Removing the just built copy of lt-testTranslator to save space..."
# 	rm -f testTranslator_$(@:.o=)
# endif

$(TEST_F77_FIXED_FORMAT_Sources): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration ${ALL_TESTCODES}
#	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f)
if !ROSE_USE_INSURE
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f.passed=.f)" $(top_srcdir)/scripts/test_exit_status $@
#	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
	@$(RTH_RUN) CMD="../../testGraphGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f.passed=.f)" $(top_srcdir)/scripts/test_exit_status $(@:.f.passed=.f.testGraphGeneration.passed)
#	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
	@$(RTH_RUN) CMD="../../testTokenGeneration $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f.passed=.f)" $(top_srcdir)/scripts/test_exit_status $(@:.f.passed=.f.testTokenGeneration.passed)
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
#	@$(RTH_RUN) CMD="sglri -i $(srcdir)/$(@:.f.passed=.f) -p $(SDF_OFP)/Fortran.tbl | implode-asfix | pp-aterm -o $(@:.f.passed=.f).aterm" $(top_srcdir)/scripts/test_exit_status $(@:.f.passed=.f.testAtermGeneration.passed) || true
# Rasmussen (12/20/2018): Turned off running experimental frontend on normal Fortran tests.  Not ready for these tests yet!
#	@$(RTH_RUN) CMD="../../testTranslator -rose:experimental_fortran_frontend$(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.f.passed=.f)" $(top_srcdir)/scripts/test_exit_status $(@:.f.passed=.f.testTokenGeneration.passed)
endif
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
#	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:f77 -c $(srcdir)/$(@:.o=.f)
	echo "Removing the just built copy of lt-testTranslator to save space..."
	rm -f testTranslator_$(@:.o=)
endif

# Note: If using gfortran version greater then 4.1, we can't call it with Fortran 2003 code since it is so buggy.
#       So we use -rose:skip_syntax_check to avoid the frontend syntax checking and -rose:skipfinalCompileStep
#       to avoid processing the generated code (which would have the same problem).  It is easier to just use
#       the version 4.1 gfortran which has better Fortran 2003 syntax checking support. This problem is confirmed 
#       on versions of gfortran 4.2 and 4.3.
# $(TEST_F03_Objects): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration 
# if !ROSE_USE_INSURE
# #	java parser.java.FortranMain --dump $(srcdir)/$(@:.o=.f03)
# #	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
# #	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
# 	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
# #	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
# #	Since gfortran does not support F03, and thus we skip the compileation using the backend gfortran compiler, we have to force the generation of an object file. 
# #	Build a object file since gfortran fails to do so (avoids remaking the makefile target)
# 	@echo "   ...force generation of $@, because gfortran compiler does not handle fortran 2003 standard (and the makefile needs $@ to avoid recompilation)."
# 	@touch $@
# else
# #	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
# #	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
# 	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
# 	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
# 	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
# 	echo "Removing the just built copy of lt-testTranslator to save space..."
# 	rm -f testTranslator_$(@:.o=)
# endif

$(TEST_F03_Sources): ../../testTranslator ../../testGraphGeneration ../../testTokenGeneration 
if !ROSE_USE_INSURE
#	$(VALGRIND) ../../testGraphGeneration $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
	@$(RTH_RUN) CMD="../../testGraphGeneration $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.f03.passed=.f03)" $(top_srcdir)/scripts/test_exit_status $@
#	$(VALGRIND) ../../testTokenGeneration $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
#	Since gfortran does not support F03, and we skip the compilation using the backend gfortran compiler, we have to force the generation of an object file. 
#	Build a object file since gfortran fails to do so (avoids remaking the makefile target)
	@echo "   ...force generation of $@, because gfortran compiler does not handle fortran 2003 standard (and the makefile needs $@ to avoid recompilation)."
	@touch $@
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
	@$(RTH_RUN) CMD="sglri -i $(srcdir)/$(@:.f03.passed=.f03) -p $(SDF_OFP)/Fortran.tbl | pp-aterm -o $(@:.f03.passed=.f03).aterm" $(top_srcdir)/scripts/test_exit_status $(@:.f03.passed=.f03.testAtermGeneration.passed) || true
endif
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
#	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; exit 1; fi
	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
	./testTranslator_$(@:.o=) $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $(srcdir)/$(@:.o=.f03)
	echo "Removing the just built copy of lt-testTranslator to save space..."
	rm -f testTranslator_$(@:.o=)
endif

# DQ (4/6/2010): Added test for parsing only (tested here for C++ code)
testParser: ../../testParser
	$(VALGRIND) ../../testParser $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/test2007_01.f90


QMTEST_Objects = ${ALL_TESTCODES:.f90=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.c)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.f90)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# DQ (11/5/2016): Fixed this to use the RTH script so that address sanitizer could be used consistantly for all tests.
# DQ (11/11/2008): Module test copied from developersScratchSpace/Dan/Fortran_tests
# This file declares a module used by module_B_file.f90
# module_A_file.o: testTranslator
# TEST_F90_Sources = ${ALL_F90_TESTCODES:.f90=.f90.passed}
module_A_file.f90.passed: testTranslator
# module_A_file.o: testTranslator
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)
#	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.F90.passed=.F90)" $(top_srcdir)/scripts/test_exit_status $@
#	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $(srcdir)/$(@:.o=.f90)
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@
#	../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)

# Force representation of module dependency so that mod files are properly generated before use.
# module_B_file.o: testTranslator module_A_file.o
module_B_file.f90.passed: testTranslator module_A_file.f90.passed
#	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)
#	../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.f90)
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@

test2019_module_1_file.f90.passed: testTranslator
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@

# Force representation of module dependency so that mod files are properly generated before use.
test2019_module_2_file.f90.passed: testTranslator test2019_module_1_file.f90.passed
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@

test2019_private_attribute2.f90.passed: testTranslator
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@

# Force representation of module dependency so that mod files are properly generated before use.
test2019_private_attribute.f90.passed: testTranslator test2019_private_attribute2.f90.passed
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@

test2020_use_iso_c_binding_3.f90.passed: testTranslator
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@

# Force representation of module dependency so that mod files are properly generated before use.
test2020_use_iso_c_binding_4.f90.passed: testTranslator test2020_use_iso_c_binding_3.f90.passed
	@$(RTH_RUN) CMD="../../testTranslator $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.f90.passed=.f90)" $(top_srcdir)/scripts/test_exit_status $@

test2010_01.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

test2010_02.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

test2010_05.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:relax_syntax_check -c $(srcdir)/$(@:.o=.f90)

# Output spew test from John.
outputSpew.o:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:caf -rose:skip_syntax_check -c $(srcdir)/$(@:.o=.caf)

# Makefile rule to reproduce a current OFP bug that causes OFP to fail silently (not returning a proper exit code).
current_OFP_error:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:exit_after_parser -c $(srcdir)/test2010_31.f90

# DQ (3/29/2011): Added special test to support this problem that appears correlated to gfortran 4.1.2
# though might be unrelated to the gfortran version number.
problemTests:
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2010_161.f90
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2011_Rice_alternate-return-argument.f90


# We will build a larger collection of module tests soon (supported by makefile dependence rules).
# PASSING_MODULE_TEST_Objects = module_B_file.o
PASSING_MODULE_TEST_Objects = module_B_file.f90.passed test2019_module_2_file.f90.passed test2019_private_attribute.f90.passed test2020_use_iso_c_binding_4.f90.passed

# DQ (10/4/2010): Test calling OFP directly and ROSE for different filename extensions.
CALL_OFP = java -Djava.class.path=$(top_srcdir)/src/3rdPartyLibraries/antlr-jars/antlr-3.5.2-complete.jar:$(top_builddir)/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-0.8.5.jar fortran.ofp.FrontEnd
testSuffixRules:
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_01.f
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_01.f
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_02.f77
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_02.f77
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_03.f90
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_03.f90
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_04.f95
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_04.f95
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_05.f03
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_05.f03
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_06.F
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_06.F
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_07.F90
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_07.F90
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_08.F95
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_08.F95
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_09.F03
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_09.F03
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_10.F08
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_10.F08
	$(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_11.f08
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/test2007_suffixTest_11.f08

testOFP:
	 $(CALL_OFP) --dump $(srcdir)/test2007_suffixTest_01.f

# DQ (10/18/2010): Test suggested from bug reported by Carlo Bertolli at Imperial.
testCPP_Defines:
#	Test source code with macro defined from the command line: e.g. "integer*PS  pe"
	../../testTranslator $(ROSE_FLAGS) -DPS=8 $(srcdir)/inputUsingDefinesOnCommandline.F90

testMPI:
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/mpi.f90
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/mpi_test_01.f90

testMPItypes:
	../../testTranslator $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep $(srcdir)/ISO_C_BINDING.f03
	../../testTranslator $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep $(srcdir)/mpi_f08_types.f03
	../../testTranslator $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep $(srcdir)/mpi_f08_interfaces_test.f03

testMultipleFortranFiles: $(srcdir)/test2010_78.f90 $(srcdir)/test2010_79.f90
	../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/test2010_78.f90 $(srcdir)/test2010_79.f90 || cat $(srcdir)/test2010_79.f90 rose_test2010_79.f90

# Pei-Hung (2/25/2020) test if output still contains comment after preprocessor
REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/Fortran_tests/referenceResults
testPreprocessedOutput: $(srcdir)/test2007_suffixTest_07.F90
	../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/test2007_suffixTest_07.F90 
	bash -c "diff <(tail -n 6 rose_test2007_suffixTest_07.F90) <(tail -n 6 $(REFERENCE_PATH)/rose_test2007_suffixTest_07.F90.output)" 
	

# DQ (11/5/2010): This bug causes the unparsed second file to include a Fortran "include" statement (magically).
# Note that the compilation will generate an error not caught by make since the use of "cat" will succeed.
riceBug1: $(srcdir)/test2010_78.f90 $(srcdir)/test2010_79.f90
	../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/test2010_78.f90 $(srcdir)/test2010_79.f90 || cat $(srcdir)/test2010_79.f90 rose_test2010_79.f90

# DQ (11/5/2010): When unparsed, this file is missing the statement "i1 = 1" in the generated code.
# DQ (11/30/2010): Rice has a fix for this in their merge that should be checked in shortly.
riceBug2:$(srcdir)/test2010_78.f90 $(srcdir)/test2010_80.f90
#	../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/test2010_78.f90
#	../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/test2010_80.f90
	../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/test2010_78.f90
	../../testGraphGeneration $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/test2010_80.f90
	@echo "Source code to test2010_80.f90"
	@cat $(srcdir)/test2010_80.f90
	@echo "Generated code for rose_test2010_80.f90 (missing \"i1 = 1\""
	@cat rose_test2010_80.f90

bug3:
	../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:output_parser_actions -I$(srcdir) -c $(srcdir)/test2010_81.f90

bug4:
	../../testTranslator $(ROSE_FLAGS) -rose:output_parser_actions -I$(srcdir) -c $(srcdir)/verify.f

# DQ (9/19/2011): Added Cray pointer test using test code from Imperial College London.
crayPointerTest: testTranslator test2011_imperial_crayPointers.f90
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:cray_pointer_support -rose:skip_syntax_check -rose:skipfinalCompileStep -c $(srcdir)/test2011_imperial_crayPointers.f90
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) -rose:f90 -rose:skip_syntax_check -rose:skipfinalCompileStep -c $(srcdir)/test2011_imperial_crayPointers.f90

# DQ (4/9/2011): Let's at least run these in parallel (more important for the Insure++ tests).
extra_tests: $(PASSING_MODULE_TEST_Objects) testCPP_Defines testMPItypes testMultipleFortranFiles testPreprocessedOutput

# DQ (2/2/2011): We are now enforcing the module names are unique so that we can always run the 
# test code in parallel and avoid name conflicts with *.mod (gfortran) and *.rmod (rose) files.
enforce_module_name_policy:
	$(top_srcdir)/scripts/policies/UniqueFortranModules.pl $(srcdir)


tests:
	@$(MAKE) enforce_module_name_policy
#	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(PASSING_TEST_Sources)
#	@$(MAKE) $(PASSING_MODULE_TEST_Objects)
#	@$(MAKE) testCPP_Defines
#	@$(MAKE) testMPItypes
#	@$(MAKE) testMultipleFortranFiles
	@$(MAKE) extra_tests
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Fortran_tests: make tests rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

conditional-check-local:
	@echo "Tests for Fortran."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These Fortran programs must be called externally to the test codes in the "TESTCODES" make variable
if !ROSE_USING_GFORTRAN_VERSION_4_0
#	DQ (2/2/2011): This test presently fails (test codes are being fixed).
	@$(MAKE) enforce_module_name_policy
#	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(PASSING_TEST_Sources)
	@$(MAKE) $(F90_COMMENTS_REQUIRED_TO_PASS_RESULTS_DIFF)
#	@$(MAKE) $(PASSING_MODULE_TEST_Objects)
#	@$(MAKE) testCPP_Defines
#	@$(MAKE) testMPItypes
#	@$(MAKE) testMultipleFortranFiles
	@$(MAKE) extra_tests
#	DQ (4/6/2010): This fails on Hudson, but I don't know why.
#	@$(MAKE) testParser
else
	@echo "Skipping Fortran code tests when using the gfortran version 4.0.x compiler (fails to support common command line options of later versions)."
endif
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Fortran_tests: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

else

# Fortran support not available in ROSE unless specified on the configure command line.
conditional-check-local:
	@echo "***********************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Fortran_tests: FORTRAN NOT CONFIGURED IN ROSE ******"
	@echo "***********************************************************************************"
endif

# This will output the example testcodes that are failing
listProblemFiles: $(F77_TESTCODES_CURRENTLY_FAILING) $(F90_TESTCODES_CURRENTLY_FAILING) $(F03_TESTCODES_CURRENTLY_FAILING) $(F77_FIXED_FORMAT_TESTCODES_CURRENTLY_FAILING)
	egrep -n "*"  $(srcdir)/$(?)


# Make sure that all the test codes end up in the distribution
# EXTRA_DIST = $(ALL_TESTCODES)
EXTRA_DIST = $(ALL_F77_FIXED_FORMAT_TESTCODES) $(ALL_F77_TESTCODES) $(ALL_F90_TESTCODES) $(ALL_F03_TESTCODES) \
   $(ALL_F90_TESTCODES_REQUIRING_PREPROCESSOR) \
   test2007_suffixTest_01.f test2007_suffixTest_02.f77 test2007_suffixTest_03.f90 test2007_suffixTest_04.f95 \
   test2007_suffixTest_05.f03 test2007_suffixTest_06.F test2007_suffixTest_07.F90 test2007_suffixTest_08.F95 \
   test2007_suffixTest_09.F03 test2007_suffixTest_10.F08 test2007_suffixTest_11.f08 \
   module_A_file.f90 module_B_file.f90 test2010_31_header.f90 test2010_50.h test2010_54.h test2010_55.h \
   original_mpif.h inputUsingDefinesOnCommandline.F90 fortran_foo_single_quote.h fortran_foo_double_quote.h \
   cpp_foo.h cpp_foobar.h ISO_C_BINDING.f03 mpi_f08_types.f03 mpi_f08_interfaces_test.f03 mpif.h mpiof.h

# DQ (3/12/2010): Fortran 2008 tests from OFP
# f08-tests

check-local: conditional-check-local

clean-local:
	rm -f *.o rose_*.f* *.rmod
	rm -f *.s *.mod rose_*.f rose_*.f77 rose_*.f90 rose_*.f95 rose_*.f03 rose_*.f08 *.dot *.pdf *~ *.ps *.out *.log rose_performance_report_lockfile.lock
	rm -f rose_*.F rose_*.F77 rose_*.F90 rose_*.F95 rose_*.F03 rose_*.F08 test2007_suffixTest_0?_postprocessed.*
	rm -f *_postprocessed.* 
	rm -rf QMTest
	rm -f *.passed *.failed *.err
	rm -f *.aterm $(F90_COMMENTS_REQUIRED_TO_PASS_RESULTS_DIFF)
if ROSE_USE_INSURE
#	Remove the links built to get the filename being compiled into the name of the executable.
	rm -f testTranslator_*
	rm -f *.rpt
endif

# SKW's trivial change to force new Hudson run
