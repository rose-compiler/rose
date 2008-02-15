include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = defaultTranslator

defaultTranslator_SOURCES = defaultTranslator.C
defaultTranslator_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 
CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files

