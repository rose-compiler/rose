include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = CudaTests

# This tests the ability to read and regnerate the input CUDA code.
CudaTests_SOURCES = CudaTests.C

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CUDA_NO_BUILT_IN_CODES = \
test_2011_01.cu \
test_2011_02.cu

CUDA_CODES = \
test_2011_11.cu \
test_2011_12.cu \
test_2011_13.cu

TESTCODES = $(CUDA_NO_BUILT_IN_CODES) $(CUDA_CODES)

TESTCODES_Objects = ${TESTCODES:.cu=.o}

$(TESTCODES_Objects): ./CudaTests
	cp $(srcdir)/$(@:.o=.cu) $(@:.o=.cu)
	./CudaTests $(@:.o=.cu)
	rm -f $(@:.o=.cu)

check-local:
if ROSE_BUILD_EDG_WITH_CUDA_SUPPORT
	@echo "Tests C for CUDA support (parsing)"
	@$(MAKE) $(TESTCODES_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CudaTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"
else
	@echo "****************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CudaTests: (ROSE not configured for these tests) ******"
	@echo "****************************************************************************************************"
endif

clean-local:
	rm -f *.o rose_* test_*

EXTRA_DIST= $(TESTCODES)

