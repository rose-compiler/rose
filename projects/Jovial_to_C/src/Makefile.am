include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  Jovial_directive
# ------------------------------------------------------------------------------

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS  = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

bin_PROGRAMS = translate

translate_SOURCES=\
	translate.C   \
        j2c/j2cStatement.C   \
        j2c/j2cExpression.C

include_HEADERS = j2c.h

EXTRA_DIST = j2c.h
