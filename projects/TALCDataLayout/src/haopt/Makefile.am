
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) \
         -I$(top_srcdir)/projects/TALCDataLayout/src/utilities

noinst_LIBRARIES = libhaopt.a

libhaopt_a_includedir = $(includedir)/haopt

libhaopt_a_SOURCES = HaOptUtil.cpp

libhaopt_a_include_HEADERS = HaOptUtil.hpp
