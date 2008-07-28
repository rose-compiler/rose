include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = stringSupport commandlineProcessing support graphs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (7/4/2004): Need this to be explicit so that it will not be set to "libroseutil.a.c"
# DIST_SOURCES =

# lib_LIBRARIES = libroseutil.a
# libroseutil_a_DEPENDENCIES = $(libroseutilDependencies)

noinst_LTLIBRARIES = libroseutil.la

libroseutil_la_SOURCES = processSupport.C processSupport.h
libroseutil_la_LIBADD = \
	stringSupport/libRoseStringSupport.la \
	commandlineProcessing/libRoseCommandlineUtil.la

include_HEADERS = setup.h processSupport.h

EXTRA_DIST = setup.h utilDocumentation.docs

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files

distclean-local:
	rm -rf Templates.DB








