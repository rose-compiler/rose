
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=mdcg-openacc

mdcg_openacc_SOURCES= \
  mdcg-openacc.cpp
mdcg_openacc_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/midend/MDCG/include \
  -I$(top_srcdir)/src/util/Handles \
  $(ROSE_INCLUDES)
mdcg_openacc_LDADD = \
  $(top_builddir)/src/util/Handles/libROSE-handles.la \
  $(top_builddir)/src/midend/MDCG/lib/libMDCG.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

check-local: mdcg-openacc
	./mdcg-openacc $(OPENACC_INC_DIR)

