
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS= openacc-parser

openacc_parser_SOURCES= \
  openacc-parser.cpp
openacc_parser_CXXFLAGS= \
  -g \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -I$(top_srcdir)/src/util/Handles \
  $(ROSE_INCLUDES)
openacc_parser_LDADD = \
  $(top_builddir)/src/util/Handles/libROSE-handles.la \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(top_builddir)/src/frontend/DLX/lib/openacc/libDLX-openacc.la \
  $(LIBS_WITH_RPATH) \
  $(ROSE_LIBS)

if ROSE_HAVE_OPENCL_INC
check-local: openacc-parser
	./openacc-parser -I$(top_srcdir)/../libopenacc/include -I$(OPENCL_INC_DIR) $(top_srcdir)/../tests/101/kernel-openacc.c
	./openacc-parser -I$(top_srcdir)/../libopenacc/include -I$(OPENCL_INC_DIR) $(top_srcdir)/../tests/102/kernel-openacc.c
	./openacc-parser -I$(top_srcdir)/../libopenacc/include -I$(OPENCL_INC_DIR) $(top_srcdir)/../tests/103/kernel-openacc.c
	./openacc-parser -I$(top_srcdir)/../libopenacc/include -I$(OPENCL_INC_DIR) $(top_srcdir)/../tests/104/kernel-openacc.c
	./openacc-parser -I$(top_srcdir)/../libopenacc/include -I$(OPENCL_INC_DIR) $(top_srcdir)/../tests/109/kernel-openacc.c
endif
