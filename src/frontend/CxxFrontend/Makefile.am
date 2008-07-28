include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = EDG_3.3 EDG_SAGE_Connection EDG_3.10

# We only to to specify the library EDG_3.3/src/libedg33.la and EDG_SAGE_Connection/sage_gen_be.lo 
# libroseEDGlibraries = EDG_3.3/src/libedg33.la EDG_SAGE_Connection/sage_gen_be.lo
if ROSE_USE_EDG_SOURCE

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

else

librose.la:

# DQ (7/23/2008): This is support for passing "make dist" in "make distcheck" so
# that we can test a binary distribution using "make distcheck"
# DQ (7/23/2008): Now we have a single library for both the EDG and EDG/ROSE connection so
# we copy that to the distribution where we need to support a binary distribution of ROSE.
dist-hook:
#	echo "Output files from current directory before copying (using scripts/libtoolcp) libroseEDG.la to $(distdir)"
#	du -a $(distdir)
#	echo "Copying (using scripts/libtoolcp) libroseEDG.la to $(distdir)"
#	We can't call scripts/libtoolcp since internally the libtool variable $old_library
#	will not be defined.  This is because our library libroseEDG.la is actually
#	just constructed from other libraries.
#	$(top_srcdir)/scripts/libtoolcp libroseEDG.la $(distdir)
#	cp -a libroseEDG.la $(distdir)
#	cp -a .libs $(distdir)
#	ln -s $(distdir)/libroseEDG.la $(distdir)/.libs/libroseEDG.la
	if [ -f libroseEDG.la ]; then \
      echo "libroseEDG.la file exists"; \
      cp -a libroseEDG.la $(distdir); \
      cp -a .libs $(distdir); \
   else \
      echo "libroseEDG file does not exist (meaningless distribution being built)"; \
   fi
#	echo "DONE Copying (using scripts/libtoolcp) libroseEDG.la to $(distdir)"
#	du -a $(distdir)
#	echo "DONE: Output files from current directory before copying (using scripts/libtoolcp) libroseEDG.la to $(distdir)"

endif
