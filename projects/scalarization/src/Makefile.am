# where to find rose and boost libraries 
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES)

bin_PROGRAMS = rose-scalarizer
rose_scalarizer_SOURCES = scalarizer.cpp 

rose_scalarizer_LDFLAGS = $(ROSE_RPATHS) 
rose_scalarizer_LDADD = $(ROSE_LIBS) 

include_HEADERS = scalarizer.h

clean:
	rm -rf rose_*.cpp *.o rose-scalarizer

