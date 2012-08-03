include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = OpenClTests

# This tests the ability to read and regnerate the input CUDA code.
OpenClTests_SOURCES = OpenClTests.C

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

OPENCL_NO_BUILT_IN_CODES = 

OPENCL_CODES = \
    transpose_kernel.cl

OPENCL_CODES_FAILING = \
    nbody.cl \
    nbody_cpu.cl \
    nbody_gpu.cl

TESTCODES=
if ROSE_USE_CLANG_FRONTEND
TESTCODES+= \
  $(OPENCL_NO_BUILT_IN_CODES) \
  $(OPENCL_CODES)
endif

TESTCODES_Objects = ${TESTCODES:.ocl=.o}

$(TESTCODES_Objects): ./OpenClTests
	cp $(srcdir)/$(@:.o=.cu) $(@:.o=.cu)
	./OpenClTests $(@:.o=.cu)
	rm -f $(@:.o=.cu)

check-local:
	@echo "Tests OpenCl support (parsing)"
	@$(MAKE) $(TESTCODES_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenClTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_* test_*
