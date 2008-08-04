include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = EDG_3.3 EDG_SAGE_Connection EDG_3.10

EXTRA_DIST = Makefile.am.forBinaryEDG

# We only to to specify the library EDG_3.3/src/libedg33.la and EDG_SAGE_Connection/sage_gen_be.lo 
# libroseEDGlibraries = EDG_3.3/src/libedg33.la EDG_SAGE_Connection/sage_gen_be.lo

# If we do have the source code then the library was built in the build tree
libroseEDGlibraries = EDG_3.3/src/libedg33.la EDG_SAGE_Connection/libsage.la

# lib_LTLIBRARIES = libroseEDG.la
noinst_LTLIBRARIES = libroseEDG.la

# We need a simple file to seed the generation of the library
libroseEDG_la_SOURCES = dummyCppFileForLibroseEDG.C
libroseEDG_la_LIBADD = $(libroseEDGlibraries) 
# libroseEDG_la_LIBADD = EDG_3.3/src/*.lo EDG_SAGE_Connection/*.lo
libroseEDG_la_LDFLAGS = -no-undefined
libroseEDG_la_DEPENDENCIES = $(libroseEDGlibraries)
# libroseEDG_la_DEPENDENCIES = dummyCppFileForLibroseEDG.C
