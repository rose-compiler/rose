include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES =  libfailsafe.la

libfailsafe_la_SOURCES = \
  failSafePragma.C

pkginclude_HEADERS =  \
  failSafePragma.h

clean-local:
	rm -rf Templates.DB ii_files ti_files

distclean-local:
	rm -rf Templates.DB

EXTRA_DIST = CMakeLists.txt  

