include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_SQLITE_DATABASE
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) $(SQLITE_DATABASE_INCLUDE)

# noinst_HEADERS = sideEffect.h
include_HEADERS = sideEffect.h \
									SqliteDatabaseGraph.h

noinst_LTLIBRARIES = libsideEffect.la

## Specifications for building the pdf library.
libsideEffect_la_SOURCES = sideEffect.C

endif

EXTRA_DIST = CMakeLists.txt
