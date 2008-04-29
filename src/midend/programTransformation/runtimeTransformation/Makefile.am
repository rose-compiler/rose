include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


INCLUDES = $(ROSE_INCLUDES) 

if DEV_LIBS
lib_LTLIBRARIES = libruntimeTransformation.la 
else
noinst_LTLIBRARIES = libruntimeTransformation.la 
endif

libruntimeTransformation_la_DEPENDENCIES =
libruntimeTransformation_la_SOURCES = RuntimeInstrumentation.cpp

RUNTIMETRANS_HEADER_FILES = \
   RuntimeInstrumentation.h 

include_HEADERS = $(RUNTIMETRANS_HEADER_FILES)

EXTRA_DIST = 



clean-local:
