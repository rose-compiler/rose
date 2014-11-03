include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  MultiLevelMemory
# ------------------------------------------------------------------------------


INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(ROSE_LIBS)

bin_PROGRAMS = mlm

mlm_SOURCES=\
	mlm.cpp\
	mlmapi.h\
	main.cpp

include_HEADERS=
