include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

libRoseStringSupportSources = string_functions.cpp FileNameClassifier.C escape.C

# lib_LIBRARIES = libRoseStringSupport.a
# libRoseStringSupport_a_SOURCES  = $(libRoseStringSupportSources)

# Tell automake install to copy this library
noinst_LTLIBRARIES = libRoseStringSupport.la
# noinst_LIBRARIES = libRoseStringSupport.so
libRoseStringSupport_la_SOURCES = $(libRoseStringSupportSources)

# Tell automake install to copy this library
# EXTRA_LIBRARIES = libRoseStringSupport.so

# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
#libRoseStringSupport.a: $(libRoseStringSupport_a_OBJECTS)
#	-rm -f $@
#	$(CXX_STATIC_LIB_UPDATE) $@ $(libRoseStringSupport_a_OBJECTS)
#	$(RANLIB) $@
#	test -f $@ || exit 1

#libRoseStringSupport.so: $(libRoseStringSupport_so_OBJECTS)
#	-rm -f $@
#	$(CXX_SHARED_LIB_UPDATE) $@ $(libRoseStringSupport_so_OBJECTS)
#	test -f $@ || exit 1

include_HEADERS = string_functions.h escape.h

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files

distclean-local:
	rm -rf Templates.DB

EXTRA_DIST = stringSupportDocumentation.docs





