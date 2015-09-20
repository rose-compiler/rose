include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  Fortran_directive
# ------------------------------------------------------------------------------

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(ROSE_LIBS)

bin_PROGRAMS = NASC


NASC_SOURCES=\
	main.C
 
include_HEADERS = 

EXTRA_DIST = $(srcdir)/NASC.h 
