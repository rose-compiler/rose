
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = polygrapher

LDADD = ../../src/libPolyModel.la ../../src/libPolyModelRoseKernel.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CXXFLAGS=-g

polygrapher_SOURCES         = \
  polygrapher.cpp \
  parsetree.cpp \
  polygraph.cpp
polygrapher_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  -I/home/tristan/opt/ppcg/include \
  $(ROSE_INCLUDES)
polygrapher_LDADD           = $(LDADD) -L/home/tristan/opt/ppcg/lib -lisl
polygrapher_includedir      = $(includedir)/polyhedral-model/projects/polygraph/
polygrapher_include_HEADERS = parsetree.hpp polygraph.hpp

../../src/libPolyModel.la:
	make -C ../../src libPolyModel.la

../../src/libPolyModelRoseKernel.la:
	make -C ../../src libPolyModelRoseKernel.la

