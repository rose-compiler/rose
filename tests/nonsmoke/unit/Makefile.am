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

###############################################################################
# Inut specimens
nonsmoke_specimens_openmp_path=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests
include $(nonsmoke_specimens_openmp_path)/Specimens.inc


###############################################################################
# A checker to test OmpSupport::getDataSharingAttribute()

noinst_PROGRAMS += getDataSharingAttribute
getDataSharingAttribute_SOURCES = getDataSharingAttribute.C


# unparsed file results
nonsmoke_specimens_openmp_c_results = $(addprefix rose_, ${nonsmoke_specimens_openmp_c})
# screen output of the tests
nonsmoke_specimens_openmp_c_output = $(addsuffix .output, ${nonsmoke_specimens_openmp_c})

$(nonsmoke_specimens_openmp_c_results): $(nonsmoke_specimens_openmp_path)/$(@:rose_%=%) getDataSharingAttribute
	$(VALGRIND) ./getDataSharingAttribute $(TEST_CXXFLAGS) -c -rose:openmp:ast_only $(nonsmoke_specimens_openmp_path)/$(@:rose_%=%) > $(@:rose_%=%).output

$(nonsmoke_specimens_openmp_c_output): $(nonsmoke_specimens_openmp_c_results)

# TODO add diff results
check-local: $(nonsmoke_specimens_openmp_c_results)


clean-local:
	rm -rf $(nonsmoke_specimens_openmp_c_results)

