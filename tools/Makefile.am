# Build the ROSE-based tools
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = 

# install the tools in 'bin'
bin_SCRIPTS = 

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = moveDeclarationToInnermostScope 

moveDeclarationToInnermostScope_SOURCES = moveDeclarationToInnermostScope.C




