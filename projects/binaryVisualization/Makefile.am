include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_OPENGL

# DQ (11/13/2008): Can't use builddir on some machines and "-I." is already included by default.
# INCLUDES = $(ROSE_INCLUDES) -I$(builddir) $(GL_CFLAGS)
INCLUDES = $(ROSE_INCLUDES) $(GL_CFLAGS)

bin_PROGRAMS = vizzBinary test

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(GL_LIBS) -L$(GLUT_LIBS) 
#-L/home/panas2/programs/freeglut/lib -lglut

vizzBinary_SOURCES = vizzBinary.C

test_SOURCES = test.C

check-local: vizzBinary test
	./test $(srcdir)/lsdiff

include_HEADERS = helper.h

EXTRA_DIST = helper.h

CLEANFILES = helper.h *.coord

clean:
	rm -rf *.dot *.gml *.o *.s *~ vizzBinary out *.coord 

else

bin_PROGRAMS =

endif


