include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_OPENGL

# DQ (11/13/2008): Can't use builddir on some machines and "-I." is already included by default.
# INCLUDES = $(ROSE_INCLUDES) -I$(builddir) $(GL_CFLAGS)
INCLUDES = $(ROSE_INCLUDES) $(GL_CFLAGS)

if USE_ROSE_GLUT_SUPPORT
bin_PROGRAMS = vizzBinary test
else
bin_PROGRAMS = test
endif

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(GL_LIBS) -L$(GLUT_LIBS) 
#-L/home/panas2/programs/freeglut/lib -lglut

if USE_ROSE_GLUT_SUPPORT
vizzBinary_SOURCES = vizzBinary.C
test_SOURCES = test.C
else
test_SOURCES = test.C
endif


check-local: test
	./test $(srcdir)/lsdiff

include_HEADERS = helper.h

EXTRA_DIST = helper.h

CLEANFILES = helper.h *.coord

clean:
	rm -rf *.dot *.gml *.o *.s *~ vizzBinary out *.coord test

else

bin_PROGRAMS =

endif


