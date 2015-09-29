include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# libRoseCommandlineUtilSources = commandline_processing.C sla.c sla++.C
libRoseCommandlineUtilSources = commandline_processing.C sla++.C

# lib_LIBRARIES = libRoseCommandlineUtil.a
# libRoseCommandlineUtil_a_SOURCES  = $(libRoseCommandlineUtilSources)

noinst_LTLIBRARIES = libRoseCommandlineUtil.la
libRoseCommandlineUtil_la_SOURCES = $(libRoseCommandlineUtilSources)

# We have to explicily build this rule since we want to avoid the use of "ar cru"
# (need to use the C++ compiler so that templates will be built)
# libRoseCommandlineUtil.a: $(libRoseCommandlineUtil_a_OBJECTS)
#	-rm -f $@
#	$(CXX_STATIC_LIB_UPDATE) $@ $(libRoseCommandlineUtil_a_OBJECTS)
#	$(RANLIB) $@
#	test -f $@ || exit 1

#libRoseCommandlineUtil.so: $(libRoseCommandlineUtil_so_OBJECTS)
#	-rm -f $@
#	$(CXX_SHARED_LIB_UPDATE) $@ $(libRoseCommandlineUtil_so_OBJECTS)
#	test -f $@ || exit 1

pkginclude_HEADERS = sla.h commandline_processing.h

# DQ (2/28/2010): New binary AST merge tests.
READ_TRANSLATOR = $(top_builddir)/tests/testAstFileRead
# READ_TRANSLATOR = $(top_builddir)/tests/parallelASTMerge

testBinaryAstMerge:
#	$(READ_TRANSLATOR) $(libRoseCommandlineUtilSources)
	roseAstFileRead $(libRoseCommandlineUtilSources) OUTPUT_BINARY_AST_FILE

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files

distclean-local:
	rm -rf Templates.DB

EXTRA_DIST = CMakeLists.txt commandlineProcessingDocumentation.docs






