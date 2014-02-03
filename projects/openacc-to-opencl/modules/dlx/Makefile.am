
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAM= 

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

check-local: openacc-parser
	./openacc-parser $(srcdir)/matmul-acc.cpp

