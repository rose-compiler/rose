include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

libfunctionCallNormalizationSources = \
   FunctionNormalization.C

if DEV_LIBS
lib_LTLIBRARIES = libfunctionCallNormalization.la
else
noinst_LTLIBRARIES = libfunctionCallNormalization.la
endif
libfunctionCallNormalization_la_SOURCES = $(libfunctionCallNormalizationSources)

include_HEADERS = FunctionNormalization.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core
