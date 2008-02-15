include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = pdfGenerator

pdfGenerator_SOURCES = pdfGenerator.C
pdfGenerator_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 
CLEANFILES = 

clean-local:
	rm -rf *.o pdfGenerator Templates.DB ii_files ti_files


