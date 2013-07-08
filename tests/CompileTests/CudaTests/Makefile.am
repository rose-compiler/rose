include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

../../testTranslator:
	make -C ../.. testTranslator

CUDA_NO_BUILT_IN_CODES = \
test_2011_01.cu \
test_2011_02.cu

CUDA_CODES = \
test_2011_11.cu \
test_2011_12.cu \
test_2011_13.cu

CUDA_CODES_FAILING = 

TESTCODES= \
  $(CUDA_NO_BUILT_IN_CODES) \
  $(CUDA_CODES)

TESTCODES_Objects = ${TESTCODES:.cu=.o}

$(TESTCODES_Objects): ../../testTranslator
	../../testTranslator -rose:skipfinalCompileStep -c $(srcdir)/$(@:.o=.cu)

check-local:
if ROSE_BUILD_CUDA_LANGUAGE_SUPPORT
	@echo "Tests C for CUDA support"
	@$(MAKE) $(TESTCODES_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CudaTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"
else
	@echo "Rose is configured without C for CUDA support"
endif

clean-local:
	rm -f *.o rose_* test_*

EXTRA_DIST= $(TESTCODES)

