
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) \
         -I$(top_srcdir)/projects/TALCDataLayout/src/utilities

noinst_LIBRARIES = libmeta.a

libmeta_a_includedir = $(includedir)/meta

libmeta_a_SOURCES = FieldBlock.cpp  Field.cpp  Meta.cpp  Parser.cpp  View.cpp

libmeta_a_include_HEADERS = FieldBlock.hpp  Field.hpp  Meta.hpp  Parser.hpp  View.hpp 
