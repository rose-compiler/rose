include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  vectorization
# ------------------------------------------------------------------------------


INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(ROSE_LIBS)

bin_PROGRAMS = vectorization

vectorization_SOURCES=\
	SIMDAnalysis.C \
	main.C \
	normalization.C \
	SIMDAnalysis.h \
	normalization.h \
	vectorization.h \
	vectorization.C

include_HEADERS=\
	rose_simd.h\
	sse_mathfun.h\ 
	avx_mathfun.h

# -msse and -msse2 are enabled for x86-64 compiler by default.
# they have to be enabled on i386 - i686 compilers.
libsimd_la_CPPFLAGS = -msse -msse2
lib_LTLIBRARIES = libsimd.la
libsimd_la_SOURCES = rosesimd.c

EXTRA_DIST = rose_simd.h sse_mathfun.h avx_mathfun.h
