include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

bin_PROGRAMS = jsonGenerator

jsonGenerator_SOURCES = jsonGenerator.C

LDADD = $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt
CLEANFILES =

clean-local:
	rm -rf *.o jsonGenerator Templates.DB ii_files ti_files
