
noinst_PROGRAMS=

if ROSE_HAVE_CUDA_LIB
  noinst_PROGRAMS += test
endif

test_SOURCES= \
  test.cpp \
  test-cuda-runtime.cpp
test_CXXFLAGS= \
  -I$(CUDA_INC_DIR) \
  -g -I$(top_srcdir)/include
test_LDADD= \
  ../../lib/libUPR-core.la \
  ../../lib/libUPR-CUDA.la \
  -L$(CUDA_LIB_DIR) -lcuda

../../lib/libUPR-core.la:
	make -C ../../lib libUPR-core.la
../../lib/libUPR-CUDA.la:
	make -C ../../lib libUPR-CUDA.la

if ROSE_HAVE_CUDA_LIB
check: test
	./test
endif

