include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

libconstantFoldingSources = \
   constantFolding.C

if DEV_LIBS
lib_LTLIBRARIES = libconstantFolding.la
else
noinst_LTLIBRARIES = libconstantFolding.la
endif
libconstantFolding_la_SOURCES = $(libconstantFoldingSources)

include_HEADERS = constantFolding.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core
