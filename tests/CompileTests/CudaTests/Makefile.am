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

CUDA_CODES =

CUDA_CODES_FAILING = \
test_2011_11.cu \
test_2011_12.cu \
test_2011_13.cu

TESTCODES=
if ROSE_USE_CLANG_FRONTEND
TESTCODES+= \
  $(CUDA_NO_BUILT_IN_CODES) \
  $(CUDA_CODES)
endif

TESTCODES_Objects = ${TESTCODES:.cu=.o}

$(TESTCODES_Objects): ./CudaTests
	cp $(srcdir)/$(@:.o=.cu) $(@:.o=.cu)
	./CudaTests $(@:.o=.cu)
	rm -f $(@:.o=.cu)

check-local:
	@echo "Tests C for CUDA support (parsing)"
	@$(MAKE) $(TESTCODES_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CudaTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_* test_*

EXTRA_DIST= $(TESTCODES)

