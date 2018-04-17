include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

bin_PROGRAMS = pdfGenerator

pdfGenerator_SOURCES = pdfGenerator.C

LDADD = $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt 
CLEANFILES = 

clean-local:
	rm -rf *.o pdfGenerator Templates.DB ii_files ti_files


