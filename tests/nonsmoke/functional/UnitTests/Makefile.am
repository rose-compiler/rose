include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#------------------------------------------------------------------------------
# Subdirectories
#------------------------------------------------------------------------------
SUBDIRS =         \
	.               \
	Rose

#------------------------------------------------------------------------------
# Global Build Flags
#------------------------------------------------------------------------------
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/nonsmoke/functional/UnitTests/include/
LDADD       = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------
# ROSE Dependency
#------------------------------------------------------------------------------
# Ensure librose has been built since our tests will link with it
librose:
	$(MAKE) librose.la -C $(top_builddir)/src

#------------------------------------------------------------------------------
# Build Targets
#------------------------------------------------------------------------------
all-local: librose

check-local:

clean-local:
	rm -f $(TEST_TARGETS)

#------------------------------------------------------------------------------
lib_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = common.C
#------------------------------------------------------------------------------

