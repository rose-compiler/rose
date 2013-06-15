include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = test

test_SOURCES = test.cpp
test_CXXFLAGS = \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)
test_LDADD= \
  $(top_builddir)/src/backend/KLT/lib/core/libKLT-core.la \
  $(top_builddir)/src/backend/KLT/lib/sequential/libKLT-sequential.la \
  $(top_builddir)/src/backend/KLT/lib/opencl/libKLT-opencl.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

