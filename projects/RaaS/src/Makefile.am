
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

bin_PROGRAMS=CompDBtoAstIO

CompDBtoAstIO_SOURCES=CompDBtoAstIO.cxx
CompDBtoAstIO_CXXFLAGS=-g -O0 -I$(srcdir)/../include $(ROSE_INCLUDES)
#CompDBtoAstIO_LDADD= -L$(builddir)/../lib $(LIBS_WITH_RPATH) $(ROSE_LIBS)
CompDBtoAstIO_LDADD= $(LIBS_WITH_RPATH) $(ROSE_LIBS)

