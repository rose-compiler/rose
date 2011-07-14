include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = ClangToDot

ClangToDot_SOURCES  = DotConsumer.C DotVisitor.C test.C
ClangToDot_LDFLAGS  = $(CLANG_LDFLAGS)
ClangToDot_CXXFLAGS = $(CLANG_CXXFLAGS)

noinst_LTLIBRARIES = libroseClangFrontend.la

libroseClangFrontend_la_DEPENDENCIES =
libroseClangFrontend_la_SOURCES      = ClangFrontend.C
libroseClangFrontend_la_LDFLAGS      = $(CLANG_LDFLAGS)
libroseClangFrontend_la_CXXFLAGS     = $(CLANG_CXXFLAGS)

dist_include_HEADERS = ClangFrontend.h ClangFrontend-private.h DotConsumer.h DotVisitor.h

include_HEADERS =


EXTRA_DIST = CMakeLists.txt
