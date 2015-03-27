include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
noinst_PROGRAMS =
EXTRA_DIST = rose.config

bin_PROGRAMS += roseDiff
roseDiff_SOURCES = roseDiff.C
roseDiff_CPPFLAGS = $(ROSE_INCLUDES)
roseDiff_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS += rosePatch
rosePatch_SOURCES = rosePatch.C
rosePatch_CPPFLAGS = $(ROSE_INCLUDES)
rosePatch_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
