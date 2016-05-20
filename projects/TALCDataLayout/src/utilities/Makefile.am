
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) \
         -I$(top_srcdir)/projects/TALCDataLayout/src/meta

noinst_LIBRARIES = libutilities.a

libutilities_a_includedir = $(includedir)/utilities

libutilities_a_SOURCES = PrintUtil.cpp  StructUtil.cpp  TALCAttribute.cpp

libutilities_a_include_HEADERS = Common.hpp  PrintUtil.hpp  StructUtil.hpp
