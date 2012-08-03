include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid naming collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  Compass2 main library
# ------------------------------------------------------------------------------

INCLUDES=\
	-I$(top_srcdir)/projects/compass2 \
	$(XERCES_CPPFLAGS) \
	$(ROSE_INCLUDES)

LDADD = \
	compass2/libcompass2.la \
	$(XERCES_LIBS) \
	$(ROSE_LIBS)

LDFLAGS = $(XERCES_LDFLAGS)

## TODO: add compass-$(ROSE_API_VERSION)

## The nobase_ prefix instructs Automake to not strip the directory part from
## each filename. For example,
##
##     Install to ${PREFIX}/lib/compass2/libcompass.la
compass_libdir = $(libdir)
nobase_lib_LTLIBRARIES = compass2/libcompass2.la

compass2_libcompass2_la_SOURCES=\
	compass2/compass.cpp

## The nobase_ prefix instructs Automake to not strip the directory part from
## each filename. For example,
##
##     Install to ${PREFIX}/include/compass2/compass.h
compass_includedir = $(includedir)
nobase_compass_include_HEADERS=\
	compass2/compass.h

# ------------------------------------------------------------------------------
#  Compass2 main executable
# ------------------------------------------------------------------------------

nobase_bin_PROGRAMS = compass2/compass2

compass2_compass2_SOURCES=\
	compass2/compass_main.cpp

compass2_compass2_LDADD=\
	compass2/libcompass2.la \
	compass2/checkers/function_pointer/libfunctionpointer.la \
	compass2/checkers/keyword_macro/libkeywordmacro.la \
	compass2/checkers/global_variables/libglobalvariables.la \
	compass2/checkers/non_global_cpp_directive/libnonglobalcppdirective.la \
	compass2/checkers/non_static_array_size/libnonstaticarraysize.la \
	$(XERCES_LIBS) \
	$(ROSE_LIBS)

# ------------------------------------------------------------------------------
#  Checkers
# ------------------------------------------------------------------------------

noinst_LTLIBRARIES=\
	compass2/checkers/function_pointer/libfunctionpointer.la \
	compass2/checkers/global_variables/libglobalvariables.la \
	compass2/checkers/keyword_macro/libkeywordmacro.la \
	compass2/checkers/non_global_cpp_directive/libnonglobalcppdirective.la \
	compass2/checkers/non_static_array_size/libnonstaticarraysize.la

compass2_checkers_function_pointer_libfunctionpointer_la_SOURCES=\
	compass2/checkers/function_pointer/function_pointer.cpp

compass2_checkers_keyword_macro_libkeywordmacro_la_SOURCES=\
	compass2/checkers/keyword_macro/keyword_macro.cpp

compass2_checkers_global_variables_libglobalvariables_la_SOURCES=\
	compass2/checkers/global_variables/global_variables.cpp

compass2_checkers_non_global_cpp_directive_libnonglobalcppdirective_la_SOURCES=\
	compass2/checkers/non_global_cpp_directive/non_global_cpp_directive.cpp

compass2_checkers_non_static_array_size_libnonstaticarraysize_la_SOURCES=\
	compass2/checkers/non_static_array_size/non_static_array_size.cpp

# ------------------------------------------------------------------------------
#  XML Schemas
# ------------------------------------------------------------------------------

COMPASS_PARAMETERS_GRAMMAR=\
	compass2/xml/compass_parameters.xsd

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
	compass2/tests/test_parameters_parser.cpp

tests_core_test_parser_SOURCES=\
	compass2/tests/test_parser.cpp

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
		$(nobase_lib_LTLIBRARIES)
	rm -f \
		$(nobase_bin_PROGRAMS)
	rm -f \
		compass2/src/compass2.lo \
		compass2/src/compass.lo  \
	rm -f \
		compass2/tools/compass_main.o

