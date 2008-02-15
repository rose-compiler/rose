include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

libcrSources = cr_abs.cpp cr_error.cpp cr_scan.cpp cr_parse.cpp

# lib_LIBRARIES = libcr.a
# libcr_a_SOURCES  = $(libcrSources)

if DEV_LIBS
lib_LTLIBRARIES = libcr.la
else
noinst_LTLIBRARIES = libcr.la
endif
libcr_la_SOURCES = $(libcrSources)

# Header files that need to be included in the distribution
include_HEADERS = cr_abs.hpp  cr_error.hpp  cr_parse.hpp  cr_scan.hpp
