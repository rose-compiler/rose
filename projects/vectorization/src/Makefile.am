
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  vectorization
# ------------------------------------------------------------------------------

#exec_prefix=${prefix}/vectorization
Vectorization_includedir=$(exec_prefix)
INCLUDES = $(ROSE_INCLUDES) -I$(Vectorization_includedir)
LDADD    = $(ROSE_LIBS)
lib_LIBRARIES = libsimd.a
bin_PROGRAMS = vectorization

vectorization_SOURCES=\
	main.C \
	normalization.C \
	vectorization.C 

Vectorization_include_HEADERS=\
  normalization.h \
  vectorization.h

include_HEADERS = ROSE_SIMD.h
  
libsimd_a_SOURCES = rosesimd.c 
