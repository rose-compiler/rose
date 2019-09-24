include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

../../testTranslator:
	make -C ../.. testTranslator

noinst_PROGRAMS = deviceTranformation

deviceTranformation_SOURCES = deviceTranformation.C

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS  = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

CUDA_NO_BUILTIN_CODES = \
test_2011_01.cu \
test_2011_02.cu

# Case with C++ 11 
if ROSE_USING_GCC_VERSION_LATER_6_1
  CUDA_NO_BUILTIN_CODES += \
    rose-2123.cu
endif

CUDA_NO_BUILTIN_CODES_Objects = ${CUDA_NO_BUILTIN_CODES:.cu=.o}

$(CUDA_NO_BUILTIN_CODES_Objects): ../../testTranslator
	../../testTranslator -DSKIP_ROSE_BUILTIN_DECLARATIONS -rose:skipfinalCompileStep -c $(srcdir)/$(@:.o=.cu)

CUDA_CODES = \
test_2011_11.cu \
test_2011_12.cu \
test_2011_13.cu \
header_test.cu \
mm_single.cu

CUDA_CODES_Objects = ${CUDA_CODES:.cu=.o}

$(CUDA_CODES_Objects): ../../testTranslator
	../../testTranslator -rose:skipfinalCompileStep -c $(srcdir)/$(@:.o=.cu)

# DQ (1/25/2016): These are conditional upon EDG 4.9 which is the only version with the C++11 support.
if !ROSE_USING_GCC_VERSION_LATER_4_9
# DQ (2/6/2017): We can't use C++11 mode on GNU 4.9 and greater with EDG 4.9 (requires EDG 4.12).
testDeviceTransformation_01: deviceTranformation
	./deviceTranformation -std=c++11 -rose:skipfinalCompileStep -c $(srcdir)/inputCode_01.cu

testDeviceTransformation_02: deviceTranformation
	./deviceTranformation -std=c++11 -rose:skipfinalCompileStep -c $(srcdir)/test2016_03.cu
endif

check-local:
if USING_GNU_COMPILER
if ROSE_BUILD_CUDA_LANGUAGE_SUPPORT
	@$(MAKE) $(CUDA_NO_BUILTIN_CODES_Objects)
	@$(MAKE) $(CUDA_CODES_Objects)
	@echo "*******************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/CudaTests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************"
else
	@echo "Rose is configured without C for CUDA support"
endif
else
	@echo "Rose is configured for another backend than GNU Compiler"
endif

clean-local:
	rm -f *.o rose_* test_*

EXTRA_DIST= $(TESTCODES)

