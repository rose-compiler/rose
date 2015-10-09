include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

if ROSE_USE_CLANG_FRONTEND

noinst_LTLIBRARIES = libroseClangFrontend.la

libroseClangFrontend_la_DEPENDENCIES =
libroseClangFrontend_la_SOURCES      = \
  clang-frontend.cpp \
  clang-frontend-decl.cpp \
  clang-frontend-stmt.cpp \
  clang-frontend-type.cpp
libroseClangFrontend_la_LDFLAGS      = $(CLANG_LDFLAGS)
libroseClangFrontend_la_CXXFLAGS     = $(CLANG_CXXFLAGS)

pkginclude_HEADERS = clang-frontend.hpp clang-frontend-private.hpp

endif

EXTRA_DIST = \
  clang-builtin-c.h \
  clang-builtin-cpp.hpp \
  clang-builtin-cuda.hpp \
  clang-builtin-opencl.h

