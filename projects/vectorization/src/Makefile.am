
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
libsimd_la_CPPFLAGS = -msse 
lib_LTLIBRARIES = libsimd.la
bin_PROGRAMS = vectorization
include_HEADERS = rose_simd.h

libsimd_la_SOURCES = rosesimd.c 

vectorization_SOURCES=\
	main.C \
	normalization.C \
	SIMDAnalysis.C \
	vectorization.C 

Vectorization_include_HEADERS=\
  normalization.h \
  SIMDAnalysis.h \
  vectorization.h

  
