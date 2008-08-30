include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_OPENGL

INCLUDES = $(ROSE_INCLUDES) -I$(builddir) $(GL_CFLAGS)

bin_PROGRAMS = vizzBinary 

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(GL_LIBS)

vizzBinary_SOURCES = vizzBinary.C

check-local: vizzBinary 

#include_HEADERS = helpers.h

#EXTRA_DIST = helpers.h

CLEANFILES = helpers.h

clean:
	rm -f *.dot *.gml *.o *.s *~ vizzBinary out

else

bin_PROGRAMS =

endif


