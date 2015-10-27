
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) \
         -I$(top_srcdir)/projects/TALCDataLayout/src/utilities \
	 -I$(top_srcdir)/projects/TALCDataLayout/src/haopt \
	 -I$(top_srcdir)/projects/TALCDataLayout/src/meta

noinst_LIBRARIES = libmodules.a

libmodules_a_includedir = $(includedir)/modules

libmodules_a_SOURCES = AOSModuleCheck.cpp AOSModule.cpp HaOptModule.cpp

libmodules_a_include_HEADERS = AOSModuleCheck.cpp AOSModule.cpp HaOptModule.cpp ModuleBase.hpp 
