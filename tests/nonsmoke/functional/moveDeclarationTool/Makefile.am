include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
AM_CPPFLAGS =  \
        $(ROSE_INCLUDES) \
        -I$(top_srcdir)/src/frontend/SageIII/sageInterface

AM_LDFLAGS = $(ROSE_RPATHS)

MOVE_TOOL = $(top_builddir)/tools/moveDeclarationToInnermostScope

$(MOVE_TOOL):
	make -C $(top_builddir)/tools
#--------------------------------------------
MOVE_DECLS_TESTCODES_FULLY_SUPPORTED  = \
  inputmoveDeclarationToInnermostScope_1.C \
  inputmoveDeclarationToInnermostScope_2.C \
  inputmoveDeclarationToInnermostScope_3.C \
  inputmoveDeclarationToInnermostScope_4.C \
  inputmoveDeclarationToInnermostScope_5.C \
  inputmoveDeclarationToInnermostScope_6.C \
  inputmoveDeclarationToInnermostScope_7.C \
  inputmoveDeclarationToInnermostScope_8.C \
  inputmoveDeclarationToInnermostScope_9.C \
  inputmoveDeclarationToInnermostScope_10.C \
  inputmoveDeclarationToInnermostScope_11.C \
  inputmoveDeclarationToInnermostScope_12.C \
  inputmoveDeclarationToInnermostScope_13.C \
  inputmoveDeclarationToInnermostScope_14.C \
  inputmoveDeclarationToInnermostScope_15.C  \
  inputmoveDeclarationToInnermostScope_16.C \
  inputmoveDeclarationToInnermostScope_18.C \
  inputmoveDeclarationToInnermostScope_19.C \
  inputmoveDeclarationToInnermostScope_20.C \
  inputmoveDeclarationToInnermostScope_21.C \
  inputmoveDeclarationToInnermostScope_22.C \
  inputmoveDeclarationToInnermostScope_23.C \
  inputmoveDeclarationToInnermostScope_24.C \
  inputmoveDeclarationToInnermostScope_25.C \
  inputmoveDeclarationToInnermostScope_26.C \
  inputmoveDeclarationToInnermostScope_27.C \
  inputmoveDeclarationToInnermostScope_test2015_126.C \
  inputmoveDeclarationToInnermostScope_test2015_127.C

# DQ (12/23/2014): This fails because the normal unparsing (without token unparsing) is failing.
# inputmoveDeclarationToInnermostScope_test2014_26.C

#MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED = 

# identity translator cannot handle them properly  
# 17: shared SgArrayType issue
MOVE_DECLS_TESTCODES_FAILING = \
  inputmoveDeclarationToInnermostScope_17.C \
  inputmoveDeclarationToInnermostScope_test2014_18.C \
  inputmoveDeclarationToInnermostScope_test2014_22.C

MOVE_DECLS_TESTCODES_CURRENTLY_FAILING =

MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS = $(addprefix rose_v1_, ${MOVE_DECLS_TESTCODES_FULLY_SUPPORTED})
#MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS = $(addprefix rose_, ${MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED})

MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS = $(addprefix rose_v2_, ${MOVE_DECLS_TESTCODES_FULLY_SUPPORTED})
#MOVE_MERGE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS = $(addprefix rose_v2_, ${MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED})

MOVE_MERGE_DECLS_NO_VAR_NORM_TESTCODES_FULLY_SUPPORTED_RESULTS = $(addprefix rose_v3_, ${MOVE_DECLS_TESTCODES_FULLY_SUPPORTED})
PASSING_MOVE_DECLS_TESTCODES_RESULTS = \
   $(MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS) 

#   $(MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS)                                                                                                                                                                                        
# DQ (12/23/2014): Added tests specific to use of move tool with token-based unparsing.                                                                                                                                                        
MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED  = \
  inputmoveDeclarationToInnermostScope_1.C \
  inputmoveDeclarationToInnermostScope_2.C \
  inputmoveDeclarationToInnermostScope_3.C \
  inputmoveDeclarationToInnermostScope_4.C \
  inputmoveDeclarationToInnermostScope_6.C \
  inputmoveDeclarationToInnermostScope_7.C \
  inputmoveDeclarationToInnermostScope_8.C \
  inputmoveDeclarationToInnermostScope_9.C \
  inputmoveDeclarationToInnermostScope_10.C \
  inputmoveDeclarationToInnermostScope_11.C \
  inputmoveDeclarationToInnermostScope_12.C \
  inputmoveDeclarationToInnermostScope_13.C \
  inputmoveDeclarationToInnermostScope_14.C \
  inputmoveDeclarationToInnermostScope_15.C \
  inputmoveDeclarationToInnermostScope_16.C \
  inputmoveDeclarationToInnermostScope_18.C \
  inputmoveDeclarationToInnermostScope_test2014_01.C \
  inputmoveDeclarationToInnermostScope_test2014_02.C \
  inputmoveDeclarationToInnermostScope_test2014_03.C \
  inputmoveDeclarationToInnermostScope_test2014_04.C \
  inputmoveDeclarationToInnermostScope_test2014_05.C \
  inputmoveDeclarationToInnermostScope_test2014_06.C \
  inputmoveDeclarationToInnermostScope_test2014_07.C \
  inputmoveDeclarationToInnermostScope_test2014_08.C \
  inputmoveDeclarationToInnermostScope_test2014_10.C \
  inputmoveDeclarationToInnermostScope_test2014_11.C \
  inputmoveDeclarationToInnermostScope_test2014_13.C \
  inputmoveDeclarationToInnermostScope_test2014_14.C \
  inputmoveDeclarationToInnermostScope_test2014_15.C \
  inputmoveDeclarationToInnermostScope_test2014_16.C \
  inputmoveDeclarationToInnermostScope_test2014_17.C \
  inputmoveDeclarationToInnermostScope_test2014_19.C \
  inputmoveDeclarationToInnermostScope_test2014_20.C \
  inputmoveDeclarationToInnermostScope_test2014_21.C \
  inputmoveDeclarationToInnermostScope_test2014_23.C \
  inputmoveDeclarationToInnermostScope_test2014_24.C \
  inputmoveDeclarationToInnermostScope_test2014_25.C \
  inputmoveDeclarationToInnermostScope_test2014_26.C \
  inputmoveDeclarationToInnermostScope_test2014_27.C \
  inputmoveDeclarationToInnermostScope_test2014_28.C \
  inputmoveDeclarationToInnermostScope_test2014_30.C \
  inputmoveDeclarationToInnermostScope_test2014_31.C \
  inputmoveDeclarationToInnermostScope_test2015_01.C \
  inputmoveDeclarationToInnermostScope_test2015_02.C \
  inputmoveDeclarationToInnermostScope_test2015_03.C \
  inputmoveDeclarationToInnermostScope_test2015_04.C \
  inputmoveDeclarationToInnermostScope_test2015_05.C \
  inputmoveDeclarationToInnermostScope_test2015_06.C \
  inputmoveDeclarationToInnermostScope_test2015_07.C \
  inputmoveDeclarationToInnermostScope_test2015_08.C \
  inputmoveDeclarationToInnermostScope_test2015_09.C \
  inputmoveDeclarationToInnermostScope_test2015_10.C \
  inputmoveDeclarationToInnermostScope_test2015_11.C \
  inputmoveDeclarationToInnermostScope_test2015_12.C \
  inputmoveDeclarationToInnermostScope_test2015_13.C \
  inputmoveDeclarationToInnermostScope_test2015_14.C \
  inputmoveDeclarationToInnermostScope_test2015_15.C \
  inputmoveDeclarationToInnermostScope_test2015_16.C \
  inputmoveDeclarationToInnermostScope_test2015_17.C \
  inputmoveDeclarationToInnermostScope_test2015_18.C \
  inputmoveDeclarationToInnermostScope_test2015_19.C \
  inputmoveDeclarationToInnermostScope_test2015_20.C \
  inputmoveDeclarationToInnermostScope_test2015_21.C \
  inputmoveDeclarationToInnermostScope_test2015_22.C \
  inputmoveDeclarationToInnermostScope_test2015_23.C \
  inputmoveDeclarationToInnermostScope_test2015_24.C \
  inputmoveDeclarationToInnermostScope_test2015_25.C \
  inputmoveDeclarationToInnermostScope_test2015_26.C \
  inputmoveDeclarationToInnermostScope_test2015_27.C \
  inputmoveDeclarationToInnermostScope_test2015_28.C \
  inputmoveDeclarationToInnermostScope_test2015_29.C \
  inputmoveDeclarationToInnermostScope_test2015_30.C \
  inputmoveDeclarationToInnermostScope_test2015_31.C \
  inputmoveDeclarationToInnermostScope_test2015_32.C \
  inputmoveDeclarationToInnermostScope_test2015_33.C \
  inputmoveDeclarationToInnermostScope_test2015_34.C \
  inputmoveDeclarationToInnermostScope_test2015_35.C \
  inputmoveDeclarationToInnermostScope_test2015_36.C \
  inputmoveDeclarationToInnermostScope_test2015_37.C \
  inputmoveDeclarationToInnermostScope_test2015_38.C \
  inputmoveDeclarationToInnermostScope_test2015_39.C \
  inputmoveDeclarationToInnermostScope_test2015_40.C \
  inputmoveDeclarationToInnermostScope_test2015_41.C \
  inputmoveDeclarationToInnermostScope_test2015_42.C \
  inputmoveDeclarationToInnermostScope_test2015_43.C \
  inputmoveDeclarationToInnermostScope_test2015_44.C \
  inputmoveDeclarationToInnermostScope_test2015_45.C \
  inputmoveDeclarationToInnermostScope_test2015_46.C \
  inputmoveDeclarationToInnermostScope_test2015_47.C \
  inputmoveDeclarationToInnermostScope_test2015_48.C \
  inputmoveDeclarationToInnermostScope_test2015_49.C \
  inputmoveDeclarationToInnermostScope_test2015_50.C \
  inputmoveDeclarationToInnermostScope_test2015_51.C \
  inputmoveDeclarationToInnermostScope_test2015_52.C \
  inputmoveDeclarationToInnermostScope_test2015_53.C \
  inputmoveDeclarationToInnermostScope_test2015_54.C \
  inputmoveDeclarationToInnermostScope_test2015_55.C \
  inputmoveDeclarationToInnermostScope_test2015_56.C \
  inputmoveDeclarationToInnermostScope_test2015_57.C \
  inputmoveDeclarationToInnermostScope_test2015_58.C \
  inputmoveDeclarationToInnermostScope_test2015_59.C \
  inputmoveDeclarationToInnermostScope_test2015_60.C \
  inputmoveDeclarationToInnermostScope_test2015_61.C \
  inputmoveDeclarationToInnermostScope_test2015_62.C \
  inputmoveDeclarationToInnermostScope_test2015_63.C \
  inputmoveDeclarationToInnermostScope_test2015_64.C \
  inputmoveDeclarationToInnermostScope_test2015_65.C \
  inputmoveDeclarationToInnermostScope_test2015_66.C \
  inputmoveDeclarationToInnermostScope_test2015_67.C \
  inputmoveDeclarationToInnermostScope_test2015_68.C \
  inputmoveDeclarationToInnermostScope_test2015_69.C \
  inputmoveDeclarationToInnermostScope_test2015_70.C \
  inputmoveDeclarationToInnermostScope_test2015_71.C \
  inputmoveDeclarationToInnermostScope_test2015_72.C \
  inputmoveDeclarationToInnermostScope_test2015_73.C \
  inputmoveDeclarationToInnermostScope_test2015_74.C \
  inputmoveDeclarationToInnermostScope_test2015_75.C \
  inputmoveDeclarationToInnermostScope_test2015_76.C \
  inputmoveDeclarationToInnermostScope_test2015_77.C \
  inputmoveDeclarationToInnermostScope_test2015_78.C \
  inputmoveDeclarationToInnermostScope_test2015_79.C \
  inputmoveDeclarationToInnermostScope_test2015_80.C \
  inputmoveDeclarationToInnermostScope_test2015_83.C \
  inputmoveDeclarationToInnermostScope_test2015_84.C \
  inputmoveDeclarationToInnermostScope_test2015_85.C \
  inputmoveDeclarationToInnermostScope_test2015_86.C \
  inputmoveDeclarationToInnermostScope_test2015_87.C \
  inputmoveDeclarationToInnermostScope_test2015_88.C \
  inputmoveDeclarationToInnermostScope_test2015_89.C \
  inputmoveDeclarationToInnermostScope_test2015_90.C \
  inputmoveDeclarationToInnermostScope_test2015_91.C \
  inputmoveDeclarationToInnermostScope_test2015_92.C \
  inputmoveDeclarationToInnermostScope_test2015_93.C \
  inputmoveDeclarationToInnermostScope_test2015_95.C \
  inputmoveDeclarationToInnermostScope_test2015_96.C \
  inputmoveDeclarationToInnermostScope_test2015_97.C \
  inputmoveDeclarationToInnermostScope_test2015_98.C \
  inputmoveDeclarationToInnermostScope_test2015_99.C \
  inputmoveDeclarationToInnermostScope_test2015_102.C \
  inputmoveDeclarationToInnermostScope_test2015_103.C \
  inputmoveDeclarationToInnermostScope_test2015_104.C \
  inputmoveDeclarationToInnermostScope_test2015_105.C \
  inputmoveDeclarationToInnermostScope_test2015_106.C \
  inputmoveDeclarationToInnermostScope_test2015_107.C \
  inputmoveDeclarationToInnermostScope_test2015_108.C \
  inputmoveDeclarationToInnermostScope_test2015_109.C \
  inputmoveDeclarationToInnermostScope_test2015_110.C \
  inputmoveDeclarationToInnermostScope_test2015_111.C \
  inputmoveDeclarationToInnermostScope_test2015_112.C \
  inputmoveDeclarationToInnermostScope_test2015_113.C \
  inputmoveDeclarationToInnermostScope_test2015_115.C \
  inputmoveDeclarationToInnermostScope_test2015_116.C \
  inputmoveDeclarationToInnermostScope_test2015_117.C \
  inputmoveDeclarationToInnermostScope_test2015_118.C \
  inputmoveDeclarationToInnermostScope_test2015_119.C \
  inputmoveDeclarationToInnermostScope_test2015_120.C \
  inputmoveDeclarationToInnermostScope_test2015_121.C \
  inputmoveDeclarationToInnermostScope_test2015_122.C \
  inputmoveDeclarationToInnermostScope_test2015_123.C \
  inputmoveDeclarationToInnermostScope_test2015_124.C \
  inputmoveDeclarationToInnermostScope_test2015_125.C \
  inputmoveDeclarationToInnermostScope_test2015_128.C \
  inputmoveDeclarationToInnermostScope_test2015_129.C \
  inputmoveDeclarationToInnermostScope_test2015_130.C \
  inputmoveDeclarationToInnermostScope_test2015_131.C \
  inputmoveDeclarationToInnermostScope_test2015_132.C \
  inputmoveDeclarationToInnermostScope_test2015_135.C \
  inputmoveDeclarationToInnermostScope_test2015_137.C \
  inputmoveDeclarationToInnermostScope_test2015_139.C \
  inputmoveDeclarationToInnermostScope_test2015_144.C \
  inputmoveDeclarationToInnermostScope_test2015_145.C \
  inputmoveDeclarationToInnermostScope_test2015_146.C \
  inputmoveDeclarationToInnermostScope_test2015_147.C \
  inputmoveDeclarationToInnermostScope_test2015_148.C \
  inputmoveDeclarationToInnermostScope_test2015_149.C \
  inputmoveDeclarationToInnermostScope_test2015_150.C \
  inputmoveDeclarationToInnermostScope_test2015_151.C \
  inputmoveDeclarationToInnermostScope_test2015_152.C \
  inputmoveDeclarationToInnermostScope_test2015_153.C \
  inputmoveDeclarationToInnermostScope_test2015_154.C \
  inputmoveDeclarationToInnermostScope_test2015_155.C \
  inputmoveDeclarationToInnermostScope_test2015_156.C \
  inputmoveDeclarationToInnermostScope_test2015_157.C \
  inputmoveDeclarationToInnermostScope_test2015_158.C \
  inputmoveDeclarationToInnermostScope_test2015_159.C \
  inputmoveDeclarationToInnermostScope_test2015_160.C \
  inputmoveDeclarationToInnermostScope_test2015_161.C \
  inputmoveDeclarationToInnermostScope_test2015_162.C \
  inputmoveDeclarationToInnermostScope_test2015_163.C \
  inputmoveDeclarationToInnermostScope_test2015_164.C \
  inputmoveDeclarationToInnermostScope_test2015_165.C \
  inputmoveDeclarationToInnermostScope_test2015_166.C \
  inputmoveDeclarationToInnermostScope_test2015_167.C \
  inputmoveDeclarationToInnermostScope_test2015_168.C

# DQ (11/13/2015): Note that inputmoveDeclarationToInnermostScope_test2015_141.C 
# and inputmoveDeclarationToInnermostScope_test2015_133.C only fail when using 
# the -rose:merge_decl_assign option.

# DQ (1/19/2018): Failing codes:
MORE_FAILING_MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED  = \
  inputmoveDeclarationToInnermostScope_test2014_29.C \
  inputmoveDeclarationToInnermostScope_test2015_81.C \
  inputmoveDeclarationToInnermostScope_test2015_82.C \
  inputmoveDeclarationToInnermostScope_test2015_94.C \
  inputmoveDeclarationToInnermostScope_test2015_126.C \
  inputmoveDeclarationToInnermostScope_test2015_127.C \
  inputmoveDeclarationToInnermostScope_test2015_134.C \
  inputmoveDeclarationToInnermostScope_test2015_143.C 

# DQ (12/23/2014): Added tests specific to use of move tool with token-based unparsing.
FAILING_MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED  = \
  inputmoveDeclarationToInnermostScope_test2014_09.C \
  inputmoveDeclarationToInnermostScope_test2014_12.C \
  inputmoveDeclarationToInnermostScope_test2015_100.C \
  inputmoveDeclarationToInnermostScope_test2015_101.C \
  inputmoveDeclarationToInnermostScope_test2015_114.C \
  inputmoveDeclarationToInnermostScope_test2015_133.C \
  inputmoveDeclarationToInnermostScope_test2015_136.C \
  inputmoveDeclarationToInnermostScope_test2015_138.C \
  inputmoveDeclarationToInnermostScope_test2015_140.C \
  inputmoveDeclarationToInnermostScope_test2015_141.C \
  inputmoveDeclarationToInnermostScope_test2015_142.C
# I add _tokenunparsing- into the names to avoid name collision of targets used by other rules.   
MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED_RESULTS = $(addprefix rose_tokenunparsing_, ${MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED})

PASSING_MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_RESULTS = \
   $(MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED_RESULTS)

PASSING_MOVE_MERGE_DECLS_TESTCODES_RESULTS = \
   $(MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS)
#   $(MOVE_MERGE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS)

REFERENCE_PATH = $(top_srcdir)/tests/nonsmoke/functional/moveDeclarationTool/referenceResults
MOVE_DECLS_DIFF_FILES=$(PASSING_MOVE_DECLS_TESTCODES_RESULTS:.C=.C.diff)

MOVE_MERGE_DECLS_DIFF_FILES=$(PASSING_MOVE_MERGE_DECLS_TESTCODES_RESULTS:.C=.C.diff)
MOVE_MERGE_DECLS_NO_VAR_NORM_DIFF_FILES=$(MOVE_MERGE_DECLS_NO_VAR_NORM_TESTCODES_FULLY_SUPPORTED_RESULTS:.C=.C.diff)

DIFF=diff
#$(MOVE_DECLS_DIFF_FILES):$(@:.C.diff=.C)
# rose_v1_input.C
$(MOVE_DECLS_DIFF_FILES):$(PASSING_MOVE_DECLS_TESTCODES_RESULTS)
	echo "Verifying moveDeclarationToInnermostScope translation by diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/$(@:.C.diff=.C) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

$(MOVE_MERGE_DECLS_DIFF_FILES):$(PASSING_MOVE_MERGE_DECLS_TESTCODES_RESULTS)
	echo "Verifying moveDeclarationToInnermostScope translation with merge by diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/withmerge/$(@:.C.diff=.C) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

$(MOVE_MERGE_DECLS_NO_VAR_NORM_DIFF_FILES):$(MOVE_MERGE_DECLS_NO_VAR_NORM_TESTCODES_FULLY_SUPPORTED_RESULTS)
	echo "Verifying moveDeclarationToInnermostScope translation with merge and token unparsing by diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/withmerge/$(@:.C.diff=.C) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

# DQ (6/28/2015): Added additional test using new option: -rose:merge_decl_assign
$(MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_tokenunparsing_%=%) $(MOVE_TOOL) 
	echo "Test without new option: -rose:merge_decl_assign"
	$(VALGRIND) $(MOVE_TOOL) $(TEST_CXXFLAGS) -rose:unparse_tokens -c -rose:debug $(srcdir)/$(@:rose_tokenunparsing_%=%)
	echo "Test using new option: -rose:merge_decl_assign"
	$(VALGRIND) $(MOVE_TOOL) $(TEST_CXXFLAGS) -rose:unparse_tokens -rose:merge_decl_assign -c -rose:debug $(srcdir)/$(@:rose_tokenunparsing_%=%)

# Without new option fails for test2015_04.C?
# With new option fails for test2014_12.C

test_move_decls: $(PASSING_MOVE_DECLS_TESTCODES_RESULTS)

# DQ (8/16/2015): This test demonstrates a recent bug in the token based unparsing caused by a 
# fix to support more general recognition of transforamtions of types).
# test2014_12.C demonstrates the first issue.
test_move_decls_with_token_unparsing: $(PASSING_MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_RESULTS)
	@echo "********************************************************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/moveDeclarationTool: make test_move_decls_with_token_unparsing rule complete (terminated normally) ******"
	@echo "********************************************************************************************************************************************************************"

# DQ (11/4/2015): Restrict testing of the token-based unparsing to later versions of GNU compilers (at least initially).
# DQ (11/2/2015): Added token-based unparsing tests using the move-tool to the move_diff_check test rule.
# test both diff cases
# move_diff_check: move_decls_diff_check move_merge_decls_diff_check
# move_diff_check: move_decls_diff_check move_merge_decls_diff_check test_move_decls_with_token_unparsing
if ROSE_USING_GCC_VERSION_LATER_4_4
move_diff_check: move_decls_diff_check move_merge_decls_diff_check test_move_decls_with_token_unparsing
else
move_diff_check: move_decls_diff_check move_merge_decls_diff_check
endif

# only move , no merge
move_decls_diff_check: $(MOVE_DECLS_DIFF_FILES)
# both move and merge
move_merge_decls_diff_check: $(MOVE_MERGE_DECLS_DIFF_FILES)

move_merge_decls_no_var_norm_diff_check: $(MOVE_MERGE_DECLS_NO_VAR_NORM_DIFF_FILES)

# rules for declaration moving tests
$(MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_v1_%=%) $(MOVE_TOOL) 
	$(VALGRIND) $(MOVE_TOOL) $(TEST_CXXFLAGS) -c -rose:debug -rose:trans-tracking $(srcdir)/$(@:rose_v1_%=%) -rose:output $@

$(MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_v2_%=%) $(MOVE_TOOL) 
	$(VALGRIND) $(MOVE_TOOL) $(TEST_CXXFLAGS) -c -rose:debug -rose:merge_decl_assign -rose:trans-tracking $(srcdir)/$(@:rose_v2_%=%) -rose:output $@

$(MOVE_MERGE_DECLS_NO_VAR_NORM_TESTCODES_FULLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_v3_%=%) $(MOVE_TOOL) 
	$(VALGRIND) $(MOVE_TOOL) $(TEST_CXXFLAGS) -c -rose:debug -rose:merge_decl_assign -rose:suppress_variable_declaration_normalization -rose:trans-tracking $(srcdir)/$(@:rose_v3_%=%) -rose:output $@



# move_decls_diff_check move_merge_decls_diff_check
# check-local: move_diff_check
check-local:
if USING_GNU_COMPILER
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "moveDeclarationTool tests require configuration with either EDG version 4.9 and GNU version 4.8 or backend compiler or EDG 4.12 and 4.8 or any later GNU compiler."
	@echo "moveDeclarationTool tests using EDG 4.12 and any supported backend compiler are allowed."
	@$(MAKE) move_diff_check
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "moveDeclarationTool tests using EDG 4.12: Clang backend compiler using EDG 4.12 frontend."
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "moveDeclarationTool tests using EDG 4.12 Intel backend compiler are allowed."
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/moveDeclarationTool: make check rule complete (terminated normally)"
	@echo "*********************************************************************************************************"

clean-local:
	rm -f *.dot $(PASSING_MOVE_DECLS_TESTCODES_RESULTS) $(MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS)
	rm -f $(MOVE_DECLS_DIFF_FILES) $(MOVE_MERGE_DECLS_DIFF_FILES)
	rm -f $(addsuffix .passed, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -f $(addsuffix .failed, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -f $(addsuffix .diff, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -rf rose_*.C *.o

