include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_TRANSLATOR   =  $(top_builddir)/tests/testTranslator
POP_DISTRIBUTION  =  $(srcdir)/poptest.tgz


if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
if !ROSE_USING_GFORTRAN_VERSION_4_0
# TOO (4/4/2011): top_builddir is calculated relative to LANL_POP/, but
# the test.sh script is inside the untarred poptest/ directory. Therefore,
# the correct path to the testTranslator would be: "../$(TEST_TRANSLATOR)".
#
# HUDSON_LDLIBS should equal exactly "$(LIBS) -lcurl" because $(LIBS) must
# be expanded in the poptest Makefiles; not in this Makefile.
check-local:
	tar xzf $(POP_DISTRIBUTION) -C .
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
	  export HUDSON_LDLIBS="\$$(LIBS) -lcurl"; \
	  export DIFF="`which diff`"; \
	  export MAKE="$(MAKE)"; \
	  export SED="$(SED)"; \
	./test.sh
	@echo '*************************************************************'
	@echo '** RunTests/FortranTests/LANL_POP make check rule complete **'
	@echo '*************************************************************'
endif
endif


clean-local:
	rm -rf poptest

EXTRA_DIST = \
	CMakeLists.txt \
	$(POP_DISTRIBUTION)
