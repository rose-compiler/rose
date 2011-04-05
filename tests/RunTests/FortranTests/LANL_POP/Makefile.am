include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
POP_DISTRIBUTION  =  poptest.tgz
TEST_TRANSLATOR   =  $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# TOO (4/4/2011): top_builddir is calculated relative to LANL_POP/, but
# the test.sh script is inside the untarred poptest/ directory. Therefore,
# the correct path to the testTranslator would be: "../$(TEST_TRANSLATOR)"
check-local:
	tar xzf $(srcdir)/$(POP_DISTRIBUTION) -C .
	cd poptest; \
	TRANSLATOR=../$(TEST_TRANSLATOR); \
          HUDSON_NUM_JOBS=$${NUM_PROCESS}; \
	  HUDSON_FC="$(GFORTRAN_PATH)"; \
	  HUDSON_FFLAGS="-ffree-line-length-0"; \
	  HUDSON_LD="$(GFORTRAN_PATH)"; \
	  HUDSON_CC="$(CC)"; \
	  HUDSON_CXX="$(CXX)"; \
	  HUDSON_Cp="/bin/cp"; \
	  HUDSON_Cpp="/lib/cpp -P"; \
	  HUDSON_AWK="$(AWK)"; \
	  HUDSON_LDLIBS="$(LIBS) -lcurl"; \
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
