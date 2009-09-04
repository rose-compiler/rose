include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Need to build the ompTranslator in the current directory first, or
# make the subdirectories trigger the build in this directory (better).

SUBDIRS = . tests includes

# DQ (1/6/2007): Try to turn this on so that we at least 
# compile the OpenMP translator even if we don't run it!
# Automake conditional to control use of ROSE with OpenMP

# Later this should reference the external Omni OMP runtile library include directory
# This is a parameter provided through the ROSE configure script (configure commandline)
OMNI_OPENMP_RUNTIME_LIBRARY = @omni_omp_runtime_support_path@

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/includes

bin_PROGRAMS = ompTranslator

ompTranslator_SOURCES = ompTranslator.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

if ROSE_USE_OMNI_OPENMP
check-local:
	@echo "Test for ROSE OpenMP Translator."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/projects/OpenMP_Translator: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"
else
check-local:
	@echo "\nOpenMP tests NOT run.  To run these reconfigure with:"
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "**********************************************************************************"
	@echo "****** make check not run here: configure --with-omp_runtime_support=<path> ******"
	@echo "**********************************************************************************"
endif

EXTRA_DIST = README 

clean-local:
	rm -f *.o a.out rose_*.C

