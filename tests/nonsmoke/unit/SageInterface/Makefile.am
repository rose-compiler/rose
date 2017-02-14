include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

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


###############################################################################
# A checker to test OmpSupport::getDataSharingAttribute()

noinst_PROGRAMS += getDataSharingAttribute
getDataSharingAttribute_SOURCES = getDataSharingAttribute.C
# nonsmoke_getDataSharingAttribute_c defines the set of c files to test getDataSharingAttribute

# unparsed file results, optional for this test
rose_nonsmoke_getDataSharingAttribute_c = $(addprefix rose_, ${nonsmoke_getDataSharingAttribute_c})
# screen output of the tests, redirect to .output file
nonsmoke_getDataSharingAttribute_c_output = $(addsuffix .output, ${nonsmoke_getDataSharingAttribute_c})

# small set to be verified for now
nonsmoke_getDataSharingAttribute_c_output_verify = $(addsuffix .output, ${nonsmoke_getDataSharingAttribute_c_verify})

#$(rose_nonsmoke_getDataSharingAttribute_c): $(nonsmoke_specimens_openmp_path)/$(@:rose_%=%) getDataSharingAttribute
#	$(VALGRIND) ./getDataSharingAttribute $(TEST_CXXFLAGS) -c -rose:openmp:ast_only $(nonsmoke_specimens_openmp_path)/$(@:rose_%=%) > $(@:rose_%=%).output

# similar to the previous one
$(nonsmoke_getDataSharingAttribute_c_output): getDataSharingAttribute
	$(VALGRIND) ./getDataSharingAttribute $(TEST_CXXFLAGS) -c -rose:openmp:ast_only $(nonsmoke_specimens_openmp_path)/$(@:%.output=%) > $@

# the diff files .output.diff
nonsmoke_getDataSharingAttribute_c_output_diff = $(addsuffix .diff, ${nonsmoke_getDataSharingAttribute_c_output_verify})

REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/unit/SageInterface/referenceOutput

# must dependent on individual output files for both versions : generated vs. reference 
# wrong to use $(nonsmoke_getDataSharingAttribute_c_output) since it is the whole set!
# Otherwise wrong dependence, redundant execution!! Still wrong. break dependence between .output.diff --> .output
#$(nonsmoke_getDataSharingAttribute_c_output_diff): $(@:output.diff=.output) $(REFERENCE_PATH)/$(@:.output.diff=.output)
$(nonsmoke_getDataSharingAttribute_c_output_diff): $(nonsmoke_getDataSharingAttribute_c_output) 
	echo "Verifying getDataSharingAttribute by diff ..."; \
	if diff $(@:.output.diff=.output) $(REFERENCE_PATH)/$(@:.output.diff=.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi

# shortcut to test generating outputs only, tested for avoid redundant running.
generateOutput: $(nonsmoke_getDataSharingAttribute_c_output)
# shortcut to test generating the diff files
getDataSharingAttribute_diff_check: $(nonsmoke_getDataSharingAttribute_c_output_diff)

#  generate the diff files
check-local: $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff)

clean-local:
	rm -rf $(rose_nonsmoke_getDataSharingAttribute_c) $(nonsmoke_getDataSharingAttribute_c_output) $(nonsmoke_getDataSharingAttribute_c_output_diff)

endif
