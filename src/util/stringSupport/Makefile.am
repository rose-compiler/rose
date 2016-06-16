include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

libRoseStringSupportSources = StringUtility.C FileNameClassifier.C FileUtility.C escape.C

# Tell automake install to copy this library
noinst_LTLIBRARIES = libRoseStringSupport.la
libRoseStringSupport_la_SOURCES = $(libRoseStringSupportSources)

pkginclude_HEADERS = StringUtility.h string_functions.h FileUtility.h escape.h

# DQ (2/8/2010): This reproduces a bug from before the FileNameClassifier.C was cleaned up.
# The code that failes has been added as: test2010_01.C in the tests/CompileTests/Cxx_tests directory.
# testBug: bug_FileNameClassifier.o

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.loT *.int.c

distclean-local:
	rm -rf Templates.DB

EXTRA_DIST = CMakeLists.txt stringSupportDocumentation.docs

