include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid naming collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  Compass2
# ------------------------------------------------------------------------------

AM_CPPFLAGS=\
	-I$(top_srcdir)/projects \
	-I$(top_srcdir)/projects/compass2 \
	$(XERCES_CPPFLAGS) \
	$(ROSE_INCLUDES)

LDADD = \
	libcompass2.la \
	$(BOOST_LDFLAGS) \
	$(XERCES_LIBS) \
	$(ROSE_LIBS)

LDFLAGS = $(XERCES_LDFLAGS)

# ------------------------------------------------------------------------------
#  libcompass2.la
# ------------------------------------------------------------------------------

compass2_API_VERSION=0.0.0
exec_prefix=${prefix}/compass2-$(compass2_API_VERSION)
compass2_datadir=${exec_prefix}/share
libcompass2_la_includedir = ${exec_prefix}/include/compass2

lib_LTLIBRARIES = libcompass2.la

#libcompass2_YFLAGS = -p ltl_ # yacc flags for LTL parser
libcompass2_CXXFLAGS = $(KNOBS) -fopenmp -Wall # -O3 -march=native -ftree-vectorize

libcompass2_la_SOURCES=\
	compass.cpp

libcompass2_la_include_HEADERS=\
	compass.h


# ------------------------------------------------------------------------------
#  Compass2 main executable
# ------------------------------------------------------------------------------

bin_PROGRAMS = bin/compass2

bin_compass2_SOURCES=\
    bin/compass_main.cpp

bin_compass2_LDADD=\
	libcompass2.la \
	bin/checkers/dead_function/libdeadfunction.la \
	bin/checkers/floating_point_exact_comparison/libfloatingpointexactcomparison.la \
	bin/checkers/function_pointer/libfunctionpointer.la \
	bin/checkers/function_with_multiple_returns/libfunctionwithmultiplereturns.la \
	bin/checkers/keyword_macro/libkeywordmacro.la \
	bin/checkers/keyword_macro/libkeywordmacro.la \
	bin/checkers/non_global_cpp_directive/libnonglobalcppdirective.la \
	bin/checkers/non_static_array_size/libnonstaticarraysize.la \
	bin/checkers/variable_name_similarity/libvariablenamesimilarity.la \
	bin/checkers/no_goto/libnogoto.la \
	bin/checkers/no_rand/libnorand.la \
	bin/checkers/boolean_is_has/libbooleanishas.la \
	bin/checkers/global_variables/libglobalvariables.la \
	bin/checkers/magic_number/libmagicnumber.la \
	bin/checkers/allocate_and_free_in_the_same_module/liballocateandfreeinthesamemodule.la \
	bin/checkers/forbidden_functions/libforbiddenfunctions.la \
	bin/checkers/comma_operator/libcommaoperator.la \
	bin/checkers/unary_minus/libunaryminus.la \
	bin/checkers/data_member_access/libdatamemberaccess.la \
	bin/checkers/float_for_loop_counter/libfloatforloopcounter.la \
	bin/checkers/ternary_operator/libternaryoperator.la \
	bin/checkers/explicit_test_for_non_boolean_value/libexplicittestfornonbooleanvalue.la \
	bin/checkers/byte_by_byte_structure_comparison/libbytebybytestructurecomparison.la \
	bin/checkers/size_of_pointer/libsizeofpointer.la \
	bin/checkers/default_argument/libdefaultargument.la \
	bin/checkers/function_prototype/libfunctionprototype.la \
	bin/checkers/do_not_delete_this/libdonotdeletethis.la \
	bin/checkers/dangerous_overload/libdangerousoverload.la \
	bin/checkers/discard_assignment/libdiscardassignment.la \
	bin/checkers/pointer_comparison/libpointercomparison.la \
	bin/checkers/no_vfork/libnovfork.la \
	bin/checkers/no_variadic_functions/libnovariadicfunctions.la \
	bin/checkers/asynchronous_signal_handler/libasynchronoussignalhandler.la \
	$(BOOST_LDFLAGS) \
	$(XERCES_LIBS) \
	$(ROSE_LIBS)


# ------------------------------------------------------------------------------
#  Checkers
# ------------------------------------------------------------------------------

noinst_LTLIBRARIES=\
	bin/checkers/pointer_comparison/libpointercomparison.la \
	bin/checkers/discard_assignment/libdiscardassignment.la \
	bin/checkers/dangerous_overload/libdangerousoverload.la \
	bin/checkers/do_not_delete_this/libdonotdeletethis.la \
	bin/checkers/function_prototype/libfunctionprototype.la \
	bin/checkers/default_argument/libdefaultargument.la \
	bin/checkers/asynchronous_signal_handler/libasynchronoussignalhandler.la \
	bin/checkers/no_variadic_functions/libnovariadicfunctions.la \
	bin/checkers/no_vfork/libnovfork.la \
	bin/checkers/size_of_pointer/libsizeofpointer.la \
	bin/checkers/byte_by_byte_structure_comparison/libbytebybytestructurecomparison.la \
	bin/checkers/explicit_test_for_non_boolean_value/libexplicittestfornonbooleanvalue.la \
	bin/checkers/ternary_operator/libternaryoperator.la \
	bin/checkers/float_for_loop_counter/libfloatforloopcounter.la \
	bin/checkers/data_member_access/libdatamemberaccess.la \
	bin/checkers/unary_minus/libunaryminus.la \
	bin/checkers/comma_operator/libcommaoperator.la \
	bin/checkers/forbidden_functions/libforbiddenfunctions.la \
	bin/checkers/allocate_and_free_in_the_same_module/liballocateandfreeinthesamemodule.la \
	bin/checkers/magic_number/libmagicnumber.la \
	bin/checkers/global_variables/libglobalvariables.la \
	bin/checkers/boolean_is_has/libbooleanishas.la \
	bin/checkers/no_rand/libnorand.la \
	bin/checkers/no_goto/libnogoto.la \
	bin/checkers/dead_function/libdeadfunction.la \
	bin/checkers/floating_point_exact_comparison/libfloatingpointexactcomparison.la \
	bin/checkers/function_pointer/libfunctionpointer.la \
	bin/checkers/function_with_multiple_returns/libfunctionwithmultiplereturns.la \
	bin/checkers/keyword_macro/libkeywordmacro.la \
	bin/checkers/non_global_cpp_directive/libnonglobalcppdirective.la \
	bin/checkers/non_static_array_size/libnonstaticarraysize.la \
	bin/checkers/variable_name_similarity/libvariablenamesimilarity.la

bin_checkers_dead_function_libdeadfunction_la_SOURCES=\
	bin/checkers/dead_function/dead_function.cpp

bin_checkers_floating_point_exact_comparison_libfloatingpointexactcomparison_la_SOURCES=\
  bin/checkers/floating_point_exact_comparison/floating_point_exact_comparison.cpp

bin_checkers_function_pointer_libfunctionpointer_la_SOURCES=\
	bin/checkers/function_pointer/function_pointer.cpp

bin_checkers_function_with_multiple_returns_libfunctionwithmultiplereturns_la_SOURCES=\
	bin/checkers/function_with_multiple_returns/function_with_multiple_returns.cpp

bin_checkers_keyword_macro_libkeywordmacro_la_SOURCES=\
	bin/checkers/keyword_macro/keyword_macro.cpp

bin_checkers_non_global_cpp_directive_libnonglobalcppdirective_la_SOURCES=\
	bin/checkers/non_global_cpp_directive/non_global_cpp_directive.cpp

bin_checkers_non_static_array_size_libnonstaticarraysize_la_SOURCES=\
	bin/checkers/non_static_array_size/non_static_array_size.cpp

bin_checkers_variable_name_similarity_libvariablenamesimilarity_la_SOURCES=\
	bin/checkers/variable_name_similarity/variable_name_similarity.cpp

bin_checkers_no_goto_libnogoto_la_SOURCES=\
	bin/checkers/no_goto/no_goto.cpp

bin_checkers_no_rand_libnorand_la_SOURCES=\
	bin/checkers/no_rand/no_rand.cpp

bin_checkers_boolean_is_has_libbooleanishas_la_SOURCES=\
	bin/checkers/boolean_is_has/boolean_is_has.cpp

bin_checkers_global_variables_libglobalvariables_la_SOURCES=\
	bin/checkers/global_variables/global_variables.cpp

bin_checkers_magic_number_libmagicnumber_la_SOURCES=\
	bin/checkers/magic_number/magic_number.cpp

bin_checkers_allocate_and_free_in_the_same_module_liballocateandfreeinthesamemodule_la_SOURCES=\
	bin/checkers/allocate_and_free_in_the_same_module/allocate_and_free_in_the_same_module.cpp

bin_checkers_forbidden_functions_libforbiddenfunctions_la_SOURCES=\
	bin/checkers/forbidden_functions/forbidden_functions.cpp

bin_checkers_comma_operator_libcommaoperator_la_SOURCES=\
	bin/checkers/comma_operator/comma_operator.cpp

bin_checkers_unary_minus_libunaryminus_la_SOURCES=\
	bin/checkers/unary_minus/unary_minus.cpp

bin_checkers_data_member_access_libdatamemberaccess_la_SOURCES=\
	bin/checkers/data_member_access/data_member_access.cpp

bin_checkers_float_for_loop_counter_libfloatforloopcounter_la_SOURCES=\
	bin/checkers/float_for_loop_counter/float_for_loop_counter.cpp

bin_checkers_ternary_operator_libternaryoperator_la_SOURCES=\
	bin/checkers/ternary_operator/ternary_operator.cpp

bin_checkers_explicit_test_for_non_boolean_value_libexplicittestfornonbooleanvalue_la_SOURCES=\
	bin/checkers/explicit_test_for_non_boolean_value/explicit_test_for_non_boolean_value.cpp

bin_checkers_byte_by_byte_structure_comparison_libbytebybytestructurecomparison_la_SOURCES=\
	bin/checkers/byte_by_byte_structure_comparison/byte_by_byte_structure_comparison.cpp

bin_checkers_size_of_pointer_libsizeofpointer_la_SOURCES=\
	bin/checkers/size_of_pointer/size_of_pointer.cpp

bin_checkers_default_argument_libdefaultargument_la_SOURCES=\
	bin/checkers/default_argument/default_argument.cpp

bin_checkers_function_prototype_libfunctionprototype_la_SOURCES=\
	bin/checkers/function_prototype/function_prototype.cpp

bin_checkers_do_not_delete_this_libdonotdeletethis_la_SOURCES=\
	bin/checkers/do_not_delete_this/do_not_delete_this.cpp

bin_checkers_dangerous_overload_libdangerousoverload_la_SOURCES=\
	bin/checkers/dangerous_overload/dangerous_overload.cpp

bin_checkers_discard_assignment_libdiscardassignment_la_SOURCES=\
	bin/checkers/discard_assignment/discard_assignment.cpp

bin_checkers_pointer_comparison_libpointercomparison_la_SOURCES=\
	bin/checkers/pointer_comparison/pointer_comparison.cpp

bin_checkers_no_vfork_libnovfork_la_SOURCES=\
	bin/checkers/no_vfork/no_vfork.cpp

bin_checkers_no_variadic_functions_libnovariadicfunctions_la_SOURCES=\
	bin/checkers/no_variadic_functions/no_variadic_functions.cpp

bin_checkers_asynchronous_signal_handler_libasynchronoussignalhandler_la_SOURCES=\
	bin/checkers/asynchronous_signal_handler/asynchronous_signal_handler.cpp

#PLACEHOLDER

# ------------------------------------------------------------------------------
#  XML Schemas
# ------------------------------------------------------------------------------

COMPASS_PARAMETERS_GRAMMAR_FILE=\
	share/xml/compass_parameters.xsd

COMPASS_PARAMETERS_DEFAULT_FILE=\
	share/xml/compass_parameters.xml

## The nobase_ prefix instructs Automake to not strip the directory part from
## each filename. For example,
##
##     Install to ${PREFIX}/share/compass2/xml/compass_parameters.xsd
compass2_xmldir = $(compass2_datadir)/xml
compass2_xml_DATA = \
	$(COMPASS_PARAMETERS_GRAMMAR_FILE) \
	$(COMPASS_PARAMETERS_DEFAULT_FILE)

CPPFLAGS += \
	-DPREFIX_COMPASS2_XML_DIR="\"$(compass2_xmldir)\"" \
	-DSOURCE_COMPASS2_XML_DIR="\"@ABS_COMPASS2_XML_SRCDIR@\""

# ------------------------------------------------------------------------------
#  Tests
# ------------------------------------------------------------------------------

SUBDIRS=\
	docs \
	tests

noinst_PROGRAMS=\
	tests/core/test_parameters_parser \
	tests/core/test_parser

tests_core_test_parameters_parser_SOURCES=\
	tests/core/test_parameters_parser.cpp

tests_core_test_parser_SOURCES=\
	tests/core/test_parser.cpp

# ------------------------------------------------------------------------------
#  Documentation
# ------------------------------------------------------------------------------

.PHONY:docs
docs:
	$(MAKE) docs -C docs/

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

clean-local:
	rm -f \
		*.xml
	rm -f \
		compass2.lo \
		compass.lo  \
	rm -f \
		bin/compass_main.o
