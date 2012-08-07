
noinst_PROGRAMS=

if ROSE_HAVE_OPENCL_LIB
  noinst_PROGRAMS += test
endif

test_SOURCES= \
  test.cpp \
  test-opencl-runtime.cpp
test_CXXFLAGS= \
  -I$(OPENCL_INC_DIR) \
  -g -I$(top_srcdir)/include
test_LDADD= \
  -L$(OPENCL_LIB_DIR) -lopencl \
  ../../lib/libUPR-core.la \
  ../../lib/libUPR-OpenCL.la

../../lib/libUPR-core.la:
	make -C ../../lib libUPR-core.la
../../lib/libUPR-OpenCL.la:
	make -C ../../lib libUPR-OpenCL.la

if ROSE_HAVE_OPENCL_LIB
check-local: test
	cp $(srcdir)/test.cl test.cl
	./test
	rm -f test.cl
endif

