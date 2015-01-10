
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) \
	 -I$(top_srcdir)/projects/backstroke2/src

PROJ_LIB=..

bin_PROGRAMS = backstroke2
backstroke2_SOURCES = backstroke2.cpp Timer.h Timer.cpp
backstroke2_CPPFLAGS = $(ROSE_INCLUDES) $(INCLUDES) 
backstroke2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
