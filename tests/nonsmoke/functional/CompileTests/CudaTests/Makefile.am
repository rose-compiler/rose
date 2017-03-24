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
test_2011_12.cu 

# DQ (1/19/2017): These are a problem for GNU 5.1 (but maybe just because __atomic_is_lock_free() is not correctly handled).
if !ROSE_USING_GCC_VERSION_LATER_5_1
CUDA_CODES += \
test_2011_13.cu
endif

CUDA_CODES_FAILING = 

TESTCODES= \
  $(CUDA_NO_BUILT_IN_CODES) \
  $(CUDA_CODES)

TESTCODES_Objects = ${TESTCODES:.cu=.o}

$(TESTCODES_Objects): ../../testTranslator
	../../testTranslator -rose:skipfinalCompileStep -c $(srcdir)/$(@:.o=.cu)

if ROSE_USE_EDG_VERSION_4_9
# DQ (1/25/2016): These are conditional upon EDG 4.9 which is the only version with the C++11 support.
if !ROSE_USING_GCC_VERSION_LATER_4_9
# DQ (2/6/2017): We can't use C++11 mode on GNU 4.9 and greater with EDG 4.9 (requires EDG 4.12).
testDeviceTransformation_01: deviceTranformation
	./deviceTranformation -std=c++11 -rose:skipfinalCompileStep -c $(srcdir)/inputCode_01.cu

testDeviceTransformation_02: deviceTranformation
	./deviceTranformation -std=c++11 -rose:skipfinalCompileStep -c $(srcdir)/test2016_03.cu
endif
endif

check-local:
if ROSE_BUILD_CUDA_LANGUAGE_SUPPORT
	@echo "Tests for CUDA support"
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == false"
	@echo "source position tests using GNU version 4.8 and earlier backend compiler."
	@$(MAKE) $(TESTCODES_Objects)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
	@echo "source position tests using GNU version 4.8 backend compiler."
	@$(MAKE) $(TESTCODES_Objects)
else  # NOT USING_GNU_COMPILER
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
# DQ (2/28/2017): Skipping these tests for now.
#	@$(MAKE) $(TESTCODES_Objects)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@$(MAKE) $(TESTCODES_Objects)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9
#	@$(MAKE) $(TESTCODES_Objects)
#if ROSE_USE_EDG_VERSION_4_9
#if !ROSE_USING_GCC_VERSION_LATER_4_9
#	@$(MAKE) testDeviceTransformation_01
#	@$(MAKE) testDeviceTransformation_02
#endif
#endif
	@echo "*******************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/CudaTests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************"
else
	@echo "Rose is configured without C for CUDA support"
endif


clean-local:
	rm -f *.o rose_* test_*

EXTRA_DIST= $(TESTCODES)

