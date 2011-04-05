include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
POP_DISTRIBUTION  =  poptest.tgz
TEST_TRANSLATOR   =  $(top_builddir)/tests/testTranslator

# TOO (4/4/2011): top_builddir is calculated relative to LANL_POP/, but
# the test.sh script is inside the untarred poptest/ directory. Therefore,
# the correct path to the testTranslator would be: "../$(TEST_TRANSLATOR)"
check-local:
	tar xzf $(srcdir)/$(POP_DISTRIBUTION) -C .
	cd poptest; \
	export TRANSLATOR=../$(TEST_TRANSLATOR); \
          export HUDSON_NUM_JOBS=$${NUM_PROCESS}; \
	  export HUDSON_FC="$(GFORTRAN_PATH)"; \
	  export HUDSON_FFLAGS="-ffree-line-length-0"; \
	  export HUDSON_LD="$(GFORTRAN_PATH)"; \
	  export HUDSON_CC="$(CC)"; \
	  export HUDSON_CXX="$(CXX)"; \
	  export HUDSON_Cp="/bin/cp"; \
	  export HUDSON_Cpp="/lib/cpp -P"; \
	  export HUDSON_AWK="$(AWK)"; \
	  export HUDSON_LDLIBS="$(LIBS) -lcurl"; \
	./test.sh; \
	rm -rf $(srcdir)/poptest
	@echo '*************************************************************'
	@echo '** RunTests/FortranTests/LANL_POP make check rule complete **'
	@echo '*************************************************************'


clean-local:
	rm -rf poptest

EXTRA_DIST = \
	CMakeLists.txt \
	$(POP_DISTRIBUTION)
endif
