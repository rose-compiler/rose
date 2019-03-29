# where to find rose and boost libraries 
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES)

bin_PROGRAMS = scalarizer
scalarizer_SOURCES = scalarizer.cpp 

scalarizer_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

include_HEADERS = scalarizer.h

clean:
	rm -rf rose_*.cpp *.o scalarizer

