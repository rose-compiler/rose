
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) \
	 -I$(top_srcdir)/projects/TALCDataLayout/src/haopt \
	 -I$(top_srcdir)/projects/TALCDataLayout/src/meta \
	 -I$(top_srcdir)/projects/TALCDataLayout/src/modules \
         -I$(top_srcdir)/projects/TALCDataLayout/src/utilities

PROJ_LIB=..

bin_PROGRAMS = talc
talc_SOURCES = Main.cpp 
talc_CPPFLAGS = $(ROSE_INCLUDES) $(AM_CPPFLAGS) 
talc_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) \
	     $(PROJ_LIB)/meta/libmeta.a \
	     $(PROJ_LIB)/modules/libmodules.a \
	     $(PROJ_LIB)/utilities/libutilities.a \
	     $(PROJ_LIB)/haopt/libhaopt.a  
