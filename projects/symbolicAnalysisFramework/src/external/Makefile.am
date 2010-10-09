include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

#INCLUDES = $(ROSE_INCLUDES) \
#           -I$(srcdir)/../external

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafexternal.la
libSafexternal_la_SOURCES = spearWrap.C
include_HEADERS = spearWrap.h

