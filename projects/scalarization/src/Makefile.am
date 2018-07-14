# where to find rose and boost libraries 
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir)/src 

bin_PROGRAMS = scalarization
scalarization_SOURCES = scalarization.cpp

scalarization_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 


clean:
	rm -rf rose_*.cpp *.o scalarization

