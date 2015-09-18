include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

libmulticore_la_CPPFLAGS = -fopenmp -O3
if ROSE_USE_LIBNUMA
libmulticore_la_CPPFLAGS += -lnuma -DHAVE_NUMA_H=1  
endif
lib_LTLIBRARIES = libmulticore.la
libmulticore_la_SOURCES =\
			 multicore.C\
			 core.C
include_HEADERS= multicore.h 

EXTRA_DIST = $(srcdir)/core.h $(srcdir)/multicore.h stencil.h \
             $(srcdir)/core.C $(srcdir)/multicore.C

