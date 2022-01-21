include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Winvalid-pch $(ROSE_INCLUDES) -I$(srcdir)/graph -I$(srcdir)/libraryIdentification

noinst_LTLIBRARIES = libbinaryMidend.la
libbinaryMidend_la_DEPENDENCIES =

libbinaryMidend_la_SOURCES =

libbinaryMidend_la_SOURCES +=					\
    libraryIdentification/libraryIdentification.C

pkginclude_HEADERS =					\
    ether.h

if ROSE_USE_SQLITE_DATABASE
pkginclude_HEADERS +=					\
    libraryIdentification/FunctionInfo.h \
    libraryIdentification/libraryIdentification.h \
    libraryIdentification/LibraryInfo.h

endif

EXTRA_DIST = CMakeLists.txt dummyBinaryMidend.C
