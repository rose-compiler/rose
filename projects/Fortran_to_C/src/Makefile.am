
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  Fortran_directive
# ------------------------------------------------------------------------------

Fortran_to_C_includedir=$(prefix)
INCLUDES = $(ROSE_INCLUDES) -I$(Fortran_to_C_includedir)
LDADD    = $(ROSE_LIBS)

bin_PROGRAMS = Fortran_to_C


Fortran_to_C_SOURCES=\
	main.C \
	f2c/f2cExpression.C \
	f2c/f2cStatement.C \
	f2c/f2cArray.C 

Fortran_to_C_include_HEADERS = f2c.h

