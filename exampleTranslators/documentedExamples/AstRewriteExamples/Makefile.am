include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# DQ (3/21/2009): fails to compile under Cygwin.
if !ROSE_BUILD_OS_IS_CYGWIN
bin_PROGRAMS = astRewriteExample1
astRewriteExample1_SOURCES = astRewriteExample1.C
endif

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# DQ this file is used in the ROSE User Manual so we need it here
EXTRA_DIST = CMakeLists.txt inputRewrite1.C exampleRewrite1.C

CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files

