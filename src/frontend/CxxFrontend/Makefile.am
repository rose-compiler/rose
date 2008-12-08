include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = EDG_3.3 EDG_SAGE_Connection EDG_3.10

EXTRA_DIST = Makefile.am.forBinaryEDG

if ROSE_USE_NEW_EDG_INTERFACE
libroseEDGlibraries = EDG_3.10/src/libedg310.la EDG_SAGE_Connection/libnewsage.la
else
libroseEDGlibraries = EDG_3.3/src/libedg33.la EDG_SAGE_Connection/libsage.la
endif

noinst_LTLIBRARIES = libroseEDG.la

# We need a simple file to seed the generation of the library (mark the library
# as having C++ code)
libroseEDG_la_SOURCES = dummyCppFileForLibroseEDG.C
libroseEDG_la_LIBADD = $(libroseEDGlibraries) 
libroseEDG_la_LDFLAGS = -no-undefined
libroseEDG_la_DEPENDENCIES = $(libroseEDGlibraries)
