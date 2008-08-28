include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_BINARY_SQL
OPENGL=-L/usr/lib64/ -lGL -lGLU -lglut

#EXTRA_DIST = buffer2.bin

#SUBDIRS = analyses graphanalyses

INCLUDES = $(ROSE_INCLUDES) -I$(builddir) $(LTDLINCL)

bin_PROGRAMS = vizzBinary 

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl $(LIBLTDL) $(OPENGL)

vizzBinary_SOURCES = vizzBinary.C

check-local: vizzBinary 

CLEANFILES = *.o

vizzBinary.o: vizzBinary.C


clean:
	rm *.o

endif
