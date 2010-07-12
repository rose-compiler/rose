include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# tps (1Dec2008) : I enabled the mysql path detection and this code fails because no such include files can be found. 
# so I disable this if --with-binarysql is used!
if ROSE_USE_BINARY_SQL
else
if ROSE_USE_SQLITE_DATABASE
INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) $(SQLITE_DATABASE_INCLUDE)

# noinst_HEADERS = sideEffect.h
include_HEADERS = sideEffect.h \
									DatabaseGraph.h

noinst_LTLIBRARIES = libsideEffect.la

## Specifications for building the pdf library.
libsideEffect_la_SOURCES = sideEffect.C

endif
endif



EXTRA_DIST = CMakeLists.txt
