
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = tooling

$(top_builddir)/src/frontend/DLX/lib/libDLX-core.la:
	make -C $(top_builddir)/src/frontend/DLX/lib/core libDLX-core.la

$(builddir)/../lib/dlx/tooling/libDLX-tooling.la:
	make -C $(builddir)/../lib/dlx/tooling libDLX-tooling.la

tooling_SOURCES = \
  tooling.cpp
tooling_CXXFLAGS = \
  -I$(srcdir)/../include \
  -I$(top_srcdir)/src/frontend/DLX/include \
  -g \
  $(ROSE_INCLUDES)
tooling_LDADD= \
  $(top_builddir)/src/frontend/DLX/lib/core/libDLX-core.la \
  $(builddir)/../lib/dlx/tooling/libDLX-tooling.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

