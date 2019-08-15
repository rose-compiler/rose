

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_WITH_GNU_OCTAVE_PARSER

BASIC_TESTCODES = \
		belief.m \
		determ.m \
		if_condition.m \
		matrix_access.m \
		mcmc_test.m \
		nargin_test.m \
		numquad_test.m \
		sam.m \
		whileTest.m

CXX_BACKEND_FAILING_TESTS = \
		forward_decl.m \
		linearreg.m

MATLAB_PARSER_FAILING_TESTS =

ALL_TESTCODES = \
                $(BASIC_TESTCODES) \
		$(CXX_BACKEND_FAILING_TESTS)

MATLAB_TEST_OBJECTS = \
                   $(patsubst %.m,output/rose_%.cc,$(ALL_TESTCODES))

m2cxx:
	$(MAKE) -C ../src m2cxx

$(MATLAB_TEST_OBJECTS): output/rose_%.cc: $(srcdir)/%.m m2cxx
	../M2Cxx.sh -I $(ARMA_PREFIX)/include $<

conditional-check-local:
	@echo "MatlabTranslation tests."
	@$(MAKE) $(MATLAB_TEST_OBJECTS)
	@echo "======== make check rule for MatlabTranslation terminated normally ========"

else

conditional-check-local:
	@echo "\nROSE MatlabTranslation tests NOT run. To run these reconfigure with:"
	@echo "*********************************************************************************************"
	@echo "******************* make check not run here: configure --with-octave ************************"
	@echo "*********************************************************************************************"

endif


check-local: conditional-check-local

clean-local:
	rm -rf output *.cc *.ti

EXTRA_DIST = $(ALL_TESTCODES)
