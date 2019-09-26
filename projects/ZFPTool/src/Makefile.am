# where to find rose and boost libraries 
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir)/src 

bin_PROGRAMS = ZFPTool
ZFPTool_SOURCES = ZFPTool.cpp

ZFPTool_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

include_HEADERS = ZFPTool.h

clean:
	rm -rf rose_*.cpp *.o ZFPTool

