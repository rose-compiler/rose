include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AM_CPPFLAGS = $(ROSE_INCLUDES)
# AM_CPPFLAGS = $(ROSE_INCLUDES) -fno-rtti
AM_CPPFLAGS = $(ROSE_INCLUDES)

# DQ (4/5/2017): eliminate warnings in initial development.
CXXFLAGS += -w

if ROSE_USE_CLANG_FRONTEND


# DQ (10/31/2020): Don't build the clang dot support just yet.
noinst_LTLIBRARIES = libroseClangFrontend.la
# noinst_LTLIBRARIES = libroseClangFrontend.la libClangToDot.la

libroseClangFrontend_la_DEPENDENCIES =
libroseClangFrontend_la_SOURCES      = \
  clang-frontend.cpp \
  clang-frontend-decl.cpp \
  clang-frontend-stmt.cpp \
  clang-frontend-type.cpp \
  clang-to-dot.cpp \
  clang-to-dot-decl.cpp \
  clang-to-dot-stmt.cpp \
  clang-to-dot-type.cpp 
libroseClangFrontend_la_LDFLAGS      = $(CLANG_LDFLAGS)
libroseClangFrontend_la_CXXFLAGS     = $(CLANG_CXXFLAGS)

# DQ (1/27/2020): Removed this file.
#  clang_graph.cpp

# libClangToDot_la_DEPENDENCIES =
# libClangToDot_la_SOURCES      = 
#  clang-to-dot.cpp 
#  clang-to-dot-decl.cpp 
#  clang-to-dot-stmt.cpp 
#  clang-to-dot-type.cpp
libClangToDot_la_LDFLAGS      = $(CLANG_LDFLAGS)
libClangToDot_la_CXXFLAGS     = $(CLANG_CXXFLAGS)

pkginclude_HEADERS = clang-frontend.hpp clang-frontend-private.hpp 	clang-to-dot-private.hpp clang-to-dot.hpp clang_graph.h

endif

EXTRA_DIST = \
  clang-builtin-c.h \
  clang-builtin-cpp.hpp \
  clang-builtin-cuda.hpp \
  clang-builtin-opencl.h

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf *.o
