include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

SUBDIRS =

# Forward declarations so we can use make's "+=" operator.  We use "noinst_PROGRAMS" instead of "bin_PROGRAMS" because
# # we generally don't want checkers used in testing to be installed by "make install".  The "TEST_TARGETS" variable
# # will be a list of all the make targets that should be run: one for each test where a "test" is some "checker" run
# # with one input "specimen" (or no inputs).
noinst_PROGRAMS =
TEST_TARGETS =

# Must have this guard, otherwise binary analysis without C++ enabled will fail.
# TODO: move this into a subdirectory
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT

###############################################################################
# Inut specimens
nonsmoke_specimens_openmp_path=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests
include $(nonsmoke_specimens_openmp_path)/Specimens.inc

# input for get_C_array_dimensions()
nonsmoke_specimens_C_tests_path=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/C_tests
include $(nonsmoke_specimens_C_tests_path)/Specimens.inc

# input for genericCoverage()
nonsmoke_specimens_Cxx_tests_path=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests
include $(nonsmoke_specimens_Cxx_tests_path)/Specimens.inc


###############################################################################
# A checker to test OmpSupport::getDataSharingAttribute()

noinst_PROGRAMS += getDataSharingAttribute get_C_array_dimensions genericCoverage
getDataSharingAttribute_SOURCES = getDataSharingAttribute.C

get_C_array_dimensions_SOURCES = get_C_array_dimensions.C

genericCoverage_SOURCES = genericCoverage.C

# nonsmoke_getDataSharingAttribute_c defines the set of c files to test getDataSharingAttribute

# unparsed file results, optional for this test
rose_nonsmoke_getDataSharingAttribute_c = $(addprefix rose_, ${nonsmoke_getDataSharingAttribute_c})
rose_nonsmoke_get_C_array_dimensions_c = $(addprefix rose_, ${nonsmoke_get_C_array_dimensions_c})
rose_nonsmoke_genericCoverage_c = $(addprefix rose_, ${nonsmoke_genericCoverage_c})

# Output to .output file
nonsmoke_getDataSharingAttribute_c_output = $(addsuffix .output, ${nonsmoke_getDataSharingAttribute_c})
nonsmoke_get_C_array_dimensions_c_output = $(addsuffix .get_C_array_dimensions.output, ${nonsmoke_get_C_array_dimensions_c})
nonsmoke_genericCoverage_c_output = $(addsuffix .genericCoverage.output, ${nonsmoke_genericCoverage_c})

# small set to be verified for now
# It could be a subset of all the tests
nonsmoke_getDataSharingAttribute_c_output_verify = $(addsuffix .output, ${nonsmoke_getDataSharingAttribute_c_verify})
nonsmoke_get_C_array_dimensions_c_output_verify = $(addsuffix .get_C_array_dimensions.output, ${nonsmoke_get_C_array_dimensions_c_verify})
nonsmoke_genericCoverage_c_output_verify = $(addsuffix .genericCoverage.output, ${nonsmoke_genericCoverage_c_verify})

#$(rose_nonsmoke_getDataSharingAttribute_c): $(nonsmoke_specimens_openmp_path)/$(@:rose_%=%) getDataSharingAttribute
#	$(VALGRIND) ./getDataSharingAttribute $(TEST_CXXFLAGS) -c -rose:openmp:ast_only $(nonsmoke_specimens_openmp_path)/$(@:rose_%=%) > $(@:rose_%=%).output

# similar to the previous one
$(nonsmoke_getDataSharingAttribute_c_output): getDataSharingAttribute
	$(VALGRIND) ./getDataSharingAttribute $(TEST_CXXFLAGS) -c -rose:openmp:ast_only $(nonsmoke_specimens_openmp_path)/$(@:%.output=%)

$(nonsmoke_get_C_array_dimensions_c_output): get_C_array_dimensions
	$(VALGRIND) ./get_C_array_dimensions $(TEST_CXXFLAGS) -c $(nonsmoke_specimens_C_tests_path)/$(@:%.get_C_array_dimensions.output=%)

$(nonsmoke_genericCoverage_c_output): genericCoverage
	echo $(VALGRIND) ./genericCoverage $(TEST_CXXFLAGS) -c $(nonsmoke_specimens_Cxx_tests_path)/$(@:%.genericCoverage.output=%)
	$(VALGRIND) ./genericCoverage $(TEST_CXXFLAGS) -c $(nonsmoke_specimens_Cxx_tests_path)/$(@:%.genericCoverage.output=%)

# the diff files .output.diff
nonsmoke_getDataSharingAttribute_c_output_diff = $(addsuffix .diff, ${nonsmoke_getDataSharingAttribute_c_output_verify})
nonsmoke_get_C_array_dimensions_c_output_diff = $(addsuffix .diff, ${nonsmoke_get_C_array_dimensions_c_output_verify})
nonsmoke_genericCoverage_c_output_diff = $(addsuffix .diff, ${nonsmoke_genericCoverage_c_output_verify})

REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/unit/SageInterface/referenceOutput

# must dependent on individual output files for both versions : generated vs. reference
# wrong to use $(nonsmoke_getDataSharingAttribute_c_output) since it is the whole set!
# Otherwise wrong dependence, redundant execution!! Still wrong. break dependence between .output.diff --> .output
#$(nonsmoke_getDataSharingAttribute_c_output_diff): $(@:output.diff=.output) $(REFERENCE_PATH)/$(@:.output.diff=.output)
$(nonsmoke_getDataSharingAttribute_c_output_diff): $(nonsmoke_getDataSharingAttribute_c_output)
	echo "Verifying getDataSharingAttribute by diff ..."; \
	if diff $(@:.output.diff=.output) $(REFERENCE_PATH)/$(@:.output.diff=.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi

$(nonsmoke_get_C_array_dimensions_c_output_diff): $(nonsmoke_get_C_array_dimensions_c_output)
	echo "Verifying get_C_array_dimensions by diff ..."; \
	if diff $(@:.get_C_array_dimensions.output.diff=.get_C_array_dimensions.output) $(REFERENCE_PATH)/$(@:.get_C_array_dimensions.output.diff=.get_C_array_dimensions.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi

$(nonsmoke_genericCoverage_c_output_diff): $(nonsmoke_genericCoverage_c_output)
	echo "Verifying genericCoverage by diff ..."; \
	if diff $(@:.genericCoverage.output.diff=.genericCoverage.output) $(REFERENCE_PATH)/$(@:.genericCoverage.output.diff=.genericCoverage.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi

# shortcut to test generating outputs only, tested for avoid redundant running.
generateOutput: $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_genericCoverage_c_output)
# shortcut to test generating the diff files
getDataSharingAttribute_diff_check: $(nonsmoke_getDataSharingAttribute_c_output_diff)
get_C_array_dimensions_diff_check: $(nonsmoke_get_C_array_dimensions_c_output_diff)
genericCoverage_diff_check: $(nonsmoke_genericCoverage_c_output_diff)

#  generate the diff files
# if !ROSE_USING_GCC_VERSION_LATER_6_1
check-local: $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff) \
             $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_get_C_array_dimensions_c_output_diff) \
             $(nonsmoke_genericCoverage_c_output) $(nonsmoke_genericCoverage_c_output_diff)
# else
## DQ (4/5/2017): GNU 6.1 includes language features that this test does not support.
#check-local: $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff) \
#             $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_get_C_array_dimensions_c_output_diff)
#endif

clean-local:
	rm -rf $(rose_nonsmoke_getDataSharingAttribute_c) $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff)
	rm -rf $(rose_nonsmoke_get_C_array_dimensions_c) $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_get_C_array_dimensions_c_output_diff)
	rm -rf $(rose_nonsmoke_genericCoverage_c) $(nonsmoke_genericCoverage_c_output) $(nonsmoke_genericCoverage_c_output_diff)

endif
