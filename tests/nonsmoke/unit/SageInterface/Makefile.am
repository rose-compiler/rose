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

# input for genericCoverage(), collectFunctionSideEffect.C
nonsmoke_specimens_Cxx_tests_path=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests
include $(nonsmoke_specimens_Cxx_tests_path)/Specimens.inc

###############################################################################
# A checker to test OmpSupport::getDataSharingAttribute()

noinst_PROGRAMS += getDataSharingAttribute get_C_array_dimensions genericCoverage collectFunctionSideEffect performAliasAnalysis

getDataSharingAttribute_SOURCES = getDataSharingAttribute.C

get_C_array_dimensions_SOURCES = get_C_array_dimensions.C

genericCoverage_SOURCES = genericCoverage.C

collectFunctionSideEffect_SOURCES = collectFunctionSideEffect.C

performAliasAnalysis_SOURCES = performAliasAnalysis.C


alias_analysis_tests_cpp = alias-001.cpp \
    alias-002.cpp \
    alias-003.cpp

# DataRaceBench file list, 1/29/2018
# it is defined as dataracebench_c_files
dataracebench_path=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests/dataRaceBench
include $(dataracebench_path)/Specimens.inc

# nonsmoke_getDataSharingAttribute_c defines the set of c files to test getDataSharingAttribute

# unparsed file results, used to support clean-local rule
rose_nonsmoke_getDataSharingAttribute_c = $(addprefix rose_, ${nonsmoke_getDataSharingAttribute_c})
rose_dataracebench_getDataSharingAttribute_c = $(addprefix rose_, ${dataracebench_c_files})
rose_nonsmoke_get_C_array_dimensions_c = $(addprefix rose_, ${nonsmoke_get_C_array_dimensions_c})
# tests/nonsmoke/functional/CompileTests/Cxx_tests/Specimens.inc defines nonsmoke_genericCoverage_c
rose_nonsmoke_genericCoverage_c = $(addprefix rose_, ${nonsmoke_genericCoverage_c})
rose_nonsmoke_collectFunctionSideEffect_cpp= $(addprefix rose_, ${nonsmoke_almost_all_cpp})

# Output to .output file original_file_name.output
nonsmoke_getDataSharingAttribute_c_output = $(addsuffix .getDataSharingAttribute.output, ${nonsmoke_getDataSharingAttribute_c})
dataracebench_getDataSharingAttribute_c_output = $(addsuffix .getDataSharingAttribute.output, ${dataracebench_c_files})

nonsmoke_get_C_array_dimensions_c_output = $(addsuffix .get_C_array_dimensions.output, ${nonsmoke_get_C_array_dimensions_c})
nonsmoke_genericCoverage_c_output = $(addsuffix .genericCoverage.output, ${nonsmoke_genericCoverage_c})
nonsmoke_collectFunctionSideEffect_cpp_output = $(addsuffix .collectFunctionSideEffect.output, ${nonsmoke_almost_all_cpp})
performAliasAnalysis_cpp_output = $(addsuffix .performAliasAnalysis.output, ${alias_analysis_tests_cpp})

# small set to be verified for now
# It could be a subset of all the tests
# nonsmoke_getDataSharingAttribute_c_verify is a subset defined in 
# tests/nonsmoke/functional/CompileTests/OpenMP_tests/Specimens.inc
nonsmoke_getDataSharingAttribute_c_output_verify = $(addsuffix .getDataSharingAttribute.output, ${nonsmoke_getDataSharingAttribute_c_verify})

# tests/nonsmoke/functional/CompileTests/C_tests/Specimens.inc defines nonsmoke_get_C_array_dimensions_c_verify
nonsmoke_get_C_array_dimensions_c_output_verify = $(addsuffix .get_C_array_dimensions.output, ${nonsmoke_get_C_array_dimensions_c_verify})
nonsmoke_genericCoverage_c_output_verify = $(addsuffix .genericCoverage.output, ${nonsmoke_genericCoverage_c_verify})
nonsmoke_collectFunctionSideEffect_cpp_output_verify = $(addsuffix .collectFunctionSideEffect.output, ${nonsmoke_almost_all_cpp})

# similar to the previous one:  original.c.getDataSharingAttribute.output -->  original.c
# the verify subset is included in getDataSharingAttribute_c_output
$(nonsmoke_getDataSharingAttribute_c_output): getDataSharingAttribute
	$(VALGRIND) ./getDataSharingAttribute $(TEST_CXXFLAGS) -c -rose:openmp:ast_only $(nonsmoke_specimens_openmp_path)/$(@:%.getDataSharingAttribute.output=%)

$(dataracebench_getDataSharingAttribute_c_output): getDataSharingAttribute
	$(VALGRIND) ./getDataSharingAttribute $(TEST_CXXFLAGS) -I$(dataracebench_path) -c -rose:openmp:ast_only $(dataracebench_path)/$(@:%.getDataSharingAttribute.output=%)

$(nonsmoke_get_C_array_dimensions_c_output): get_C_array_dimensions
	$(VALGRIND) ./get_C_array_dimensions $(TEST_CXXFLAGS) -c $(nonsmoke_specimens_C_tests_path)/$(@:%.get_C_array_dimensions.output=%)

$(nonsmoke_genericCoverage_c_output): genericCoverage
	echo $(VALGRIND) ./genericCoverage $(TEST_CXXFLAGS) -c $(nonsmoke_specimens_Cxx_tests_path)/$(@:%.genericCoverage.output=%)
	$(VALGRIND) ./genericCoverage $(TEST_CXXFLAGS) -c $(nonsmoke_specimens_Cxx_tests_path)/$(@:%.genericCoverage.output=%)

$(nonsmoke_collectFunctionSideEffect_cpp_output): collectFunctionSideEffect
	$(VALGRIND) ./collectFunctionSideEffect -sort $(TEST_CXXFLAGS) -annot $(top_srcdir)/tests/nonsmoke/unit/SageInterface/vectorXY.annot -c $(nonsmoke_specimens_Cxx_tests_path)/$(@:%.collectFunctionSideEffect.output=%)

$(performAliasAnalysis_cpp_output): performAliasAnalysis 
	$(VALGRIND) ./performAliasAnalysis$(TEST_CXXFLAGS) -c $(top_srcdir)/tests/nonsmoke/unit/SageInterface/tests/$(@:%.performAliasAnalysis.output=%)

# the diff files originalfile.c.output.diff
nonsmoke_getDataSharingAttribute_c_output_diff = $(addsuffix .diff, ${nonsmoke_getDataSharingAttribute_c_output_verify})
dataracebench_getDataSharingAttribute_c_output_diff = $(addsuffix .diff, ${dataracebench_getDataSharingAttribute_c_output})
nonsmoke_get_C_array_dimensions_c_output_diff = $(addsuffix .diff, ${nonsmoke_get_C_array_dimensions_c_output_verify})
nonsmoke_genericCoverage_c_output_diff = $(addsuffix .diff, ${nonsmoke_genericCoverage_c_output_verify})
nonsmoke_collectFunctionSideEffect_cpp_output_diff = $(addsuffix .diff, ${nonsmoke_collectFunctionSideEffect_cpp_output_verify})
performAliasAnalysis_cpp_output_diff = $(addsuffix .diff, ${performAliasAnalysis_cpp_output})

REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/unit/SageInterface/referenceOutput

# must dependent on individual output files for both versions : generated vs. reference
# wrong to use $(nonsmoke_getDataSharingAttribute_c_output) since it is the whole set!
# Otherwise wrong dependence, redundant execution!! Still wrong. break dependence between .output.diff --> .output
#$(nonsmoke_getDataSharingAttribute_c_output_diff): $(nonsmoke_getDataSharingAttribute_c_output)
$(nonsmoke_getDataSharingAttribute_c_output_diff):  %.diff:%
	echo "Verifying getDataSharingAttribute by diff ..."; \
	if diff $(@:.getDataSharingAttribute.output.diff=.getDataSharingAttribute.output) $(REFERENCE_PATH)/$(@:.getDataSharingAttribute.output.diff=.getDataSharingAttribute.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

$(dataracebench_getDataSharingAttribute_c_output_diff):  %.diff:%
	echo "Verifying getDataSharingAttribute using DataRaceBench by diff ..."; \
	if diff $(@:.getDataSharingAttribute.output.diff=.getDataSharingAttribute.output) $(REFERENCE_PATH)/$(@:.getDataSharingAttribute.output.diff=.getDataSharingAttribute.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi


$(nonsmoke_get_C_array_dimensions_c_output_diff):  %.diff:%
	echo "Verifying get_C_array_dimensions by diff ..."; \
	if diff $(@:.get_C_array_dimensions.output.diff=.get_C_array_dimensions.output) $(REFERENCE_PATH)/$(@:.get_C_array_dimensions.output.diff=.get_C_array_dimensions.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

$(nonsmoke_genericCoverage_c_output_diff): %.diff:%
	echo "Verifying genericCoverage by diff ..."; \
	if diff $(@:.genericCoverage.output.diff=.genericCoverage.output) $(REFERENCE_PATH)/$(@:.genericCoverage.output.diff=.genericCoverage.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

$(nonsmoke_collectFunctionSideEffect_cpp_output_diff): %.diff:%
	echo "Verifying collectFunctionSideEffect by diff ..."; \
	if diff $(@:.collectFunctionSideEffect.output.diff=.collectFunctionSideEffect.output) $(REFERENCE_PATH)/$(@:.collectFunctionSideEffect.output.diff=.collectFunctionSideEffect.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

$(performAliasAnalysis_cpp_output_diff): %.diff:%
	echo "Verifying alias analysis by diff ..."; \
	if diff $(@:.performAliasAnalysis.output.diff=.performAliasAnalysis.output) $(REFERENCE_PATH)/$(@:.performAliasAnalysis.output.diff=.performAliasAnalysis.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi


# shortcut to test generating outputs only, tested for avoid redundant running.
generateOutput: $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_genericCoverage_c_output) $(nonsmoke_collectFunctionSideEffect_cpp_output)
# shortcut to test generating the diff files
getDataSharingAttribute_diff_check: $(nonsmoke_getDataSharingAttribute_c_output_diff)
get_C_array_dimensions_diff_check: $(nonsmoke_get_C_array_dimensions_c_output_diff)
genericCoverage_diff_check: $(nonsmoke_genericCoverage_c_output_diff)
collectFunctionSideEffect_diff_check: $(nonsmoke_collectFunctionSideEffect_cpp_output_diff)
performAliasAnalysis_diff_check: $(performAliasAnalysis_cpp_output_diff)
dataracebench_diff_check: $(dataracebench_getDataSharingAttribute_c_output_diff)

dataracebench_output:$(dataracebench_getDataSharingAttribute_c_output)

#  generate the diff files
#  TODO: add dataracebench_getDataSharingAttribute_c_output_diff when ready
if !ROSE_USING_GCC_VERSION_LATER_6_1
check-local: $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff) \
             $(dataracebench_getDataSharingAttribute_c_output) $(dataracebench_getDataSharingAttribute_c_output_diff) \
             $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_get_C_array_dimensions_c_output_diff) \
             $(nonsmoke_genericCoverage_c_output) $(nonsmoke_genericCoverage_c_output_diff) \
	     $(nonsmoke_collectFunctionSideEffect_cpp_output_diff) \
	     $(performAliasAnalysis_cpp_output_diff)
else
# DQ (4/5/2017): GNU 6.1 includes language features that this test does not support.
check-local: $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff) \
             $(dataracebench_getDataSharingAttribute_c_output) $(dataracebench_getDataSharingAttribute_c_output_diff) \
             $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_get_C_array_dimensions_c_output_diff) \
	     $(nonsmoke_collectFunctionSideEffect_cpp_output_diff)
endif

clean-local:
	rm -rf $(rose_nonsmoke_getDataSharingAttribute_c) $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff)
	rm -rf $(rose_nonsmoke_get_C_array_dimensions_c) $(nonsmoke_get_C_array_dimensions_c_output) $(nonsmoke_get_C_array_dimensions_c_output_diff)
	rm -rf $(rose_nonsmoke_genericCoverage_c) $(nonsmoke_genericCoverage_c_output) $(nonsmoke_genericCoverage_c_output_diff) $(performAliasAnalysis_cpp_output_diff)
	rm -rf *.output *.diff rose_*.*
	rm -rf $(rose_dataracebench_getDataSharingAttribute_c) $(dataracebench_getDataSharingAttribute_c_output) $(dataracebench_getDataSharingAttribute_c_output_diff)

endif
