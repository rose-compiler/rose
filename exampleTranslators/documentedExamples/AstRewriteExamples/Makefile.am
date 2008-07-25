include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = astRewriteExample1

astRewriteExample1_SOURCES = astRewriteExample1.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# DQ this file is used in the ROSE User Manual so we need it here
EXTRA_DIST = inputRewrite1.C exampleRewrite1.C

CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files

