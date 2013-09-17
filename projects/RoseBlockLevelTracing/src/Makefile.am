include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects


RoseBlockLevelTracing_includedir=$(prefix)
INCLUDES = $(ROSE_INCLUDES) -I$(RoseBlockLevelTracing_includedir)
LDADD    = $(ROSE_LIBS)

bin_PROGRAMS = RoseBlockLevelTracing


RoseBlockLevelTracing_SOURCES=\
	roseBlockLevelTracing.C 


