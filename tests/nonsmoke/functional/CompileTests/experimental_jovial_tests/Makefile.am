include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

AM_CPPFLAGS = $(ROSE_INCLUDES) 
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

JOVIAL_TESTCODES = \
 tiny.jov                               \
 bit_conversion.jov                     \
 int_conversion.jov                     \
 item_desc_conversion.jov               \
 parens.jov                             \
 typename_conv.jov                      \
 1_2_2_non-nested-subroutine_a.jov      \
 1_2_2_non-nested-subroutine_b.jov      \
 1_2_2_procedure-module_a.jov           \
 1_2_2_procedure-module_b.jov           \
 1_2_2_procedure-module_e.jov           \
 1_2_2_procedure-module_f.jov           \
 1_2_3_main-program-module_a.jov        \
 1_2_3_main-program-module_b.jov        \
 1_2_3_main-program-module_c.jov        \
 1_2_3_main-program-module_d.jov        \
 2_1_1_4_bit_type.jov                   \
 2_1_1_7_pointer_type.jov               \
 2_1_2_simple_table_var.jov             \
 2_1_2_compound_table_var.jov           \
 2_1_2_1_table_dimension_list_a.jov     \
 2_1_2_1_table_dimension_list_b.jov     \
 2_1_2_2_structure_spec.jov             \
 2_1_2_3_packing_spec.jov               \
 2_1_3_constant_table_var.jov           \
 2_1_6_block_preset_list.jov            \
 2_4_define_j.jov                       \
 3_2_function_result.jov                \
 4_2_loop_stmt_b.jov                    \
 4_2_loop_stmt_b1.jov                   \
 4_2_loop_stmt_d.jov                    \
 4_2_loop_stmt_d1.jov                   \
 4_2_loop_stmt_while_a.jov              \
 4_2_loop_stmt_while_b.jov              \
 4_3_if_stmt_a.jov                      \
 4_3_if_stmt_b.jov                      \
 4_3_if_stmt_c.jov                      \
 4_3_if_stmt_d.jov                      \
 4_3_if_stmt_e.jov                      \
 4_4_case_stmt_a.jov                    \
 4_5_proc_call_stmt.jov                 \
 4_8_exit_stmt_a.jov                    \
 4_9_stop_stmt_a.jov                    \
 4_9_stop_stmt_b.jov                    \
 4_9_stop_stmt_c.jov                    \
 4_10_abort_stmt_a.jov                  \
 4_10_abort_stmt_b.jov                  \
 5_2_logical_continuation_a.jov         \
 5_2_logical_continuation_b.jov         \
 6_1_ptr_dereference.jov                \
 6_3_user_def_function_call.jov         \
 6_3_5_shift_function.jov               \
 6_3_6_abs_function.jov                 \
 6_3_10_nwdsen_function.jov             \
 9_0_directives.jov                     \
 rose-issue-1507.jov                    \
 rose-issue-1516.jov                    \
 rose-issue-2100.jov                    \
 rose-issue-2143_j.jov                  \
 rose-issue-2775.jov                    \
 rose-issue-2776.jov                    \
 rose-issue-rc-2.jov                    \
 rose-issue-rc-19.jov                   \
 rose-issue-rc-41a.jov                  \
 rose-issue-rc-41b.jov                  \
 rose-issue-rc-42a.jov                  \
 rose-issue-rc-42b.jov                  \
 rose-issue-rc-42c.jov                  \
 rose-issue-rc-69.jov                   \
 rose-issue-rc-84.jov                   \
 rose-issue-rc-85.jov                   \
 rose-issue-rc-86.jov                   \
 rose-issue-rc-88.jov                   \
 rose-issue-rc-91.jov                   \
 rose-issue-rc-94.jov                   \
 rose-issue-rc-96.jov

# status_enumval.jov                     \
# 1_2_3_program-body_a.jov               \
# 1_2_3_program-body_b.jov               \
# 1_2_3_program-body_c.jov

JOVIAL_PARSER_TESTCODES = \
 fixed_cast.jov                         \
 tables_and_pointers.jov                \
 2_1_1_5_char_item_desc.jov             \
 4_3_if_else_stmt.jov                   \
 4_6_return_stmt_a.jov                  \
 4_7_goto_stmt_a.jov                    \
 5_1_1_integer-precedence.jov           \
 5_1_1_integer-primary.jov              \
 5_1_1_integer-term.jov                 \
 5_2_1_relational_expressions_a.jov     \
 6_3_2_next_function_pointer.jov        \
 6_3_2_next_function_status.jov

COMPOOL_TESTCODES = \
 define_dollar.cpl                      \
 simple_compool.cpl                     \
 status_type.cpl                        \
 2_4_define.cpl                         \
 3_0_rent_and_rec.cpl                   \
 rose-issue-2143.cpl                    \
 rose-issue-2267.cpl                    \
 rose-issue-2654.cpl                    \
 rose-issue-2658.cpl                    \
 rose-issue-2683.cpl                    \
 rose-issue-2687.cpl                    \
 rose-issue-2690.cpl                    \
 rose-issue-2691.cpl                    \
 rose-issue-2698.cpl                    \
 rose-issue-2701.cpl                    \
 rose-issue-2702.cpl                    \
 rose-issue-2766.cpl                    \
 rose-issue-2777.cpl                    \
 rose-issue-rc-6.cpl                    \
 rose-issue-rc-7.cpl                    \
 rose-issue-rc-9.cpl                    \
 rose-issue-rc-11.cpl                   \
 rose-issue-rc-12.cpl                   \
 rose-issue-rc-14.cpl                   \
 rose-issue-rc-15.cpl                   \
 rose-issue-rc-16.cpl                   \
 rose-issue-rc-49.cpl                   \
 rose-issue-rc-50.cpl                   \
 rose-issue-rc-51.cpl                   \
 rose-issue-rc-52.cpl                   \
 rose-issue-rc-53.cpl                   \
 rose-issue-rc-55.cpl                   \
 rose-issue-rc-55a.cpl                  \
 rose-issue-rc-56.cpl                   \
 rose-issue-rc-79a.cpl                  \
 rose-issue-rc-87.cpl                   \
 rose-issue-rc-92a.cpl                  \
 rose-issue-rc-93.cpl                   \
 rose-issue-rc-95.cpl                   \
 rose-issue-rc-118.cpl                  \
 rose-issue-rc-119a.cpl                 \
 rose-issue-rc-120.cpl

COMPOOL_DIRECTIVE_TESTCODES = \
 9_1_compool_directive.cpl              \
 rose-issue-rc-79b.cpl                  \
 rose-issue-rc-92b.cpl                  \
 rose-issue-rc-119b.cpl

# proc_refs.cpl                          


COMPOOL_PARSER_TESTCODES = \
 constant_item_presets.cpl              \
 item_and_status_type_J.cpl             \
 simple_def_ambiguity_J.cpl             \
 size_function_J.cpl                    \
 spec_table_decl_ambiguity_J.cpl        \
 spec_table_options_null_J.cpl          \
 table_and_status_type_J.cpl            \
 table_preset_J.cpl                     \
 table_ref_formula_ambiguity_J.jov      \
 2_2_compound_block_type.cpl            \
 2_2_compound_table_type.cpl            \
 2_2_simple_block_type.cpl              \
 2_2_simple_table_type.cpl              \
 2_5_def_ref_item.cpl                   \
 5_1_fixed_or_float.cpl                 \
 rose-issue-1518.cpl                    \
 rose-issue-1520.cpl                    \
 rose-issue-2286.cpl

# should finish formulas next
#
JOVIAL_TESTCODES_PASS_TRAVERSE = \
 4_0_compound_stmt_a.jov                \
 4_0_compound_stmt_b.jov                \
 4_0_null_block_stmt.jov                \
 4_3_if_stmt_d.jov                      \
 4_3_if_stmt_e.jov                      \
 4_7_goto_stmt_b.jov                    \
 5_3_character_formula_a.jov            \
 5_3_character_formula_b.jov            \
 5_3_character_formula_c.jov            \
 5_3_character_formula_d.jov            \
 5_4_status_formula_a.jov               \
 5_4_status_formula_b.jov               \
 5_4_status_formula_c.jov               \
 5_4_status_formula_d.jov               \
 5_5_pointer_formula_a.jov              \
 5_5_pointer_formula_b.jov              \
 5_5_pointer_formula_c.jov              \
 5_5_pointer_formula_d.jov              \
 table_type.cpl                         \
 rose-issue-2144.cpl

JOVIAL_TESTCODES_WORKING_ON = \
 rose-issue-2101.jov                    \
 rose-issue-2102.jov                    \
 rose-issue-2104.jov                    \
 rose-issue-2105.jov                    \
 rose-issue-2106.jov                    \
 rose-issue-2107.jov                    \
 rose-issue-2145.jov                    \
 rose-issue-2146.jov                    \
 rose-issue-2147.jov                    \
 rose-issue-2148.jov                    \
 rose-issue-2149.jov                    \
 rose-issue-2150.jov                    \
 1_2_2_procedure_module_c.jov           \
 1_2_2_procedure_module_d.jov           \
 1_2_2_procedure_module_g.jov           \
 1_2_3_program-body_d.jov               \
 1_2_3_program-body_e.jov               \
 1_2_3_program-body_f.jov               \
 1_2_3_program-body_g.jov               \
 1_2_3_program-body_h.jov               \
 4_1_assignment_stmt_a.jov              \
 4_1_assignment_stmt_b.jov              \
 4_2_loop_stmt_c.jov                    \
 4_6_return_stmt_b.jov                  \
 4_10_abort_stmt_c.jov                  \
 5_1_1_integer-factor.jov               \
 5_1_1_integer-formula.jov

## TODO: Don't seem to be able to run just parser tests
#  - but don't really need to anymore as we've moved beyond just parsing
#
TEST_JOVIAL_PARSER_Objects  = ${JOVIAL_PARSER_TESTCODES:.jov=.aterm}
TEST_JOVIAL_PARSER_Sources  = ${JOVIAL_PARSER_TESTCODES:.jov=.jov.passed}

TEST_COMPOOL_PARSER_Objects = ${COMPOOL_PARSER_TESTCODES:.cpl=.aterm}
TEST_COMPOOL_PARSER_Sources = ${COMPOOL_PARSER_TESTCODES:.cpl=.cpl.passed}

TEST_JOVIAL_Objects  = ${JOVIAL_TESTCODES:.jov=.aterm}
TEST_JOVIAL_Sources  = ${JOVIAL_TESTCODES:.jov=.jov.passed}

TEST_COMPOOL_Objects = ${COMPOOL_TESTCODES:.cpl=.aterm}
TEST_COMPOOL_Sources = ${COMPOOL_TESTCODES:.cpl=.cpl.passed}

TEST_DIRECTIVE_Objects = ${COMPOOL_DIRECTIVE_TESTCODES:.cpl=.aterm}
TEST_COMPOOL_Sources = ${COMPOOL_DIRECTIVE_TESTCODES:.cpl=.cpl.passed}

PASSING_TEST_Objects = $(TEST_COMPOOL_Objects) $(TEST_JOVIAL_Objects) $(TEST_DIRECTIVE_Objects)
PASSING_COMPOOL_TEST_Objects = $(TEST_COMPOOL_Objects)

# Go back and build the translators we are using to test Jovial (in case make check
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).
#../../testParser:
#	cd ../..; $(MAKE) testParser
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

$(TEST_JOVIAL_PARSER_Objects): ../../testParser
	../../testParser $(ROSE_FLAGS) -rose:jovial $(srcdir)/$(@:.aterm=.jov)

$(TEST_COMPOOL_PARSER_Objects): ../../testParser
	../../testParser $(ROSE_FLAGS) -rose:jovial $(srcdir)/$(@:.aterm=.cpl)

$(TEST_JOVIAL_Objects): ../../testTranslator
	../../testTranslator -rose:jovial $(srcdir)/$(@:.aterm=.jov)

$(TEST_COMPOOL_Objects): ../../testTranslator
	../../testTranslator -rose:jovial $(srcdir)/$(@:.aterm=.cpl)

$(TEST_DIRECTIVE_Objects): ../../testTranslator
	../../testTranslator -rose:jovial $(srcdir)/$(@:.aterm=.cpl)

check-compool:
	@echo $(TEST_COMPOOL_Objects)
	@echo $(PASSING_TEST_COMPOOL_Objects)
check-jovial:
	@echo "TEST_JOVIAL_Objects:"
	@echo $(TEST_JOVIAL_Objects)
	@echo "PASSING_TEST_Objects:"
	@echo $(PASSING_TEST_Objects)

clean-local:
	rm -f rose_*.* *.aterm *.rcmp *.dot

check-local:
	@echo "Tests for experimental Jovial frontend."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_jovial_tests: make check terminated successfully ******"
	@echo "***********************************************************************************************************************"
