
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=mdcg-opencl

mdcg_opencl_SOURCES= \
  mdcg-opencl.cpp
mdcg_opencl_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/util/Handles \
  $(ROSE_INCLUDES)
mdcg_opencl_LDADD = \
  $(top_builddir)/src/util/Handles/libROSE-handles.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

check-local: mdcg-opencl
	./mdcg-opencl $(OPENCL_INC_DIR)

