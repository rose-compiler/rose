include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

../../testTranslator:
	make -C ../.. testTranslator

noinst_PROGRAMS = deviceTranformation

deviceTranformation_SOURCES = deviceTranformation.C

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

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

if ROSE_USE_EDG_VERSION_4_9
# DQ (1/25/2016): These are conditional upon EDG 4.9 which is the only version with the C++11 support.
testDeviceTransformation_01: deviceTranformation
	./deviceTranformation -std=c++11 -rose:skipfinalCompileStep -c $(srcdir)/inputCode_01.cu

testDeviceTransformation_02: deviceTranformation
	./deviceTranformation -std=c++11 -rose:skipfinalCompileStep -c $(srcdir)/test2016_03.cu
endif

check-local:
if ROSE_BUILD_CUDA_LANGUAGE_SUPPORT
	@echo "Tests C for CUDA support"
	@$(MAKE) $(TESTCODES_Objects)
if ROSE_USE_EDG_VERSION_4_9
	@$(MAKE) testDeviceTransformation_01
	@$(MAKE) testDeviceTransformation_02
endif
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CudaTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"
else
	@echo "Rose is configured without C for CUDA support"
endif

clean-local:
	rm -f *.o rose_* test_*

EXTRA_DIST= $(TESTCODES)

