include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# The new EDG-Sage connection requires some built header files from EDG 3.10,
# so that directory must be before EDG_SAGE_Connection
# SUBDIRS = EDG_3.3 EDG_3.10 EDG_SAGE_Connection
SUBDIRS = EDG_3.3 EDG_3.10 EDG_4.0 EDG_SAGE_Connection

EXTRA_DIST = Makefile.am.forBinaryEDG

if ROSE_USE_NEW_EDG_INTERFACE
if ROSE_USE_EDG_VERSION_4
libroseEDGlibraries = EDG_4.0/src/libedg40.la EDG_SAGE_Connection/libnewsage.la
else
libroseEDGlibraries = EDG_3.10/src/libedg310.la EDG_SAGE_Connection/libnewsage.la
endif
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


BINARY_EDG_DIRNAME=xxx_trash_xxx
testEDGBinaryMakefileLogic:
	if test "$(am__untar)" != "false"; then \
	  if test -f $(srcdir)/$(BINARY_EDG_DIRNAME).tar.gz; then \
	    echo "Found $(srcdir)/$(BINARY_EDG_DIRNAME).tar.gz: proceeding to untar file $(BINARY_EDG_DIRNAME).tar.gz" >&2 && \
	    GZIP=$(GZIP_ENV) gunzip -c $(srcdir)/$(BINARY_EDG_DIRNAME).tar.gz | $(am__untar) && \
	    mv $(BINARY_EDG_DIRNAME)/libroseEDG.* . && \
	    mv $(BINARY_EDG_DIRNAME)/.libs . && \
	    rmdir $(BINARY_EDG_DIRNAME)/; \
	  else \
	    echo "There is no binary version of EDG (which should be named $(BINARY_EDG_DIRNAME).tar.gz) in your copy of ROSE -- this should have been caught by configure" >&2 && \
	    exit 1; \
	  fi \
	else \
	  echo "ERROR: am__untar set to false! (am__untar == $(am__untar)) Can't untar $(BINARY_EDG_DIRNAME).tar.gz) -- this should have been caught by configure" >&2 && \
	  exit 1; \
	fi

