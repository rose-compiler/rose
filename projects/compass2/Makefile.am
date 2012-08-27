include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid naming collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  Compass2
# ------------------------------------------------------------------------------

INCLUDES=\
	-I$(top_srcdir)/projects \
	-I$(top_srcdir)/projects/compass2 \
	$(XERCES_CPPFLAGS) \
	$(ROSE_INCLUDES)

LDADD = \
	libcompass2.la \
	$(XERCES_LIBS) \
	$(ROSE_LIBS)

LDFLAGS = $(XERCES_LDFLAGS)

# ------------------------------------------------------------------------------
#  libcompass2.la
# ------------------------------------------------------------------------------

compass2_API_VERSION=0.0.0
exec_prefix=${prefix}/compass2-$(compass2_API_VERSION)
libcompass2_la_includedir = ${exec_prefix}/include/compass2

lib_LTLIBRARIES = libcompass2.la

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
	bin/checkers/default_argument/libdefaultargument.la \
	bin/checkers/function_pointer/libfunctionpointer.la \
	bin/checkers/function_prototype/libfunctionprototype.la \
	bin/checkers/function_with_multiple_returns/libfunctionwithmultiplereturns.la \
	bin/checkers/global_variables/libglobalvariables.la \
	bin/checkers/keyword_macro/libkeywordmacro.la \
	bin/checkers/keyword_macro/libkeywordmacro.la \
	bin/checkers/non_global_cpp_directive/libnonglobalcppdirective.la \
	bin/checkers/non_static_array_size/libnonstaticarraysize.la \
	bin/checkers/variable_name_similarity/libvariablenamesimilarity.la \
	$(XERCES_LIBS) \
	$(ROSE_LIBS)

# ------------------------------------------------------------------------------
#  Checkers
# ------------------------------------------------------------------------------

noinst_LTLIBRARIES=\
	bin/checkers/dead_function/libdeadfunction.la \
	bin/checkers/default_argument/libdefaultargument.la \
	bin/checkers/function_pointer/libfunctionpointer.la \
	bin/checkers/function_prototype/libfunctionprototype.la \
	bin/checkers/function_with_multiple_returns/libfunctionwithmultiplereturns.la \
	bin/checkers/global_variables/libglobalvariables.la \
	bin/checkers/keyword_macro/libkeywordmacro.la \
	bin/checkers/non_global_cpp_directive/libnonglobalcppdirective.la \
	bin/checkers/non_static_array_size/libnonstaticarraysize.la \
	bin/checkers/variable_name_similarity/libvariablenamesimilarity.la

bin_checkers_dead_function_libdeadfunction_la_SOURCES=\
	bin/checkers/dead_function/dead_function.cpp

bin_checkers_default_argument_libdefaultargument_la_SOURCES=\
	bin/checkers/default_argument/default_argument.cpp

bin_checkers_function_pointer_libfunctionpointer_la_SOURCES=\
	bin/checkers/function_pointer/function_pointer.cpp

bin_checkers_function_prototype_libfunctionprototype_la_SOURCES=\
	bin/checkers/function_prototype/function_prototype.cpp

bin_checkers_function_with_multiple_returns_libfunctionwithmultiplereturns_la_SOURCES=\
	bin/checkers/function_with_multiple_returns/function_with_multiple_returns.cpp

bin_checkers_keyword_macro_libkeywordmacro_la_SOURCES=\
	bin/checkers/keyword_macro/keyword_macro.cpp

bin_checkers_global_variables_libglobalvariables_la_SOURCES=\
	bin/checkers/global_variables/global_variables.cpp

bin_checkers_non_global_cpp_directive_libnonglobalcppdirective_la_SOURCES=\
	bin/checkers/non_global_cpp_directive/non_global_cpp_directive.cpp

bin_checkers_non_static_array_size_libnonstaticarraysize_la_SOURCES=\
	bin/checkers/non_static_array_size/non_static_array_size.cpp

bin_checkers_variable_name_similarity_libvariablenamesimilarity_la_SOURCES=\
	bin/checkers/variable_name_similarity/variable_name_similarity.cpp

# ------------------------------------------------------------------------------
#  XML Schemas
# ------------------------------------------------------------------------------

COMPASS_PARAMETERS_GRAMMAR=\
	share/xml/compass_parameters.xsd

## The nobase_ prefix instructs Automake to not strip the directory part from
## each filename. For example,
##
##     Install to ${PREFIX}/share/compass2/xml/compass_parameters.xsd
compass2_xmldir = $(datadir)
nobase_compass2_xml_DATA = $(COMPASS_PARAMETERS_GRAMMAR)

CPPFLAGS=\
	-DPREFIX_COMPASS2_XML_DIR="\"$(compass2_xmldir)/compass2/xml\"" \
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

