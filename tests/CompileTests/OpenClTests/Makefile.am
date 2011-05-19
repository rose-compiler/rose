include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = OpenClTests

# This tests the ability to read and regnerate the input CUDA code.
OpenClTests_SOURCES = OpenClTests.C

# Use names set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

OPENCL_NO_BUILT_IN_CODES = 

OPENCL_CODES = 

TESTCODES = $(OPENCL_NO_BUILT_IN_CODES) $(OPENCL_CODES)

TESTCODES_Objects = ${TESTCODES:.ocl=.o}

$(TESTCODES_Objects): ./OpenClTests
	cp $(srcdir)/$(@:.o=.cu) $(@:.o=.cu)
	./OpenClTests $(@:.o=.cu)
	rm -f $(@:.o=.cu)

check-local:
if ROSE_BUILD_EDG_WITH_OPENCL_SUPPORT
	@echo "Tests OpenCl support (parsing)"
	@$(MAKE) $(TESTCODES_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenClTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"
else
	@echo "****************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenClTests: (ROSE not configured for these tests) ******"
	@echo "****************************************************************************************************"
endif

clean-local:
	rm -f *.o rose_* test_*
