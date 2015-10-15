# Make stub common to all ROSE-HPCT submodules

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Root of ROSE-HPCT source tree
ROSEHPCT_SRC_ROOT = $(top_srcdir)/src/roseExtensions/roseHPCToolkit
ROSEHPCT_BUILD_ROOT = $(top_builddir)/src/roseExtensions/roseHPCToolkit

# Root of ROSE-HPCT header file installation tree
rosehpctincdir = $(pkgincludedir)/rosehpct

# Default header file search path
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) \
           $(ROSE_INCLUDES) \
           $(ROSEHPCT_DEP_INCLUDES) \
           -I$(ROSEHPCT_SRC_ROOT)/include

# Default link flags for executables
ROSEHPCT_LIBS = $(ROSEHPCT_BUILD_ROOT)/src/librosehpct.la \
                $(ROSE_LIBS) $(RT_LIBS)
LDADD = $(ROSEHPCT_DEP_LIBS)

# eof
