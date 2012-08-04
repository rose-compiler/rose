
noinst_PROGRAMS=

if HAVE_OPENCL
  noinst_PROGRAMS += test
endif

test_SOURCES= \
  test.cpp \
  test-opencl-runtime.cpp
test_CXXFLAGS= \
  $(CL_CFLAGS) \
  -g -I$(top_srcdir)/include
test_LDADD= \
  $(CL_LIBS) \
  ../../lib/libUPR-core.la \
  ../../lib/libUPR-OpenCL.la

../../lib/libUPR-core.la:
	make -C ../../lib libUPR-core.la
../../lib/libUPR-OpenCL.la:
	make -C ../../lib libUPR-OpenCL.la

check-local: test
	cp $(srcdir)/test.cl test.cl
	./test
	rm -f test.cl

