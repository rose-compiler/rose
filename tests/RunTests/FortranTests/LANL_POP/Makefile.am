include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
POP_DISTRIBUTION  =  poptest.tgz
TEST_TRANSLATOR   =  $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

check-local:
	tar xzf $(srcdir)/$(POP_DISTRIBUTION) -C $(builddir)
	cd poptest; \
	export TRANSLATOR=$(TEST_TRANSLATOR); \
        export HUDSON_NUM_JOBS=$${NUM_PROCESS}; \
	export HUDSON_FC="$(GFORTRAN_PATH)"; \
	export HUDSON_FFLAGS="-ffree-line-length-0"; \
	export HUDSON_LD="$(GFORTRAN_PATH)"; \
	export HUDSON_CC="$(CC)"; \
	export HUDSON_Cp="/bin/cp"; \
	export HUDSON_Cpp="/lib/cpp -P"; \
	export HUDSON_AWK="$(AWK)"; \
	export HUDSON_LDLIBS="$(LIBS) -lcurl"; \
	./test.sh; \
        rm -rf poptest

# TODO: add CMakeLists.txt
EXTRA_DIST = \
	CMakeLists.txt \
	$(POP_DISTRIBUTION)
endif
