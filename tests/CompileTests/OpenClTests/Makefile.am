include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

../../testTranslator:
	make -C../.. testTranslator

OPENCL_CODES = \
  empty.cl

if ROSE_USE_CLANG_FRONTEND
OPENCL_CODES += \
  transpose_kernel.cl \
  nbody.cl \
  nbody_cpu.cl \
  nbody_gpu.cl
else
OPENCL_CODES_FAILING = \
  transpose_kernel.cl \
  nbody.cl \
  nbody_cpu.cl \
  nbody_gpu.cl
endif

TESTCODES= \
  $(OPENCL_CODES)

TESTCODES_Objects = ${TESTCODES:.cl=.o}

$(TESTCODES_Objects): ../../testTranslator
	../../testTranslator -rose:skipfinalCompileStep $(srcdir)/$(@:.o=.cl)


check-local:
if ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT
	@echo "Tests OpenCl support (parsing)"
	@$(MAKE) $(TESTCODES_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenClTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"
else
	@echo "Rose is configured without OpenCL support"
endif

clean-local:
	rm -f *.o rose_* test_*
